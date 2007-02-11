#pragma ident "$Id$"

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//  
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

#include <fstream>
#include <string>

#include <BasicFramework.hpp>
#include <MSCData.hpp>
#include <MSCStream.hpp>
#include <EpochClockModel.hpp>

#include "OrdEngine.hpp"
#include "ObsReader.hpp"
#include "EphReader.hpp"

#include "DDEpoch.hpp"
#include "PhaseCleaner.hpp"
#include "CycleSlipList.hpp"
#include "SvElevationMap.hpp"


using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;


class DDGen : public gpstk::BasicFramework
{
public:
   DDGen() throw();
   
   bool initialize(int argc, char *argv[]) throw();

protected:
   virtual void spinUp();
   virtual void process();

private:
   string ddMode;
   double minArcGap; // seconds
   double minArcTime; // seconds
   unsigned long minArcLen; // epochs
   unsigned long msid;
   Triple antennaPos;

   ObsEpochMap obs1, obs2;

   CommandOptionWithAnyArg obs1FileOption, obs2FileOption, ephFileOption;

   void readObsFile(const CommandOptionWithAnyArg& obsFileOption,
                    const EphemerisStore& eph,
                    ObsEpochMap &oem);
};

//-----------------------------------------------------------------------------
// The constructor basically just sets up all the command line options
//-----------------------------------------------------------------------------
DDGen::DDGen() throw()
   : BasicFramework("ddGen", "Computes double-difference residuals from raw observations."),
     ddMode("all"), minArcGap(60), minArcTime(60), minArcLen(5), msid(0),
     obs1FileOption('1', "obs1", 
                    "Where to get the first receiver's obs data.", true),
   
     obs2FileOption('2', "obs2", 
                    "Where to get the second receiver's obs data.", true),

     ephFileOption('e', "eph",  "Where to get the ephemeris data. Can be "
                   " rinex, fic, or sp3", true)
{}


//-----------------------------------------------------------------------------
// Here the command line options parsed and used to configure the program
//-----------------------------------------------------------------------------
bool DDGen::initialize(int argc, char *argv[]) throw()
{
   CommandOptionWithAnyArg
      ddModeOption('\0', "ddmode", "Specifies what observations are used to "
                   "compute the double difference residuals. Valid values are:"
                   "all. The default is " + ddMode),

      cycleSlipOption('\0', "cycle-slips", "Output a list of cycle slips"),
   
      minArcTimeOption('\0', "min-arc-time", "The minimum length of time "
                       "(in seconds) that a sequence of observations must "
                       "span to be considered as an arc. The default "
                       "value is " + asString(minArcTime, 1) + " seconds."),

      minArcGapOption('\0', "min-arc-gap", "The minimum length of time "
                      "(in seconds) between two arcs for them to be "
                      "considered separate arcs. The default value "
                      "is " + asString(minArcGap, 1) + " seconds."),

      mscFileOption('c', "msc", "Station coordinate file."),

      minArcLenOption('\0', "min-arc-length", "The minimum number of "
                      "epochs that can be considered an arc. The "
                      "default value is " + asString(minArcLen) +
                      " epochs."),
      antennaPosOption('p', "pos", "Location of the antenna in meters ECEF.");

   CommandOptionWithNumberArg 
      msidOption('m', "msid", "Station to process data for. Used to "
                 "select a station position from the msc file or data "
                 "from a SMODF file.");
   
   if (!BasicFramework::initialize(argc,argv)) 
      return false;

   if (msidOption.getCount())
      msid = asUnsigned(msidOption.getValue().front());

   // Get the station position
   if (antennaPosOption.getCount())
   {
      string aps = antennaPosOption.getValue()[0];
      if (numWords(aps) != 3)
      {
         cerr << "Please specify three coordinates in the antenna postion." << endl;
         return false;
      }
      else
         for (int i=0; i<3; i++)
            antennaPos[i] = asDouble(word(aps, i));
   }
   else if (msid && mscFileOption.getCount())
   {
      string mscfn = (mscFileOption.getValue())[0];
      MSCStream msc(mscfn.c_str(), ios::in);
      MSCData mscd;
      while (msc >> mscd && mscd.station != msid)
         ;
      if (mscd.station == msid)
         antennaPos = mscd.coordinates;
   }
   else
   {
      string fn = (obs1FileOption.getValue())[0];
      ObsReader obsReader(fn, verboseLevel);
      if (obsReader.inputType == FFIdentifier::tRinexObs)
         antennaPos = obsReader.roh.antennaPosition;
   }

   if (RSS(antennaPos[0], antennaPos[1], antennaPos[2]) < 1)
   {
      cerr << "Warning! The antenna appears to be within one meter of the" << endl
           << "center of the geoid. This program is not capable of" << endl
           << "accurately estimating the propigation of GNSS signals" << endl
           << "through solids such as a planetary crust or magma. Also," << endl
           << "if this location is correct, your antenna is probally" << endl
           << "no longer in the best of operating condition." << endl;
      return false;
   }

   if (ddModeOption.getCount())
      ddMode = lowerCase(ddModeOption.getValue()[0]);

   if (minArcTimeOption.getCount())
      minArcTime = asDouble(minArcTimeOption.getValue().front());
   
   if (minArcLenOption.getCount())
      minArcLen = asUnsigned(minArcLenOption.getValue().front());

   if (minArcGapOption.getCount())
      minArcGap = asDouble(minArcGapOption.getValue().front());
   return true;
}


