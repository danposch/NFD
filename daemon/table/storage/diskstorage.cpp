#include "diskstorage.hpp"

namespace nfd {
namespace cs {

DiskStorage::DiskStorage(int maxEntries) : Storage(maxEntries)
{
  currentEntries = 0;
}

bool DiskStorage::hasSpace()
{
  if(currentEntries < maxEntries)
    return true;
  return false;
}

bool DiskStorage::write(StorageEntry *entry)
{

  if(hasSpace())
  {
    Data d = entry->getData();
    Block b = d.wireEncode();

    std::string dname = entry->getFullName ()[-1].toUri();
    boost::erase_all(dname, "/");
    std::string fname (DISK_STORE_PATH + dname);

    //write data to disk
    FILE* f = fopen(fname.c_str (), "wb");
    fwrite(b.wire(), sizeof(uint8_t), b.size(), f);
    fclose(f);

    //delete data from MM
    entry->m_data.reset();

    currentEntries++;

    //fprintf(stderr, "DiskStorage: Added %s\n", entry->getName ().toUri().c_str());

    return true;
  }

  fprintf(stderr, "DiskStorage: Could NOT add %s\n", entry->getName ().toUri().c_str());

  return false;
}

inline bool fexist (const std::string& name) {
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}

void DiskStorage::evict(StorageEntry *entry)
{
  std::string dname = entry->getFullName ()[-1].toUri();
  boost::erase_all(dname, "/");
  std::string fname (DISK_STORE_PATH + dname);

  if(!fexist(fname))
  {
    fprintf(stderr, "DiskStorage: Could NOT evict %s\n", entry->getName ().toUri().c_str());
  }

  //fprintf(stderr, "DiskStorage: Evicted %s\n", entry->getName ().toUri().c_str());

  currentEntries--;
}

shared_ptr<const Data> DiskStorage::read(StorageEntry *entry)
{
  //read data from disk

  std::string dname = entry->getFullName ()[-1].toUri();
  boost::erase_all(dname, "/");

  std::string fname (DISK_STORE_PATH + dname);

  if(!fexist(fname))
  {
    fprintf(stderr, "DiskStorage: Could NOT find %s on disk.\n", entry->getName ().toUri().c_str());
    return nullptr;
  }

  FILE* f = fopen(fname.c_str (), "rb");

  //obtain file size:
  fseek (f , 0 , SEEK_END);
  long lSize = ftell (f);
  rewind (f);

  // allocate memory to contain the whole file:
  uint8_t *buffer = (uint8_t*) malloc (sizeof(uint8_t)*lSize);

  //fprintf(stderr, "Reading Data from disk: %s --- %lu bytes\n", fname.c_str (), sizeof(uint8_t)*lSize);

  // copy the file into the buffer:
  if(fread (buffer,sizeof(uint8_t),lSize,f) != ( (size_t)lSize * sizeof(uint8_t)))
  {
    fprintf(stderr, "Read Error. Returning NULL\n");
    return nullptr;
  }

  fclose(f);

  Block b(buffer, (size_t) lSize);
  //shared_ptr<const Data> data(new Data(b));
  shared_ptr<const Data> data = make_shared<Data>(b); // use this always!
  free(buffer);

  //fprintf(stderr, "DiskStorage: Read %s\n", entry->getName ().toUri().c_str());

  return data;
}

}
}
