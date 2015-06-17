#ifndef EXAMPLESTORAGESTRATEGY_HPP
#define EXAMPLESTORAGESTRATEGY_HPP

#include "storagestrategy.hpp"
#include "storagecomperator.hpp"

#include "storageentry.hpp"

namespace nfd {
namespace cs {

typedef std::priority_queue<Storage*, std::vector<Storage*>, StorageComperator> MyQueue;

class ExampleStorageStrategy : public StorageStrategy
{
public:
  ExampleStorageStrategy(std::vector<Storage*> storages);

  virtual bool insert(StorageEntry *entry);

  virtual shared_ptr<const Data> getData(const Name& name);

protected:

  MyQueue squeue;


};

}
}

#endif // EXAMPLESTORAGESTRATEGY_HPP