//-----------------------------------------------------------------------------
// General program setup
//-----------------------------------------------------------------------------
void DDGen::spinUp()
{
   if (verboseLevel)
   {
      cout << "# Double difference mode: " << ddMode << endl
           << "# Minimum arc time: " << minArcTime << " seconds" << endl
           << "# Minimum arc length: " << minArcLen << " epochs" << endl
           << "# Minimum gap length: " << minArcGap << " seconds" << endl
           << "# Antenna Position: " << setprecision(8) << antennaPos << endl;
      if (msid)
         cout << "# msid: " << msid << endl;
   }
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void DDGen::process()
{
   EphReader ephReader;
   ephReader.verboseLevel = verboseLevel;
   for (int i=0; i<ephFileOption.getCount(); i++)
      ephReader.read(ephFileOption.getValue()[i]);

   ObsEpochMap oem1, oem2;

   if (debugLevel)
      cout << "# Reading obs from Rx1" << endl;
   readObsFile(obs1FileOption, *ephReader.eph, oem1);

   if (verboseLevel)
      cout << "# Reading obs from Rx2" << endl;
   readObsFile(obs2FileOption, *ephReader.eph, oem2);

   SvElevationMap pem;
   pem = elevation_map(oem1, antennaPos, *ephReader.eph);

   // This computes a simple double difference on all observables
   DDEpochMap ddem;
   ddem.debugLevel = debugLevel;
   ddem.compute(oem1, oem2, pem);

   // Here we compute a phase double difference that is Better(TM)
   PhaseCleaner pc(minArcLen, minArcTime, minArcGap);
   pc.debugLevel = debugLevel;
   pc.addData(oem1, oem2);
   pc.debias(pem);
   pc.getPhaseDD(ddem);

   // Now write out all the double differences
   ddem.dump(cout);

//   CycleSlipList sl;
//   pc.getSlips(sl, pem);
//   dump(cout, sl);    

}


//-----------------------------------------------------------------------------
// Read a single file of observation data, computing receiver clock offsets along
// the way.
//-----------------------------------------------------------------------------
void DDGen::readObsFile(
   const CommandOptionWithAnyArg& obsFileOption, 
   const EphemerisStore& eph,
   ObsEpochMap &oem)
{
   // Just a placeholder
   gpstk::WxObsData wod;

   // Use a New Brunswick trop model.
   NBTropModel tm;

   // Now set up the function object that is used to compute the ords.
   OrdEngine ordEngine(eph, wod, antennaPos, "smart", tm);
   ordEngine.verboseLevel = verboseLevel;
   ordEngine.debugLevel = debugLevel;

   // Set up a simple epoch clock model.
   EpochClockModel cm(1.5, 10, ObsClockModel::HEALTHY);
   const GPSGeoid gm;

   // Walk through each obs file, reading and computing ords along the way.
   for (int i=0; i<obsFileOption.getCount(); i++)
   {
      string fn = (obsFileOption.getValue())[i];
      ObsReader obsReader(fn, verboseLevel);
      obsReader.msid = msid;

      while (obsReader())
      {
         ObsEpoch obs(obsReader.getObsEpoch());
         if (!obsReader())
            break;

         ORDEpoch oe = ordEngine(obs);
         cm.addEpoch(oe);

         if (cm.isOffsetValid())
         {
            // Need to keep clock offset in seconds
            obs.rxClock = cm.getOffset() / gm.c();
            oem[obs.time] = obs;
         }
         else
         {
            if (verboseLevel>2)
               cout << "# Could not estimate clock for epoch at " << obs.time << endl;
         }
      }
   }
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   try
   {
      DDGen crap;
      if (!crap.initialize(argc, argv))
         exit(0);
      crap.run();
   }
   catch (gpstk::Exception &exc)
   { cerr << exc << endl; }
   catch (std::exception &exc)
   { cerr << "Caught std::exception " << exc.what() << endl; }
   catch (...)
   { cerr << "Caught unknown exception" << endl; }
}
