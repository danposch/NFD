#include "safentry.hpp"

using namespace nfd;
using namespace nfd::fw;

SAFEntry::SAFEntry(std::vector<int> faces, shared_ptr<fib::Entry> fibEntry)
{

  pthread_mutex_init( &mutex, NULL );

  this->fibEntry = fibEntry;
  this->faces = faces;
  initFaces();

  smeasure = boost::shared_ptr<Mratio>(new Mratio(this->faces));
  //smeasure = boost::shared_ptr<MDelay>(new MDelay(this->faces));
  ftable = boost::shared_ptr<SAFForwardingTable>(new SAFForwardingTable(this->faces, this->preferedFaces));
  fallbackCounter = 0;
}

void SAFEntry::initFaces ()
{
  const fib::NextHopList& nexthops = fibEntry->getNextHops();
  for(fib::NextHopList::const_iterator it = nexthops.begin (); it != nexthops.end (); it++)
  {
    std::vector<int>::iterator face = std::find(faces.begin (),faces.end (), (*it).getFace()->getId());
    if(face != faces.end ())
    {
      //fprintf(stderr, "costs=%d\n",it->getCost());
      preferedFaces[*face]=it->getCost () + 1; // costs of 0 are not allowed
    }
  }
}

int SAFEntry::determineNextHop(const Interest& interest, std::vector<int> alreadyTriedFaces)
{
  //fprintf(stderr,"determineNextHop LOCK\n");
  pthread_mutex_lock( &mutex);
  int nextHop = ftable->determineNextHop (interest,alreadyTriedFaces);
  pthread_mutex_unlock( &mutex);
  //fprintf(stderr,"determineNextHop UNLOCK\n");

  return nextHop;
}

void SAFEntry::update()
{
  //fprintf(stderr,"update LOCK\n");
  pthread_mutex_lock( &mutex);
  smeasure->update(ftable->getCurrentReliability ());
  ftable->update (smeasure);
  pthread_mutex_unlock( &mutex);
  //fprintf(stderr,"update UNLOCK\n");
  //ftable->crossLayerAdaptation (smeasure);

  /*if(!evaluateFallback())
    ftable->update (smeasure);
  else
    ftable = boost::shared_ptr<SAFForwardingTable>(new SAFForwardingTable(this->faces, this->preferedFaces));*/
}

void SAFEntry::logSatisfiedInterest(shared_ptr<pit::Entry> pitEntry,const Face& inFace, const Data& data)
{
  //fprintf(stderr,"logSatisfiedInterest LOCK\n");
  pthread_mutex_lock( &mutex);
  smeasure->logSatisfiedInterest (pitEntry,inFace,data);
  pthread_mutex_unlock( &mutex);
  //fprintf(stderr,"logSatisfiedInterest UNLOCK\n");
}

void SAFEntry::logExpiredInterest(shared_ptr< pit::Entry > pitEntry)
{
  //fprintf(stderr,"logExpiredInterest LOCK\n");
  pthread_mutex_lock( &mutex);
  smeasure->logExpiredInterest (pitEntry);
  pthread_mutex_unlock( &mutex);
  //fprintf(stderr,"logExpiredInterest UNLOCK\n");
}

void SAFEntry::logNack(const Face& inFace, const Interest& interest)
{
  //fprintf(stderr,"logNack LOCK\n");
  pthread_mutex_lock( &mutex);
  smeasure->logNack (inFace, interest);
  pthread_mutex_unlock( &mutex);
  //fprintf(stderr,"logNack UNLOCK\n");
}

void SAFEntry::logRejectedInterest(shared_ptr<pit::Entry> pitEntry, int face_id)
{
  //fprintf(stderr,"logRejectedInterest LOCK\n");
  pthread_mutex_lock( &mutex);
  smeasure->logRejectedInterest(pitEntry, face_id);
  pthread_mutex_unlock( &mutex);
  //fprintf(stderr,"logRejectedInterest UNLOCK\n");
}

bool SAFEntry::evaluateFallback()
{
  bool fallback = false;
  bool increaseFallback = true;

  if(smeasure->getTotalForwardedInterests (0) == 0)
    return false;

  for(std::vector<int>::iterator it=faces.begin (); it != faces.end (); ++it)
  {
    if(*it == DROP_FACE_ID)
      continue;

    //NS_LOG_UNCOND("forwarded=" << smeasure->getForwardedInterests(*it, 0) << ", linkReliability=" << smeasure50->getLinkReliability (*it, 0));
    if( smeasure->getForwardedInterests (*it, 0) > 0 && smeasure->getFaceReliability (*it, 0) > 0)
    {
      increaseFallback = false;
      break;
    }
  }

  if(increaseFallback)
    fallbackCounter++;
  else if (fallbackCounter > 0)
    fallbackCounter--;

  if(fallbackCounter >= 10.0 / ParameterConfiguration::getInstance ()->getParameter ("UPDATE_INTERVALL"))
  {
    fallbackCounter = 0;
    fallback = true;
  }

  return fallback;
}

void SAFEntry::addFace(shared_ptr<Face> face)
{
  //add new entry to meassure..
  //fprintf(stderr,"Face is to be added\n");
  pthread_mutex_lock( &mutex);
  smeasure->addFace(face);
  ftable->addFace (face);
  pthread_mutex_unlock( &mutex);
  //fprintf(stderr,"Face added\n");
}

void SAFEntry::removeFace(shared_ptr<Face> face)
{
  //fprintf(stderr,"Face is to be removed\n");
  pthread_mutex_lock( &mutex);
  ftable->removeFace (face);
  smeasure->removeFace (face);
  pthread_mutex_unlock( &mutex);
  //fprintf(stderr,"Face removed\n");
}
