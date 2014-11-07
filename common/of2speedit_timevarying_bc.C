#include "of2speedit_timevarying_bc.H"


int
n_components(const volScalarField & field)
{
    return 1 ; 
} ;
int
n_components(const volVectorField & field)
{
    return 3 ; 
} ;
int
n_components(const surfaceScalarField & field)
{
    return 1 ; 
} ;


bool
is_time_varying( const std::string & type )
{
    if (
            "timeVaryingUniformFixedValue" == type 
         )
    {
        return true ;
    } ;

    return false ;
} ;

bool
is_groovy( const std::string & type )
{
    if (
            
            "groovyBC"                     == type              
         )
    {
        return true ;
    } ;

    return false ;
} ;

bool
is_totalPressure( const std::string & type )
{
    if (
            "totalPressure"                     == type              
         )
    {
        return true ;
    } ;

    return false ;
} ;


