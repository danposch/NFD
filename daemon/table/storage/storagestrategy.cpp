#include "storagestrategy.hpp"

namespace nfd {
namespace cs {


StorageStrategy::StorageStrategy(std::vector<Storage*> storages)
{
  this->storages = storages;
}

Storage* StorageStrategy::getStorageContaining(const std::string &key)
{
  /*fprintf(stderr, "Searching for: %s\n", key.c_str ());

  for(std::map<std::string, Storage*>::iterator it = index.begin (); it != index.end (); it++)
      fprintf(stderr, "E[%s]=%d\n", it->first.c_str(), it->second);*/

  std::map<std::string, Storage*>::iterator it = index.find(key);

  if(it != index.end())
    return it->second;
  else
    return NULL;
}

}
}
