#ifndef __OF2SPEEDIT_TIMEVARYING_BC_H__
#define __OF2SPEEDIT_TIMEVARYING_BC_H__

#include "fvCFD.H"
#include "vector2DField.H"
#include "IFstream.H"

#include "of2speedit.H"
#include "of2speedit_boundary.H"
#include "of2speedit_io.H"
#include <vector>
#include <fstream>
#include <sstream>
#include <string>


template<class TField>
bool
has_timevarying_bc( const TField & field )
{
    for (int i=0 ; i<field.boundaryField().size() ; i++)
    {
        if (
                    "timeVaryingUniformFixedValue" == field.boundaryField().types()[i]
             )
        {
            return true ;
        } ;
    } ;

    return false ;
} ;

template<class TField>
bool
has_groovy_bc( const TField & field )                           
{
    for (int i=0 ; i<field.boundaryField().size() ; i++)
    {
        if (
                    "groovyBC" == field.boundaryField().types()[i]
             )
        {
            return true ;
        } ;
    } ;

    return false ;
} ;

template<class TField>
bool
has_totalPressure_bc( const TField & field )                    
{
    for (int i=0 ; i<field.boundaryField().size() ; i++)
    {
        if (
                    "totalPressure" == field.boundaryField().types()[i]
             )
        {
            return true ;
        } ;
    } ;

    return false ;
} ;


int
n_components( const volScalarField     & field ) ;
int
n_components( const volVectorField     & field ) ;
int
n_components( const surfaceScalarField & field ) ;

bool
is_time_varying( const std::string & type ) ;

bool
is_groovy( const std::string & type ) ;

bool
is_totalPressure( const std::string & type ) ;


// On GPU side all boundary faces are stored in one flat array, which has to be sorted
// by face owner cell indices. Due to sorting faces from OpenFOAM boundary patches are
// usually scattered in various positions. Thus, for each OpenFOAM time varying patch
// we need a full list of boundary face indices in GPU memory.
class
TimeVaryingBoundaryPatch
{
    public:

        TimeVaryingBoundaryPatch() : initialized(false)
        {} ;

        bool             initialized ;
        std::vector<int> face_positions ; // Indices of boundary faces in GPU memory, which should be changed

        std::vector<              // consecutive time steps
      std::vector<            // boundary face values for single time step - each component (x, y, z)
              std::vector<double>   // is a vector on current level
                       >
                   >     face_values   ;
        // Example: for 10 time steps and 12 faces values, which are 3D vectors
        //          face_values is an array of dimensions [10][3][12]
        // This way allows to save memory and disc space, when all face values for given
        // time step are the same.
} ;
class
GroovyBoundaryPatch                                                  
{
    public:

        GroovyBoundaryPatch() : initialized(false)
        {} ;

        bool             initialized ;
        std::vector<int> face_positions ; // Indices of boundary faces in GPU memory, which should be changed

        //std::vector<              // consecutive time steps  <- for groovy take only initial time step 
      std::vector<            // boundary face values for single time step - each component (x, y, z)
              std::vector<double>   // is a vector on current level
                       //>                                  
                   >     face_values   ;

        // This way allows to save memory and disc space, when all face values for given
        // time step are the same.
} ;
class
totalPressureBoundaryPatch                                             
{
    public:

        totalPressureBoundaryPatch() : initialized(false)
        {} ;

        bool             initialized ;
        std::vector<int> face_positions ; // Indices of boundary faces in GPU memory, which should be changed

        //std::vector<              // consecutive time steps  <- for groovy take only initial time step 
      std::vector<            // boundary face values for single time step - each component (x, y, z)
              std::vector<double>   // is a vector on current level
                       //>                                  
                   >     face_values   ;

        // This way allows to save memory and disc space, when all face values for given
        // time step are the same.
} ;


