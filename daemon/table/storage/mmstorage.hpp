#ifndef MMSTORAGE_HPP
#define MMSTORAGE_HPP

#include "storage.hpp"
#include "storageentry.hpp"

namespace nfd {
namespace cs {

//class StorageEntry;

class MMStorage : public Storage
{
public:
  MMStorage(int maxEntries);

  virtual bool write(StorageEntry *entry);
  virtual void evict(StorageEntry *entry);
  virtual shared_ptr<const Data> read(StorageEntry *entry);

  virtual bool hasSpace();

protected:
  int currentEntries;

};

}
}

#endif // MMSTORAGE_HPP
