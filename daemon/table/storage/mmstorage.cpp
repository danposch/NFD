#include "mmstorage.hpp"

namespace nfd {
namespace cs {

MMStorage::MMStorage() : Storage ()
{
}

bool MMStorage::write(StorageEntry *entry)
{
  //nothing to do as we keep entry in main memory...
  return true;
}

shared_ptr<const Data> MMStorage::read(const Name &name)
{
  //this should never be called as StorageEntry has a pointer to the memory of the data object...
  return nullptr;
}

}
}
