#pragma ident "$Id: //depot/sgl/gpstk/dev/src/TimeTag.cpp#2 $"

#include "TimeTag.hpp"
#include "StringUtils.hpp"

namespace gpstk
{
   const std::string TimeTag::formatPrefixInt( "%[ 0-]?[[:digit:]]*" );
   const std::string TimeTag::formatPrefixFloat( TimeTag::formatPrefixInt + 
                                                 "(\\.[[:digit:]]+)?" );
   
   void TimeTag::scanf( const std::string& str,
                        const std::string& fmt )
      throw( gpstk::InvalidRequest,
             gpstk::StringUtils::StringException )
   {
      try
      {
            // Get the mapping of character (from fmt) to value (from str).
         IdToValue info;
         getInfo( str, fmt, info );
         
            // Attempt to set this object using the IdToValue object
         if( !setFromInfo( info ) )
         {
            gpstk::InvalidRequest ir( "Incomplete time specification." );
            GPSTK_THROW( ir );
         }
      }
      catch( gpstk::StringUtils::StringException& se )
      {
         GPSTK_RETHROW( se );
      }
   }
   
   void TimeTag::getInfo( const std::string& str,
                          const std::string& fmt,
                          IdToValue& info )
      throw( gpstk::StringUtils::StringException ) 
   {
      try
      {
         using namespace gpstk::StringUtils;

            // Copy the arguments to strings we can modify.
         std::string f = fmt;
         std::string s = str;
      
            // Parse strings...  As we process each part, it's removed from both
            // strings so when we reach 0, we're done
         while( !s.empty() && !f.empty() )
         {
               // Remove everything in f and s up to the first % in f
               // (these parts of the strings must be identical or this will 
               // break after it tries to remove it!)
            while ( !s.empty() && 
                    !f.empty() && 
                    ( f[0] != '%' ) )
            {
                  // remove that character now and other whitespace
               s.erase( 0, 1 );
               f.erase( 0, 1 );
               stripLeading( s );
               stripLeading( f );
            }
         
               // check just in case we hit the end of either string...
            if ( s.empty() || f.empty() )
            {
               break;
            }
            
               // lose the '%' in f...
            f.erase( 0, 1 );
            
               // If the format string is %03f, get '3' as the field length.
            std::string::size_type fieldLength = std::string::npos;
            
            if( !isalpha( f[0] ) )
            {
               fieldLength = asInt( f );
               
                  // remove everything else up to the next character
                  // (in "%03f", that would be 'f')
               while ( ( !f.empty() ) && 
                       ( !isalpha( f[0] ) ) )
               {
                  f.erase( 0, 1 );
               }
               
               if ( f.empty() )
               {
                  break;
               }
            }
            
               // finally, get the character that should end this field, if any
            char delimiter = 0;
            if ( f.size() > 1 )
            {
               if ( f[1] != '%' )
               {
                  delimiter = f[1];
                  
                  if ( fieldLength == std::string::npos )
                  {
                     fieldLength = s.find( delimiter, 0 );
                  }
               }
                  // if the there is no delimiter character and the next field
                  // is another part of the time to parse, assume the length
                  // of this field is 1
               else if ( fieldLength == std::string::npos )
               {
                  fieldLength = 1;
               }
            }
            
               // figure out the next string to be removed.  if there is a field
               // length, use that first
            std::string value( s.substr( 0, fieldLength ) );
            
               // based on char at f[0], we know what to do...
            info[ f[0] ] = value;
            
               // remove the part of str that we processed
            stripLeading( s, value, 1 );
            
               // remove the character we processed from fmt
            f.erase( 0, 1 );    
            
               // check for whitespace again...
            stripLeading( f );
            stripLeading( s );

         } // end of while( (s.size() > 0) && (f.size() > 0) )
         
         if ( !s.empty() ) 
         {
               // throw an error - something didn't get processed in the strings
            gpstk::StringUtils::StringException se(
               "Processing error - parts of strings left unread - " + s );
            GPSTK_THROW( se );
         }
         
         if ( !f.empty() )
         {
               // throw an error - something didn't get processed in the strings
            gpstk::StringUtils::StringException se(
               "Processing error - parts of strings left unread - " + f );
            GPSTK_THROW( se );
         }
         
      }
      catch( gpstk::StringUtils::StringException& se )
      {
         GPSTK_RETHROW( se );
      }
   }

} // namespace

std::ostream& operator<<( std::ostream& s,
                          const gpstk::TimeTag& t )
{
   s << t.printf( t.getDefaultFormat() );
   return s;
}