template<class TField>
void
build_time_varying_patch_face_indices(
                    TimeVaryingBoundaryPatch & patch ,
                            const TField                   & field ,
                            const Foam::fvMesh             & mesh  ,
                                  int                        bfpatch_idx
)
{
    // Obtain indices into the aggregated, unsorted array containing consecutive OpenFOAM patches.
    std::vector<int> unsorted_face_positions ;
    int idx = 0 ;
    for (int i=0 ; i < bfpatch_idx  ; i++) // Find start position (skip all previous patches)
    {
        idx += field.boundaryField()[i].size() ;
    } ;
    for (int i=0 ; i < field.boundaryField()[bfpatch_idx].size() ; i++) // Add all faces from current patch
    {
        unsorted_face_positions.push_back( idx + i ) ;
    } ;

    // Compute indices after sorting ALL OpenFOAM boundary faces by owner cell indices

    // bfaces_reorder_table(mesh) contains information, from which position in unsorted
    // array to GET value to produce sorted array. Thus, bfaces_reorder_table() is useful,
    // when sorted array of values has to be produced from aggregated OpenFOAM boundary
    // faces.
    std::vector<int> bf_reorder_table = bfaces_reorder_table( mesh ) ;
    // However, we need information, where to PUT value from given position in unsorted,
    // aggregated array. Thus, for each element in unsorted array all bfaces_reorder_table
    // should be traversed to find index of the element. Traversing can be avoided
    // through building the reversed bfaces_reorder_table, i.e. the array, which contains
    // information, where to PUT values from given position in unsorted, aggregated array
    // created by gluing OpenFOAM boundary patches.
    std::vector<int> bf_reorder_table_2 ;
    bf_reorder_table_2.assign( bf_reorder_table.size(), -1) ;
    for ( unsigned int i=0 ; i<bf_reorder_table.size() ; i++ )
    {
        bf_reorder_table_2[ bf_reorder_table[i] ] = i ;
    } ;
    // Now we can create an array, which has information, where to put values in GPU memory
    // for only some boundary faces (taken from one OpenFOAM patch).
    for (unsigned int i=0 ; i < unsorted_face_positions.size() ; i++)
    {
        patch.face_positions.push_back( bf_reorder_table_2[ unsorted_face_positions[ i ] ] ) ;
    } ;
} ;

template<class TField>
void
build_groovy_patch_face_indices(                                                //ak20130704
                    GroovyBoundaryPatch & patch ,
                            const TField                   & field ,
                            const Foam::fvMesh             & mesh  ,
                                  int                        bfpatch_idx
)
{
    // Obtain indices into the aggregated, unsorted array containing consecutive OpenFOAM patches.
    std::vector<int> unsorted_face_positions ;
    int idx = 0 ;
    for (int i=0 ; i < bfpatch_idx  ; i++) // Find start position (skip all previous patches)
    {
        idx += field.boundaryField()[i].size() ;
    } ;
    for (int i=0 ; i < field.boundaryField()[bfpatch_idx].size() ; i++) // Add all faces from current patch
    {
        unsorted_face_positions.push_back( idx + i ) ;
    } ;

    // Compute indices after sorting ALL OpenFOAM boundary faces by owner cell indices

    // bfaces_reorder_table(mesh) contains information, from which position in unsorted
    // array to GET value to produce sorted array. Thus, bfaces_reorder_table() is useful,
    // when sorted array of values has to be produced from aggregated OpenFOAM boundary
    // faces.
    std::vector<int> bf_reorder_table = bfaces_reorder_table( mesh ) ;
    // However, we need information, where to PUT value from given position in unsorted,
    // aggregated array. Thus, for each element in unsorted array all bfaces_reorder_table
    // should be traversed to find index of the element. Traversing can be avoided
    // through building the reversed bfaces_reorder_table, i.e. the array, which contains
    // information, where to PUT values from given position in unsorted, aggregated array
    // created by gluing OpenFOAM boundary patches.
    std::vector<int> bf_reorder_table_2 ;
    bf_reorder_table_2.assign( bf_reorder_table.size(), -1) ;
    for ( unsigned int i=0 ; i<bf_reorder_table.size() ; i++ )
    {
        bf_reorder_table_2[ bf_reorder_table[i] ] = i ;
    } ;
    // Now we can create an array, which has information, where to put values in GPU memory
    // for only some boundary faces (taken from one OpenFOAM patch).
    for (unsigned int i=0 ; i < unsorted_face_positions.size() ; i++)
    {
        patch.face_positions.push_back( bf_reorder_table_2[ unsorted_face_positions[ i ] ] ) ;
    } ;
} ;

