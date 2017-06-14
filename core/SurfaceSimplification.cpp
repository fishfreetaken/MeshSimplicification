// SurfaceSimplification.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "IncludeHeader.h"

#include "MyOpenMesh.h"
void main_test();
int main(int argc, char** argv)
{
	//main_test(); //测试

	MyOpenMesh  ims;
	
	float dest = 0.6; //缩小原来点的多少 dest*100%
	string inputfilename = "DIT2-2.ply";
	string outputfilename = "DIT2-2-simplify-60.ply";
	ims.Readfile(inputfilename.c_str());
	ims.MainLoop(dest);
	ims.Writefile(outputfilename.c_str(),1);
	system("pause");
	return 0;
}

void main_test2() {
	TriMyMesh mesh;
	// Request required status flags

	// Add some vertices as in the illustration above
	MyMesh::VertexHandle vhandle[10];
	vhandle[0] = mesh.add_vertex(MyMesh::Point(1, 5, 0));
	vhandle[1] = mesh.add_vertex(MyMesh::Point(2, 3, 0));
	vhandle[2] = mesh.add_vertex(MyMesh::Point(5, 1, 0));
	vhandle[3] = mesh.add_vertex(MyMesh::Point(6, 2, 0));
	vhandle[4] = mesh.add_vertex(MyMesh::Point(4, 2, 0));
	vhandle[5] = mesh.add_vertex(MyMesh::Point(2, 2, 0));
	vhandle[6] = mesh.add_vertex(MyMesh::Point(0, 2, 0));
	vhandle[7] = mesh.add_vertex(MyMesh::Point(1, 4, 0));
	vhandle[8] = mesh.add_vertex(MyMesh::Point(3, 4, 0));
	vhandle[9] = mesh.add_vertex(MyMesh::Point(5, 4, 0));
}

