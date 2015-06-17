#include "storagecomperator.hpp"

namespace nfd {
namespace cs {

StorageComperator::StorageComperator(const bool& revparam)
{
  reverse = revparam;
}

bool StorageComperator::operator() (const Storage& l, const Storage& r) const
{
  if(reverse)
    return l.getDelay() > r.getDelay();

  return l.getDelay() < r.getDelay();
}

bool StorageComperator::operator() (Storage* l, Storage* r)
{
  if(reverse)
    return l->getDelay() > r->getDelay();

  return l->getDelay() < r->getDelay();
}


}
}
