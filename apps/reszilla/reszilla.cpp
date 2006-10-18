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

//
// Compute observed range deviations (ORDs) and double
// difference residuals from rinex obs files.
//

#include <fstream>
#include <BCEphemerisStore.hpp>
#include <CommandOptionWithTimeArg.hpp>

#include "readers.hpp"
#include "util.hpp"
#include "ordUtils.hpp"
#include "PhaseCleaner.hpp"
#include "RobustLinearEstimator.hpp"

using namespace std;

int verbosity;
string timeFormat;
ElevationRangeList elr;

using namespace gpstk::StringUtils;

char* verboseHelp =
"\n"
"Verbosity values:\n"
"  0: nothing but the results\n"
"  1: Output status before potentially time consuming operations (default)\n"
"  2: more details about each step and the options chosen\n"
"  3: add the reasons for editing data\n"
"  4: dump intermediate values for each epoch (can be QUITE verbose)\n"
"\n"
"Types in the raw output files:\n"
"   0 - c1p2 observed range deviation\n"
"   50 - computed clock, difference from estimate, strip\n"
"   51 - linear clock estimate, abdev \n"
"Double difference types:\n"
"   10 - c1     20 - c2\n"
"   11 - p1     21 - p2\n"
"   12 - l1     22 - l2\n"
"   13 - d1     23 - d2\n"
"   14 - s1     24 - s2 (Why? Because I can!)\n"
"\n"
"Misc notes:\n"
"\n"
"The criteria min-arc-time and min-arc-length are both required to be met\n"
"for a arc to be valid in double difference mode.\n"
"\n"
"Example command to compute ORDs on an ICD-GPS-211 formated smoothed\n"
"measurement data file:\n"
"   reszilla --omode=p1 --svtime --msc=mscoords.cfg -m 85401\n"
"      -o asm2004.138 -e s011138a.04n\n"
"\n"
"All output quantities (stddev, min, max, ord, clock, double differnce, ...)\n"
"are in meters.\n"
;