template<class TField>
void
build_totalPressure_patch_face_indices(                                                
                    totalPressureBoundaryPatch & patch ,
                            const TField                   & field ,
                            const Foam::fvMesh             & mesh  ,
                                  int                        bfpatch_idx
)
{
    // Obtain indices into the aggregated, unsorted array containing consecutive OpenFOAM patches.
    std::vector<int> unsorted_face_positions ;
    int idx = 0 ;
    for (int i=0 ; i < bfpatch_idx  ; i++) // Find start position (skip all previous patches)
    {
        idx += field.boundaryField()[i].size() ;
    } ;
    for (int i=0 ; i < field.boundaryField()[bfpatch_idx].size() ; i++) // Add all faces from current patch
    {
        unsorted_face_positions.push_back( idx + i ) ;
    } ;

    // Compute indices after sorting ALL OpenFOAM boundary faces by owner cell indices

    // bfaces_reorder_table(mesh) contains information, from which position in unsorted
    // array to GET value to produce sorted array. Thus, bfaces_reorder_table() is useful,
    // when sorted array of values has to be produced from aggregated OpenFOAM boundary
    // faces.
    std::vector<int> bf_reorder_table = bfaces_reorder_table( mesh ) ;
    // However, we need information, where to PUT value from given position in unsorted,
    // aggregated array. Thus, for each element in unsorted array all bfaces_reorder_table
    // should be traversed to find index of the element. Traversing can be avoided
    // through building the reversed bfaces_reorder_table, i.e. the array, which contains
    // information, where to PUT values from given position in unsorted, aggregated array
    // created by gluing OpenFOAM boundary patches.
    std::vector<int> bf_reorder_table_2 ;
    bf_reorder_table_2.assign( bf_reorder_table.size(), -1) ;
    for ( unsigned int i=0 ; i<bf_reorder_table.size() ; i++ )
    {
        bf_reorder_table_2[ bf_reorder_table[i] ] = i ;
    } ;
    // Now we can create an array, which has information, where to put values in GPU memory
    // for only some boundary faces (taken from one OpenFOAM patch).
    for (unsigned int i=0 ; i < unsorted_face_positions.size() ; i++)
    {
        patch.face_positions.push_back( bf_reorder_table_2[ unsorted_face_positions[ i ] ] ) ;
    } ;
} ;

inline
Foam::scalar
get_component( const Foam::vector & v, int cidx )
{
    return v.component( cidx ) ;
} ;
inline
Foam::scalar
get_component( double d, int cidx )
{
    if (0 != cidx) throw std::string("Can not extract component greater than 0 from scalar") ;
    return d ;
} ;


template<class TField>
void
build_time_varying_patch_face_values(
                    TimeVaryingBoundaryPatch & patch   ,
                                  TField                   & field   ,
                            const Foam::fvMesh             & mesh    ,
                                  Foam::Time               & runTime ,
                                  int                        bfpatch_idx
)
{
    unsigned int n_time_steps = (N_TIME_STEPS) ;

    patch.face_values.resize(n_time_steps) ;

    dimensionedScalar start_time       = runTime.startTime() ;
    label             start_time_index = runTime.startTimeIndex() ;

    field.correctBoundaryConditions();

    unsigned int time_idx = 0 ;
    while (runTime.loop())
    {
        cout << "<<<<<<<<< Saving time step " << time_idx << "\n" ;

        field.correctBoundaryConditions() ;

        patch.face_values[ time_idx ].resize( n_components( field ) ) ;

        int nfaces_in_patch = field.boundaryField()[ bfpatch_idx ].size() ;

        Info << gMax(field.boundaryField()[ bfpatch_idx ]) << endl;

        // Each component is treated separately
        for (int c=0 ; c < n_components(field) ; c++)
        {
            // Test, if all values are the same. Allows for optimisation of memory usage.
            bool identical = true ;
            for(int fidx=0 ; fidx < nfaces_in_patch ; fidx++)
            {
                if ( get_component( field.boundaryField()[ bfpatch_idx ][ fidx ], c ) !=
                       get_component( field.boundaryField()[ bfpatch_idx ][ 0    ], c ) )
                {
                    identical = false ;
                    break ;
                } ;
            } ;

            // Extract values from OpenFOAM boundary faces
            if ( identical )
            {
                patch.face_values[ time_idx ][ c ].resize( 1 ) ;
                patch.face_values[ time_idx ][ c ][ 0 ] =
                    get_component( field.boundaryField()[ bfpatch_idx ][ 0 ], c ) ;

            } else {

                patch.face_values[ time_idx ][ c ].resize( nfaces_in_patch ) ;
                for(int fidx=0 ; fidx < nfaces_in_patch ; fidx++)
                {
                    patch.face_values[ time_idx ][ c ][ fidx ] =
                        get_component( field.boundaryField()[ bfpatch_idx ][ fidx ], c ) ;
                } ;
            } ;
        } ;

        time_idx ++ ;
    } ;

    runTime.setTime( start_time, start_time_index ) ;
    field.correctBoundaryConditions();
} ;

