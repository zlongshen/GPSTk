/** $Id: //depot/msn/prototype/brent/IGEB_Demo/SVNumXRef.cpp#2 $
*   SVNumXRefMap.cpp
*
*   Applied Research Laboratories, The University of Texas at Austin
*
*/
   // Language Headers

   // Library Headers
   // Project Headers
#include "SVNumXRef.hpp"

using namespace std;
namespace gpstk
{

SVNumXRef::SVNumXRef( )
{
      // Note: This table start with Block II values
      // Set up NAVSTAR -> PRN ID relationship
      // NAVSTAR ID first, PRN ID second
   NtoPMap.insert( make_pair( 13,  2 ));
   NtoPMap.insert( make_pair( 14, 14 ));
   NtoPMap.insert( make_pair( 15, 15 ));
   NtoPMap.insert( make_pair( 16, 16 ));
   NtoPMap.insert( make_pair( 17, 17 ));
   NtoPMap.insert( make_pair( 18, 18 ));
   NtoPMap.insert( make_pair( 19, 19 ));
   NtoPMap.insert( make_pair( 20, 20 ));
   NtoPMap.insert( make_pair( 21, 21 ));
   NtoPMap.insert( make_pair( 22, 22 ));
   NtoPMap.insert( make_pair( 23, 23 ));
   NtoPMap.insert( make_pair( 24, 24 ));
   NtoPMap.insert( make_pair( 25, 25 ));
   NtoPMap.insert( make_pair( 26, 26 ));
   NtoPMap.insert( make_pair( 27, 27 ));
   NtoPMap.insert( make_pair( 28, 28 ));
   NtoPMap.insert( make_pair( 29, 29 ));
   NtoPMap.insert( make_pair( 30, 30 ));
   NtoPMap.insert( make_pair( 31, 31 ));
   NtoPMap.insert( make_pair( 32,  1 ));
   NtoPMap.insert( make_pair( 33,  3 ));
   NtoPMap.insert( make_pair( 34,  4 ));
   NtoPMap.insert( make_pair( 35,  5 ));
   NtoPMap.insert( make_pair( 36,  6 ));
   NtoPMap.insert( make_pair( 37,  7 ));
   NtoPMap.insert( make_pair( 38,  8 ));
   NtoPMap.insert( make_pair( 39,  9 ));
   NtoPMap.insert( make_pair( 40, 10 )); 
   NtoPMap.insert( make_pair( 41, 14 ));
    // no NAVSTAR 42, IIR-1 was a launch failure
   NtoPMap.insert( make_pair( 43, 13 )); 
   NtoPMap.insert( make_pair( 44, 28 ));
   NtoPMap.insert( make_pair( 45, 21 ));
   NtoPMap.insert( make_pair( 46, 11 ));
   NtoPMap.insert( make_pair( 47, 22 ));
   NtoPMap.insert( make_pair( 51, 20 ));
   NtoPMap.insert( make_pair( 52, 31 ));
   NtoPMap.insert( make_pair( 53, 17 ));
   NtoPMap.insert( make_pair( 54, 18 ));
   NtoPMap.insert( make_pair( 55, 15 ));
   NtoPMap.insert( make_pair( 56, 16 ));
   NtoPMap.insert( make_pair( 58, 12 ));
   NtoPMap.insert( make_pair( 59, 19 ));
   NtoPMap.insert( make_pair( 60, 23 ));
   NtoPMap.insert( make_pair( 61,  2 ));
      
      // Set up PRN ID -> NAVSTAR relationship
   PtoNMap.insert( make_pair( 1, SVNumXRefNode( 32, 
                                       DayTime( 1992, 11, 22, 0, 0, 0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( make_pair( 2, SVNumXRefNode( 13, 
                                       DayTime( 1989,  6, 10, 0, 0, 0.0),
                                       DayTime( 2004,  5, 12, 0, 0, 0.0))));
   PtoNMap.insert( make_pair( 2, SVNumXRefNode( 61, 
                                       DayTime( 2004,  6,  6, 0, 0, 0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( make_pair( 3, SVNumXRefNode( 33, 
                                       DayTime( 1996,  3, 28, 0, 0, 0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( make_pair( 4, SVNumXRefNode( 34, 
                                       DayTime( 1993, 10, 26, 0, 0, 0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( make_pair( 5, SVNumXRefNode( 35, 
                                       DayTime( 1993,  8, 30, 0, 0, 0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( make_pair( 6, SVNumXRefNode( 36, 
                                       DayTime( 1995,  3, 10, 0, 0, 0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( make_pair( 7, SVNumXRefNode( 37, 
                                       DayTime( 1993,  5, 13, 0, 0, 0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( make_pair( 8, SVNumXRefNode( 38, 
                                       DayTime( 1997, 11, 06, 0, 0, 0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( make_pair( 9, SVNumXRefNode( 39, 
                                       DayTime( 1993,  6, 26, 0, 0, 0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( make_pair(10, SVNumXRefNode( 40, 
                                       DayTime( 1996,  7, 16, 0, 0, 0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( make_pair(11, SVNumXRefNode( 46, 
                                       DayTime( 1999, 10,  7, 0, 0, 0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( make_pair(12, SVNumXRefNode( 58, 
                                       DayTime( 2006, 11, 17, 0, 0, 0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( make_pair(13, SVNumXRefNode( 43, 
                                       DayTime( 1997,  7, 23, 0, 0, 0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( make_pair(14, SVNumXRefNode( 14, 
                                       DayTime( 1989,  2, 14, 0, 0, 0.0),
                                       DayTime( 2000,  4, 14, 0, 0, 0.0))));
   PtoNMap.insert( make_pair(14, SVNumXRefNode( 41, 
                                       DayTime( 2000, 11, 10, 0, 0, 0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( make_pair(15, SVNumXRefNode( 15, 
                                       DayTime( 1990, 10, 01, 0, 0, 0.0),
                                       DayTime( 2007,  3, 15, 0, 0, 0.0))));
   PtoNMap.insert( make_pair(15, SVNumXRefNode( 55, 
                                       DayTime( 2007, 10, 17, 0, 0, 0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( make_pair(16, SVNumXRefNode( 16, 
                                       DayTime( 1989,  8, 18, 0, 0, 0.0),
                                       DayTime( 2000, 10, 13, 0, 0, 0.0))));
   PtoNMap.insert( make_pair(16, SVNumXRefNode( 56, 
                                       DayTime( 2003,  1, 29, 0, 0, 0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( make_pair(17, SVNumXRefNode( 17, 
                                       DayTime( 1989, 12, 11, 0, 0, 0.0),
                                       DayTime( 2005,  2, 23, 0, 0, 0.0))));
   PtoNMap.insert( make_pair(17, SVNumXRefNode( 53, 
                                       DayTime( 2005,  9, 26, 0, 0, 0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( make_pair(18, SVNumXRefNode( 18, 
                                       DayTime( 1990,  1, 24, 0, 0, 0.0),
                                       DayTime( 2000,  8, 18, 0, 0, 0.0))));
   PtoNMap.insert( make_pair(18, SVNumXRefNode( 54, 
                                       DayTime( 2001,  1, 30, 0, 0, 0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( make_pair(19, SVNumXRefNode( 19, 
                                       DayTime( 1989, 10, 21, 0, 0, 0.0),
                                       DayTime( 2001,  9, 11, 0, 0, 0.0))));
   PtoNMap.insert( make_pair(19, SVNumXRefNode( 59, 
                                       DayTime( 2004,  3, 20, 0, 0, 0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( make_pair(20, SVNumXRefNode( 20, 
                                       DayTime( 1990,  3, 26, 0, 0, 0.0),
                                       DayTime( 1996, 12, 13, 0, 0, 0.0))));
   PtoNMap.insert( make_pair(20, SVNumXRefNode( 51, 
                                       DayTime( 2000,  5, 11, 0, 0, 0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( make_pair(21, SVNumXRefNode( 21, 
                                       DayTime( 1990,  8,  2, 0, 0, 0.0),
                                       DayTime( 2003,  1, 27, 0, 0, 0.0))));
   PtoNMap.insert( make_pair(21, SVNumXRefNode( 45, 
                                       DayTime( 2003,  3, 31, 0, 0, 0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( make_pair(22, SVNumXRefNode( 22, 
                                       DayTime( 1993,  2,  3, 0, 0, 0.0),
                                       DayTime( 2003,  8,  6, 0, 0, 0.0))));
   PtoNMap.insert( make_pair(22, SVNumXRefNode( 47, 
                                       DayTime( 2003, 12, 21, 0, 0, 0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( make_pair(23, SVNumXRefNode( 23, 
                                       DayTime( 1990, 11, 26, 0, 0, 0.0),
                                       DayTime( 2004,  2, 13, 0, 0, 0.0))));
   PtoNMap.insert( make_pair(23, SVNumXRefNode( 60, 
                                       DayTime( 2004,  6, 23, 0, 0, 0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( make_pair(24, SVNumXRefNode( 24, 
                                       DayTime( 1991,  7,  4, 0, 0, 0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( make_pair(25, SVNumXRefNode( 25, 
                                       DayTime( 1992,  2, 23, 0, 0, 0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( make_pair(26, SVNumXRefNode( 26, 
                                       DayTime( 1992,  7,  7, 0, 0, 0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( make_pair(27, SVNumXRefNode( 27, 
                                       DayTime( 1992,  9,  9, 0, 0, 0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( make_pair(28, SVNumXRefNode( 28, 
                                       DayTime( 1992,  4, 10, 0, 0, 0.0),
                                       DayTime( 1997,  8, 15, 0, 0, 0.0))));
   PtoNMap.insert( make_pair(28, SVNumXRefNode( 44,
                                       DayTime( 2000,  7, 16, 0, 0, 0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( make_pair(29, SVNumXRefNode( 29, 
                                       DayTime( 1992, 12, 18, 0, 0, 0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( make_pair(30, SVNumXRefNode( 30, 
                                       DayTime( 1996,  9, 12, 0, 0, 0.0),
                                       DayTime::END_OF_TIME  )));
   PtoNMap.insert( make_pair(31, SVNumXRefNode( 31, 
                                       DayTime( 1993,  3, 30, 0, 0, 0.0),
                                       DayTime( 2005, 10, 24, 0, 0, 0.0))));
   PtoNMap.insert( make_pair(31, SVNumXRefNode( 52, 
                                       DayTime( 2006,  9, 25, 0, 0, 0.0),
                                       DayTime::END_OF_TIME  )));


}

int SVNumXRef::getNAVSTAR( const int PRNID ) const
{
   return( getNAVSTAR( PRNID, DayTime() ) );
}
bool SVNumXRef::NAVSTARIDAvailable( const int PRNID ) const
{
   return( NAVSTARIDAvailable( PRNID, DayTime() ) );
}

int SVNumXRef::getNAVSTAR( const int PRNID, const gpstk::DayTime dt ) const
{
   SVNumXRefPair p = PtoNMap.equal_range( PRNID );
   for (SVNumXRefListCI ci=p.first; ci != p.second; ++ci )
   {
      if (ci->second.isApplicable( dt )) return( ci->second.getNAVSTARNum() );
   }
   
      // We didn't find a NAVSTAR # for this PRN ID and date, so throw an 
      // exception.
   char textOut[80];
   sprintf(textOut,"No NAVSTAR # found associated with PRN ID %d at requested date: %s.", 
            PRNID,dt.printf("%02m/%02d/%04Y").c_str() ); 
   std::string sout = textOut;
   NoPRNNumberFound noFound( sout );
   GPSTK_THROW(noFound); 
}

bool SVNumXRef::NAVSTARIDAvailable( const int PRNID, const gpstk::DayTime dt ) const
{
   SVNumXRefPair p = PtoNMap.equal_range( PRNID );
   for (SVNumXRefListCI ci=p.first; ci != p.second; ++ci )
   {
      if (ci->second.isApplicable( dt )) return( true );
   }
   return( false ); 
}

int SVNumXRef::getPRNID( const int NAVSTARID ) const
{
   NAVNumXRefCI p = NtoPMap.find( NAVSTARID );
   if (p!=NtoPMap.end()) return(p->second);
   
      // We didn't find a PRN ID for this NAVSTAR #, so throw an 
      // exception.
   char textOut[80];
   sprintf(textOut,"No PRN ID found associated with NAVSTAR Num %d.", 
            NAVSTARID);
   std::string sout = textOut;
   NoNAVSTARNumberFound noFound( sout );
   GPSTK_THROW(noFound); 
   throw( noFound );
}

bool SVNumXRef::PRNIDAvailable(  const int NAVSTARID ) const
{
   NAVNumXRefCI p = NtoPMap.find( NAVSTARID );
   if (p!=NtoPMap.end()) return(true);
   return(false);
}

//-------------- Nethods for SVNumXRefNode -----------------
SVNumXRefNode::SVNumXRefNode( const int NAVSTARNumArg,
                             const gpstk::DayTime begDT,
                             const gpstk::DayTime endDT )
{
   NAVSTARNum = NAVSTARNumArg;
   begValid = begDT;
   endValid = endDT;
}

bool SVNumXRefNode::isApplicable( gpstk::DayTime dt ) const
{
   if (dt>=begValid && dt<=endValid) return(true);
   return(false);
}
}