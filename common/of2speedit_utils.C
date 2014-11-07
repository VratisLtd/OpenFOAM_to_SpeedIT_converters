#include "of2speedit_utils.H"


void
set_vec_val (
		std::vector< std::vector<double> > & vout , // result
		scalar                               val  , // data source
		int                                  idx
		)
{
	vout[0][ idx ] = val ;
} ;

void
set_vec_val (
		std::vector< std::vector<int   > > & vout , // result
		int                                  val  , // data source
		int                                  idx
		)
{
	vout[0][ idx ] = val ;
} ;
void
set_vec_val (
		std::vector< std::vector<double> > & vout , // result
		vector                               val  , // data source
		int                                  idx
		)
{
	vout[0][ idx ] = val.x() ;
	vout[1][ idx ] = val.y() ;
	vout[2][ idx ] = val.z() ;
} ;