void main_test() {
	map<int, int*>cp;
	cp[1] = new int(65);
	cp[7] = new int(77);
	cp[9] = new int(89);

	cout << *cp[1] << endl;
	cp.erase(1);
	cout << cp[1] << endl;
	cout << cp.size() << endl;
	if (cp[1] == NULL) {
		cout << "yes" << endl;
	}
	/*Matrix3d test;
	test << 1, 1.1, 2,
		0, 1, 1,
		0, 0, 1;

	Vector3d x(7.562, 7.4523, 5.269599);
	Vector3d ff;
	ff = test.colPivHouseholderQr().solve(x);
	cout << "QR:" << ff << " : " << (test*ff - x).norm() << endl;

	ff = test.fullPivHouseholderQr().solve(x);
	cout << "Full QR:" << ff << " : " << (test*ff - x).norm() << endl;

	ff = test.llt().solve(x);
	cout << "LLT:" << ff << " : " << (test*ff - x).norm() << endl;*/

	/*map<int, float> vsf;
	vsf[52] = 52.36;
	vsf[4] = 7.256;
	vsf[75] = 52;
	vsf[14] = 4;

	map<int, float >::iterator it;
	vsf.erase(52);
	vsf.erase(51);
	for (it = vsf.begin(); it != vsf.end(); it++) {
	cout << it->first << " " << it->second << endl;
	}*/

	//MyMesh mesh;
	TriMyMesh mesh;
	// Request required status flags

	// Add some vertices as in the illustration above
	MyMesh::VertexHandle vhandle[10];
	vhandle[0] = mesh.add_vertex(MyMesh::Point(1, 1, 0));
	vhandle[1] = mesh.add_vertex(MyMesh::Point(3, 1, 0));
	vhandle[2] = mesh.add_vertex(MyMesh::Point(5, 1, 0));
	vhandle[3] = mesh.add_vertex(MyMesh::Point(6, 2, 0));
	vhandle[4] = mesh.add_vertex(MyMesh::Point(4, 2, 0));
	vhandle[5] = mesh.add_vertex(MyMesh::Point(2, 2, 0));
	vhandle[6] = mesh.add_vertex(MyMesh::Point(0, 2, 0));
	vhandle[7] = mesh.add_vertex(MyMesh::Point(1, 4, 0));
	vhandle[8] = mesh.add_vertex(MyMesh::Point(3, 4, 0));
	vhandle[9] = mesh.add_vertex(MyMesh::Point(5, 4, 0));

	// Add three quad faces
	std::vector<MyMesh::VertexHandle> face_vhandles;

	MyMesh::FaceHandle   fhandle[10];

	face_vhandles.push_back(vhandle[5]);
	face_vhandles.push_back(vhandle[1]);
	face_vhandles.push_back(vhandle[4]);
	fhandle[0] = mesh.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[5]);
	face_vhandles.push_back(vhandle[0]);
	face_vhandles.push_back(vhandle[1]);
	fhandle[1] = mesh.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[5]);//
	face_vhandles.push_back(vhandle[6]);
	face_vhandles.push_back(vhandle[0]);
	fhandle[2] = mesh.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[5]);
	face_vhandles.push_back(vhandle[7]);
	face_vhandles.push_back(vhandle[6]);
	fhandle[3] = mesh.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[5]);
	face_vhandles.push_back(vhandle[8]);
	face_vhandles.push_back(vhandle[7]);
	fhandle[4] = mesh.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[5]);
	face_vhandles.push_back(vhandle[4]);
	face_vhandles.push_back(vhandle[8]);
	fhandle[5] = mesh.add_face(face_vhandles);

	face_vhandles.clear();//
	face_vhandles.push_back(vhandle[4]);
	face_vhandles.push_back(vhandle[9]);
	face_vhandles.push_back(vhandle[8]);
	fhandle[6] = mesh.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[4]);
	face_vhandles.push_back(vhandle[3]);
	face_vhandles.push_back(vhandle[9]);
	fhandle[7] = mesh.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[4]);
	face_vhandles.push_back(vhandle[2]);
	face_vhandles.push_back(vhandle[3]);
	fhandle[8] = mesh.add_face(face_vhandles);

	face_vhandles.clear();//
	face_vhandles.push_back(vhandle[4]);
	face_vhandles.push_back(vhandle[1]);
	face_vhandles.push_back(vhandle[2]);
	fhandle[9] = mesh.add_face(face_vhandles);

	//mesh.delete_face(fhandle[7]);

	printf("face num:%d\n", mesh.n_faces());
	printf("vertex num:%d\n", mesh.n_vertices());

	///*for (MyMesh::FaceIter  it = mesh.faces_begin(); it != mesh.faces_end(); ++it) {
	//	for (MyMesh::FaceVertexIter it_f = mesh.fv_begin(*it); it_f != mesh.fv_end(*it); it_f++) {
	//		cout << it_f->idx() << " " << mesh.point(*it_f) << endl;
	//	}
	//	cout << "~~~~~~~~~~~~" << endl;
	//}*/

	//MyMesh::VertexHandle vhandle_insert = mesh.add_vertex(MyMesh::Point(3, 7, 0)); //不能在内部进行
	//face_vhandles.clear();
	//face_vhandles.push_back(vhandle[5]);
	//face_vhandles.push_back(vhandle_insert);
	//face_vhandles.push_back(vhandle[4]);
	//mesh.add_face(face_vhandles);

	//face_vhandles.clear();
	//face_vhandles.push_back(vhandle[4]);
	//face_vhandles.push_back(vhandle_insert);
	//face_vhandles.push_back(vhandle[8]);
	//mesh.add_face(face_vhandles);

	//face_vhandles.clear();
	//face_vhandles.push_back(vhandle[8]);
	//face_vhandles.push_back(vhandle_insert);
	//face_vhandles.push_back(vhandle[5]);
	//
	//mesh.add_face(face_vhandles);

	//printf("face num:%d\n", mesh.n_faces());

	//for (MyMesh::HalfedgeIter it = mesh.halfedges_begin(); it != mesh.halfedges_end(); ++it) {
	//	if (mesh.to_vertex_handle(*it) == vhandle[3] &&
	//		mesh.from_vertex_handle(*it) == vhandle[2])
	//	{
	//		// Collapse edge
	//		mesh.collapse(*it);
	//		break;
	//	}
	//}
	//printf("vertex num:%d\n", mesh.n_vertices());

	mesh.request_vertex_status();
	mesh.request_edge_status();
	mesh.request_face_status();

	for (MyMesh::VertexIter v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it)
	{
		cout << "All vertex :" << v_it->idx() << " " << mesh.point(*v_it) << endl;
		if (vhandle[5] == mesh.vertex_handle(v_it->idx())) {
			cout << "vhandle[5] :" << v_it->idx() << endl;
		}
		if (vhandle[9] == mesh.vertex_handle(v_it->idx())) {
			cout << "vhandle[9] :" << v_it->idx() << endl;
		}
	}

	for (MyMesh::EdgeIter e_it = mesh.edges_begin(); e_it != mesh.edges_end(); e_it++) {

		
		//cout << "All edge :" << e_it->idx() << endl;
	}
	for (MyMesh::VertexEdgeIter ve_it = mesh.ve_begin(vhandle[5]); ve_it.is_valid(); ve_it++)
	{
		MyMesh::HalfedgeHandle h = mesh.halfedge_handle(mesh.edge_handle(ve_it->idx()), 0);
		MyMesh::VertexHandle a[2];
		a[0] = mesh.to_vertex_handle(h);
		a[1] = mesh.from_vertex_handle(h);
		cout <<"1 : "<<mesh.point(a[0]) << endl;
		cout <<"2 : "<<mesh.point(a[1]) << endl;
		cout << "~~~~~~~~~~~~~~~~~~~~~" << endl;
	//	cout << "Vertex edge 5 :" << ve_it->idx() << endl;
	}
	for (MyMesh::VertexEdgeIter ve_it = mesh.ve_begin(vhandle[4]); ve_it.is_valid(); ve_it++)
	{
		cout << "Vertex edge 4 :" << ve_it->idx() << endl;
	}

	//OpenMesh::IO::write_mesh(mesh, "fps1.stl", 0);
	for (MyMesh::HalfedgeIter it = mesh.halfedges_begin(); it != mesh.halfedges_end(); ++it) {
		if (mesh.to_vertex_handle(*it) == vhandle[4] &&
			mesh.from_vertex_handle(*it) == vhandle[5])
		{
			// Collapse edge
			mesh.collapse(*it); //已经标记了进行删除，所以直接进行garbage_collection();
			if (!mesh.is_collapse_ok(*it)) {
				cout << "YES" << endl;
			}
			cout << "Collapse over!" << endl;
								//if (!mesh.is_collapse_ok(*it)) {// 
								//	/*mesh.delete_face(fhandle[5]);
								//	mesh.delete_face()*/
								//	//mesh.delete_vertex(vhandle[5]);
								//	cout << "YES" << endl;
								//}
			break;
		}
	}
	for (MyMesh::EdgeIter e_it = mesh.edges_begin(); e_it != mesh.edges_end(); e_it++) {
		cout << "All edge :" << e_it->idx() << endl;
	}

	for (MyMesh::VertexVertexIter vv_it = mesh.vv_iter(vhandle[4]); vv_it.is_valid(); ++vv_it)
	{
		cout << mesh.point(*vv_it) << " " << vv_it->idx() << endl;
	}
	printf("vertex num:%d\n", mesh.n_vertices());
	printf("face num:%d\n", mesh.n_faces());


	for (MyMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it) {
		std::cout << "The face's valence is " << mesh.valence(*f_it) << std::endl;
		//if (!mesh.valence(*f_it)) {
		//	mesh.delete_face(*f_it);
		//}
		//else {
		for (MyMesh::FaceVertexIter vf_it = mesh.fv_iter(*f_it); vf_it.is_valid(); ++vf_it) {
			cout << mesh.point(*vf_it) << " " << vf_it->idx() << " || ";
		}
		cout << endl;
		//}
	}
	cout << "collapsed" << endl;
	for (MyMesh::VertexIter v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it)
	{
		cout << "All vertex :" << v_it->idx() << " " << mesh.point(*v_it) << endl;

		if (vhandle[5] == mesh.vertex_handle(v_it->idx())) {
			cout << "vhandle[5] :" << v_it->idx() << endl;
		}
		if (vhandle[9] == mesh.vertex_handle(v_it->idx())) {
			cout << "vhandle[9] :" << v_it->idx() << endl;
		}
	}
	for (MyMesh::EdgeIter e_it = mesh.edges_begin(); e_it != mesh.edges_end(); e_it++) {
		cout << "All edge :" << e_it->idx() << endl;
	}

	for (MyMesh::VertexEdgeIter ve_it = mesh.ve_begin(vhandle[5]); ve_it.is_valid(); ve_it++)
	{
		cout << "Vertex edge 5 :" << ve_it->idx() << endl;
	}
	for (MyMesh::VertexEdgeIter ve_it = mesh.ve_begin(vhandle[4]); ve_it.is_valid(); ve_it++)
	{
		cout << "Vertex edge 4 :" << ve_it->idx() << endl;
	}

	mesh.garbage_collection();
	cout << "garbage_collection" << endl;

	for (MyMesh::VertexEdgeIter ve_it = mesh.ve_begin(vhandle[5]); ve_it.is_valid(); ve_it++)
	{
		cout << "Vertex edge 5 :" << ve_it->idx() << endl;
	}

	for (MyMesh::VertexEdgeIter ve_it = mesh.ve_begin(vhandle[4]); ve_it.is_valid(); ve_it++)
	{
		cout << "Vertex edge 4 :" << ve_it->idx() << endl;
	}

	for (MyMesh::EdgeIter e_it = mesh.edges_begin(); e_it != mesh.edges_end(); e_it++) {
		cout << "All edge :" << e_it->idx() << endl;
	}
	for (MyMesh::VertexIter v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it)
	{
		cout << "All vertex :" << v_it->idx() << " " << mesh.point(*v_it) << endl;

		if (vhandle[5] == mesh.vertex_handle(v_it->idx())) {
			cout << "vhandle[5] :" << v_it->idx() << endl;
		}
		if (vhandle[9] == mesh.vertex_handle(v_it->idx())) {
			cout << "vhandle[9] :" << v_it->idx() << endl;
		}
	}

	printf("face num:%d\n", mesh.n_faces());
	OpenMesh::IO::write_mesh(mesh, "fps.stl", 0);
	//mesh.set_point(vhandle[4], MyMesh::Point(3, 3, 0));
	//cout << mesh.point(vhandle[4]) << endl;
}