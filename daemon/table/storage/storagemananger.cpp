#include "storagemananger.hpp"

namespace nfd {
namespace cs {

StorageMananger* StorageMananger::instance = NULL;

StorageMananger::StorageMananger()
{

  std::vector<Storage*> storages;
  storages.push_back (new MMStorage());
  //storages.push_back (new DiskStorage());

  strategy = new ExampleStorageStrategy(storages);
}

bool StorageMananger::insert(StorageEntry *entry)
{
  strategy->insert (entry);
  return true;
}

shared_ptr<const Data> StorageMananger::getData(const Name& name) const
{
  return strategy->getData(name);
}

StorageMananger *StorageMananger::getInstance()
{
  if(instance == NULL)
    instance = new StorageMananger();

  return instance;
}

}
}
