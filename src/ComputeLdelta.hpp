
/**
 * @file ComputeLdelta.hpp
 * This class eases computing Ldelta combination for GNSS data structures.
 */

#ifndef Compute_LDELTA_GPSTK
#define Compute_LDELTA_GPSTK

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
//  Dagoberto Salazar - gAGE. 2007
//
//============================================================================



#include "ComputeCombination.hpp"


namespace gpstk
{

    /** @addtogroup DataStructures */
    //@{


    /// This class eases computing Ldelta combination for GNSS data structures.
    class ComputeLdelta : public ComputeCombination
    {
    public:

        /// Default constructor
        ComputeLdelta() : DEN(L1_FREQ - L2_FREQ)
        {
            type1 = TypeID::L1;
            type2 = TypeID::L2;
            resultType = TypeID::Ldelta;
        };


        /** Returns a satTypeValueMap object, adding the new data generated when calling this object.
         *
         * @param gData     Data object holding the data.
         */
        virtual satTypeValueMap& Combine(satTypeValueMap& gData)
        {
            ComputeCombination::Combine(gData);

            return gData;
        }


        /// Destructor
        virtual ~ComputeLdelta() {};


    protected:
        /// Compute the combination of observables.
        virtual double getCombination(const double& obs1, const double& obs2)
        {
            return ( ( L1_FREQ*obs1 - L2_FREQ*obs2 ) / ( DEN ) );
        };

    private:

        const double DEN;       // DEN = L1_FREQ - L2_FREQ

   }; // end class ExtractLdelta
   

   //@}
   
}

#endif
