#include "examplestoragestrategy.hpp"

namespace nfd {
namespace cs {

NFD_LOG_INIT ("ExampleStorageStrategy");

ExampleStorageStrategy::ExampleStorageStrategy(std::vector<Storage*> storages) : StorageStrategy(storages)
{
  for(std::vector<Storage*>::iterator it = storages.begin (); it != storages.end (); it++)
    squeue.push (*it);
}

//just put entry on the "best" storage
bool ExampleStorageStrategy::insert(StorageEntry *entry)
{
  Storage *bs = squeue.top ();
  if(!bs->hasSpace ())
  {
    bool entryMoved = false;
    //we first have to make space just shift one entry from bs to any free storage..
    NFD_LOG_DEBUG("BestStorage full. Evicting a random Entry");
    std::tuple<StorageEntry*, shared_ptr<const Data> > evicted_tuple = evictRandomEntryFromStorage(bs);

    //very simple solution actually shifts the data only to the next best storage that has a free slot
    MyQueue qo (squeue);
    Storage *so;
    while (qo.size () > 0)
    {
      so = qo.top ();

      if(bs != so && so->hasSpace ())
      {
        //now the diry part... write data that has been read back to the entry so storage "so" can write it and remove it later
        std::get<0>(evicted_tuple)->m_data = std::get<1>(evicted_tuple);
        if(so->write (std::get<0>(evicted_tuple)))
        {
          index[std::get<0>(evicted_tuple)] = so; //set new index
          entryMoved = true;
          break;
        }
      }
      qo.pop ();
    }
    if(!entryMoved)
    {
      NFD_LOG_DEBUG("ERROR could not move entry to another storage!!\n");
    }
  }

  if(bs->write (entry)) // try to write
  {
    //store in which storage the data is stored
    index[entry] = bs; // we use the adress of the object as key
    return true;
  }

  NFD_LOG_DEBUG("Critical Error when writing Entry\n");
  return false;
}


//put entry on the first free storage
/*bool ExampleStorageStrategy::insert(StorageEntry *entry)
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
      index[entry] = s; // we use the adress of the object as key
      return true;
    }

    q.pop ();
  }

  return false;
}*/

shared_ptr<const Data> ExampleStorageStrategy::getData(StorageEntry *entry)
{
  NFD_LOG_DEBUG("trying to find " << entry->getName ().toUri() << " in Storages");
  Storage *s = getStorageContaining(entry);

  if(s == NULL)
  {
    NFD_LOG_DEBUG("Could not find data in Index, can not fetch it. This should not happen!\n");
    return NULL;
  }

  shared_ptr<const Data> d = s->read (entry);

  //check if data was in memory
  if(dynamic_cast<MMStorage*>(s) == NULL)
  {
    //put it into memory NOW

    NFD_LOG_DEBUG("Entry " << entry->getName ().toUri() << " was not stored im memmory!");

    Storage *mem_s = NULL;
    MyQueue q (squeue);

    //find the mmstorage
    while (q.size () > 0)
    {
      if(dynamic_cast<MMStorage*>(q.top ()) != NULL)
      {
        mem_s = q.top();
        break;
      }
      q.pop ();
    }

    if(mem_s == NULL) // no memory storage found, so we cant put it there. This is bad...
      return d;

    if(!mem_s->hasSpace ())
    {
      NFD_LOG_DEBUG("MMStorage full. Evicting a random Entry");
      std::tuple<StorageEntry*, shared_ptr<const Data> > evicted_tuple = evictRandomEntryFromStorage(mem_s);

      //very simple solution actually shifts the data only to the next best storage that has a free slot
      MyQueue qo (squeue);
      Storage *so;
      bool entryMoved = false;
      while (qo.size () > 0)
      {
        so = qo.top ();

        if(mem_s != so && so->hasSpace ())
        {
          //now the diry part... write data that has been read back to the entry so storage "so" can write it and remove it later
          std::get<0>(evicted_tuple)->m_data = std::get<1>(evicted_tuple);
          so->write (std::get<0>(evicted_tuple));
          index[std::get<0>(evicted_tuple)] = so; //set new index
          entryMoved = true;
          break;
        }
        qo.pop ();
      }

      if(!entryMoved)
        NFD_LOG_DEBUG("ExampleStorageStrategy::getData Critical Error Could not move Entry to another storage!");
    }

    //now the diry part... write data that has been read back to the entry
    entry->m_data = d;
    mem_s->write (entry); //write the entry into mem_s
    index[entry] = mem_s; // create new index entry
    NFD_LOG_DEBUG("Entry " << entry->getName ().toUri() << " stored in MMStorage\n");
  }

   NFD_LOG_DEBUG("Entry " << entry->getName ().toUri() << " found and returned");

  return d;
}

void ExampleStorageStrategy::evict(StorageEntry *entry)
{
  Storage *s = getStorageContaining(entry); // we use the hash only as key

  if(s == NULL)
  {
    //This may happen as a new StorageEntry call evict() when setData() is called to ensure any previous data is cleared..
    //fprintf(stderr, "Could not find data in Index, can not call evict(). Data was not in storage!\n");
    return;
  }

  NFD_LOG_DEBUG("Evicting: " << entry->getName() << " from storage");
  s->evict(entry);
  index.erase (entry);
}

std::tuple<StorageEntry*, shared_ptr<const Data> > ExampleStorageStrategy::evictRandomEntryFromStorage(Storage* s)
{
  //randomly iterate over all entries, find one that has the same storage as s and evict that one entry

  NFD_LOG_DEBUG("Evicting a random Entry:");

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0,index.size ()-1);

  shared_ptr<const Data> evicted_data = nullptr;

  std::map<StorageEntry*, Storage*>::iterator it;
  while(true)
  {
     it = index.begin ();
     std::advance(it, dis(gen));

     if(it->second == s) //check if same storage
     {
       NFD_LOG_DEBUG("Evicting:" << it->first);
       evicted_data = s->read (it->first);
       s->evict (it->first);
       index.erase (it);
       return std::make_tuple(it->first, evicted_data);
     }
  }
}

}
}
