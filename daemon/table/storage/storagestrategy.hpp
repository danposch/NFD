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
  virtual shared_ptr<const Data> getData(const Name& name) = 0;

protected:

  Storage* getStorageContaining(const std::string& key);

  std::vector<Storage*> storages;
  std::map<std::string, Storage*> index;

};


}
}
#endif // PAGINGSTRATEGY_HPP
