#include "of2speedit_error.H"
#include "of2speedit.H"
#include "fvCFD.H"
#include <stdlib.h>

void
handle_error(
		const std::string & error_string ,
		const std::string & dir_path
)
{
	remove_directory( dir_path.c_str() ) ;

	Foam::FatalError << error_string.c_str() ;
	Foam::FatalError << Foam::exit(Foam::FatalError, -1) ;
} ;                                                                   
