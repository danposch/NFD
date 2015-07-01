#ifndef STORAGEENTRY_HPP
#define STORAGEENTRY_HPP

#include "../cs-entry-impl.hpp"
#include "storagemananger.hpp"
#include "ndn-cxx/key-locator.hpp"

namespace nfd {
namespace cs {

class StorageEntry : public EntryImpl
{
//add friend classes  that are allowed to modify the entries protected attributes
friend class DiskStorage;
friend class MMStorage;
friend class ExampleStorageStrategy;

public:
  StorageEntry(const Name& name);
  StorageEntry(shared_ptr<const Data> data, bool isUnsolicited);

  virtual ~StorageEntry();

  virtual const Data& getData() const;

  virtual const Name& getName() const;

  virtual const Name& getFullName() const;

  virtual bool canSatisfy(const Interest& interest) const;

  virtual bool hasData() const;

  virtual void setData(shared_ptr<const Data> data, bool isUnsolicited);

  virtual void updateStaleTime();

  virtual void reset();

  virtual void unsetUnsolicited();

  virtual bool operator<(const StorageEntry& other) const;

  virtual time::milliseconds getFreshnessPeriod() const {return m_freshnessPeriod;}

protected:
  bool m_hasData;
  bool ImatchD(const Interest& interest, const ndn::Name &dataName, const ndn::Name &dataFullName) const;
  virtual void insertCurrentDataToStorage();
  Name m_fullName;
  time::milliseconds m_freshnessPeriod;
};

}
}

#endif // STORAGEENTRY_HPP
