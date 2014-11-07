#ifndef __OF2SPEEDIT_FIELD_INTERNALS_H__
#define __OF2SPEEDIT_FIELD_INTERNALS_H__

#include <vector>
#include "fvCFD.H"

std::vector< std::vector<double> >
internal_values ( volScalarField const & field ) ;

std::vector< std::vector<double> >
internal_values ( volVectorField const & field ) ;

std::vector< std::vector<double> >
internal_values ( surfaceVectorField const & field ) ;

std::vector< std::vector<double> >
internal_values ( surfaceScalarField const & field ) ;

#endif
