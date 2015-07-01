#include "storage.hpp"

namespace nfd {
namespace cs {

const uint8_t* Storage::buf = (uint8_t*) malloc(BUFFER_LENGTH_DATA*sizeof(uint8_t));

Storage::Storage(int maxEntries)
{
  this->maxEntries = maxEntries;
}

void Storage::benchStorage ()
{

  shared_ptr<const Data> benchData(new Data(Name("/bench/data/packet")));

  //TODO fix this
  //benchData->setContent((const uint8_t*)buf, (size_t) BUFFER_LENGTH_DATA);

  boost::posix_time::ptime t1(boost::posix_time::microsec_clock::local_time());

  //write (benchData);

  //todo read;

  boost::posix_time::ptime t2(boost::posix_time::microsec_clock::local_time());

  delay_msec = t2 - t1;
}

boost::posix_time::time_duration Storage::getDelay() const
{
  return delay_msec;
}

}
}
