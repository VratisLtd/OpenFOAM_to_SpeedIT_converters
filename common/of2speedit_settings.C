#include "of2speedit.H"

bool
test_iterative_solver(const Foam::dictionary & solver)
{
	word s ( solver.lookup("solver") ) ;

	if ("PCG"   == s ) goto solver_OK ;
	if ("PBiCG" == s ) goto solver_OK ;

	throw std::string("Unsupported solver ") + s +
				", the only supported solvers are: PCG PBiCG" ;
	
solver_OK:

	word prec( solver.lookup("preconditioner") ) ;

	if ("diagonal" 		== prec) goto precond_OK ;
	if ("none"     		== prec) goto precond_OK ;
	if ("GAMG"	== prec) goto precond_OK ;

	throw std::string("Unsupported preconditioner ") + prec +
	      " for solver " + s + 
				", the only supported preconditioners are: diagonal none GAMG" ;

precond_OK:

	return true ;
} ;

class IterSolverSettings
{
	public:
		IterSolverSettings(const dictionary & dict) ;

		int         max_iterations     ;
		double      required_tolerance ;
                double      relative_tolerance ;
		std::string preconditioner     ;
} ;

IterSolverSettings::IterSolverSettings(const dictionary & dict)
{
	test_iterative_solver(dict) ;

	max_iterations     = dict.lookupOrDefault<label> ("maxIter"   , 1000 ) ;
	required_tolerance = dict.lookupOrDefault<scalar>("tolerance" , 1e-6 ) ;
        relative_tolerance = dict.lookupOrDefault<scalar>("relTol"    ,    0 ) ;

	word prec( dict.lookup("preconditioner")) ;
	preconditioner     = std::string(prec) ;
} ;

void
save_iter_solvers_settings( std::ostream & f, Foam::IOobject & obj)
{
		const dictionary& fvSolution = obj.db().lookupObject<IOdictionary> ("fvSolution");
		dictionary solvers(fvSolution.subDict("solvers"));
		
		{
			IterSolverSettings settings(solvers.subDict("U")) ;

			setting_name(f, SETTING_UEQN_MAX_ITER)      << settings.max_iterations     << "\n" ;
			setting_name(f, SETTING_UEQN_REQ_TOLERANCE) << settings.required_tolerance << "\n" ;
                        setting_name(f, SETTING_UEQN_REL_TOLERANCE) << settings.relative_tolerance << "\n" ;
			setting_name(f, SETTING_UEQN_PRECOND)       << settings.preconditioner     << "\n" ;
		}
		{
			IterSolverSettings settings(solvers.subDict("p")) ;

			setting_name(f, SETTING_PEQN_MAX_ITER)      << settings.max_iterations     << "\n" ;
			setting_name(f, SETTING_PEQN_REQ_TOLERANCE) << settings.required_tolerance << "\n" ;
                        setting_name(f, SETTING_PEQN_REL_TOLERANCE) << settings.relative_tolerance << "\n" ;
			setting_name(f, SETTING_PEQN_PRECOND)       << settings.preconditioner     << "\n" ;
		}
} ;

void
save_iter_solvers_settings_turb( std::ostream & f, Foam::IOobject & obj)
{
        const dictionary& fvSolution = obj.db().lookupObject<IOdictionary> ("fvSolution");
        dictionary solvers(fvSolution.subDict("solvers"));

        {
            IterSolverSettings settings(solvers.subDict("k")) ;

            setting_name(f, SETTING_KEQN_MAX_ITER)      << settings.max_iterations     << "\n" ;
            setting_name(f, SETTING_KEQN_REQ_TOLERANCE) << settings.required_tolerance << "\n" ;
            setting_name(f, SETTING_KEQN_REL_TOLERANCE) << settings.relative_tolerance << "\n" ;
            setting_name(f, SETTING_KEQN_PRECOND)       << settings.preconditioner     << "\n" ;
        }
        {
            IterSolverSettings settings(solvers.subDict("omega")) ;

            setting_name(f, SETTING_OMEGAEQN_MAX_ITER)      << settings.max_iterations     << "\n" ;
            setting_name(f, SETTING_OMEGAEQN_REQ_TOLERANCE) << settings.required_tolerance << "\n" ;
            setting_name(f, SETTING_OMEGAEQN_REL_TOLERANCE) << settings.relative_tolerance << "\n" ;
            setting_name(f, SETTING_OMEGAEQN_PRECOND)       << settings.preconditioner     << "\n" ;
        }
} ;
