#include "storagestrategy.hpp"

namespace nfd {
namespace cs {


StorageStrategy::StorageStrategy(std::vector<Storage*> storages)
{
  this->storages = storages;
}

Storage* StorageStrategy::getStorageContaining(StorageEntry *key)
{
  /*fprintf(stderr, "Searching for: %p\n", key);

  std::map<StorageEntry*, Storage*>::iterator it2;

  for(it2 = index.begin (); it2 != index.end (); it2++)
    fprintf(stderr, "entry=%p; storage=%p\n", it2->first,it2->second);*/

  std::map<StorageEntry*, Storage*>::iterator it = index.find(key);

  if(it != index.end())
    return it->second;

  return NULL;
}

int StorageStrategy::storedEntries ()
{
  return index.size ();
}

}
}