template<class TField>
void
build_groovy_patch_face_values(                                                 
                    GroovyBoundaryPatch & patch   ,
                                  TField                   & field   ,
                            const Foam::fvMesh             & mesh    ,
                                  Foam::Time               & runTime ,
                                  int                        bfpatch_idx
)
{
    //unsigned int n_time_steps = (N_TIME_STEPS) ;

    //patch.face_values.resize(n_time_steps) ;

    dimensionedScalar start_time       = runTime.startTime() ;
    label             start_time_index = runTime.startTimeIndex() ;

    field.correctBoundaryConditions();

    unsigned int time_idx = 0 ;
    //while (runTime.loop())
    //{
        cout << "<<<<<<<<< Saving time step " << time_idx << "\n" ;

        field.correctBoundaryConditions() ;

        patch.face_values.resize( n_components( field ) ) ;

        int nfaces_in_patch = field.boundaryField()[ bfpatch_idx ].size() ;

        //Info << field.boundaryField()[ bfpatch_idx ] << endl; //Info << gMax(field.boundaryField()[ bfpatch_idx ]) << endl;

        // Each component is treated separately
        for (int c=0 ; c < n_components(field) ; c++)
        {
            // Test, if all values are the same. Allows for optimisation of memory usage.
            bool identical = true ;
            for(int fidx=0 ; fidx < nfaces_in_patch ; fidx++)
            {
                if ( get_component( field.boundaryField()[ bfpatch_idx ][ fidx ], c ) !=
                       get_component( field.boundaryField()[ bfpatch_idx ][ 0    ], c ) )
                {
                    identical = false ;
                    break ;
                } ;
            } ;

            // Extract values from OpenFOAM boundary faces
            if ( identical )
            {
                patch.face_values[ c ].resize( 1 ) ;
                patch.face_values[ c ][ 0 ] =
                    get_component( field.boundaryField()[ bfpatch_idx ][ 0 ], c ) ;

            } else {

                patch.face_values[ c ].resize( nfaces_in_patch ) ;
                for(int fidx=0 ; fidx < nfaces_in_patch ; fidx++)
                {
                    patch.face_values[ c ][ fidx ] =
                        get_component( field.boundaryField()[ bfpatch_idx ][ fidx ], c ) ;
                } ;
            } ;
        } ;

        //time_idx ++ ;
    //} ;

    runTime.setTime( start_time, start_time_index ) ;
    field.correctBoundaryConditions();
} ;

template<class TField>
void
build_totalPressure_patch_face_values(                                                 
                    totalPressureBoundaryPatch & patch   ,
                                  TField                   & field   ,
                            const Foam::fvMesh             & mesh    ,
                                  Foam::Time               & runTime ,
                                  int                        bfpatch_idx
)
{
    //unsigned int n_time_steps = (N_TIME_STEPS) ;

    //patch.face_values.resize(n_time_steps) ;

    dimensionedScalar start_time       = runTime.startTime() ;
    label             start_time_index = runTime.startTimeIndex() ;

    field.correctBoundaryConditions();

    unsigned int time_idx = 0 ;
    //while (runTime.loop())
    //{
        cout << "<<<<<<<<< Saving time step " << time_idx << "\n" ;

        field.correctBoundaryConditions() ;

        patch.face_values.resize( n_components( field ) ) ;

        int nfaces_in_patch = field.boundaryField()[ bfpatch_idx ].size() ;

        //Info << field.boundaryField()[ bfpatch_idx ] << endl; //Info << gMax(field.boundaryField()[ bfpatch_idx ]) << endl;

        // Each component is treated separately
        for (int c=0 ; c < n_components(field) ; c++)
        {
            // Test, if all values are the same. Allows for optimisation of memory usage.
            bool identical = true ;
            for(int fidx=0 ; fidx < nfaces_in_patch ; fidx++)
            {
                if ( get_component( field.boundaryField()[ bfpatch_idx ][ fidx ], c ) !=
                       get_component( field.boundaryField()[ bfpatch_idx ][ 0    ], c ) )
                {
                    identical = false ;
                    break ;
                } ;
            } ;

            // Extract values from OpenFOAM boundary faces
            if ( identical )
            {
                patch.face_values[ c ].resize( 1 ) ;
                patch.face_values[ c ][ 0 ] =
                    get_component( field.boundaryField()[ bfpatch_idx ][ 0 ], c ) ;

            } else {

                patch.face_values[ c ].resize( nfaces_in_patch ) ;
                for(int fidx=0 ; fidx < nfaces_in_patch ; fidx++)
                {
                    patch.face_values[ c ][ fidx ] =
                        get_component( field.boundaryField()[ bfpatch_idx ][ fidx ], c ) ;
                } ;
            } ;
        } ;

        //time_idx ++ ;
    //} ;

    runTime.setTime( start_time, start_time_index ) ;
    field.correctBoundaryConditions();
} ;


