/*---------------------------------------------------------------------------*\
License
    This file is based on OpenFOAM.

    OpenFOAM is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "wallDist.H"
#include "nearWallDist.H"
#include "of2speedit.H"
#include "of2speedit_divSchemes.H"

#include "singlePhaseTransportModel.H"
#include "turbulenceModel.H"
#include "simpleControl.H"

#ifndef OF_VERSION
	#error "Undefined OpenFOAM version"
#endif

#if 1 == OF_VERSION || 2 == OF_VERSION
//	#warning "OpenFOAM version OK"
#else
	#error "Unsupported OpenFOAM version"
#endif

#if 2 == OF_VERSION
    #include "simpleControl.H"
#endif


int main(int argc, char *argv[])
{
	const char * dir_path = DATA_DIR_NAME ;
	create_directory(dir_path) ;

    try
	{
		LOAD_OF_CASE 
		#include "createTime.H"	
        check_application_name (runTime, "simpleFoam") ;
        #include "createMesh.H"


		Info<< "Reading transportProperties\n" << endl;                          
		IOdictionary transportProperties
		(
			IOobject
			(
				"transportProperties",
				runTime.constant(),
				mesh, 
				IOobject::MUST_READ,
				IOobject::NO_WRITE
			)
		);
		
        Info<< "Reading RASProperties\n" << endl;
        IOdictionary RASProperties
        (
            IOobject
            (
                "RASProperties",
                runTime.constant(),
                mesh,
                IOobject::MUST_READ,
                IOobject::NO_WRITE
            )
        );


		dimensionedScalar nu
		(
			transportProperties.lookup("nu")
		);

		Info<< "Reading field p\n" << endl;
		volScalarField p
		(
			IOobject
			(
				"p",
				runTime.timeName(),
				mesh,
				IOobject::MUST_READ,
				IOobject::AUTO_WRITE
			),
			mesh
		);

		Info<< "Reading field k\n" << endl;
		volScalarField k
		(
			IOobject
			(
				"k",
				runTime.timeName(),
				mesh,
				IOobject::MUST_READ,
				IOobject::AUTO_WRITE
			),
			mesh
		);

		Info<< "Reading field nut\n" << endl;
		volScalarField nut
		(
			IOobject
			(
				"nut",
				runTime.timeName(),
				mesh,
				IOobject::MUST_READ,
				IOobject::AUTO_WRITE
			),
			mesh
		);

		Info<< "Reading field omega\n" << endl;
		volScalarField omega
		(
			IOobject
			(
				"omega",
				runTime.timeName(),
				mesh,
				IOobject::MUST_READ,
				IOobject::AUTO_WRITE
			),
			mesh
		);

		Info<< "Reading field U\n" << endl;
		volVectorField U
		(
			IOobject
			(
				"U",
				runTime.timeName(),
				mesh,
				IOobject::MUST_READ,
				IOobject::AUTO_WRITE 
			),
			mesh
		);

		Info<< "Reading/calculating face flux field phi\n" << endl;
		surfaceScalarField phi
		(
			IOobject
			(
				"phi",
				runTime.timeName(),
				mesh,
				IOobject::READ_IF_PRESENT,
				IOobject::AUTO_WRITE
			),
			linearInterpolate(U) & mesh.Sf()
		);               

		Info<< "Calculating y (wallDistance)  field\n" << endl;
		wallDist	wallDistance
		(
			mesh,
			true
		);

        nearWallDist	nearWallDistance
        (
            mesh
        );

        volScalarField nearWallDistanceField
        (
                    "yNW", wallDistance
        );


        nearWallDistanceField.boundaryField() = nearWallDistance.y();

        label pRefCell = 0;
		scalar pRefValue = 0.0;
        setRefCell(p, mesh.solutionDict().subDict("SIMPLE"), pRefCell, pRefValue);

#if 2 == OF_VERSION
        simpleControl simple(mesh);
#endif
#if 1 == OF_VERSION
            #include "readSIMPLEControls.H"
#endif

		Info << "Saving settings\n" ;
		std::string filename = std::string(dir_path) + "/" + FILE_NAME_SETTINGS ; 
		std::ofstream file( filename.c_str() ) ;

        SAVE_SETTINGS_TURB( file, std::string( SOLVER_SIMPLE_TURBULENCE ) ) ;

        setting_name(file, SETTING_N_NONRTH_CORR_ITER) ;

#if 1 == OF_VERSION
        file << nNonOrthCorr << "\n" ;
#endif
#if 2 == OF_VERSION
        file << simple.nNonOrthCorr() << "\n" ;
#endif


#if (2 == OF_VERSION && 1 > OF_VERSION_MINOR ) || 1 == OF_VERSION
        if (U.mesh().relax(U.name()))
        {
            setting_name(file, SETTING_U_RELAXATION_FACTOR) << U.mesh().relaxationFactor(U.name()) << "\n" ;
        }
        if (p.mesh().relax(p.name()))
        {
            setting_name(file, SETTING_P_RELAXATION_FACTOR) << p.mesh().relaxationFactor(p.name()) << "\n" ;
        }

        if (k.mesh().relax(k.name()))
        {
             setting_name(file, SETTING_K_RELAXATION_FACTOR) << k.mesh().relaxationFactor(k.name()) << "\n" ;
        }
        if (omega.mesh().relax(omega.name()))
        {
            setting_name(file, SETTING_OMEGA_RELAXATION_FACTOR) << omega.mesh().relaxationFactor(omega.name()) << "\n" ;
        }
#endif
#if (2 == OF_VERSION && 1 <= OF_VERSION_MINOR )
        if (U.mesh().relaxEquation(U.name()))
        {
            setting_name(file, SETTING_U_RELAXATION_FACTOR) << U.mesh().equationRelaxationFactor(U.name()) << "\n" ;
        }
        if (p.mesh().relaxField(p.name()))
        {
            setting_name(file, SETTING_P_RELAXATION_FACTOR) << p.mesh().fieldRelaxationFactor(p.name()) << "\n" ;
        }

        if (k.mesh().relaxEquation(k.name()))
        {
             setting_name(file, SETTING_K_RELAXATION_FACTOR) << k.mesh().equationRelaxationFactor(k.name()) << "\n" ;
        }
        if (omega.mesh().relaxEquation(omega.name()))
        {
            setting_name(file, SETTING_OMEGA_RELAXATION_FACTOR) << omega.mesh().equationRelaxationFactor(omega.name()) << "\n" ;
        }
#endif

        divSchemeSetting(file, "div(phi,U)",                   SETTING_DIV_PHIU,     mesh);
        divSchemeSetting(file, "div(phi,k)",                   SETTING_DIV_PHIK,     mesh);
        divSchemeSetting(file, "div(phi,omega)",               SETTING_DIV_PHIOMEGA, mesh);
        divSchemeSetting(file, "div((nuEff*dev(T(grad(U)))))", SETTING_DIV_DEV_U,    mesh);



        dictionary kOmegaSSTCoeffsDict
        (
            RASProperties.subOrEmptyDict("kOmegaSSTCoeffs")
        );



        scalar alphaK1_ = kOmegaSSTCoeffsDict.lookupOrDefault<scalar>("alphaK1", 0.85034);
        setting_name(file, SETTING_KOMEGASST_ALPHAK1) << alphaK1_ << "\n" ;

        scalar alphaK2_ = kOmegaSSTCoeffsDict.lookupOrDefault<scalar>("alphaK2", 1.0);
        setting_name(file, SETTING_KOMEGASST_ALPHAK2) << alphaK2_ << "\n" ;

        scalar alphaOmega1_ = kOmegaSSTCoeffsDict.lookupOrDefault<scalar>("alphaOmega1", 0.5);
        setting_name(file, SETTING_KOMEGASST_ALPHAOMEGA1) << alphaOmega1_ << "\n" ;

        scalar alphaOmega2_ = kOmegaSSTCoeffsDict.lookupOrDefault<scalar>("alphaOmega2", 0.85616);
        setting_name(file, SETTING_KOMEGASST_ALPHAOMEGA2) << alphaOmega2_ << "\n" ;

        scalar gamma1_ = kOmegaSSTCoeffsDict.lookupOrDefault<scalar>("gamma1", 0.5532);
        setting_name(file, SETTING_KOMEGASST_GAMMA1) << gamma1_ << "\n" ;

        scalar gamma2_ = kOmegaSSTCoeffsDict.lookupOrDefault<scalar>("gamma2", 0.4403);
        setting_name(file, SETTING_KOMEGASST_GAMMA2) << gamma2_ << "\n" ;

        scalar beta1_ = kOmegaSSTCoeffsDict.lookupOrDefault<scalar>("beta1", 0.0750);
        setting_name(file, SETTING_KOMEGASST_BETA1) << beta1_ << "\n" ;

        scalar beta2_ = kOmegaSSTCoeffsDict.lookupOrDefault<scalar>("beta2", 0.0828);
        setting_name(file, SETTING_KOMEGASST_BETA2) << beta2_ << "\n" ;

        scalar betaStar_ = kOmegaSSTCoeffsDict.lookupOrDefault<scalar>("betaStar", 0.09);
        setting_name(file, SETTING_KOMEGASST_BETASTAR) << betaStar_ << "\n" ;

        scalar a1_ = kOmegaSSTCoeffsDict.lookupOrDefault<scalar>("a1", 0.31);
        setting_name(file, SETTING_KOMEGASST_A1) << a1_ << "\n" ;

        scalar c1_ = kOmegaSSTCoeffsDict.lookupOrDefault<scalar>("c1", 10);
        setting_name(file, SETTING_KOMEGASST_C1) << c1_ << "\n" ;

        scalar Cmu_ = kOmegaSSTCoeffsDict.lookupOrDefault<scalar>("Cmu", 0.09);
        setting_name(file, SETTING_KOMEGASST_CMU) << Cmu_ << "\n" ;

        bool kappaESaved = false;


        scalar kappa_ = 0.41;
        scalar E_ = 9.8;

        forAll(nut.boundaryField(), PatchI)
        {

            Info << nut.boundaryField()[PatchI].patch().name() << " " << nut.boundaryField()[PatchI].type()  << "\n";
            if ((nut.boundaryField()[PatchI].type() == "nutkWallFunction" || nut.boundaryField()[PatchI].type() == "mynutkWallFunction")&& kappaESaved == false)
            {

                setting_name(file, SETTING_KOMEGASST_KAPPA) << kappa_ << "\n" ;

                setting_name(file, SETTING_KOMEGASST_E) << E_ << "\n" ;

                kappaESaved = true;

            }


        }

        scalar ypl = 11.0;

        for (int i=0; i<10; i++)
        {
            ypl = ::log(E_*ypl)/kappa_;
        }

        scalar yPlusLam = ypl;

        setting_name(file, SETTING_YPLUSLAM) << yPlusLam << "\n" ;


        Info << "Saving mesh  in SpeedIT format\n" ;
		save_mesh  (      mesh, dir_path) ;
        Info << "Saving p     in SpeedIT format\n" ;
		save_field ( p   , mesh, dir_path, & runTime) ;

        Info << "Saving U     in SpeedIT format\n" ;
		save_field ( U   , mesh, dir_path, & runTime) ;
        Info << "Saving phi   in SpeedIT format\n" ;
		save_field ( phi , mesh, dir_path, & runTime) ; // used only to write information about boundary patches
	
        Info << "Saving k                in SpeedIT format\n" ;
		save_field ( k	, mesh, dir_path, & runTime );
        Info << "Saving omega            in SpeedIT format\n" ;
		save_field ( omega	, mesh, dir_path, & runTime );
        Info << "Saving wallDistance     in SpeedIT format\n" ;
		save_field ( wallDistance, mesh, dir_path, & runTime );
        Info << "Saving nearWallDistance in SpeedIT format\n" ;
        save_field ( nearWallDistanceField, mesh, dir_path, & runTime );
        Info << "Saving nut              in SpeedIT format\n" ;
        save_field( nut, mesh, dir_path, & runTime );

        Info << "wallDistance.name() = " << wallDistance.name() << "\n";
        Info << "nearWallDistanceField.name() = " << nearWallDistanceField.name() << "\n";


	}                                                                     
	catch (std::string & err_str)                                         
	{                                                                     
		handle_error(err_str, dir_path) ;
	} ;  

	return 0 ;
} ;
