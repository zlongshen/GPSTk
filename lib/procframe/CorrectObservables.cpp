#pragma ident "$Id$"

/**
 * @file CorrectObservables.cpp
 * This class corrects observables from effects such as antenna excentricity,
 * difference in phase centers, offsets due to tide effects, etc.
 */

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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008
//
//============================================================================


#include "CorrectObservables.hpp"


namespace gpstk
{

      // Index initially assigned to this class
   int CorrectObservables::classIndex = 4300000;


      // Returns an index identifying this object.
   int CorrectObservables::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string CorrectObservables::getClassName() const
   { return "CorrectObservables"; }



      /* Returns a satTypeValueMap object, adding the new data generated
       * when calling this object.
       *
       * @param time      Epoch corresponding to the data.
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& CorrectObservables::Process(const DayTime& time,
                                                satTypeValueMap& gData)
      throw(ProcessingException)
   {

      try
      {

            // Compute station latitude and longitude
         double lat(nominalPos.geodeticLatitude());
         double lon(nominalPos.longitude());

            // Define station position as a Triple, in ECEF
         Triple staPos( nominalPos.getX(),
                        nominalPos.getY(),
                        nominalPos.getZ() );

            // Compute displacement vectors for L1 and L2, in meters [UEN]
         Triple dispL1(extraBiases + monumentVector + L1PhaseCenter);
         Triple dispL2(extraBiases + monumentVector + L2PhaseCenter);
         Triple dispL5(extraBiases + monumentVector + L5PhaseCenter);
         Triple dispL6(extraBiases + monumentVector + L6PhaseCenter);
         Triple dispL7(extraBiases + monumentVector + L7PhaseCenter);
         Triple dispL8(extraBiases + monumentVector + L8PhaseCenter);

            // Define a Triple that will hold satellite position, in ECEF
         Triple svPos(0.0, 0.0, 0.0);

         SatIDSet satRejectedSet;

            // Loop through all the satellites
         satTypeValueMap::iterator it;
         for (it = gData.begin(); it != gData.end(); ++it)
         {

               // Use ephemeris if satellite position is not already computed
            if( ( (*it).second.find(TypeID::satX) == (*it).second.end() ) ||
                ( (*it).second.find(TypeID::satY) == (*it).second.end() ) ||
                ( (*it).second.find(TypeID::satZ) == (*it).second.end() ) )
            {

               if(pEphemeris==NULL)
               {

                  // If ephemeris is missing, then remove all satellites
                  satRejectedSet.insert( (*it).first );

                  continue;
               }
               else
               {
                     // Try to get satellite position
                     // if it is not already computed
                  try
                  {
                        // For our purposes, position at receive time
                        // is fine enough
                     Xvt svPosVel(pEphemeris->getXvt( (*it).first, time ));

                        // If everything is OK, then continue processing.
                     svPos[0] = svPosVel.x.theArray[0];
                     svPos[1] = svPosVel.x.theArray[1];
                     svPos[2] = svPosVel.x.theArray[2];

                  }
                  catch(...)
                  {

                        // If satellite is missing, then schedule it
                        // for removal
                     satRejectedSet.insert( (*it).first );

                     continue;

                  }
               }

            }  // End of 'if( ( (*it).second.find(TypeID::satX) == ...'
            else
            {

                  // Get satellite position out of GDS
               svPos[0] = (*it).second[TypeID::satX];
               svPos[1] = (*it).second[TypeID::satY];
               svPos[2] = (*it).second[TypeID::satZ];

            }

               // Compute vector station-satellite, in ECEF
            Triple ray(svPos - staPos);

               // Rotate vector ray to UEN reference frame
            ray = (ray.R3(lon)).R2(-lat);

               // Convert ray to an unitary vector
            ray = ray.unitVector();

               // Compute corrections = displacement vectors components
               // along ray direction.
            double corrL1(dispL1.dot(ray));
            double corrL2(dispL2.dot(ray));
            double corrL5(dispL5.dot(ray));
            double corrL6(dispL6.dot(ray));
            double corrL7(dispL7.dot(ray));
            double corrL8(dispL8.dot(ray));


               // Find which observables are present, and then
               // apply corrections

               // Look for C1
            if( (*it).second.find(TypeID::C1) != (*it).second.end() )
            {
               (*it).second[TypeID::C1] = (*it).second[TypeID::C1] + corrL1;
            };

               // Look for P1
            if( (*it).second.find(TypeID::P1) != (*it).second.end() )
            {
               (*it).second[TypeID::P1] = (*it).second[TypeID::P1] + corrL1;
            };

               // Look for L1
            if( (*it).second.find(TypeID::L1) != (*it).second.end() )
            {
               (*it).second[TypeID::L1] = (*it).second[TypeID::L1] + corrL1;
            };

               // Look for C2
            if( (*it).second.find(TypeID::C2) != (*it).second.end() )
            {
               (*it).second[TypeID::C2] = (*it).second[TypeID::C2] + corrL2;
            };

               // Look for P2
            if( (*it).second.find(TypeID::P2) != (*it).second.end() )
            {
               (*it).second[TypeID::P2] = (*it).second[TypeID::P2] + corrL2;
            };

               // Look for L2
            if( (*it).second.find(TypeID::L2) != (*it).second.end() )
            {
               (*it).second[TypeID::L2] = (*it).second[TypeID::L2] + corrL2;
            };

               // Look for C5
            if( (*it).second.find(TypeID::C5) != (*it).second.end() )
            {
               (*it).second[TypeID::C5] = (*it).second[TypeID::C5] + corrL5;
            };

               // Look for L5
            if( (*it).second.find(TypeID::L5) != (*it).second.end() )
            {
               (*it).second[TypeID::L5] = (*it).second[TypeID::L5] + corrL5;
            };

               // Look for C6
            if( (*it).second.find(TypeID::C6) != (*it).second.end() )
            {
               (*it).second[TypeID::C6] = (*it).second[TypeID::C6] + corrL6;
            };

               // Look for L6
            if( (*it).second.find(TypeID::L6) != (*it).second.end() )
            {
               (*it).second[TypeID::L6] = (*it).second[TypeID::L6] + corrL6;
            };

               // Look for C7
            if( (*it).second.find(TypeID::C7) != (*it).second.end() )
            {
               (*it).second[TypeID::C7] = (*it).second[TypeID::C7] + corrL7;
            };

               // Look for L7
            if( (*it).second.find(TypeID::L7) != (*it).second.end() )
            {
               (*it).second[TypeID::L7] = (*it).second[TypeID::L7] + corrL7;
            };

               // Look for C8
            if( (*it).second.find(TypeID::C8) != (*it).second.end() )
            {
               (*it).second[TypeID::C8] = (*it).second[TypeID::C8] + corrL8;
            };

               // Look for L8
            if( (*it).second.find(TypeID::L8) != (*it).second.end() )
            {
               (*it).second[TypeID::L8] = (*it).second[TypeID::L8] + corrL8;
            };

         }

            // Remove satellites with missing data
         gData.removeSatID(satRejectedSet);

         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::int2x( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'CorrectObservables::Process()'


}  // End of namespace gpstk