template<class TField>
void
build_time_varying_boundary_patch (
                    TimeVaryingBoundaryPatch & patch   ,
                                  TField                   & field   ,
                            const Foam::fvMesh             & mesh    ,
                                  Foam::Time               & runTime ,
                                  int                        bfpatch_idx
)
{
    patch.initialized = false ;

    if ( false == is_time_varying( field.boundaryField().types()[ bfpatch_idx ] ) )
        throw std::string("Can not build time varying boundary patch from OpenFOAM patch of type \"") +
              field.boundaryField().types()[ bfpatch_idx ] + "\", possibly wrong patch index passed" ;

    if ( (0 > bfpatch_idx) || (field.boundaryField().size() <= bfpatch_idx) )
        throw std::string("Invalid patch index passed") ;

    build_time_varying_patch_face_indices( patch, field, mesh, bfpatch_idx ) ;
    build_time_varying_patch_face_values ( patch, field, mesh, runTime, bfpatch_idx ) ;

    patch.initialized = true ;
} ;

template<class TField>
void
build_groovy_boundary_patch (                                               //ak20130704
                    GroovyBoundaryPatch & patch   ,
                                  TField                   & field   ,
                            const Foam::fvMesh             & mesh    ,
                                  Foam::Time               & runTime ,
                                  int                        bfpatch_idx
)
{
    patch.initialized = false ;

    if ( false == is_groovy( field.boundaryField().types()[ bfpatch_idx ] ) )
        throw std::string("Can not build groovy boundary patch from OpenFOAM patch of type \"") +
              field.boundaryField().types()[ bfpatch_idx ] + "\", possibly wrong patch index passed" ;

    if ( (0 > bfpatch_idx) || (field.boundaryField().size() <= bfpatch_idx) )
        throw std::string("Invalid patch index passed") ;

    build_groovy_patch_face_indices( patch, field, mesh, bfpatch_idx ) ;
    build_groovy_patch_face_values ( patch, field, mesh, runTime, bfpatch_idx ) ;

    patch.initialized = true ;
} ;

template<class TField>
void
build_totalPressure_boundary_patch (                                             
                    totalPressureBoundaryPatch & patch   ,
                                  TField                   & field   ,
                            const Foam::fvMesh             & mesh    ,
                                  Foam::Time               & runTime ,
                                  int                        bfpatch_idx
)
{
    patch.initialized = false ;

    if ( false == is_totalPressure( field.boundaryField().types()[ bfpatch_idx ] ) )
        throw std::string("Can not build totalPressure boundary patch from OpenFOAM patch of type \"") +
              field.boundaryField().types()[ bfpatch_idx ] + "\", possibly wrong patch index passed" ;

    if ( (0 > bfpatch_idx) || (field.boundaryField().size() <= bfpatch_idx) )
        throw std::string("Invalid patch index passed") ;

    build_totalPressure_patch_face_indices( patch, field, mesh, bfpatch_idx ) ;
    build_totalPressure_patch_face_values ( patch, field, mesh, runTime, bfpatch_idx ) ;

    patch.initialized = true ;
} ;

// Function returns an array, which contains indices to OpenFOAM boundary patches
// of TimeVarying... types.
template<class TField>
std::vector<int>
time_varying_patch_indices( const TField & field )
{
    std::vector<int> result ;

    for (int i=0 ; i<field.boundaryField().size() ; i++)
    {
        if ( is_time_varying( field.boundaryField().types()[i] ) )
        {
            result.push_back( i ) ;
        } ;
    } ;

    return result ;
} ;

