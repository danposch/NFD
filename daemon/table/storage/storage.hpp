#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <stdlib.h>
#include <boost/date_time/posix_time/posix_time.hpp>

#define BUFFER_LENGTH_DATA (size_t)2048

namespace nfd {
namespace cs {

class StorageEntry;

class Storage
{
public:
  Storage(int maxEntries);

  /*writes a complete entry*/
  virtual bool write(StorageEntry *entry) = 0;

  virtual void evict(StorageEntry *entry) = 0;
  virtual shared_ptr<const Data> read(StorageEntry *entry) = 0;

  virtual bool hasSpace() = 0;

  virtual boost::posix_time::time_duration getDelay() const;

protected:
  void benchStorage();
  boost::posix_time::time_duration delay_msec;
  int maxEntries;

private:
  static const uint8_t* buf;
};

}
}
#endif // PAGER_HPP
