#include "parameterconfiguration.hpp"

/**SAF SETTINGS**/
//default parameters can be overriden:
#define P_LAMBDA 0.35 // rate to adapt reliability threshold
#define P_UPDATE_INTERVALL 1000.0 // duration of a period in milliseconds
#define P_MAX_LAYERS 1 //number of layers per content
#define P_DROP_FACE_ID -1 // the id of the virtual dropping face
#define P_RELIABILITY_THRESHOLD_MIN 0.75 // the minimum required reliablity
#define P_RELIABILITY_THRESHOLD_MAX 0.999 // the maximum enforced reliability
#define P_HISTORY_SIZE 6 // sample size of the windows for the statisticmeasure class.
#define P_SAFPrefixComponents 1

/**STORAGE SETTINGS**/
#define P_MaxMMStorageEntries 61035 // 250MB by 4096 byte large packets
//#define P_MaxDiskStorageEntries 262144 // 1 GB
//#define P_MaxCsEntries 393216 //1.5 GB in total
#define P_MaxDiskStorageEntries 0 // 0 GB
#define P_MaxCsEntries 61035 //ca 250 MB in total by 4096 byte large packets

//#define P_MaxMMStorageEntries 5
//#define P_MaxDiskStorageEntries 10
//#define P_MaxCsEntries 15

ParameterConfiguration* ParameterConfiguration::instance = NULL;

ParameterConfiguration::ParameterConfiguration()
{
  setParameter ("LAMBDA", P_LAMBDA);
  setParameter ("UPDATE_INTERVALL", P_UPDATE_INTERVALL);
  setParameter ("MAX_LAYERS", P_MAX_LAYERS);
  setParameter ("DROP_FACE_ID", P_DROP_FACE_ID);
  setParameter ("RELIABILITY_THRESHOLD_MIN", P_RELIABILITY_THRESHOLD_MIN);
  setParameter ("RELIABILITY_THRESHOLD_MAX", P_RELIABILITY_THRESHOLD_MAX);
  setParameter ("HISTORY_SIZE",P_HISTORY_SIZE);
  setParameter ("MaxCsEntries", P_MaxCsEntries);
  setParameter ("MaxMMStorageEntries", P_MaxMMStorageEntries);
  setParameter ("MaxDiskStorageEntries", P_MaxDiskStorageEntries);
  setParameter ("SAFPrefixComponents", P_SAFPrefixComponents);
}


void ParameterConfiguration::setParameter(std::string para_name, double value)
{
  pmap[para_name] = value;
}

double ParameterConfiguration::getParameter(std::string para_name)
{
  return pmap[para_name];
}

ParameterConfiguration *ParameterConfiguration::getInstance()
{
  if(instance == NULL)
    instance = new ParameterConfiguration();

  return instance;
}
