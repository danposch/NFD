#ifndef DISKSTORAGE_HPP
#define DISKSTORAGE_HPP

#include <stdio.h>
#include <sys/stat.h>

#include "storage.hpp"
#include "storageentry.hpp"
#include <boost/algorithm/string.hpp>

#define DISK_STORE_PATH "/tmp/_NFD_" // must not end with trailing '/' to support "empty" names

namespace nfd {
namespace cs {

class DiskStorage : public Storage
{
public:
  DiskStorage();

  virtual bool write(StorageEntry *entry);
  virtual shared_ptr<const Data> read(const Name& name);
};

}
}

#endif // DISKSTORAGE_HPP
