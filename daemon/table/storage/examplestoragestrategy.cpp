#include "examplestoragestrategy.hpp"

namespace nfd {
namespace cs {

ExampleStorageStrategy::ExampleStorageStrategy(std::vector<Storage*> storages) : StorageStrategy(storages)
{
  for(std::vector<Storage*>::iterator it = storages.begin (); it != storages.end (); it++)
    squeue.push (*it);
}

bool ExampleStorageStrategy::insert(StorageEntry *entry)
{
  MyQueue q (squeue);
  Storage *s;
  while (q.size () > 0)
  {
    s = q.top ();

    if(s->write (entry)) // try to write
    {
      //store in which storage the data is stored
      fprintf(stderr, "Added %s to storage.\n", entry->getName().toUri().c_str());
      index[entry->getFullName ()[-1].toUri()] = s; // we use the hash only as key
      return true;
    }

    q.pop ();
  }

  return false;
}

shared_ptr<const Data> ExampleStorageStrategy::getData(const Name &name)
{
  fprintf(stderr, "trying to find %s in storages\n", name.toUri().c_str ());
  Storage *s = getStorageContaining(name[-1].toUri()); // we use the hash only as key

  if(s == NULL)
  {
    fprintf(stderr, "Could not find data in Index. This should not happen!\n");
    return NULL;
  }

  return s->read (name);
}

}
}
