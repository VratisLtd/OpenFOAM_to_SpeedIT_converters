#include "of2speedit.H"
#include "of2speedit_defines.H"
#include "of2speedit_boundary.H"
#include "of2speedit_io.H"
#include "of2speedit_utils.H"
#include "of2speedit_field_internals.H"
#include "of2speedit_mesh.H"
#include "of2speedit_error.H"
#include "of2speedit_timevarying_bc.H"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <algorithm>
#include <vector>
#include <fstream>
#include <string>

void
create_directory ( const char * path )
{
    int status = mkdir( path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH );

    if (0 > status) {
        if (errno == EEXIST) {
            std::cout << "\n Directory " << path ; 
            std::cout << " already exists, files will be overwritten. \n" ; 
        } ;
        if (errno != EEXIST) {
            Foam::FatalError << "Can not create directory " << path ;
            Foam::FatalError << Foam::exit(Foam::FatalError, -1) ;
        } ;
    } ;
} ;

void
remove_directory(const char * dir_path)
{
    std::string command = std::string("rm -rf ") + dir_path ;
    if ( system(command.c_str()) ) return ;
} ;

void
check_application_name ( Foam::Time const & runTime, char const * app_name)
{
    word _name ( runTime.controlDict().lookup("application") ) ;

    if (app_name == _name) {
    } else {
        throw std::string("Your case seems to be prepared for \"") + _name +
                                            "\" but it should be prepared for \"" + app_name +
                          "\". Please set your case properly." ;
    } ;
} ;



template<class TField>
void
save_field(
              TField         & field      ,
        const Foam::fvMesh   & mesh       ,
        const std::string    & dir_path   ,
              Foam::Time     * runTime    ,
              bool             save_data = true
        )
{
    std::string field_name ( field.name() ) ;
    std::string filename = dir_path + "/" + field_name ;

    try
    {
        std::ofstream file( filename.c_str(), std::ios::out | std::ios::binary );

        if ( !file.is_open() ) throw "Can not open file " + filename ;

        file << FILE_HEADER  << "\n" ;

//		std::ofstream file ;
//		open_output_file ( file, filename ) ;

        file << FIELD_HEADER << "\n" << field_name << "\n" ;

        if ( typeid(TField) == typeid(volScalarField     ) ||
               typeid(TField) == typeid(surfaceScalarField ) ||
         typeid(TField) == typeid(volVectorField     ) )
        {
        } else {
            throw std::string("Unsupported field type") ;
        } ;

        if (true == save_data)
        {
            save_vec( file , internal_values( field ), CELL_VALS_HEADER ) ;

            BFaceValues bfv = boundary_values( field ) ;

            save_vec( file, bfv.vals , BFACE_VALS_HEADER  ) ;
            save_vec( file, bfv.types, BFACE_TYPES_HEADER ) ;
            save_vec( file, bfv.refVals, BFACE_REFVALS_HEADER ) ;
        } ;

        {
            OStringStream hss ;

            field.writeHeader(hss) ;
            hss.writeKeyword("dimensions") << field.dimensionedInternalField().dimensions() << token::END_STATEMENT << nl << nl ;

            if (hss.bad()) throw std::string("Can not generate OpenFOAM file header") ;

            file << "\n" << FIELD_OF_HEADER << "\n" ;
            file << hss.str().size() << "\n" ;
            file << hss.str() ;
        } ;
        if (true == save_data)
        {
            OStringStream bss ;

            field.boundaryField().writeEntry("boundaryField", bss);
            field.writeEndDivider(bss) ;

            if (bss.bad()) throw std::string("Can not generate OpenFOAM file footer") ;

            file << "\n" << FIELD_OF_FOOTER << "\n" ;
            file << bss.str().size() << "\n" ;
            file << bss.str() ;
        }
//        else // Used only for "phi" field
//        {
//            file << "\n" << FIELD_OF_BOUNDARY_PATCHES << "\n" ;
//            file << field.boundaryField().size() << "\n" ;
//            for (int i=0 ; i<field.boundaryField().size() ; i++)
//            {
//                if ( ("empty"      != field.boundaryField()   .types()[i]) &&
//                       ("calculated" != field.boundaryField()   .types()[i]) )
//                    throw string("Unsupported boundary patch type \"") +
//                          field.boundaryField().types()[i] + "\", the only supported types " +
//                                "are \"empty\" \"calculated\"" ;

//                file << field.boundaryField()[i].patch().name() << " " ;
//                file << field.boundaryField()   .types()[i]     << " " ;
//                file << field.boundaryField()[i].patch().size()        ;
//                file << "\n" ;
//            } ;
//        } ;

        file << "\n" << FIELD_OF_BOUNDARY_PATCHES << "\n" ;
        file << field.boundaryField().size() << "\n";

        for (int i = 0; i < field.boundaryField().size(); i++)
        {
            file << field.boundaryField()[i].patch().name() << " " ;
            file << field.boundaryField()   .types()[i]     << " " ;
            file << field.boundaryField()[i].patch().start() << " ";
            file << field.boundaryField()[i].patch().size() << " ";

            OStringStream strStream;
            field.boundaryField()[i].write(strStream);
            std::string str = strStream.str();

            file << std::count(str.begin(), str.end(), '\n') << "\n{\n";
            file << str;
            file << "}\n";
        }


        test_io(file) ;

        if (NULL != runTime) save_time_varying_bc (field, mesh, dir_path, *runTime) ;
        if (NULL != runTime) save_groovy_bc       (field, mesh, dir_path, *runTime) ;

        //if (NULL != runTime) save_totalPressure_bc(field, mesh, dir_path, *runTime) ;
    }
    catch (std::string & err_str)
    {
        handle_error("save_field ERROR when saving " + field.name() + " : " + err_str,
                dir_path) ;
    } ;
} ;

