#ifndef __OF2SPEEDIT_UTILS_H__
#define __OF2SPEEDIT_UTILS_H__

#include "fvCFD.H"
#include <vector>

void
set_vec_val (
		std::vector< std::vector<double> > & vout , // result
		scalar                               val  , // data source
		int                                  idx
		) ;
void
set_vec_val (
		std::vector< std::vector<int   > > & vout , // result
		int                                  val  , // data source
		int                                  idx
		) ;
void
set_vec_val (
		std::vector< std::vector<double> > & vout , // result
		vector                               val  , // data source
		int                                  idx
		) ;

template<class Type>
std::vector< Type >
vector_reorder(
		std::vector< Type > const & in_vec        ,
		std::vector< int  > const & reorder_table
)
{
	std::vector< Type > vres ;
	
	vres.resize( in_vec.size() ) ;

	for (unsigned int i=0 ; i<in_vec.size() ; i++){
		vres[ i ] = in_vec[ reorder_table[ i ] ] ;
	} ;

	return vres ;
} ;

template<class Type>
std::vector< std::vector<Type> >
vector_reorder(
		std::vector< std::vector<Type> > const & in_vec        ,
		std::vector< int >               const & reorder_table
		)
{
	std::vector< std::vector<Type> > vres ;

	vres.resize( in_vec.size() ) ;
	for (unsigned int i=0 ; i<in_vec.size() ; i++)
	{
		vres[ i ] = vector_reorder( in_vec[ i ], reorder_table ) ;
	} ;

	return vres ;
} ;

#endif
