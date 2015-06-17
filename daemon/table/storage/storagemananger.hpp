#ifndef STORAGEMANANGER_HPP
#define STORAGEMANANGER_HPP

//#include "storageentry.hpp"
#include "storagestrategy.hpp"
#include "mmstorage.hpp"
#include "diskstorage.hpp"
#include "examplestoragestrategy.hpp"

namespace nfd {
namespace cs {

class StorageEntry; //forward dec.
//class StorageStrategy;

class StorageMananger
{
public:

  bool insert(StorageEntry *entry);
  shared_ptr<const Data> getData(const Name& name) const;

  static StorageMananger* getInstance();

protected:
  StorageMananger();
  static StorageMananger* instance;

  StorageStrategy* strategy;

};

}
}

#endif // PAGINGMANANGER_HPP
