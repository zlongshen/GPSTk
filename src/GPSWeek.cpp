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

#include "GPSWeek.hpp"
#include "TimeConstants.hpp"

namespace gpstk
{
   const int GPSWeek::MAX_WEEK = (CommonTime::END_LIMIT_JDAY - GPS_EPOCH_JDAY)/7;

   GPSWeek& GPSWeek::operator=(const GPSWeek& right)
      throw()
   {
      week = right.week;
      return *this;
   }

   std::string GPSWeek::printf( const std::string& fmt ) const
      throw( gpstk::StringUtils::StringException )
   {
      try
      {
         using gpstk::StringUtils::formattedPrint;
         std::string rv = fmt;
         
         rv = formattedPrint( rv, getFormatPrefixInt() + "E",
                              "Eu", getEpoch() );
         rv = formattedPrint( rv, getFormatPrefixInt() + "F", 
                              "Fu", week );
         rv = formattedPrint( rv, getFormatPrefixInt() + "G", 
                              "Gu", getWeek10() );
         return rv;
      }
      catch( gpstk::StringUtils::StringException& exc )
      {
         GPSTK_RETHROW( exc );
      }
   }
   
   std::string GPSWeek::printError( const std::string& fmt ) const
      throw( gpstk::StringUtils::StringException )
   {
      try
      {
         using gpstk::StringUtils::formattedPrint;
         std::string rv = fmt;
         
         rv = formattedPrint( rv, getFormatPrefixInt() + "E",
                              "Es", getError().c_str() );
         rv = formattedPrint( rv, getFormatPrefixInt() + "F", 
                              "Fs", getError().c_str() );
         rv = formattedPrint( rv, getFormatPrefixInt() + "G", 
                              "Gs", getError().c_str() );
         return rv;
      }
      catch( gpstk::StringUtils::StringException& exc )
      {
         GPSTK_RETHROW( exc );
      }
   }
   
      /**
       * Set this object using the information provided in \a info.
       * @param info the IdToValue object to which this object shall be set.
       * @return true if this object was successfully set using the 
       *  data in \a info, false if not.
       */
   bool GPSWeek::setFromInfo( const IdToValue& info )
      throw()
   {
      using namespace gpstk::StringUtils;
      
      for( IdToValue::const_iterator i = info.begin(); i != info.end(); i++ )
      {
            // based on the character, we know what to do...
         switch ( i->first ) 
         {
            case 'E':
               setEpoch( asInt( i->second ) );
               break;
            case 'F':
               week = asInt( i->second );
               break;
            case 'G':
               setWeek10( asInt( i->second ) );
               break;
            default:
                  // do nothing
               break;
         };
         
      } // end of for loop
      
      return true;
   }

} // namespace
