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

#ifndef DDEPOCH_HPP
#define DDEPOCH_HPP

#include "DayTime.hpp"
#include "stl_helpers.hpp"
#include "icd_200_constants.hpp"
#include "Stats.hpp"

#include "util.hpp"

struct DDEpoch
{
   DDEpoch() : valid(false){};
   SvOIDM dd;

   SvShortMap health;

   double clockOffset;
   gpstk::SatID masterPrn;
   bool valid;

   // Computes a single difference between two sets of obs
   OIDM singleDifference(
      const gpstk::SvObsEpoch& rx1obs,
      const gpstk::SvObsEpoch& rx2obs);
   
   // Sets the valid flag true if successfull
   // also sets the masterPrn to the one actually used
   void doubleDifference(
      const gpstk::ObsEpoch& rx1,
      const gpstk::ObsEpoch& rx2);

   void selectMasterPrn(
      const gpstk::ObsEpoch& rx1, 
      const gpstk::ObsEpoch& rx2,
      SvElevationMap& pem);
};

typedef std::map<gpstk::DayTime, DDEpoch> DDEpochMap;

void computeDDEpochMap(
   gpstk::ObsEpochMap& rx1,
   gpstk::ObsEpochMap& rx2,
   SvElevationMap& pem,
   const gpstk::EphemerisStore& eph,
   DDEpochMap& ddem);

void dump(std::ostream& s,
          DDEpochMap& ddem,
          SvElevationMap& pem);

void dumpStats(
   DDEpochMap& oem,
   const CycleSlipList& csl,
   SvElevationMap& pem);

std::string computeStats(
   const gpstk::ObsID oid,
   DDEpochMap& oem,
   const ElevationRange er,
   SvElevationMap& pem);

#endif
