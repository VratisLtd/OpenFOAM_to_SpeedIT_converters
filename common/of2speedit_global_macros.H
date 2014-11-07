#ifndef __OF2SPEEDIT_GLOBAL_MACROS_H__
#define __OF2SPEEDIT_GLOBAL_MACROS_H__

#include "of2speedit_defines.H"
#include "of2speedit_settings.H"
#include "of2speedit_error.H"
#include <fstream>

inline
int compute_n_time_steps(Foam::Time & runTime)
{
	dimensionedScalar start_time       = runTime.startTime() ;
	label             start_time_index = runTime.startTimeIndex() ;

	int time_idx = 0 ;
	while (runTime.loop())
	{
		time_idx ++ ;
	} ;

	runTime.setTime( start_time, start_time_index ) ;

	return time_idx ;
} ;

#define N_TIME_STEPS compute_n_time_steps(runTime)

#define SAVE_SETTINGS_COMMON(file, solver_name)                          \
{                                                                        \
	if ( !file.is_open() ) throw "Can not open file " + filename ;         \
																																			   \
	file << FILE_HEADER     << "\n" ;                                      \
	file << SETTINGS_HEADER << "\n" ;                                      \
																																			   \
	file << "\n" ;                                                         \
																																			   \
	setting_name(file, SETTING_SOLVER_NAME) << solver_name <<"\n" ;        \
																																			   \
	setting_name(file, SETTING_N_TIME_STEPS) ;                             \
	file << static_cast<int>( N_TIME_STEPS ) ;                             \
	file << "\n" ;                                                         \
	setting_name(file, SETTING_P_FIXED_CELL_IDX) ;                         \
	file << static_cast<int>( pRefCell ) << "\n" ;                         \
	setting_name(file, SETTING_P_FIXED_CELL_VALUE) << pRefValue << "\n" ;  \
	setting_name(file, SETTING_NU) << nu.value() << "\n" ;                 \
                                                                         \
	save_iter_solvers_settings(file, U) ;                                  \
}

#define SAVE_SETTINGS_TURB(file, solver_name)                          \
{                                                                        \
    if ( !file.is_open() ) throw "Can not open file " + filename ;         \
                                                                                                                                               \
    file << FILE_HEADER     << "\n" ;                                      \
    file << SETTINGS_HEADER << "\n" ;                                      \
                                                                                                                                               \
    file << "\n" ;                                                         \
                                                                                                                                               \
    setting_name(file, SETTING_SOLVER_NAME) << solver_name <<"\n" ;        \
                                                                                                                                               \
    setting_name(file, SETTING_N_TIME_STEPS) ;                             \
    file << static_cast<int>( N_TIME_STEPS ) ;                             \
    file << "\n" ;                                                         \
    setting_name(file, SETTING_P_FIXED_CELL_IDX) ;                         \
    file << static_cast<int>( pRefCell ) << "\n" ;                         \
    setting_name(file, SETTING_P_FIXED_CELL_VALUE) << pRefValue << "\n" ;  \
    setting_name(file, SETTING_NU) << nu.value() << "\n" ;                 \
                                                                         \
    save_iter_solvers_settings(file, U) ;                                  \
    save_iter_solvers_settings_turb(file, U) ;                                   \
}

#define LOAD_OF_CASE                                                       \
                                                                           \
	Foam::argList args(argc, argv);                                          \
                                                                           \
	/* Parallel run is not supported */                                      \
	if (args.optionFound("parallel"))                                        \
	{                                                                        \
		throw std::string( "Can not convert parallel case,") +                 \
		                   " please reorganize your case to sigle-domain" ;    \
	} ;                                                                      \
                                                                           \
	if (!args.checkRootCase())                                               \
	{                                                                        \
		throw std::string("Wrong parameters passed, OpenFOAM check failed") ;  \
	}                                                                        \


#define LOAD_OF_FIELDS                                                     \
                                                                           \
	Info<< "Reading transportProperties\n" << endl;                          \
                                                                           \
	IOdictionary transportProperties                                         \
		(                                                                      \
		 IOobject                                                              \
		 (                                                                     \
			"transportProperties",                                               \
			runTime.constant(),                                                  \
			mesh,                                                                \
			IOobject::MUST_READ,                                                 \
			IOobject::NO_WRITE                                                   \
		 )                                                                     \
		);                                                                     \
                                                                           \
	dimensionedScalar nu                                                     \
		(                                                                      \
		 transportProperties.lookup("nu")                                      \
		);                                                                     \
                                                                           \
	Info<< "Reading field p\n" << endl;                                      \
	volScalarField p                                                         \
		(                                                                      \
		 IOobject                                                              \
		 (                                                                     \
			"p",                                                                 \
			runTime.timeName(),                                                  \
			mesh,                                                                \
			IOobject::MUST_READ,                                                 \
			IOobject::AUTO_WRITE                                                 \
		 ),                                                                    \
		 mesh                                                                  \
		);                                                                     \
                                                                           \
	Info<< "Reading field U\n" << endl;                                      \
	volVectorField U                                                         \
		(                                                                      \
		 IOobject                                                              \
		 (                                                                     \
			"U",                                                                 \
			runTime.timeName(),                                                  \
			mesh,                                                                \
			IOobject::MUST_READ,                                                 \
			IOobject::AUTO_WRITE                                                 \
		 ),                                                                    \
		 mesh                                                                  \
		);                                                                     \
                                                                           \
	Info<< "Reading/calculating face flux field phi\n" << endl;              \
	surfaceScalarField phi                                                   \
	(                                                                        \
			IOobject                                                             \
			(                                                                    \
					"phi",                                                           \
					runTime.timeName(),                                              \
					mesh,                                                            \
					IOobject::READ_IF_PRESENT,                                       \
					IOobject::AUTO_WRITE                                             \
			),                                                                   \
			linearInterpolate(U) & mesh.Sf()                                     \
	);                                                                       \


#endif
