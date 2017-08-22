#include "stdafx.h"
#include "CgalSimplification.h"


CgalSimplification::CgalSimplification(MyMesh&sm):
	surface_mesh(sm)
{
}


CgalSimplification::~CgalSimplification()
{
}
namespace SMS = CGAL::Surface_mesh_simplification;

void CgalSimplification::MeshSimplication(double x)
{
	int n_edges = surface_mesh.n_edges();
	int n_dif = x*n_edges;
	std::cout << "sour: " << n_edges << " dif: " << n_dif << std::endl;
	n_edges = (1-x)*n_edges;

	Constrained_edge_map constraints_map(surface_mesh);

	edge_iterator b, e;
	int count = 0;
	for (boost::tie(b, e) = edges(surface_mesh); b != e; ++b) {
		put(constraints_map, *b, (count++ <100));
	}
	// This is a stop predicate (defines when the algorithm terminates).
	// In this example, the simplification stops when the number of undirected edges
	// left in the surface mesh drops below the specified number (1000)
	SMS::Count_stop_predicate<MyMesh> stop(n_edges);

	// This the actual call to the simplification algorithm.
	// The surface mesh and stop conditions are mandatory arguments.
	int r = SMS::edge_collapse
	(surface_mesh
		, stop
		, CGAL::parameters::halfedge_index_map(get(CGAL::halfedge_index, surface_mesh))
		.vertex_point_map(get(boost::vertex_point, surface_mesh))
		.edge_is_constrained_map(constraints_map)
	);

	surface_mesh.garbage_collection();
	std::cout << "\nFinished...\n" << r << " edges removed.\n"
		<< num_edges(surface_mesh) << " final edges.\n";
}

