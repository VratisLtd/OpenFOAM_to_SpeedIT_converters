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
#include "of2speedit.H"
#include "of2speedit_divSchemes.H"

#ifndef OF_VERSION
    #error "Undefined OpenFOAM version"
#endif

#if 1 == OF_VERSION || 2 == OF_VERSION
//	#warning "OpenFOAM version OK"
#else
    #error "Unsupported OpenFOAM version"
#endif

int main(int argc, char *argv[])
{
	const char * dir_path = DATA_DIR_NAME ;
	create_directory(dir_path) ;
	
	try
	{
		LOAD_OF_CASE 
		#include "createTime.H"	
		check_application_name (runTime, "icoFoam") ;
		#include "createMesh.H"
		LOAD_OF_FIELDS

		label pRefCell = 0;
		scalar pRefValue = 0.0;
		setRefCell(p, mesh.solutionDict().subDict("PISO"), pRefCell, pRefValue);	

		#include "readPISOControls.H"

		Info << "Saving settings\n" ;
		std::string filename = std::string(dir_path) + "/" + FILE_NAME_SETTINGS ; 
		std::ofstream file( filename.c_str() ) ;
		SAVE_SETTINGS_COMMON( file, std::string(SOLVER_PISO) ) ;

		setting_name(file, SETTINGS_DELTA_T) ;
		file << runTime.deltaT().value() << "\n" ;
		setting_name(file, SETTING_N_PISO_CORR_ITER  ) << nCorr        << "\n" ; 
		setting_name(file, SETTING_N_NONRTH_CORR_ITER) << nNonOrthCorr << "\n" ;
		
		divSchemeSetting(file, "div(phi,U)", SETTING_DIV_PHIU, mesh);

		Info << "Saving mesh in SpeedIT format\n" ;
		save_mesh  (      mesh, dir_path) ;
		Info << "Saving p    in SpeedIT format\n" ;
		save_field (p   , mesh, dir_path, & runTime) ;
		Info << "Saving U    in SpeedIT format\n" ;
		save_field (U   , mesh, dir_path, & runTime) ;
		Info << "Saving phi  in SpeedIT format\n" ;
		save_field (phi , mesh, dir_path, & runTime) ; // used only to write information about boundary patches
	}                                                                     
	catch (std::string & err_str)                                         
	{                                                                     
		handle_error(err_str, dir_path) ;
	} ;  

	return 0 ;
} ;