// Function returns an array, which contains indices to OpenFOAM boundary patches
// of groovyBC type.
template<class TField>                                      
std::vector<int>
groovy_patch_indices( const TField & field )
{
    std::vector<int> result ;

    for (int i=0 ; i<field.boundaryField().size() ; i++)
    {
        if ( is_groovy( field.boundaryField().types()[i] ) )
        {
            result.push_back( i ) ;
        } ;
    } ;

    return result ;
} ;

// Function returns an array, which contains indices to OpenFOAM boundary patches
// of groovyBC type.
template<class TField>                                      
std::vector<int>
totalPressure_patch_indices( const TField & field )
{
    std::vector<int> result ;

    for (int i=0 ; i<field.boundaryField().size() ; i++)
    {
        if ( is_totalPressure( field.boundaryField().types()[i] ) )
        {
            result.push_back( i ) ;
        } ;
    } ;

    return result ;
} ;

inline
void
save_time_varying_boundary_patch(
                    const TimeVaryingBoundaryPatch & patch,
                          std::ostream             & file
        )
{
    if ( false == patch.initialized)
        throw std::string("Can not save uninitialized time varying boundary patch") ;

    file << TIME_VARYING_BOUNDARY_PATCH_HEADER << "\n" ;
    save_vec( file, patch.face_positions, TIME_VARYING_PATCH_FACE_POSITIONS_HEADER ) ;

    file << "\n" << TIME_VARYING_PATCH_VALUES_HEADER ;

    for ( unsigned int t=0 ; t < patch.face_values.size() ; t++ )
    {
        file << "\n\nTime " << t << "\n" ;
        for ( unsigned int c=0 ; c < patch.face_values[ t ].size() ; c++ )
        {
            std::stringstream cname ;
            cname << "component_" << c ;
            save_vec( file, patch.face_values[ t ][ c ], cname.str().c_str() ) ;
        } ;
    } ;
} ;

inline
void
save_groovy_boundary_patch(                                     
                    const GroovyBoundaryPatch & patch,
                          std::ostream             & file
        )
{
    if ( false == patch.initialized)
        throw std::string("Can not save uninitialized time varying boundary patch") ;

    file << GROOVY_BOUNDARY_PATCH_HEADER << "\n" ;
    save_vec( file, patch.face_positions, GROOVY_PATCH_FACE_POSITIONS_HEADER ) ;

    file << "\n" << GROOVY_PATCH_VALUES_HEADER ;

    //for ( unsigned int t=0 ; t < patch.face_values.size() ; t++ )
    //{
        //file << "\n\nTime " << t << "\n" ;
        for ( unsigned int c=0 ; c < patch.face_values.size() ; c++ )
        {
            std::stringstream cname ;
            cname << "component_" << c ;
            save_vec( file, patch.face_values[ c ], cname.str().c_str() ) ;
        } ;
    //} ;
} ;

inline
void
save_totalPressure_boundary_patch(                                     
                    const totalPressureBoundaryPatch & patch,
                          std::ostream             & file
        )
{
    if ( false == patch.initialized)
        throw std::string("Can not save uninitialized time varying boundary patch") ;

    file << TOTALPRESSURE_BOUNDARY_PATCH_HEADER << "\n" ;
    save_vec( file, patch.face_positions, TOTALPRESSURE_PATCH_FACE_POSITIONS_HEADER ) ;

    file << "\n" << TOTALPRESSURE_PATCH_VALUES_HEADER ;

    //for ( unsigned int t=0 ; t < patch.face_values.size() ; t++ )
    //{
        //file << "\n\nTime " << t << "\n" ;
        for ( unsigned int c=0 ; c < patch.face_values.size() ; c++ )
        {
            std::stringstream cname ;
            cname << "component_" << c ;
            save_vec( file, patch.face_values[ c ], cname.str().c_str() ) ;
        } ;
    //} ;
} ;

