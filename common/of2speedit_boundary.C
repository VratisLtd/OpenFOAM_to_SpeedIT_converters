#include "of2speedit_boundary.H"
#include "of2speedit_utils.H"


class BFacePos {
	public:
		int    pos   ;
		int    owner ;
} ;
class sort_op
{
	public:
		bool operator() (BFacePos const & a, BFacePos const & b) {
			return a.owner < b.owner ;
		} ;
} ;

std::vector<int>
bfaces_reorder_table (const fvMesh & mesh)
{
	std::vector< BFacePos > vbp ; 
	int pos = 0 ;

	for   (int z=0 ; z < mesh.boundary()   .size() ; z++) {
		for (int i=0 ; i < mesh.boundary()[z].size() ; i++) {

			BFacePos bp ;

			bp.pos   = pos                               ;
			bp.owner = mesh.boundary()[z].faceCells()[i] ;

			pos ++ ;

			vbp.push_back(bp) ;
		} ;
	} ;

	//std::sort( vbp.begin(), vbp.end(), sort_op() ) ;
	std::stable_sort( vbp.begin(), vbp.end(), sort_op() ) ;

	std::vector<int> rt (vbp.size()) ;

	for (unsigned int i=0 ; i<rt.size() ; i++) {
		rt[i] = vbp[i].pos ;
	} ;

	return rt ;
} ;

std::vector<int>
bfaces_reorder_table_ak (const fvMesh & mesh)
{

    int pos = 0 ;
    int pos_begin = 0;

    std::vector<int> rt;

    for   (int z=0 ; z < mesh.boundary()   .size() ; z++) {

        std::vector< BFacePos > vbp ;

        for (int i=0 ; i < mesh.boundary()[z].size() ; i++) {



            BFacePos bp ;

            bp.pos   = pos                               ;
            bp.owner = mesh.boundary()[z].faceCells()[i] ;

            pos ++ ;

            vbp.push_back(bp) ;


        } ;

        //std::sort( vbp.begin(), vbp.end(), sort_op() ) ;
        std::stable_sort( vbp.begin(), vbp.end(), sort_op() ) ;

        for (unsigned int i=0 ; i<vbp.size() ; i++) {
            rt.push_back(vbp[i].pos) ;
        } ;

        pos_begin +=vbp.size();

    } ;



    return rt ;
} ;

BoundaryFaceType
bface_type ( std::string type_name)
{
	if (
            "zeroGradient"      == type_name ||
            "kqRWallFunction"   == type_name
		 ) 
	{
		return ZERO_GRADIENT ;
	} else if (
			"fixedValue"                   	== type_name ||
			"surfaceNormalFixedValue"	== type_name ||	
			"timeVaryingUniformFixedValue" 	== type_name ||
               		"groovyBC"	== type_name 
			) 
	{
		return FIXED_VALUE   ;
    }
    else if (
                   "calculated"   == type_name
                  )
    {
            return CALCULATED    ;
    }
    else if( "omegaWallFunction"                   	== type_name)
    {
        return OMEGA_WALL_FUNCTION;
    }
    else if( "nutkWallFunction"                   	== type_name )
    {
        return NUT_WALL_FUNCTION;
    }
    else if ("slip"                         	== type_name ||
             "symmetryPlane"                         == type_name )
    {
        return SLIP_SYMMETRY;
    }
    else if ("totalPressure"                         	== type_name )
    {
        return TOTALPRESSURE;
    }
    else if ("inletOutlet"                         	== type_name )
    {
        return INLETOUTLET;
    }
    else if (
	    "empty"        == type_name
	    )
	{
		return UNKNOWN ; // TODO: Maybe should be special type "EMPTY" ?
	} else
	{
        throw std::string("Unsupported boundary face type \"" + type_name + "\". The only supported boundary face types are \"zeroGradient\", \"fixedValue\", \"calculated\", \"kqRWallFunctinon\", \"omegaWallFunctinon\", \"nutWallFunctinon\", \"timeVaryingUniformFixedValue\", \"slip\" and \"inletOutlet\" ") ;
	} ;
} ;
template<template<class> class Field, class Type>
int 
number_of_elements (
		FieldField<Field, Type> const & field
		)
{
	int n = 0 ;

	for (int z=0 ; z<field.size() ; z++) {
		n += field[z].size() ;
	} ;

	return n ;
} ;

