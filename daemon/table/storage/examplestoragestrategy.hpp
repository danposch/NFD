#ifndef EXAMPLESTORAGESTRATEGY_HPP
#define EXAMPLESTORAGESTRATEGY_HPP

#include "storagestrategy.hpp"
#include "storagecomperator.hpp"
#include <tuple>

#include "core/logger.hpp"

#include "storageentry.hpp"

namespace nfd {
namespace cs {

typedef std::priority_queue<Storage*, std::vector<Storage*>, StorageComperator> MyQueue;

class ExampleStorageStrategy : public StorageStrategy
{
public:
  ExampleStorageStrategy(std::vector<Storage*> storages);

  virtual bool insert(StorageEntry *entry);
  virtual void evict(StorageEntry *entry);

  virtual shared_ptr<const Data> getData(StorageEntry *entry);

protected:

  std::tuple<StorageEntry*, shared_ptr<const Data> > evictRandomEntryFromStorage(Storage* s);

  MyQueue squeue;


};

}
}

#endif // EXAMPLESTORAGESTRATEGY_HPP
