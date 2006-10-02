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

#ifndef DATAREADER_HPP
#define DATAREADER_HPP

/** @file This is a class that reads in GPS data obs or nav data without the
    caller needing to know the format the data is suppllied in. The
    observation data formats that are are supported: rinex, smoothmdf, mdp. The 
    navigation data formats that are supported: rinex nav, fic, sp3, mdp
**/

#include <string>
#include <vector>

#include "CommandOption.hpp"
#include "EphemerisStore.hpp"
#include "DayTime.hpp"
#include "ObsEpochMap.hpp"

namespace gpstk
{
   class DataReader
   {
   public:
      DataReader();

      int verbosity;
      std::string timeFormat;

      void read(CommandOption& files);
      double estimate_data_rate(const std::string& fn);
      void read_msc_file(const std::string& fn);

      unsigned long msid;
      DayTime startTime;
      DayTime stopTime;

      std::vector<std::string> filesRead;

      ObsEpochMap oem;
      gpstk::Triple antennaPosition;
      EphemerisStore* eph;

      bool haveEphData;
      bool haveObsData;
      bool havePosData;

   private:
      void read_rinex_obs_data(const std::string& fn);
      void read_smo_data(const std::string& fn);
      void read_mdp_data(const std::string& fn);
      void read_rinex_nav_data(const std::string& fn);
      void read_fic_data(const std::string& fn);
      void read_sp3_data(const std::string& fn);
   };
}
#endif
