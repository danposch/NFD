#include "diskstorage.hpp"

namespace nfd {
namespace cs {

DiskStorage::DiskStorage() : Storage()
{

}

bool DiskStorage::write(StorageEntry *entry)
{
  Data d = entry->getData();
  Block b = d.wireEncode();

  std::string dname = d.getFullName()[-1].toUri();
  boost::erase_all(dname, "/");
  std::string fname (DISK_STORE_PATH + dname);

  fprintf(stderr, "Write Data to disk: %s --- %lu bytes\n", fname.c_str (), b.size());

  //write data to disk
  FILE* f = fopen(fname.c_str (), "wb");
  fwrite(b.wire(), sizeof(uint8_t), b.size(), f);
  fclose(f);

  /*fprintf(stderr, "Writing:\n");
  for(int i = 0; i < b.size(); i++)
    fprintf(stderr,"%x ", b.wire()[i]& 0xff);
  fprintf (stderr,"\n");*/

  //delete data from MM
  fprintf(stderr, "Shared Ptr Count = %lu\n", entry->m_data.use_count());
  entry->m_data.reset();

  return true;
}

inline bool fexist (const std::string& name) {
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}

shared_ptr<const Data> DiskStorage::read(const Name &name)
{
  //read data from disk

  std::string dname = name[-1].toUri();
  boost::erase_all(dname, "/");

  std::string fname (DISK_STORE_PATH + dname);

  if(!fexist(fname))
    return nullptr;

  FILE* f = fopen(fname.c_str (), "rb");

  //obtain file size:
  fseek (f , 0 , SEEK_END);
  long lSize = ftell (f);
  rewind (f);

  // allocate memory to contain the whole file:
  uint8_t *buffer = (uint8_t*) malloc (sizeof(uint8_t)*lSize);

  fprintf(stderr, "Reading Data from disk: %s --- %lu bytes\n", fname.c_str (), sizeof(uint8_t)*lSize);

  // copy the file into the buffer:
  if(fread (buffer,sizeof(uint8_t),lSize,f) != ( (size_t)lSize * sizeof(uint8_t)))
  {
    fprintf(stderr, "Read Error. Returning NULL\n");
    return nullptr;
  }

  fclose(f);

  /*fprintf(stderr, "Reading:\n");
  for(int i = 0; i < lSize; i++)
    fprintf(stderr,"%x ", buffer[i]& 0xff);
  fprintf (stderr,"\n");*/

  Block b(buffer, (size_t) lSize);
  //shared_ptr<const Data> data(new Data(b));
  shared_ptr<const Data> data = make_shared<Data>(b); // use this always!
  free(buffer);

  //fprintf(stderr, "Paresed Data: %s\n", data->getName().toUri().c_str());

  return data;
}

}
}
