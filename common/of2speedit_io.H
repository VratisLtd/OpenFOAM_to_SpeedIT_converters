#ifndef __OF2SPEEDIT_IO_H__
#define __OF2SPEEDIT_IO_H__

#include <iomanip>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <typeinfo>
#include <sys/stat.h>

#include "of2speedit_defines.H"

bool
test_io (std::ostream & f) ;

//std::ofstream &
//open_output_file( std::ofstream & file, std::string const & filename ) ;

template<class T>
std::string
data_type_header()
{
	if ( typeid(T) == typeid(double) ) return DATA_DOUBLE ;
	if ( typeid(T) == typeid(int   ) ) return DATA_INT    ;

	throw std::string("Unsupported data type in vector : \"") + typeid(T).name() + "\", the only supported data types are: double int." ;
} ;

void
save_string( std::ostream & f, const std::string & header) ;

template<class T>
void
save_vec_header(std::ostream & f, unsigned int size, unsigned int ncoordinates, const std::string & vec_name)
{
	save_string ( f, ""            ) ;
	save_string ( f, vec_name      ) ;
	save_string ( f, VECTOR_HEADER ) ;

	f << data_type_header<T>() << "\n" ;
	f << size                  << " "  ;
	f << ncoordinates          << "\n" ;

	test_io(f) ;
} ;

template<class T>
void
save_vec_data (
		std::ostream & f    ,
		const T      * data ,
		unsigned int   size
		)
{
#ifdef BINARY_IO
	f.write( reinterpret_cast<const char *>( data ), size * sizeof(T) ) ;
#else
//
//	ASCII output
//
	for (unsigned int i=0 ; i<size ; i++)
	{
		f << std::scientific << std::setprecision(80) << data[i] << "\n" ;
	} ;
#endif
	test_io(f) ;
} ;

template<class T>
void
save_vec (std::ostream & f, std::vector< std::vector< T > > const & vec, const std::string & vec_name)
{
	save_vec_header<T>( f, vec[0].size(), vec.size(), vec_name ) ;

	for (unsigned int i=0 ; i < vec.size() ; i++) 
	{
		save_vec_data( f, &(vec[i][0]), vec[0].size() ) ;
	} ;
} ;

template<class T>
void
save_vec (std::ostream & f, const T * data, unsigned int size, const std::string & vec_name)
{
	save_vec_header<T>( f, size, 1, vec_name ) ;
	save_vec_data     ( f, data, size        ) ;
} ;

template<class T>
void
save_vec( std::ostream & f, const std::vector<T> & vec, const std::string & vec_name)
{
	save_vec( f, &(vec[0]), vec.size(), vec_name ) ;
} ;

#endif
