#pragma ident "$Id:$"

/**
 * @file ObsArray.cpp
 * Provides ability to operate mathematically on large, logical groups of observations
 * Class definitions.
 */

#include <vector>
#include <set>

#include "ValarrayUtils.hpp"
#include "PRSolution.hpp"

#include "RinexObsStream.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsData.hpp"
#include "FileUtils.hpp"
#include "ObsArray.hpp"

namespace gpstk 
{
   
   ObsArray::ObsArray(void) 
         : numObsTypes(0)
   {
   }

   ObsIndex ObsArray::add(RinexObsHeader::RinexObsType type)
   {
      isBasic[numObsTypes]=true;
      basicTypeMap[numObsTypes]=type;
      return numObsTypes++;
   }
   
   ObsIndex ObsArray::add(const std::string& expression)
   {
      isBasic[numObsTypes]=false;
      expressionMap[numObsTypes] = Expression(expression);
      expressionMap[numObsTypes].setGPSConstants();
      return numObsTypes++;
   }

   void ObsArray::load(const std::string& obsfilename, 
                       const std::string& navfilename)
   {
         // First check for existance of input files
      if (!FileUtils::fileAccessCheck(obsfilename))
      {
         ObsArrayException oae("Cannot read obs file " + obsfilename);
         GPSTK_THROW(oae);
      }

      if (!FileUtils::fileAccessCheck(navfilename))
      {
         ObsArrayException oae("Cannot read nav file " + navfilename);
         GPSTK_THROW(oae);
      }

         // Load the ephemeris information from the named NAV file.
      ephStore.loadFile(navfilename);

         // Load the obs file header
      RinexObsStream robs(obsfilename.c_str());
      RinexObsStream robsAgain(obsfilename.c_str());
      RinexObsHeader roh;
      
      robs >> roh;
      
         // Verify we have a suggested approximate location. If not, note that.
      bool staticPositionDefined=false;
      Triple antennaPos;
      
      if ( (roh.valid & RinexObsHeader::antennaPositionValid) == 
           RinexObsHeader::antennaPositionValid)
      {
         antennaPos = roh.antennaPosition;
         staticPositionDefined=true;
      }
      
         // Remember the data collection rate. If not available, note that.
      bool intervalDefined=false;
      double interval;
      
      if ( (roh.valid & RinexObsHeader::intervalValid) == 
           RinexObsHeader::intervalValid)
      {
         interval = roh.interval;
         intervalDefined=true;
      }

      RinexObsData rod;
      RinexObsData::RinexPrnMap::const_iterator it;
      
         // Read through file the first time.
         // In this pass, get the "size" of the data
         // Calculate if needed an approximate user positoin,
         // and data collection interval.

      numSatEpochs = 0;

      bool firstEpochCompleted = false;
      DayTime lastEpochValue;
      std::set<double> intervalDifferences;
      
      while ( robs >> rod )
      { 
            // Account for total amount of obs data in this file
         numSatEpochs += rod.obs.size();

            // Record the interval differences
         if (!intervalDefined)
         {
            if (!firstEpochCompleted)
            {
               lastEpochValue = rod.time;
               firstEpochCompleted = true;
            }
            else
            {
               intervalDifferences.insert(ceil(rod.time - lastEpochValue));
               lastEpochValue = rod.time;
            }
         }
         

            // If necessary, determine the initial user position
         if (!staticPositionDefined)
         {
            PRSolution prEst;
            ZeroTropModel nullTropModel;
            
            std::vector<RinexPrn> sats;
            std::vector<double> ranges;
            RinexObsData::RinexPrnMap::const_iterator it;

            for (it = rod.obs.begin(); it!= rod.obs.end(); it++)
            {
               RinexObsData::RinexObsTypeMap otmap;
               RinexObsData::RinexObsTypeMap::const_iterator itPR;
               otmap = it->second;
               itPR = otmap.find(RinexObsHeader::P1);
               if (itPR == otmap.end())
                 itPR = otmap.find(RinexObsHeader::C1);

               if (itPR!=otmap.end())
               {
                  sats.push_back(it->first);
                  ranges.push_back(itPR->second.data);
               }
            }
            
            prEst.RMSLimit = 10000;
            prEst.RAIMCompute(rod.time, sats, ranges, ephStore, 
                              &nullTropModel);
            
            if (prEst.isValid())
            {
               antennaPos[0] = prEst.Solution[0];
               antennaPos[1] = prEst.Solution[1];
               antennaPos[2] = prEst.Solution[2];
               staticPositionDefined = true;
            }
         } // End first blush estimate of static or initial position

      } // Finish first run through file

      if (!intervalDefined)
      {
         
         using namespace ValarrayUtils;   
         std::cout << "intervals were: " << intervalDifferences << std::endl;
         std::set<double>::iterator itEpochDiff = intervalDifferences.begin();
         interval = *itEpochDiff;
         intervalDefined = true;
      }
      
      if (!intervalDefined)
      {
         ObsArrayException oae("Cannot determine data interval for " + obsfilename);
         GPSTK_THROW(oae);
      }

      int i=0;

         // Size the storage valarrays. 
      observation.resize(numSatEpochs*numObsTypes);
      epoch.resize(numSatEpochs);
      satellite.resize(numSatEpochs);
      epoch.resize(numSatEpochs);
      azimuth.resize(numSatEpochs);
      elevation.resize(numSatEpochs);
      validAzEl.resize(numSatEpochs);
      pass.resize(numSatEpochs);
      pass = -1;
      
      validAzEl = true;
      size_t satEpochIdx=0;
         
      std::map<RinexPrn, DayTime> lastObsTime;
      std::map<RinexPrn, DayTime>::const_iterator it2;
      std::map<RinexPrn, long> currPass;
     
      long highestPass = 0;
      long thisPassNo;
      
         // Second time through, fill in observations and pass numbers      
         // First step through each epoch of observation
      while (robsAgain >> rod)
      {
            // Second step through the obs for each SV 
         
         for (it = rod.obs.begin(); it!=rod.obs.end(); it++)
         {
            it2 = lastObsTime.find((*it).first);
               //std::cout << (*it).first << std::endl;
 
             if (  (it2==lastObsTime.end()) || 
                   ( (rod.time-lastObsTime[(*it).first]) > 1.1*interval) )
            {
               thisPassNo = highestPass;
               lastObsTime[(*it).first]=rod.time;
               currPass[(*it).first]=highestPass++;
            }
            else
            { 
               thisPassNo = currPass[(*it).first];
               lastObsTime[(*it).first]=rod.time;
            }
            pass[satEpochIdx]=thisPassNo;

            for (int idx=0; idx<numObsTypes; idx++)
            {
               if (isBasic[idx])
               {
                  observation[i] = rod.obs[it->first][basicTypeMap[idx]].data;
               }
               else 
               {
                  expressionMap[idx].setRinexObs(rod.obs[it->first]);
                  observation[i] = expressionMap[idx].evaluate();
               }

               satellite[satEpochIdx]  = it->first;               
               i++;
            } // end of walk through observations to record
 
            // Get topocentric coords for given sat
            try
            {
               Xvt svPos = ephStore.getPrnXvt(it->first.prn, rod.time);
               elevation[satEpochIdx]= antennaPos.elvAngle(svPos.x);
               azimuth[satEpochIdx]  = antennaPos.azAngle( svPos.x);
            }
            catch(EphemerisStore::NoEphemerisFound)
            {
               validAzEl[satEpochIdx]=false;
            }
               //std::cout << "i: (" << satEpochIdx << ")" << rod.time << std::endl;
            

            epoch[satEpochIdx]=rod.time;

            satEpochIdx++;
         } // end of walk through prns at this epoch
      } // end of walk through obs file



   } // end of ObsArray::load(...)
   
} // end namespace gpstk
 

