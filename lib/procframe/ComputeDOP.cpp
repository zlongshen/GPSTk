#pragma ident "$Id$"

/**
 * @file ComputeDOP.cpp
 * This class computes the usual DOP values: GDOP, PDOP, TDOP, HDOP and VDOP.
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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2008
//
//============================================================================


#include "ComputeDOP.hpp"


namespace gpstk
{

      // Index initially assigned to this class
   int ComputeDOP::classIndex= 4800000;


      // Returns an index identifying this object.
   int ComputeDOP::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string ComputeDOP::getClassName() const
   { return "ComputeDOP"; }



      /* Returns a satTypeValueMap object, adding the new data generated when
       * calling this object.
       *
       * @param time      Epoch corresponding to the data.
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& ComputeDOP::Process( const DayTime& time,
                                         satTypeValueMap& gData)
      throw(ProcessingException)
   {

      try
      {

         bool valid1(false), valid2(false); 

            // First, let's define a set with XYZt unknowns
         TypeIDSet tempSet1;
         tempSet1.insert(TypeID::dx);
         tempSet1.insert(TypeID::dy);
         tempSet1.insert(TypeID::dz);
         tempSet1.insert(TypeID::cdt);

            // Second, let's define a set with NEUt unknowns
         TypeIDSet tempSet2;
         tempSet2.insert(TypeID::dLat);
         tempSet2.insert(TypeID::dLon);
         tempSet2.insert(TypeID::dH);
         tempSet2.insert(TypeID::cdt);

            // Then, generate the corresponding geometry/design matrices
         Matrix<double> dMatrix1(gData.getMatrixOfTypes(tempSet1));
         Matrix<double> dMatrix2(gData.getMatrixOfTypes(tempSet2));

            // Afterwards, compute the appropriate extra matrices
         Matrix<double> AT1(transpose(dMatrix1));
         Matrix<double> covM1(AT1 * dMatrix1);

         Matrix<double> AT2(transpose(dMatrix2));
         Matrix<double> covM2(AT2 * dMatrix2);

            // Let's try to invert AT*A matrices
         try
         {

            covM1 = inverseChol( covM1 );
            valid1 = true;

         }
         catch(...)
         {

            valid1 = false;
         }

         try
         {

            covM2 = inverseChol( covM2 );
            valid2 = true;

         }
         catch(...)
         {
            valid2 = false;
         }

         if( valid1 )
         {

            gdop = std::sqrt(covM1(0,0)+covM1(1,1)+covM1(2,2)+covM1(3,3));
            pdop = std::sqrt(covM1(0,0)+covM1(1,1)+covM1(2,2));
            tdop = std::sqrt(covM1(3,3));

         }
         else
         {
            gdop = -1.0;
            pdop = -1.0;
            tdop = -1.0;
         }

         if( valid2 )
         {
            hdop = std::sqrt(covM2(0,0)+covM2(1,1));
            vdop = std::sqrt(covM2(2,2));
         }
         else
         {
            hdop = -1.0;
            vdop = -1.0;
         }

         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'ComputeDOP::Process()'



}  // End of namespace gpstk
