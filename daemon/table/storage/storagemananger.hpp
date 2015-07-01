#ifndef STORAGEMANANGER_HPP
#define STORAGEMANANGER_HPP

//#include "storageentry.hpp"
#include "storagestrategy.hpp"
#include "mmstorage.hpp"
#include "diskstorage.hpp"
#include "examplestoragestrategy.hpp"
#include "utils/parameterconfiguration.hpp"

namespace nfd {
namespace cs {

class StorageEntry; //forward dec.

class StorageMananger
{
public:

  bool insert(StorageEntry *entry);
  void evict(StorageEntry *entry);
  shared_ptr<const Data> getData(StorageEntry *entry) const;

  int storedEntries();

  static StorageMananger* getInstance();

protected:
  StorageMananger();
  static StorageMananger* instance;

  StorageStrategy* strategy;

};

}
}

#endif // PAGINGMANANGER_HPP
