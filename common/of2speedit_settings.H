#ifndef __OF2SPEEDIT_SETTINGS_H__
#define __OF2SPEEDIT_SETTINGS_H__

#include "fvCFD.H"
#include <fstream>
#include <iomanip>

template<class T>
std::ostream &
setting_name( std::ostream & f, T & name )
{
	f << std::left ;
	f << std::setw(60) << name ;
	return f ;
} ;

void
save_iter_solvers_settings( std::ostream & f, Foam::IOobject & obj) ;
void
save_iter_solvers_settings_turb( std::ostream & f, Foam::IOobject & obj) ;

#endif
