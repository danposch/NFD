#ifndef STORAGECOMPERATOR_HPP
#define STORAGECOMPERATOR_HPP

#include "storage.hpp"

namespace nfd {
namespace cs {

class StorageComperator
{

public:
  StorageComperator(const bool& revparam = false);

  bool operator() (const Storage& l, const Storage& r) const;
  bool operator() (Storage* l, Storage* r);

private:
  bool reverse;


};

}
}

#endif // STORAGECOMPERATOR_HPP