template<template<class> class Field>
void 
init_vec (
           std::vector< std::vector<double> >       & vec   ,
					 FieldField< Field, scalar >        const & field
         )
{
	int nbfaces = number_of_elements( field ) ;

	vec   .resize(1) ;
	vec[0].resize( nbfaces ) ;
} ;

template<template<class> class Field>
void 
init_vec (
           std::vector< std::vector<double> >       & vec   ,
					 FieldField< Field, vector >        const & field
         )
{
	int nbfaces = number_of_elements( field ) ;

	vec   .resize(3) ;
	vec[0].resize( nbfaces ) ;
	vec[1].resize( nbfaces ) ;
	vec[2].resize( nbfaces ) ;
} ;

template<template<class> class Field, class Type>
std::vector< std::vector<double> >
field_values(
						 FieldField<Field, Type> const & field
            )
{
	std::vector< std::vector<double> > vres ;

	init_vec(vres, field) ;

	int idx = 0 ;
	for (int z=0 ; z < field.size() ; z++) {

		for (int i=0 ; i < field[z].size() ; i++) {

			set_vec_val(vres, field[ z ][ i ], idx) ;

			idx ++ ;
		} ;
	} ;

	return vres ;
} ;

template<template<class> class Field, class Type>
std::vector< std::vector<double> >
field_values_reordered(
						 FieldField<Field, Type> const & field,
						 std::vector<int>              & reorder_table
            )
{
	return vector_reorder( field_values(field), reorder_table ) ;
} ;

template<class TField>
BFaceValues
boundary_values ( const TField & field, bool values_only)
{
	if (
			typeid(TField) != typeid( volScalarField     ) &&
			typeid(TField) != typeid( volVectorField     ) &&
			typeid(TField) != typeid( surfaceScalarField ) &&
			typeid(TField) != typeid( surfaceVectorField )
		 ) 
	{
		throw std::string("Unsupported boundary field data type") ;
	} ;

    std::vector< int > vbp = bfaces_reorder_table(field.mesh()) ; 

	int nbfaces = vbp.size() ;
	
	BFaceValues result ;

	result.vals = field_values_reordered(field.boundaryField(), vbp) ;

	if (false == values_only)
	{
		std::vector< int > types( nbfaces ) ;

		int idx = 0 ;
		for (int z=0 ; z < field.boundaryField().size() ; z++) 
		{
			BoundaryFaceType ft = bface_type( std::string(field.boundaryField().types()[z]) ) ;

			for (int i=0 ; i < field.boundaryField()[z].size() ; i++) 
			{
				types[idx] = ft ;
				idx ++ ;
			} ;
		} ;

		result.types.resize( 1 ) ;
		result.types[0] = vector_reorder( types, vbp ) ;
	} ;


    TField rVField
    (
        IOobject
        (
            "rVField",
            //runTime.timeName(),
            field.mesh(),
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        field.mesh(),
        SetZeroHelper<typename TField::value_type>::set()
    );

    //TField rVField(temp);

    for (int z=0 ; z < field.boundaryField().size() ; z++) {

        if (field.boundaryField()[z].type() == "inletOutlet")
        {
            inletOutletFvPatchField<typename TField::value_type>& kpatch = (inletOutletFvPatchField<typename TField::value_type>&)(field.boundaryField()[z]);
            rVField.boundaryField()[z] = kpatch.refValue();
        }
//        else
//        {
//            rVField.boundaryField()[z] = SetZeroHelper<typename TField::value_type>::set();

//            //typedef typename TField::value_type myType;//defaultVal;
////            rVField.boundaryField()[z] = myType::zero;//defaultVal;
//        }

    }

    result.refVals = field_values_reordered(rVField.boundaryField(), vbp) ;

	return result ;
} ;

BFaceValues
boundary_values ( const volScalarField & field, bool values_only )
{
	return boundary_values<volScalarField> ( field, values_only ) ;
} ;
BFaceValues
boundary_values ( const volVectorField & field, bool values_only )
{
	return boundary_values<volVectorField> ( field, values_only ) ;
} ;
BFaceValues
boundary_values ( const surfaceScalarField & field, bool values_only )
{
	return boundary_values<surfaceScalarField> ( field, values_only ) ;
} ;
BFaceValues
boundary_values ( const surfaceVectorField & field, bool values_only )
{
	return boundary_values<surfaceVectorField> ( field, values_only ) ;
} ;