template<class TField>
void
save_time_varying_bc(
                          TField       & field    ,
                    const Foam::fvMesh & mesh     ,
                    const std::string  & dir_path ,
                                Foam::Time   & runTime
)
{
    std::vector<int> tv_OF_patch_indices = time_varying_patch_indices( field ) ;

    if (0 == tv_OF_patch_indices.size() ) return ;

    std::string field_name ( field.name() ) ;
    std::string filename = std::string(dir_path) + "/" + field_name + "_time_varying_boundary";
    Info << "  Saving time varying boundary conditions\n" ;

    std::ofstream file( filename.c_str(), std::ios::out | std::ios::binary );
    if ( !file.is_open() ) throw "Can not open file " + filename ;

    file << FILE_HEADER                  << "\n" ;
    file << TIME_VARYING_BOUNDARY_HEADER << "\n" ;
    file << field.name()                 << "\n" ;
    file << n_components( field )        << "\n" ; // number of components for single face (scalar/vector)
    file << N_TIME_STEPS                 << "\n" ; // number of time steps
    file << tv_OF_patch_indices.size()   << "\n" ; // number of boundary patches

    for (unsigned int i=0 ; i<tv_OF_patch_indices.size() ; i++)
    {
        TimeVaryingBoundaryPatch tvb_patch ;

        build_time_varying_boundary_patch( tvb_patch, field, mesh, runTime, tv_OF_patch_indices[ i ] ) ;
        save_time_varying_boundary_patch ( tvb_patch, file ) ;
    } ;
} ;

/*inline
void save_groovy_timelines(
                             std::ifstream timefile,
                             std::ofstream timelinesfile
)
{




    std::vector< std::vector <double> > timelines;

    timelines.resize(2);

    double temp1, temp2;

    timefile >> temp1;
    //vala1 = atof(temp1.c_str());
    //vala2 = atof(temp2.c_str());

    timelines[0].push_back(temp1);
    timelines[1].push_back(temp2);

    int size = timelines.size();

    Info << "timelines : " << size << " " <<  endl;

    //save_vec(timelinesfile, timelines, "timelines");


}*/

template<class TField>
void
save_groovy_bc(                                             
                          TField       & field    ,
                    const Foam::fvMesh & mesh     ,
                    const std::string  & dir_path ,
                                Foam::Time   & runTime
)
{
    std::vector<int> g_OF_patch_indices = groovy_patch_indices( field ) ;

    if (0 == g_OF_patch_indices.size() ) return ;

    std::string field_name ( field.name() ) ;
    std::string filename = std::string(dir_path) + "/" + field_name + "_groovy_boundary";
    std::string timelinesfilename = std::string(dir_path) + "/" + field_name + "_groovy_timelines";
    Info << "  Saving groovy boundary conditions\n" ;

    std::ofstream file( filename.c_str(), std::ios::out | std::ios::binary );
    if ( !file.is_open() ) throw "Can not open file " + filename ;
    std::ofstream timelinesfile( timelinesfilename.c_str(), std::ios::out | std::ios::binary  );
    if ( !timelinesfile.is_open() ) throw "Can not open file " + timelinesfilename ;

    // Finding the name of file with timelines
    std::string tempname;
    std::string timefilename;
    std::string case_dir_name = getenv("FOAM_CASE");

    Info << "FOAM_CASE" << case_dir_name << endl;

    std::string lookfortimefilename = case_dir_name + "/0/" + field_name;

    std::ifstream lookfortimefile (lookfortimefilename.c_str() );
    if ( !lookfortimefile.is_open() ) throw "Can not open file " + lookfortimefilename ;



    file << FILE_HEADER                  << "\n" ;
    file << GROOVY_BOUNDARY_HEADER << "\n" ;
    file << field.name()                 << "\n" ;
    file << n_components( field )        << "\n" ; // number of components for single face (scalar/vector)
    //file << N_TIME_STEPS                 << "\n" ; // number of time steps
    file << g_OF_patch_indices.size()   << "\n" ; // number of boundary patches

    timelinesfile << FILE_HEADER                  << "\n" ;
    timelinesfile << GROOVY_TIMELINES_HEADER << "\n" ;
    timelinesfile << field.name()                 << "\n" ;
    timelinesfile << "2"                   << "\n" ; // number of components for single face (scalar/vector)
    //timelinesfile << N_TIME_STEPS                 << "\n" ; // number of time steps
    timelinesfile << g_OF_patch_indices.size()   << "\n" ; // number of boundary patches

    for (unsigned int i=0 ; i<g_OF_patch_indices.size() ; i++)
    {
        GroovyBoundaryPatch gb_patch ;

        build_groovy_boundary_patch( gb_patch, field, mesh, runTime, g_OF_patch_indices[ i ] ) ;
        save_groovy_boundary_patch ( gb_patch, file ) ;

        while (lookfortimefile >> tempname) {
            if (tempname == "fileName") { lookfortimefile >> timefilename; break;}
        }

        if (!timefilename.empty())
        {
            timefilename.erase(std::remove(timefilename.begin(), timefilename.end(), '\"'), timefilename.end());
            timefilename.erase(std::remove(timefilename.begin(), timefilename.end(), ';'), timefilename.end());
            timefilename.erase(0,11);

            timefilename.insert(0,"/");
            timefilename.insert(0,case_dir_name);

            Info << "File with timelines : " << timefilename << endl;
        }

        //std::ifstream timefile (timefilename.c_str() );
        //if ( !timefile.is_open() ) throw "Can not open file " + timefilename ;

        IFstream timefile(timefilename);

        //save_groovy_timelines (timefile, timelinesfile);

        vector2DField values(timefile);

        //Info << "values : " << values[0].x() << " " << values[0].y()  << "size = " << values.size() << endl;

        //timelinesfile << "" << "\n";
        timelinesfile << GROOVY_TIMELINES_PATCH_HEADER << "\n";
        timelinesfile << "" << "\n";
        timelinesfile << GROOVY_TIMELINES_TIME_HEADER << "\n";
        timelinesfile << "time" << "\n";
        timelinesfile << VECTOR_HEADER << "\n";
        timelinesfile << data_type_header<double>() << "\n";
        timelinesfile << values.size() << " 1" << "\n";

        for (int i = 0; i < values.size(); i++) timelinesfile << values[i].x() << "\n";

        timelinesfile << "" << "\n";
        timelinesfile << GROOVY_TIMELINES_VALUES_HEADER << "\n";
        timelinesfile << "values" << "\n";
        timelinesfile << VECTOR_HEADER << "\n";
        timelinesfile << data_type_header<double>() << "\n";
        timelinesfile << values.size() << " 1" << "\n";

        for (int i = 0; i < values.size(); i++) timelinesfile << values[i].y() << "\n";

    } ;
} ;

