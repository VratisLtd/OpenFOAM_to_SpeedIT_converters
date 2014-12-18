#ifndef __OF2SPEEDIT_BOUNDARY_H__
#define __OF2SPEEDIT_BOUNDARY_H__

#include "fvCFD.H"
#include <vector>

#include "inletOutletFvPatchField.H"
#include "totalPressureFvPatchScalarField.H"


class BFaceValues {
	public:
		std::vector< std::vector<double> > vals  ;
		std::vector< std::vector<int   > > types ;
        std::vector< std::vector<double> > refVals  ; 
} ;

// WARNING - The values defined here MUST be identical
//           with values from arael.h file
enum BoundaryFaceType {
	
	FIXED_VALUE                = 0   ,
	CALCULATED                 = 1   , 
	FIXED_VALUE_USER_BEGIN     = 2   ,
    SLIP_SYMMETRY              = 3   ,
    TOTALPRESSURE              = 4   ,

    NUT_WALL_FUNCTION           = 65 ,

    INLETOUTLET                = 96,

	ZERO_GRADIENT              = 128 ,
	FIXED_GRADIENT             = 129 ,
	FIXED_GRADIENT_USER_BEGIN  = 130 ,
    OMEGA_WALL_FUNCTION        = 131 ,

	UNKNOWN                    = 255
} ;

std::vector<int>
bfaces_reorder_table (const fvMesh & mesh) ;

std::vector<int>
bfaces_reorder_table_ak (const fvMesh & mesh) ;

BoundaryFaceType
bface_type ( std::string type_name) ;

BFaceValues
boundary_values ( const volScalarField & field, bool values_only = false ) ;
BFaceValues
boundary_values ( const volVectorField & field, bool values_only = false ) ;
BFaceValues
boundary_values ( const surfaceScalarField & field, bool values_only = false ) ;
BFaceValues
boundary_values ( const surfaceVectorField & field, bool values_only = false ) ;

template<typename T>
struct SetZeroHelper
{
};

template<>
struct SetZeroHelper<scalar>
{
    static scalar set()
    {
        return scalar(0);// val = 0;
    }
};

template<>
struct SetZeroHelper<vector>
{
    static vector set()
    {
        return vector(0, 0, 0);
    }
};

template<typename VAL_TYPE, typename TYPE>
struct NewHelper{};

template<typename TYPE>
struct NewHelper<scalar, TYPE>
{
    static void run(const TYPE& field, TYPE& rVField, int z)
    {
        totalPressureFvPatchScalarField& kpatch = (totalPressureFvPatchScalarField&)(field.boundaryField()[z]);
        rVField.boundaryField()[z] = kpatch.p0();
        cout << "scalarField\n";
    }
};

template<typename TYPE>
struct NewHelper<vector, TYPE>
{
    static void run(const TYPE& val, TYPE& rVField, int z)
    {

    }
};

template<class TField>
struct isScalarField
{
    static const bool value = false;
};

template<>
struct isScalarField< scalar  >
{
    static const bool value = true;
};


#endif
