
/**
 * @file PoleTides.hpp
 * Computes the effect of pole tides at a given position and epoch.
 */

#ifndef GPSTK_POLETIDES_HPP
#define GPSTK_POLETIDES_HPP

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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007
//
//============================================================================

 
#include <cmath>
#include <string>

#include "Triple.hpp"
#include "Position.hpp"
#include "DayTime.hpp"
#include "icd_200_constants.hpp"
#include "geometry.hpp"



namespace gpstk
{
    /** @addtogroup GPSsolutions */
    //@{
   
    /** This class computes the effect of pole tides, or more properly
     * called, "rotational deformations due to polar motion" at a given
     * position and epoch.
     *
     * The model used is the one proposed by the "International Earth
     * Rotation and Reference Systems Service" (IERS) in its upcomming
     * "IERS Conventions" document (Chapter 7), available at:
     *
     * http://tai.bipm.org/iers/convupdt/convupdt.html
     *
     * The pole movement parameters x, y for a given epoch may be
     * found at:
     *
     * ftp://hpiers.obspm.fr/iers/eop/eop.others
     *
     * Maximum displacements because of this effect are:
     *
     * Vertical:    2.5 cm
     * Horizontal:  0.7 cm
     *
     */
    class PoleTides
    {
    public:

        /// Default constructor. Sets zero pole displacement
        PoleTides() : xdisp(0.0), ydisp(0.0) {};

        /** Common constructor
         * @param x     Pole displacement x, in arcseconds
         * @param y     Pole displacement y, in arcseconds
         */
        PoleTides(const double& x, const double& y) : xdisp(x), ydisp(y) {};

        /// Destructor
        virtual ~PoleTides() {};


        /** Returns the effect of pole tides (meters) at the given
         * position and epoch, in the Up-East-North (UEN) reference frame.
         *
         * @param[in] t Epoch to look up
         * @param[in] p Position of interest
         *
         * @return a Triple with the pole tide effect, in meters and in
         * the UEN reference frame.
         *
         * @throw InvalidRequest If the request can not be completed for any
         * reason, this is thrown. The text may have additional information
         * as to why the request failed.
         *
         * @warning In order to use this method, you must have previously
         * set the current pole displacement parameters
         *
         */
        Triple getPoleTide(const DayTime& t, const Position& p)  throw(InvalidRequest);


        /** Returns the effect of pole tides (meters) on the given
         * position, in the Up-East-North (UEN) reference frame.
         *
         * @param[in] p Position of interest
         * @param[in] x Pole displacement x, in arcseconds
         * @param[in] y Pole displacement y, in arcseconds
         *
         * @return a Triple with the pole tide effect, in meters and in
         * the UEN reference frame.
         *
         * @throw InvalidRequest If the request can not be completed for any
         * reason, this is thrown. The text may have additional information
         * as to why the request failed.
         */
        Triple getPoleTide(const DayTime& t, const Position& p, const double& x, const double& y) throw(InvalidRequest)
        {
            (*this).setXY(x,y);
            return ((*this).getPoleTide(t, p));
        };


        /** Method to set the pole displacement parameters
         * @param x     Pole displacement x, in arcseconds
         * @param y     Pole displacement y, in arcseconds
         */
        void setXY(const double& x, const double& y)
        {
            xdisp = x;
            ydisp = y;
        };


        /// Method to get the x pole displacement parameter
        double getX(void) const
        {
            return xdisp;
        };


        /// Method to get the y pole displacement parameter
        double getY(void) const
        {
            return ydisp;
        };


    private:

        /// Pole displacement x, in arcseconds
        double xdisp;


        /// Pole displacement y, in arcseconds
        double ydisp;


    }; // end class PoleTides


   //@}
   
} // namespace gpstk
#endif  // GPSTK_SOLIDTIDES_HPP