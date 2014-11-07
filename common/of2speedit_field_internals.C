#include "of2speedit_field_internals.H"
#include "of2speedit_utils.H"


template< class T >
int
number_of_elements ( 
                     Foam::Field< T > const & field 
									 )
{
	return field.size() ;
} ;

template<template<class> class Field>
void
init_vec (
		std::vector< std::vector<double> >       & v,
		Field<scalar>                      const & field
		)
{
	v   .resize( 1 ) ;
	v[0].resize( number_of_elements(field) ) ;
} ;

template<template<class> class Field>
void
init_vec (
		std::vector< std::vector<double> >       & v,
		Field<vector>                      const & field
		)
{
	int nelements = number_of_elements(field) ;

	v   .resize( 3 ) ;
	v[0].resize( nelements ) ;
	v[1].resize( nelements ) ;
	v[2].resize( nelements ) ;
} ;

template< class T >
std::vector< std::vector<double> >
field_values ( Field< T > const & field )
{
	std::vector< std::vector<double> > vres ;

	init_vec(vres, field) ;

	int nelements = number_of_elements(field) ;

	for (int i=0 ; i < nelements ; i++) {

		set_vec_val(vres, field[ i ], i) ;

	} ;

	return vres ;
} ;


template<class T, template<class> class TBoundary, class TMesh>
std::vector< std::vector<double> >
internal_values_templ ( GeometricField < T, TBoundary, TMesh > const & field )
{
	if (
			typeid(T) != typeid(scalar) &&
			typeid(T) != typeid(vector)
		 ) 
	{
		throw std::string("Unsupported field data type") ;
	} ;

	return field_values( field.internalField() ) ;
} ;


std::vector< std::vector<double> >
internal_values ( volScalarField const & field )
{
	return internal_values_templ( field ) ;
} ;

std::vector< std::vector<double> >
internal_values ( volVectorField const & field )
{
	return internal_values_templ( field ) ;
} ;

std::vector< std::vector<double> >
internal_values ( surfaceVectorField const & field )
{
	return internal_values_templ( field ) ;
} ;

std::vector< std::vector<double> >
internal_values ( surfaceScalarField const & field )
{
	return internal_values_templ( field ) ;
} ;