template<class TField>
void
save_totalPressure_bc(                                             
                          TField       & field    ,
                    const Foam::fvMesh & mesh     ,
                    const std::string  & dir_path ,
                                Foam::Time   & runTime
)
{
    std::vector<int> tP_OF_patch_indices = totalPressure_patch_indices( field ) ;

    if (0 == tP_OF_patch_indices.size() ) return ;

    std::string field_name ( field.name() ) ;
    std::string filename = std::string(dir_path) + "/" + field_name + "_totalPressure_boundary";

    Info << "  Saving totalPressure boundary conditions\n" ;

    std::ofstream file( filename.c_str(), std::ios::out | std::ios::binary );
    if ( !file.is_open() ) throw "Can not open file " + filename ;

    file << FILE_HEADER                  << "\n" ;
    file << TOTALPRESSURE_BOUNDARY_HEADER << "\n" ;
    file << field.name()                 << "\n" ;
    file << n_components( field )        << "\n" ; // number of components for single face (scalar/vector)
    //file << N_TIME_STEPS                 << "\n" ; // number of time steps
    file << tP_OF_patch_indices.size()   << "\n" ; // number of boundary patches

    for (unsigned int i=0 ; i<tP_OF_patch_indices.size() ; i++)
    {
        totalPressureBoundaryPatch tPb_patch ;

        build_totalPressure_boundary_patch( tPb_patch, field, mesh, runTime, tP_OF_patch_indices[ i ] ) ;
        save_totalPressure_boundary_patch ( tPb_patch, file ) ;

    } ;
} ;

// Empty function used for phi field, where time varying boundary conditions should not occur.
inline
void
save_time_varying_bc(
                          surfaceScalarField  & field    ,
                    const Foam::fvMesh        & mesh     ,
                    const std::string         & dir_path ,
                                Foam::Time          & runTime
)
{
} ;

inline
void
save_groovy_bc(
                          surfaceScalarField  & field    ,
                    const Foam::fvMesh        & mesh     ,
                    const std::string         & dir_path ,
                                Foam::Time          & runTime
)
{
} ;

inline
void
save_totalPressure_bc(
                          surfaceScalarField  & field    ,
                    const Foam::fvMesh        & mesh     ,
                    const std::string         & dir_path ,
                                Foam::Time          & runTime
)
{
} ;


#endif
