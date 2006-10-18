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

#ifndef PHASECLEANER_HPP
#define PHASECLEANER_HPP

#include <set>

#include "DDEpoch.hpp"
#include "PhaseResidual.hpp"

class PhaseCleaner
{
public:
   PhaseCleaner(long al, double at, double gt);

   void addData(
      const gpstk::ObsEpochMap& rx1, 
      const gpstk::ObsEpochMap& rx2);

   void debias(SvElevationMap& pem);

   void selectMasters(
      const gpstk::ObsID& oid, 
      const gpstk::SatID& prn,
      SvElevationMap& pem);

   void doubleDifference(
      const gpstk::ObsID& oid, 
      const gpstk::SatID& prn,
      SvElevationMap& pem);

   void getSlips(
      CycleSlipList& csl,
      SvElevationMap& pem) const;

   void getPhaseDD(DDEpochMap& ddem) const;

   void dump(std::ostream& s) const;

   typedef std::set<gpstk::ObsID> ObsIDSet;
   ObsIDSet phaseObsTypes;

   mutable OIDM lamda;

   // And an set of arcs for each PRN
   typedef std::map<gpstk::SatID, PhaseResidual::ArcList> PraPrn;

   // And a set of those for each obs type
   typedef std::map<gpstk::ObsID, PraPrn> PraPrnOt;

   // Rx1 - Rx2 clock, in meters.
   TimeDoubleMap clockOffset;
   
   // SV line-of-sight motion, in meters/second
   typedef std::map<gpstk::SatID, TimeDoubleMap> PrnTimeDoubleMap;
   PrnTimeDoubleMap rangeRate;

   PraPrnOt pot;
   
   long minArcLen;
   double minArcTime, maxGapTime;

   typedef std::map<gpstk::DayTime, gpstk::SatID> TimePrnMap;

   class goodMaster
   {
   public:
      goodMaster(double v, const gpstk::SatID& p, const gpstk::DayTime& t, PrnTimeDoubleMap& rr)
         : minVal(v), prn(p), time(t), rangeRate(rr){};

      const double minVal; // Above this elevation
      const gpstk::SatID& prn;  // Not this prn
      const gpstk::DayTime& time;  // time to evaluate range rate
      PrnTimeDoubleMap& rangeRate;

      double bestElev;
      gpstk::SatID bestPrn;
      bool operator()(const SvDoubleMap::value_type& pdm);
   };
};
#endif
