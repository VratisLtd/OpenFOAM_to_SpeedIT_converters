#include "of2speedit_mesh.H"
#include "of2speedit_io.H"
#include "of2speedit_boundary.H"
#include "of2speedit_utils.H"

void
save_mesh_description( Foam::fvMesh & mesh, std::ostream & file )
{
		file << mesh.nCells()         << " " ;          // number of cells
		file << mesh.nInternalFaces() << " " ;          // number of internal faces
		std::vector<int> bowners = boundary_owners(mesh) ;
		file << bowners.size() ;

		file << "\n" ;

#if (2 == OF_VERSION && 1 > OF_VERSION_MINOR ) || 1 == OF_VERSION
		if ( mesh.orthogonal() ) 
		{
			file << "orthogonal\n" ;
		} else {
			file << "nonorthogonal\n" ;
		} ;
#endif
#if (2 == OF_VERSION && 1 <= OF_VERSION_MINOR )
        if ( mesh.checkFaceOrthogonality() )
        {
            file << "orthogonal\n" ;
        } else {
            file << "nonorthogonal\n" ;
        } ;
#endif

		
    Vector<scalar>::labelType validComponents
     (
         pow
         (
             mesh.solutionD(),
             pTraits<powProduct<Vector<label>, Vector<scalar>::rank>::type>::zero
         )
     );
		file << "X  " << (1 == validComponents[0]) << "\n" ;
		file << "Y  " << (1 == validComponents[1]) << "\n" ;
		file << "Z  " << (1 == validComponents[2]) ;


		test_io(file) ;
} ;

std::vector<int>
boundary_owners( Foam::fvMesh const & mesh )
{
	std::vector<int> v ;

	for   (int z=0 ; z < mesh.boundary().size()                ; z++) {
		for (int i=0 ; i < mesh.boundary()[z].faceCells().size() ; i++) {
			v.push_back( mesh.boundary()[z].faceCells()[i] ) ;
		} ;
	} ;

	std::vector< int > vbp = bfaces_reorder_table(mesh) ;

	return vector_reorder( v, vbp ) ;
} ;
