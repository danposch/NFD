#include "storageentry.hpp"

namespace nfd {
namespace cs {

StorageEntry::StorageEntry(const Name &name) : EntryImpl(name)
{
  m_queryName = name;
  m_hasData = false;
}

StorageEntry::StorageEntry(shared_ptr<const Data> data, bool isUnsolicited) : EntryImpl(data, isUnsolicited)
{
  this->setData(data, isUnsolicited); //do it again as we need the method of *this
}

const Data& StorageEntry::getData() const
{
  BOOST_ASSERT(this->hasData());

  //check if we have data on pointer
  if(m_data) // shared pointer has operator bool
    return *m_data;

  /*//TODO check if this behavior is ok ? i think  as the strategy should decide wether to keep data in mem?
  shared_ptr<const Data> d = (StorageMananger::getInstance ()->getData (m_fullName));
  fprintf(stderr, "GetData():: Ret = %s, refcout(d)=%lu\n", d->getName().toUri().c_str(), d.use_count());
  return *d;*/

  //We have to store the data here, as the API requires us to return a Data& and
  // no one takes the responsibility to delete the object later. Bad design we need shared_ptr here!!
  m_data = StorageMananger::getInstance ()->getData (m_fullName.toUri());
  return *m_data;
}

const Name& StorageEntry::getName() const
{
  return m_queryName; // return stored name
}

const Name& StorageEntry::getFullName() const
{
  return m_fullName;
}

bool StorageEntry::ImatchD(const Interest& interest, const ndn::Name& dataName, const ndn::Name& dataFullName) const
{
  size_t interestNameLength = interest.getName().size();
  size_t fullNameLength = dataName.size() + 1;

  // check MinSuffixComponents
  bool hasMinSuffixComponents = interest.getMinSuffixComponents() >= 0;
  size_t minSuffixComponents = hasMinSuffixComponents ?
                               static_cast<size_t>(interest.getMinSuffixComponents()) : 0;
  if (!(interestNameLength + minSuffixComponents <= fullNameLength))
    return false;

  // check MaxSuffixComponents
  bool hasMaxSuffixComponents = interest.getMaxSuffixComponents() >= 0;
  if (hasMaxSuffixComponents &&
      !(interestNameLength + interest.getMaxSuffixComponents() >= fullNameLength))
    return false;

  // check prefix
  if (interestNameLength == fullNameLength) {
    if (interest.getName().get(-1).isImplicitSha256Digest()) {
      if (interest.getName() != dataFullName)
        return false;
    }
    else {
      // Interest Name is same length as Data full Name, but last component isn't digest
      // so there's no possibility of matching
      return false;
    }
  }
  else {
    // Interest Name is a strict prefix of Data full Name
    if (!interest.getName().isPrefixOf(dataName))
      return false;
  }

  // check Exclude
  // Exclude won't be violated if Interest Name is same as Data full Name
  if (!interest.getExclude().empty() && fullNameLength > interestNameLength) {
    if (interestNameLength == fullNameLength - 1) {
      // component to exclude is the digest
      if (interest.getExclude().isExcluded(dataFullName.get(interestNameLength)))
        return false;
    }
    else {
      // component to exclude is not the digest
      if (interest.getExclude().isExcluded(dataName.get(interestNameLength)))
        return false;
    }
  }

  const Data& data = getData();

  // check PublisherPublicKeyLocator
  const ndn::KeyLocator& publisherPublicKeyLocator = interest.getPublisherPublicKeyLocator();
  if (!publisherPublicKeyLocator.empty()) {
    const ndn::Signature& signature = data.getSignature();
    const Block& signatureInfo = signature.getInfo();
    Block::element_const_iterator it = signatureInfo.find(tlv::KeyLocator);
    if (it == signatureInfo.elements_end()) {
      return false;
    }
    if (publisherPublicKeyLocator.wireEncode() != *it) {
      return false;
    }
  }

  return true;
}

bool StorageEntry::canSatisfy(const Interest& interest) const
{
  BOOST_ASSERT(this->hasData());

  if (!ImatchD(interest, m_queryName, m_fullName))
  {
    return false;
  }

  if (interest.getMustBeFresh() == static_cast<int>(true) && this->isStale())
  {
    return false;
  }

  return true;
}

bool StorageEntry::hasData() const
{
  return m_hasData;
}

void StorageEntry::setData(shared_ptr<const Data> data, bool isUnsolicited)
{
  //todo erase old entry from storage?

  m_queryName = data->getName();
  m_fullName = data->getFullName();
  m_freshnessPeriod = data->getFreshnessPeriod();
  m_data = data;
  m_isUnsolicited = isUnsolicited;
  m_hasData = true;

  insertCurrentDataToStorage ();
}

void StorageEntry::updateStaleTime()
{
  BOOST_ASSERT(this->hasData());

  if (m_freshnessPeriod >= time::milliseconds::zero())
  {
    m_staleTime = time::steady_clock::now() + time::milliseconds(m_freshnessPeriod);
  }
  else
  {
    m_staleTime = time::steady_clock::TimePoint::max();
  }
}

void StorageEntry::reset()
{
  //TODO implement evict from storage...
  m_data.reset(); //equivalent to nullptr!
  //m_data = nullptr;

  m_isUnsolicited = false;
  m_staleTime = time::steady_clock::TimePoint();
}

void StorageEntry::unsetUnsolicited()
{
  BOOST_ASSERT(!this->isQuery());

  m_isUnsolicited = false;
  updateStaleTime();
}

void StorageEntry::insertCurrentDataToStorage()
{
  StorageMananger::getInstance ()->insert (this);
}

int compareQueryWithData(const Name& queryName, const Name& dataName, const Name& dataFullName)
{
  bool queryIsFullName = !queryName.empty() && queryName[-1].isImplicitSha256Digest();

  int cmp = queryIsFullName ?
            queryName.compare(0, queryName.size() - 1, dataName) :
            queryName.compare(dataName);

  if (cmp != 0) { // Name without digest differs
    return cmp;
  }

  if (queryIsFullName) { // Name without digest equals, compare digest
    return queryName[-1].compare(dataFullName[-1]);
  }
  else { // queryName is a proper prefix of Data fullName
    return -1;
  }
}

int compareDataWithData(const Name& lhsName, const Name& lhsFullName, const Name& rhsName, const Name& rhsFullName)
{
  int cmp = lhsName.compare(rhsName);
  if (cmp != 0) {
    return cmp;
  }

  return lhsFullName[-1].compare(rhsFullName[-1]);
}

bool StorageEntry::operator<(const StorageEntry& other) const
{
  if (this->isQuery()) {
    if (other.isQuery()) {
      return m_queryName < other.m_queryName;
    }
    else {
      return compareQueryWithData(m_queryName, other.getName (), other.getFullName ()) < 0;
    }
  }
  else {
    if (other.isQuery()) {
      return compareQueryWithData(other.m_queryName, this->getName(), this->getFullName ()) > 0;
    }
    else {
      return compareDataWithData(this->getName(), this->getFullName (), other.getName(), other.getFullName ()) < 0;
    }
  }
}

}
}
