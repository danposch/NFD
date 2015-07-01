#include "mmstorage.hpp"

namespace nfd {
namespace cs {

MMStorage::MMStorage(int maxEntries) : Storage (maxEntries)
{
  currentEntries = 0;
}

bool MMStorage::hasSpace ()
{
  if(currentEntries < maxEntries)
    return true;

  return false;
}

bool MMStorage::write(StorageEntry *entry)
{
  if(currentEntries < maxEntries)
  {
    //nothing to do as we keep entry in main memory...
    currentEntries++;
    //fprintf(stderr, "MMStorage: Added %s\n", entry->getName ().toUri().c_str());
    return true;
  }
  return false;
}

void MMStorage::evict (StorageEntry *entry)
{
  currentEntries--;
  //fprintf(stderr, "MMStorage: Evicted %s\n", entry->getName ().toUri().c_str());

  //entry->m_data.reset(); // remove data packet from memory // not needed should happen automatically...
  //the entry in the cs will be removed by the strategy.
}

shared_ptr<const Data> MMStorage::read(StorageEntry *entry)
{
  //this should never be called as StorageEntry has a pointer to the memory of the data object...

  //fprintf(stderr, "MMStorage: read %s\n", entry->getName ().toUri().c_str());

  return entry->m_data;
}

}
}
