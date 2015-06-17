#ifndef MMSTORAGE_HPP
#define MMSTORAGE_HPP

#include "storage.hpp"

namespace nfd {
namespace cs {

//class StorageEntry;

class MMStorage : public Storage
{
public:
  MMStorage();

  virtual bool write(StorageEntry *entry);
  virtual shared_ptr<const Data> read(const Name& name);
};

}
}

#endif // MMSTORAGE_HPP
