#ifndef __OF2SPEEDIT_H__
#define __OF2SPEEDIT_H__

#include "fvCFD.H"
#include "of2speedit_global_macros.H"

void
create_directory ( const char * dir_path ) ;
void
remove_directory ( const char * dir_path) ;

void
check_application_name ( 
                         Foam::Time const & runTime , 
												 char       const * app_name
						           ) ;

void
save_field(
			      volScalarField     & field    ,
			const Foam::fvMesh       & mesh     ,
			const	std::string        & dir_path ,
						Foam::Time         * runTime  = NULL
					) ;
void
save_field(
			      volVectorField     & field    ,
			const Foam::fvMesh       & mesh     ,
			const	std::string        & dir_path ,
						Foam::Time         * runTime  = NULL
					) ;
void
save_field(
			      surfaceScalarField & field    ,
			const Foam::fvMesh       & mesh     ,
			const	std::string        & dir_path ,
						Foam::Time         * runTime  = NULL
		) ;
void
save_mesh(
            fvMesh      & mesh ,
						std::string   dir_path
         ) ;


#endif