void
save_field(
              volScalarField     & field      ,
        const Foam::fvMesh       & mesh       ,
        const std::string        & dir_path   ,
            Foam::Time         * runTime
        )
{
    save_field <volScalarField> (field, mesh, dir_path, runTime) ;
} ;
void
save_field(
              volVectorField     & field      ,
        const Foam::fvMesh       & mesh       ,
        const std::string        & dir_path   ,
              Foam::Time         * runTime
        )
{
    save_field <volVectorField> (field, mesh, dir_path, runTime) ;
} ;
void
save_field(
              surfaceScalarField & field      ,
        const Foam::fvMesh       & mesh       ,
        const std::string        & dir_path   ,
              Foam::Time         * runTime
        )
{
    save_field <surfaceScalarField> (field, mesh, dir_path, runTime, false) ; // used only for field "phi", where data is not needed
} ;

void
save_mesh(
            fvMesh      & mesh ,
                        std::string   dir_path
         )
{
    std::string filename = dir_path + "/" + FILE_NAME_MESH ;

    try
    {
        std::ofstream file( filename.c_str(), std::ios::out | std::ios::binary );

        if ( !file.is_open() ) throw "Can not open file " + filename ;

        file << FILE_HEADER << "\n" ;
        file << MESH_HEADER << "\n" ;

        save_mesh_description( mesh, file ) ;

        save_vec ( file, internal_values( mesh.Sf() )                            , IFACE_AREA_VECTORS_HEADER   ) ;
        save_vec ( file, mesh.weights()    .cdata(), mesh.weights()    .size()   , IFACE_WEIGHTS_HEADER        ) ;
        save_vec ( file, mesh.deltaCoeffs().cdata(), mesh.deltaCoeffs().size()   , IFACE_DELTA_COEFFS_HEADER   ) ;
        if (false == mesh.orthogonal()) {
            save_vec( file, internal_values( mesh.correctionVectors() )            , IFACE_CORRECTION_VEC_HEADER ) ;
        } ;
        save_vec( file, mesh.owner()    .cdata()   , mesh.owner()    .size()     , IFACE_OWNERS_HEADER         ) ;
        save_vec( file, mesh.neighbour().cdata()   , mesh.neighbour().size()     , IFACE_NEIGHBOURS_HEADER     ) ;


        save_vec  ( file, boundary_values( mesh.Sf()               , true ).vals , BFACE_AREA_VECTORS_HEADER   ) ;
        save_vec  ( file, boundary_values( mesh.weights()          , true ).vals , BFACE_WEIGHTS_HEADER        ) ;
        save_vec  ( file, boundary_values( mesh.deltaCoeffs()      , true ).vals , BFACE_DELTA_COEFFS_HEADER   ) ;
        if (false == mesh.orthogonal()) {

            for (int i=0 ; i < mesh.correctionVectors().boundaryField().size() ; i++)
                for (int j=0 ; j < mesh.correctionVectors().boundaryField()[i].size() ; j++)
                {
                    if (
                                0 != mesh.correctionVectors().boundaryField()[i][j].x() ||
                                0 != mesh.correctionVectors().boundaryField()[i][j].y() ||
                                0 != mesh.correctionVectors().boundaryField()[i][j].z()
                         )
                        throw std::string("boundary correction vectors different than 0 are not supported - do you try to use \"coupled\" boundary faces ?") ;
                } ;

         save_vec ( file, boundary_values( mesh.correctionVectors(), true ).vals , BFACE_CORRECTION_VEC_HEADER ) ;
        } ;
        save_vec  ( file, boundary_owners( mesh )                                , BFACE_OWNERS_HEADER         ) ;
        save_vec  ( file, bfaces_reorder_table( mesh )                           , BFACE_REORDER_TABLE_HEADER  ) ;


        save_vec  ( file, mesh.V().cdata()        , mesh.V().size()              , CELL_VOLUMES_HEADER         ) ;
    }
    catch (std::string & err_str)
    {
        handle_error("save_mesh ERROR : " + err_str,
                dir_path) ;
    } ;
} ;


