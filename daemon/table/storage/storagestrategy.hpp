#ifndef STORAGESTRATEGY_HPP
#define STORAGESTRATEGY_HPP

#include <vector>
#include <queue>
#include "storage.hpp"

namespace nfd {
namespace cs {

class StorageStrategy
{
public:
  StorageStrategy(std::vector<Storage*> storages);

  virtual bool insert(StorageEntry *entry) = 0;
  virtual void evict(StorageEntry *entry) = 0;
  virtual shared_ptr<const Data> getData(StorageEntry *entry) = 0;

  virtual int storedEntries();

protected:

  Storage* getStorageContaining(StorageEntry *key);

  std::vector<Storage*> storages;
  std::map<StorageEntry*, Storage*> index;

};


}
}
#endif // PAGINGSTRATEGY_HPP
