#include "safengine.hpp"

using namespace nfd;
using namespace nfd::fw;

NFD_LOG_INIT ("SAFEngine");

SAFEngine::SAFEngine(const FaceTable& table, unsigned int prefixComponentNumber)
{
  initFaces(table);
  this->prefixComponentNumber = prefixComponentNumber;

  time::nanoseconds delay = time::milliseconds((int)ceil(ParameterConfiguration::getInstance ()->getParameter ("UPDATE_INTERVALL")));
  updateEventFWT = nfd::scheduler::schedule(delay, bind(&SAFEngine::update, this));
}

SAFEngine::~SAFEngine()
{
  nfd::scheduler::cancel(updateEventFWT);
}

void SAFEngine::initFaces(const nfd::FaceTable& table)
{

  NFD_LOG_INFO("Number of faces = " << table.size());

  faces.clear ();
  faces.push_back (DROP_FACE_ID);

  //fbMap.clear ();

  for(nfd::FaceTable::const_iterator it = table.begin (); it != table.end (); ++it)
  {
    if((*it)->isLocal())
    {
      //NFD_LOG_INFO("FOUND LOCAL FACE["<< (*it)->getId() << "]:" << (*it)->getDescription().c_str());

      //IF WE WANT TO SUPPORT APPS WE MUST NOT USE CONTIUNE...
      //continue;
    }
    /*else
      NFD_LOG_INFO("FOUND REMOTE FACE["<< (*it)->getId() << "]:" << (*it)->getDescription().c_str());*/

    faces.push_back((*it)->getId());
    //fbMap[(*it)->getId()] = boost::shared_ptr<FaceLimitManager>(new FaceLimitManager(*it));
  }

  std::sort(faces.begin(), faces.end());
}

int SAFEngine::determineNextHop(const Interest& interest, std::vector<int> alreadyTriedFaces, shared_ptr<fib::Entry> fibEntry)
{
  //check if content prefix has been seen
  std::string prefix = extractContentPrefix(interest.getName());

  if(entryMap.find(prefix) == entryMap.end ())
  {
    entryMap[prefix] = boost::shared_ptr<SAFEntry>(new SAFEntry(faces, fibEntry));

    // add buckets for all faces
    /*for(FaceLimitMap::iterator it = fbMap.begin (); it != fbMap.end (); it++)
    {
      it->second->addNewPrefix(prefix);
    }*/
  }

  boost::shared_ptr<SAFEntry> entry = entryMap.find(prefix)->second;
  return entry->determineNextHop(interest, alreadyTriedFaces);
}

bool SAFEngine::tryForwardInterest(const Interest& interest, shared_ptr<Face> outFace)
{
  //No limiter on real simulations
  return true;
  /*
  if( dynamic_cast<ns3::ndn::NetDeviceFace*>(&(*outFace)) == NULL) //check if its a NetDevice
  {
    return true;
  }

  std::string prefix = extractContentPrefix(interest.getName());
  SAFEntryMap::iterator it = entryMap.find (prefix);
  if(it == entryMap.end ())
  {
    fprintf(stderr,"Error in SAFEntryLookUp\n");
    return false;
  }
  else
  {
    return fbMap[outFace->getId ()]->tryForwardInterest(prefix);
  }*/
}

void SAFEngine::update ()
{
  NFD_LOG_DEBUG("\nFWT UPDATE triggered:");
  for(SAFEntryMap::iterator it = entryMap.begin (); it != entryMap.end (); ++it)
  {
    NFD_LOG_DEBUG("Updating Prefix " << it->first);
    it->second->update();
  }

  time::nanoseconds delay = time::milliseconds((int)ceil(ParameterConfiguration::getInstance ()->getParameter ("UPDATE_INTERVALL")));
  updateEventFWT = nfd::scheduler::schedule(delay, bind(&SAFEngine::update, this));
}

void SAFEngine::logSatisfiedInterest(shared_ptr<pit::Entry> pitEntry,const Face& inFace, const Data& data)
{
  std::string prefix = extractContentPrefix(pitEntry->getName());
  SAFEntryMap::iterator it = entryMap.find (prefix);
  if(it == entryMap.end ())
    fprintf(stderr,"Error in SAFEntryLookUp\n");
  else
    it->second->logSatisfiedInterest(pitEntry,inFace,data);
}

void SAFEngine::logExpiredInterest(shared_ptr< pit::Entry > pitEntry)
{
  std::string prefix = extractContentPrefix(pitEntry->getName());
  SAFEntryMap::iterator it = entryMap.find (prefix);
  if(it == entryMap.end ())
    fprintf(stderr,"Error in SAFEntryLookUp\n");
  else
    it->second->logExpiredInterest(pitEntry);
}

void SAFEngine::logNack(const Face& inFace, const Interest& interest)
{
  //log the nack
  std::string prefix = extractContentPrefix(interest.getName());
  SAFEntryMap::iterator it = entryMap.find (prefix);
  if(it == entryMap.end ())
    fprintf(stderr,"Error in SAFEntryLookUp\n");
  else
    it->second->logNack(inFace, interest);

  // No limiter in real simulations
 /* //return the token?
  SAFEntryMap::iterator i = entryMap.find (prefix);
  if(i == entryMap.end ())
    fprintf(stderr,"Error in SAFEntryLookUp\n");
  else
    return fbMap[inFace.getId ()]->receivedNack(prefix);*/
}

void SAFEngine::logRejectedInterest(shared_ptr<pit::Entry> pitEntry, int face_id)
{
  std::string prefix = extractContentPrefix(pitEntry->getName());
  SAFEntryMap::iterator it = entryMap.find (prefix);
  if(it == entryMap.end ())
    fprintf(stderr,"Error in SAFEntryLookUp\n");
  else
    it->second->logRejectedInterest(pitEntry, face_id);
}

std::string SAFEngine::extractContentPrefix(nfd::Name name)
{
  //fprintf(stderr, "extracting from %s\n", name.toUri ().c_str ());

  std::string prefix = "";
  for(unsigned int i=0; i <= prefixComponentNumber; i++)
  {
    prefix.append ("/");
    prefix.append (name.get (i).toUri ());
  }
  return prefix;
}

void SAFEngine::addFace(shared_ptr<Face> face)
{
  /*if(face->isLocal())
  {
    //NFD_LOG_INFO("FOUND LOCAL FACE:" << (*it)->getDescription().c_str());
    return; // local faces are not considered by SAF
  }*/

  NFD_LOG_INFO("ADDING REMOTE FACE:" << face->getId());
  faces.push_back(face->getId());
  //fbMap[(*it)->getId()] = boost::shared_ptr<FaceLimitManager>(new FaceLimitManager(*it));
  std::sort(faces.begin(), faces.end());

  for(SAFEntryMap::iterator it = entryMap.begin (); it != entryMap.end (); ++it)
  {
    it->second->addFace(face);
  }
}

void SAFEngine::removeFace(shared_ptr<Face> face)
{
  /*if(face->isLocal())
  {
    //NFD_LOG_INFO("FOUND LOCAL FACE:" << (*it)->getDescription().c_str());
    return; // local faces are not considered by SAF
  }*/

  NFD_LOG_INFO("REMOVING REMOTE FACE:" << face->getId());

  for(SAFEntryMap::iterator it = entryMap.begin (); it != entryMap.end (); ++it)
  {
    it->second->removeFace(face);
  }

  faces.erase(std::find(faces.begin (), faces.end (), face->getId()));
  //fbMap[(*it)->getId()] = boost::shared_ptr<FaceLimitManager>(new FaceLimitManager(*it));
  std::sort(faces.begin(), faces.end());

}