int main(int argc, char *argv[])
{
   string hmsFmt="%Y %3j %02H:%02M:%04.1f";
   string hmsFmt2="%Y %3j %02H:%02M:%02S";
   string sodFmt="%Y %3j %7.1s";
   string ordMode="p1p2";
   string ddMode="sv";
   unsigned long msid=0;
   double sigmaMask=6;

   double minArcGap = 60; // seconds
   double minArcTime = 60; // seconds
   long minArcLen=5; // epochs

   bool rawOutput=false;
   bool keepUnhealthy=false;
   bool keepWarts=false;


   gpstk::DayTime startTime(gpstk::DayTime::BEGINNING_OF_TIME);
   gpstk::DayTime stopTime(gpstk::DayTime::END_OF_TIME);

   timeFormat = hmsFmt;
   verbosity = 1;

   try
   {
      gpstk::CommandOptionWithAnyArg
         obs1FileOption('o', "obs1", "Observation data file name. If this "
                        "option is specified more than once the contents of "
                        "all files will be used.", true);

      gpstk::CommandOptionWithAnyArg
         obs2FileOption('2', "obs2", " Second receiver's observation data "
                        "file name. Only used when computing a double "
                        "difference. If this option is specified more than "
                        "once the contents of all the files will be used.");

      gpstk::CommandOptionWithAnyArg
         mscFileOption('\0', "msc", "Station coordinate file");

      gpstk::CommandOptionWithNumberArg
         msidOption('m', "msid", "Station to process data for. Used to select "
                    "a station from smoothed data files.");
         
      gpstk::CommandOptionWithAnyArg
         ephFileOption('e', "ephemeris", "Ephemeris data file name (either "
                       "broadcast in RINEX nav, broadcast in FIC, or precise "
                       "in SP3).", false);

      gpstk::CommandOptionWithAnyArg
         metFileOption('w', "weather", "Weather data file name (RINEX met "
                       "format only).");

      gpstk::CommandOptionNoArg
         nearOption('n', "search-near", "Use BCEphemeris.searchNear()");
      
      gpstk::CommandOptionNoArg
         svTimeOption('\0', "svtime", "Observation data is in SV time frame. "
                      "The default is RX time frame.");

      gpstk::CommandOptionNoArg
         keepWartsOption('\0', "keep-warts", "Keep any warts that are in "
                         "the data. The defailt is to remove them.");

      gpstk::CommandOptionNoArg
         checkObsOption('\0',"check-obs", "Report data rate, order of data, "
                        "data present, data gaps");

      gpstk::CommandOptionNoArg
         keepUnhealthyOption('\0',"keep-unhealthy", "Use unhealthy SVs in the "
                             "clock computition and statistics, the default "
                             "is to toss.");

      gpstk::CommandOptionNoArg
         statsOption('s', "no-stats", "Don't compute & output the statistics");

      gpstk::CommandOptionNoArg
         cycleSlipOption('\0', "cycle-slips", "Output a list of cycle slips");
   
      gpstk::CommandOptionWithAnyArg
         rawOutputOption('r', "raw-output", "Dump the computed residuals/ords "
                         "into specified file. If '-' is given as the file "
                         "name, the output is sent to standard output. The "
                         "default is to not otput the raw residuals.");

      gpstk::CommandOptionWithTimeArg
         startTimeOption('\0', "start-time", "%4Y/%03j/%02H:%02M:%05.2f",
                         "Ignore obs data prior to this time in the "
                         "analysis. The time is specified using the format "
                         "%4Y/%03j/%02H:%02M:%05.2f. The default value is to "
                         "start with the first data found.");
      
      gpstk::CommandOptionWithTimeArg
         stopTimeOption('\0', "stop-time",  "%4Y/%03j/%02H:%02M:%05.2f",
                        "Ignore obs data after to this time in the "
                        "analysis. The time is specified using the format "
                        "%4Y/%03j/%02H:%02M:%05.2f. The default value is to "
                        "process all data.");
      
      gpstk::CommandOptionWithAnyArg
         timeFmtOption('t', "time-format", "Daytime format specifier used for "
                       "the timestamps in the raw output. The default is \"" 
                       + timeFormat + "\". If this option is specified with "
                       "the format as \"s\", the format \"" + sodFmt + "\" "
                       "is used. If this option is specified with the format "
                       "as \"s\", the format \"" + hmsFmt2 + "\" is used.");

      gpstk::CommandOptionWithAnyArg
         ordModeOption('\0', "omode", "ORD mode: p1p2, c1p2, c1, p1, c2, p2, smo. "
                       "Note that the smo mode often requires the --svtime "
                       "option to be specified. The default is " + ordMode);

      gpstk::CommandOptionNoArg
         clkAnalOption('\0', "clock-est", "Compute a linear clock estimate");

      gpstk::CommandOptionWithAnyArg
         ddModeOption('\0', "ddmode", "Double difference residual mode: none, "
                      "sv, or c1p2. The default is " + ddMode + ".");

      gpstk::CommandOptionWithAnyArg
         minArcTimeOption('\0', "min-arc-time", "The minimum length of time "
                          "(in seconds) that a sequence of observations must "
                          "span to be considered as an arc. The default "
                          "value is " + asString(minArcTime, 1) + " seconds.");

      gpstk::CommandOptionWithAnyArg
         minArcGapOption('\0', "min-arc-gap", "The minimum length of time "
                         "(in seconds) between two arcs for them to be "
                         "considered separate arcs. The default value "
                         "is " + asString(minArcGap, 1) + " seconds.");

      gpstk::CommandOptionWithNumberArg
         minArcLenOption('\0', "min-arc-length", "The minimum number of "
                         "epochs that can be considered an arc. The "
                         "default value is " + asString(minArcLen) +
                         " epochs.");

      gpstk::CommandOptionWithAnyArg
         elevBinsOption('b', "elev-bin", "A range of elevations, used in "
                        "computing the statistical summaries. Repeat to "
                        "specify multiple bins. The default is \"-b 0-10 "
                        "-b 10-20 -b 20-60 -b 10-90\".");

      gpstk::CommandOptionWithAnyArg
         sigmaOption('\0', "sigma", "Multiplier for sigma stripping used "
                     "in computation of the the statistics "
                     "on the raw residuals. The default value is "
                     + asString((int)sigmaMask) + ".");

      gpstk::CommandOptionNoArg
         helpOption('h', "help", "Print usage. Repeat for more info. ");

      gpstk::CommandOptionWithNumberArg
         verbosityOption('v', "verbosity", "How much detail to provide "
                         "about intermediate steps. The default is 1. "
                         "Specify -hh for more help.");

      string appDesc("Computes various residuals from GPS observations.");
      gpstk::CommandOptionParser cop(appDesc);
      cop.parseOptions(argc, argv);

      if (helpOption.getCount() || cop.hasErrors())
      {
         if (cop.hasErrors() && helpOption.getCount()==0)
         {
            cop.dumpErrors(cout);
            cout << "use -h for help, -hh for more help." << endl;
         }
         else
         {
            cop.displayUsage(cout);
            if (helpOption.getCount() > 1)
               cout << verboseHelp << endl;
         }
         exit(0);
      }

      if (verbosityOption.getCount())
         verbosity = asInt(verbosityOption.getValue()[0]);

      if (ddModeOption.getCount())
         ddMode = lowerCase(ddModeOption.getValue().front());
      
      if (ordModeOption.getCount())
         ordMode = lowerCase(ordModeOption.getValue()[0]);
      
      bool svTime = svTimeOption.getCount()>0;

      if (msidOption.getCount())
         msid = asUnsigned(msidOption.getValue().front());

      if (sigmaOption.getCount())
         sigmaMask = asDouble(sigmaOption.getValue().front());

      if (minArcTimeOption.getCount())
         minArcTime = asDouble(minArcTimeOption.getValue().front());

      if (minArcLenOption.getCount())
         minArcLen = asUnsigned(minArcLenOption.getValue().front());

      if (minArcGapOption.getCount())
         minArcGap = asDouble(minArcGapOption.getValue().front());

      if (keepUnhealthyOption.getCount())
         keepUnhealthy=true;
      
      if (keepWartsOption.getCount())
         keepWarts=true;

      if (timeFmtOption.getCount())
      {
         if ((timeFmtOption.getValue())[0] == "s")
            timeFormat = sodFmt;
         else
            timeFormat = (timeFmtOption.getValue())[0];
      }

      if (startTimeOption.getCount())
         startTime = startTimeOption.getTime()[0];
      if (stopTimeOption.getCount())
         stopTime = stopTimeOption.getTime()[0];

      // set up where the raw data will be written
      ofstream ofs;
      string outputFileName;
      if (rawOutputOption.getCount())
      {
         rawOutput=true;

         outputFileName=(rawOutputOption.getValue())[0];
         if (outputFileName != "-")
            ofs.open(outputFileName.c_str());
         else
         {
            ofs.copyfmt(std::cout);
            ofs.clear(std::cout.rdstate());
            ofs.std::basic_ios<char>::rdbuf(std::cout.rdbuf());
         }
      }

      if (ddMode=="none" && obs2FileOption.getCount())
      {
         cout << "Specifying two sets of obs data requires a ddmode other than 'none'." << endl;
         exit(1);
      }

      // Set up the elevation ranges for the various statistical summaries
      if (elevBinsOption.getCount())
      {
         for (int i=0; i<elevBinsOption.getCount(); i++)
         {
            string pr = elevBinsOption.getValue()[i];
            float minElev = asFloat(pr);
            stripFirstWord(pr, '-');
            float maxElev = asFloat(pr);
            elr.push_back( ElevationRange(minElev, maxElev) );
         }
      }
      else
      {
         elr.push_back( ElevationRange( 0, 10) );
         elr.push_back( ElevationRange(10, 20) );
         elr.push_back( ElevationRange(20, 60) );
         elr.push_back( ElevationRange(60, 90) );
         elr.push_back( ElevationRange(10, 90) );
      }

      if (verbosity)
      {
         cout << "--------------------------------------------------------------" << endl
              << "Observed Rage Deviation (ORD) mode: " << ordMode << endl
              << "Format to use for time in raw output: " << timeFormat << endl
              << "Data time tag: " << (svTime?"sv":"rx") << endl
              << "Sigma stripping multiplier: " << sigmaMask << endl
              << "Verbosity: " << verbosity << endl
              << "Elevation bins: ";
         
         for (ElevationRangeList::const_iterator i=elr.begin(); i!= elr.end(); i++)
            cout << i->first << "-" << i->second << " ";
         cout << endl;

         if (msid)
         {
            cout << "msid: " << msid << endl;
            if (mscFileOption.getCount()>0)
               cout << "msc file: " << mscFileOption.getValue().front() << endl;
         }
         
         if (startTime != gpstk::DayTime(gpstk::DayTime::BEGINNING_OF_TIME) ||
             stopTime != gpstk::DayTime(gpstk::DayTime::END_OF_TIME))
            cout << "Start time: " << startTime.printf(timeFormat) << endl
                 << "Stop time: " << stopTime.printf(timeFormat) << endl;

         if (rawOutput)
            cout << "Raw output file: " << outputFileName << endl;
         
         if (keepUnhealthy)
            cout << "Keeping unhealthy SVs in statistics." << endl;
         else
            cout << "Ignoring unhealthy SVs in statistics." << endl;
            

         if (obs2FileOption.getCount())
            cout << "Double difference mode: " << ddMode << endl
                 << "Minimum arc time: " << minArcTime << " seconds" << endl
                 << "Minimum arc length: " << minArcLen << " epochs" << endl
                 << "Minimum gap length: " << minArcGap << " seconds" << endl;

         cout << "--------------------------------------------------------------" << endl;
      }

      // -------------------------------------------------------------------
      // End of processing/checking command line arguments, now on to the
      // data processing portion. First we get all the data into memory.
      // -------------------------------------------------------------------

      // Get the ephemeris data
      gpstk::EphemerisStore& eph = *read_eph_data(ephFileOption);
      if (nearOption.getCount())
      {
         gpstk::BCEphemerisStore& bce = dynamic_cast<gpstk::BCEphemerisStore&>(eph);
         bce.SearchNear();
      }

      // The weather data...
      gpstk::WxObsData& wod = *read_met_data(metFileOption);

      // The obs data...
      gpstk::ObsEpochMap obs1;
      gpstk::Triple ap1;
      if (verbosity)
         cout << "Reading obs1 data." << endl;
      read_obs_data(obs1FileOption, msid, obs1, ap1, startTime, stopTime);
      if (checkObsOption.getCount())
         check_data(ap1, obs1);

      // If a msid & msc file is specified, then get & use the msc file
      // to overwrite the position in the rinex obs header
      if (msid && mscFileOption.getCount()>0)
      {
         string mscfn = (mscFileOption.getValue())[0];
         read_msc_data(mscfn, msid, ap1);
      }

      // If we are given a second set of obs data, don't compute separate ords
      // since this is a double-difference run
      if (ordMode!="none" && !obs2FileOption.getCount())
      {
         // Compute the ords
         gpstk::ORDEpochMap ord1;
         computeOrds(ord1, obs1, ap1, eph, wod, svTimeOption, 
                     keepUnhealthy, keepWarts, ordMode);

         // Now, output statistics to stdout
         if (statsOption.getCount()==0)
            dumpStats(ord1, ordMode, sigmaMask);

         // Save the raw ORDs to a file
         if (rawOutput)
            dumpOrds(ofs, ord1);

         RobustLinearEstimator rle;
         if (clkAnalOption.getCount())
            estimateClock(ord1, rle);

         if (rawOutput)
            dumpClock(ofs, ord1, rle);
      }

      // Now compute double difference residuals on the obs2 data
      if (obs2FileOption.getCount())
      {
         SvElevationMap pem = elevation_map(obs1, ap1, eph);

         gpstk::ObsEpochMap obs2;
         obs2.clear();
         gpstk::Triple ap2;
         if (verbosity>1)
            cout << "Reading obs data from receiver 2." << endl;
         read_obs_data(obs2FileOption, msid, obs2, ap2, startTime, stopTime);
         if (checkObsOption.getCount())
            check_data(ap2, obs2);

         // copy the position from the other file if it was taken from the msc
         if (msid && mscFileOption.getCount()>0)
            ap2 = ap1;

         if (ddMode == "sv")
         {
            // we need the rx clock offset for this double differece so
            // compute the ords just to get a clock offset. Scope these so
            // the ORDEpochMap objects get removed after they are computed.
            {
               gpstk::ORDEpochMap ord1,ord2;
               computeOrds(ord1, obs1, ap1, eph, wod, svTimeOption, keepUnhealthy, keepWarts, ordMode);
               computeOrds(ord2, obs2, ap2, eph, wod, svTimeOption, keepUnhealthy, keepWarts, ordMode);
               add_clock_to_obs(obs1, ord1);
               add_clock_to_obs(obs2, ord2);
            }

            DDEpochMap ddem;

            computeDDEpochMap(obs1, obs2, pem, eph, ddem);

            PhaseCleaner pc(minArcLen, minArcTime, minArcGap);

            pc.addData(obs1, obs2);
            pc.debias(pem);
            pc.getPhaseDD(ddem);

            CycleSlipList sl;
            pc.getSlips(sl, pem);
            
            if (statsOption.getCount()==0)
               dumpStats(ddem, sl, pem);

            if (cycleSlipOption.getCount())
               dump(cout, sl);

            if (rawOutput)
               dump(ofs, ddem, pem);   
         }

         else if (ddMode=="c1p2")
         {
            DD2EpochMap ddem;
            computeDD2(obs1, obs2, ddem);
            if (statsOption.getCount()==0)
               dumpStats(ddem, pem);

            if (rawOutput)
               dump(ofs, ddem, pem);            
         }
         else
            cout << "Unknow ddMode:" << ddMode << endl;
      } // end for()
   }
   catch (gpstk::Exception& e)
   {
      cerr << "Caught Excption: " << typeid(e).name() << endl
           << "Terminating." << endl;
      exit(0);
   }
   exit(0);
}
