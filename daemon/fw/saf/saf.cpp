#include "saf.hpp"

using namespace nfd;
using namespace nfd::fw;

NFD_LOG_INIT ("SAF");

const Name SAF::STRATEGY_NAME("ndn:/localhost/nfd/strategy/saf/%FD%01");
NFD_REGISTER_STRATEGY(SAF);

SAF::SAF(Forwarder &forwarder, const Name &name) : Strategy(forwarder, name)
{
  NFD_LOG_INFO("Strategy SAF LOADED");

  const FaceTable& ft = getFaceTable();
  engine = boost::shared_ptr<SAFEngine>(new SAFEngine(ft, ParameterConfiguration::getInstance ()->getParameter ("SAFPrefixComponents")));

  /*this->afterAddFace.connect([this] (shared_ptr<Face> face)
  {
    NFD_LOG_INFO("Strategy SAF new Initialized");
    int prefixComponets = 0;
    engine = boost::shared_ptr<SAFEngine>(new SAFEngine(getFaceTable(), prefixComponets));
  });

  this->beforeRemoveFace.connect([this] (shared_ptr<Face> face)
  {
    NFD_LOG_INFO("Strategy SAF new Initialized");
    int prefixComponets = 0;
    engine = boost::shared_ptr<SAFEngine>(new SAFEngine(getFaceTable(), prefixComponets));
  });*/

  // The nice way to do it...
  this->afterAddFace.connect([this] (shared_ptr<Face> face)
  {
    NFD_LOG_INFO("Strategy SAF adding new face");
    engine->addFace (face);
  });

  this->beforeRemoveFace.connect([this] (shared_ptr<Face> face)
  {
    NFD_LOG_INFO("Strategy SAF removing face");
    engine->removeFace (face);
  });
}

SAF::~SAF()
{
}

void SAF::afterReceiveInterest(const Face& inFace, const Interest& interest ,shared_ptr<fib::Entry> fibEntry, shared_ptr<pit::Entry> pitEntry)
{
  /* Attention!!! interest != pitEntry->interest*/ // necessary to emulate NACKs in ndnSIM2.0
  /* interst could be /NACK/suffix, while pitEntry->getInterest is /suffix */

  //fprintf(stderr, "Incoming Interest = %s\n", interest.getName().toUri().c_str());

  //find + exclude inface(s) and already tried outface(s)
  std::vector<int> originInFaces = getAllInFaces(pitEntry);
  std::vector<int> alreadyTriedFaces; // keep them empty for now and check if nack or retransmission?

  std::string prefix = interest.getName().get(0).toUri();
  if(prefix.compare("NACK") == 0)
  {
    //fprintf(stderr, "Received Nack %s on face[%d]\n", interest.getName().toUri().c_str(), inFace.getId ());
    engine->logNack(inFace, pitEntry->getInterest());
    alreadyTriedFaces = getAllOutFaces(pitEntry);
  }

  const Interest int_to_forward = pitEntry->getInterest();
  int nextHop = engine->determineNextHop(int_to_forward, alreadyTriedFaces, fibEntry);
  while(nextHop != DROP_FACE_ID && (std::find(originInFaces.begin (),originInFaces.end (), nextHop) == originInFaces.end ()))
  {
    bool success = engine->tryForwardInterest (int_to_forward, getFaceTable ().get (nextHop));

    /*DISABLING LIMITS FOR NOW*/
    success = true; // as not used in the SAF paper.

    if(success)
    {
      //fprintf(stderr, "Transmitting %s on face[%d]\n", int_to_forward.getName().toUri().c_str(), nextHop);

      if(getFaceTable ().get (nextHop) == NULL) //due to asynchron face deletion
      {
        alreadyTriedFaces.push_back (nextHop);
        nextHop = engine->determineNextHop(int_to_forward, alreadyTriedFaces, fibEntry);
        continue;
      }

      sendInterest(pitEntry, getFaceTable ().get (nextHop));
      return;
    }

    engine->logNack((*getFaceTable ().get(nextHop)), pitEntry->getInterest());
    alreadyTriedFaces.push_back (nextHop);
    nextHop = engine->determineNextHop(int_to_forward, alreadyTriedFaces, fibEntry);
  }
  //fprintf(stderr, "Rejecting Interest %s\n", int_to_forward.getName ().toUri ().c_str ());
  engine->logRejectedInterest(pitEntry, nextHop);
  rejectPendingInterest(pitEntry);
}

void SAF::beforeSatisfyInterest(shared_ptr<pit::Entry> pitEntry,const Face& inFace, const Data& data)
{
  engine->logSatisfiedInterest (pitEntry, inFace, data);
  Strategy::beforeSatisfyInterest (pitEntry,inFace, data);
}

void SAF::beforeExpirePendingInterest(shared_ptr< pit::Entry > pitEntry)
{
  //fprintf(stderr, "Timeout %s\n", pitEntry->getInterest().getName().toUri().c_str());
  engine->logExpiredInterest(pitEntry);
  Strategy::beforeExpirePendingInterest (pitEntry);
}

std::vector<int> SAF::getAllInFaces(shared_ptr<pit::Entry> pitEntry)
{
  std::vector<int> faces;
  const nfd::pit::InRecordCollection records = pitEntry->getInRecords();

  for(nfd::pit::InRecordCollection::const_iterator it = records.begin (); it!=records.end (); ++it)
  {
    if(! (*it).getFace()->isLocal())
      faces.push_back((*it).getFace()->getId());
  }
  return faces;
}

std::vector<int> SAF::getAllOutFaces(shared_ptr<pit::Entry> pitEntry)
{
  std::vector<int> faces;
  const nfd::pit::OutRecordCollection records = pitEntry->getOutRecords();

  for(nfd::pit::OutRecordCollection::const_iterator it = records.begin (); it!=records.end (); ++it)
    faces.push_back((*it).getFace()->getId());

  return faces;
}

signal::Signal< FaceTable, shared_ptr< Face > > & afterAddFace();

signal::Signal< FaceTable, shared_ptr< Face > > & beforeRemoveFace();

