#include "storagemananger.hpp"

namespace nfd {
namespace cs {

StorageMananger* StorageMananger::instance = NULL;

StorageMananger::StorageMananger()
{
  std::vector<Storage*> storages;

  //add +1 as Cs evicts after insert.. not before!
  storages.push_back (new MMStorage(ParameterConfiguration::getInstance()->getParameter("MaxMMStorageEntries") + 1 ));
  storages.push_back (new DiskStorage(ParameterConfiguration::getInstance()->getParameter("MaxDiskStorageEntries") + 1));

  strategy = new ExampleStorageStrategy(storages);
}

bool StorageMananger::insert(StorageEntry *entry)
{
  return strategy->insert (entry);
}

void StorageMananger::evict(StorageEntry *entry)
{
  strategy->evict (entry);
}


shared_ptr<const Data> StorageMananger::getData(StorageEntry *entry) const
{
  return strategy->getData(entry);
}

StorageMananger *StorageMananger::getInstance()
{
  if(instance == NULL)
    instance = new StorageMananger();

  return instance;
}

int StorageMananger::storedEntries ()
{
  return strategy->storedEntries ();
}

}
}
