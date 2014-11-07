#include "of2speedit_io.H"

bool
test_io (std::ostream & f)
{
	if (f.good()) {
	} else {
		throw std::string("Can not write data to file") ;
	} ;
	return true ;
} ;

void
save_string( std::ostream & f, const std::string & str)
{
	f << str << "\n" ;
	test_io(f) ;
} ;


