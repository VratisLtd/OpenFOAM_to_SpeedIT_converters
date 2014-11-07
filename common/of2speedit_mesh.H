#ifndef __OF2SPEEDIT_MESH_H__
#define __OF2SPEEDIT_MESH_H__

#include "fvCFD.H"
#include <fstream>
#include <vector>

void
save_mesh_description( Foam::fvMesh & mesh, std::ostream & file ) ;

std::vector<int>
boundary_owners( Foam::fvMesh const & mesh ) ;

#endif
