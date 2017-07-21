// SurfaceSimplification.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "IncludeHeader.h"

#include "MyOpenMesh.h"

#include "TriMeshSimplify.h"

void main_test();
void main_test2();
void main_test3();

void go_test(string inputfilename,float dest) {
	MyOpenMesh  ims;
	//float dest = 0.6; //缩小原来点的多少 dest*100%

	int ith = inputfilename.find('.');
	string  outputfilename = inputfilename.substr(0,ith)+"-Simplify"+ inputfilename.substr(ith);
	cout <<"简化输出文件 ："<<outputfilename << endl;
	cout << "目标简化量：" <<dest<< endl;
	//string outputfilename = "DIT2-4-simplify-60.ply";
	ims.Readfile(inputfilename.c_str());
	cout << "读取文件完毕，开始网格简化..." << endl;
	ims.MeshSimplification(dest);
	cout << "简化完毕，正在生成简化网格文件..." << endl;
	ims.Writefile(outputfilename.c_str(), 1);
}

void go_test2(string inputfilename, float dest) {
	MyTriOpenMesh  ims;
	//float dest = 0.6; //缩小原来点的多少 dest*100%

	int ith = inputfilename.find('.');
	string  outputfilename = inputfilename.substr(0, ith) + "-Tri-Simplify" + inputfilename.substr(ith);
	cout << "简化输出文件 ：" << outputfilename << endl;
	cout << "目标简化量：" << dest << endl;
	//string outputfilename = "DIT2-4-simplify-60.ply";
	ims.Readfile(inputfilename.c_str());
	cout << "读取文件完毕，开始网格简化..." << endl;
	ims.MeshSimplification(dest);
	cout << "简化完毕，正在生成简化网格文件..." << endl;
	ims.Writefile(outputfilename.c_str(), 1);
}
int main(int argc, char** argv)
{
	/*MyMesh mesh2;
	OpenMesh::IO::Options opt;
	OpenMesh::IO::read_mesh(mesh2, "fps2.stl", opt);
	cout << mesh2.n_faces() << endl;*/

	//main_test(); //测试
	//main_test2();
	//main_test3();

	//MyOpenMesh  ims;
	//
	//float dest = 0.6; //缩小原来点的多少 dest*100%
	//string inputfilename = "DIT2-4.ply";
	//string outputfilename = "DIT2-4-simplify-60.ply";
	//ims.Readfile(inputfilename.c_str());
	//ims.MeshSimplification(dest);
	//ims.Writefile(outputfilename.c_str(),1);
	//system("pause");

	//MyTriOpenMesh  ims;
	//
	//float dest = 0.8; //缩小原来点的多少 dest*100%
	//string inputfilename = "Total-denoised-Tri-simplify-80.ply";
	//string outputfilename = "Total-denoised-Tri-simplify-802.ply";
	///*string inputfilename = "DIT-2.stl";
	//string outputfilename = "DIT-2-Tri-simplify-80.stl";*/
	//if (ims.Readfile(inputfilename.c_str())) {
	//	return 0;
	//}
	//ims.MeshSimplification(dest);
	//ims.Writefile(outputfilename.c_str(),0);
	//system("pause");

	if (argc<4) {
		cout << "您输入的参数不足" << endl;
	}
	cout << argv[1] << endl;
	cout << argv[2] << endl;
	cout << argv[3] << endl;
	float dest = 0;
	dest = atof(argv[3]);
	MyTriOpenMesh ims;
	ims.Readfile(argv[1]);
	cout << "读取文件完毕，开始网格简化..." << endl;
	ims.MeshSimplification(dest);
	cout << "简化完毕，正在生成简化网格文件..." << endl;
	ims.Writefile(argv[2]);

	/*while (1) {
		cout << "请输入需要简化二进制文件路径（*.obj *.stl *.off *.ply）：" << endl;
		string strs;
		cin >> strs;
		cout << "请输入简化量（比如0.6：删除原来百分之六十的三角形）：" << endl;
		string num;
		cin >> num;
		float dest=atof(num.c_str());
		go_test2(strs,dest);
	}*/

	return 0;
}

void main_test3() {
	MyMesh mesh;
	// Request required status flags
	// Add some vertices as in the illustration above
	MyMesh::VertexHandle vhandle[10];
	vhandle[0] = mesh.add_vertex(MyMesh::Point(0, 0, 0));

	vhandle[1] = mesh.add_vertex(MyMesh::Point(4, 0, 0));
	vhandle[2] = mesh.add_vertex(MyMesh::Point(2, 2, 0));

	vhandle[3] = mesh.add_vertex(MyMesh::Point(2, 4, 0));

	vector<MyMesh::VertexHandle> face_vhandles;
	MyMesh::FaceHandle   fhandle[10];

	face_vhandles.push_back(vhandle[0]);
	face_vhandles.push_back(vhandle[2]);
	face_vhandles.push_back(vhandle[1]);
	fhandle[0] = mesh.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[2]);
	face_vhandles.push_back(vhandle[0]);
	face_vhandles.push_back(vhandle[3]);
	fhandle[1] = mesh.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[2]);
	face_vhandles.push_back(vhandle[3]);
	face_vhandles.push_back(vhandle[1]);
	fhandle[1] = mesh.add_face(face_vhandles);

	printf("face num:%d\n", mesh.n_faces());
	printf("vertex num:%d\n", mesh.n_vertices());
	
	for (MyMesh::VertexOHalfedgeIter it = mesh.voh_begin(vhandle[0]); it.is_valid(); it++) {
		MyMesh::HalfedgeHandle hf = mesh.halfedge_handle(it->idx());
		if (mesh.to_vertex_handle(hf) == vhandle[3]) {
			mesh.collapse(hf);
		}
	}
	OpenMesh::IO::write_mesh(mesh, "ppst2.obj");
}

void main_test2() {
	MyMesh mesh;
	// Request required status flags
	// Add some vertices as in the illustration above
	MyMesh::VertexHandle vhandle[10];
	vhandle[0] = mesh.add_vertex(MyMesh::Point(6, 0, 0));

	vhandle[1] = mesh.add_vertex(MyMesh::Point(0, 1, 0));
	vhandle[2] = mesh.add_vertex(MyMesh::Point(12, 1, 0));

	vhandle[3] = mesh.add_vertex(MyMesh::Point(3, 2, 0));
	vhandle[4] = mesh.add_vertex(MyMesh::Point(9, 2, 0));

	vhandle[5] = mesh.add_vertex(MyMesh::Point(6, 3, 0));

	vhandle[6] = mesh.add_vertex(MyMesh::Point(1, 4, 0));
	vhandle[7] = mesh.add_vertex(MyMesh::Point(11, 4, 0));

	vhandle[8] = mesh.add_vertex(MyMesh::Point(6, 5, 0));

	vector<MyMesh::VertexHandle> face_vhandles;
	MyMesh::FaceHandle   fhandle[10];
	int acf = 0;
	
	face_vhandles.push_back(vhandle[1]);
	face_vhandles.push_back(vhandle[0]);
	face_vhandles.push_back(vhandle[3]);
	fhandle[0] = mesh.add_face(face_vhandles);
	cout << acf++ << endl;

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[1]);
	face_vhandles.push_back(vhandle[3]);
	face_vhandles.push_back(vhandle[6]);
	fhandle[1] = mesh.add_face(face_vhandles);
	cout << acf++ << endl;

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[6]);//
	face_vhandles.push_back(vhandle[3]);
	face_vhandles.push_back(vhandle[5]);
	fhandle[2] = mesh.add_face(face_vhandles);
	cout << acf++ << endl;

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[5]);
	face_vhandles.push_back(vhandle[3]);
	face_vhandles.push_back(vhandle[4]);
	fhandle[3] = mesh.add_face(face_vhandles);
	cout << acf++ << endl;

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[4]);
	face_vhandles.push_back(vhandle[3]);
	face_vhandles.push_back(vhandle[0]);
	fhandle[4] = mesh.add_face(face_vhandles);
	cout << acf++ << endl;

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[4]);
	face_vhandles.push_back(vhandle[0]);
	face_vhandles.push_back(vhandle[2]);
	fhandle[5] = mesh.add_face(face_vhandles);
	cout << acf++ << endl;

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[4]);
	face_vhandles.push_back(vhandle[2]);
	face_vhandles.push_back(vhandle[7]);
	fhandle[6] = mesh.add_face(face_vhandles);
	cout << acf++ << endl;

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[4]);
	face_vhandles.push_back(vhandle[7]);
	face_vhandles.push_back(vhandle[5]);
	fhandle[7] = mesh.add_face(face_vhandles);
	cout << acf++ << endl;

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[5]);
	face_vhandles.push_back(vhandle[7]);
	face_vhandles.push_back(vhandle[8]);
	fhandle[8] = mesh.add_face(face_vhandles);
	cout << acf++ << endl;

	face_vhandles.clear();//
	face_vhandles.push_back(vhandle[5]);
	face_vhandles.push_back(vhandle[8]);
	face_vhandles.push_back(vhandle[6]);
	fhandle[9] = mesh.add_face(face_vhandles);
	cout << acf++ << endl;

	printf("face num:%d\n", mesh.n_faces());
	printf("vertex num:%d\n", mesh.n_vertices());
	printf("half num:%d\n", mesh.n_halfedges());
	printf("edges num:%d\n", mesh.n_edges());

	mesh.request_vertex_status();
	mesh.request_edge_status();
	mesh.request_face_status();
	

	/*MyTriOpenMesh  ims(mesh);
	ims.MeshSimplification(0.5);
	ims.Writefile("hui.obj");*/

	//OpenMesh::IO::write_mesh(mesh, "ppsj.obj");

	/*cout << fhandle[3].idx() << endl;
	for (MyMesh::FaceFaceIter f_iter = mesh.ff_begin(fhandle[3]); f_iter.is_valid(); ++f_iter) {
		cout << f_iter->idx() << endl;
	}*/

	for (MyMesh::VertexOHalfedgeIter voh_iter = mesh.voh_begin(mesh.vertex_handle(5)); voh_iter.is_valid(); voh_iter++) {
		MyMesh::HalfedgeHandle hh = mesh.halfedge_handle(voh_iter->idx());
		MyMesh::HalfedgeHandle hnt = mesh.opposite_halfedge_handle(hh);
		if (mesh.to_vertex_handle(hh).idx() == 3) {
			MyMesh::HalfedgeHandle hn = mesh.opposite_halfedge_handle(mesh.next_halfedge_handle(hh));
			cout << "collapse edge:" << hh.idx() << endl;
			cout << "opposit half edge:" << hn.idx() << endl;
			//MyMesh::HalfedgeHandle hn = mesh.prev_halfedge_handle(hh);

			MyMesh::FaceHalfedgeIter fh_iter = mesh.fh_begin(fhandle[3]);
			MyMesh::HalfedgeHandle mhf = mesh.halfedge_handle(fh_iter->idx());
			cout << "arount face 3 half:" << mhf.idx() << " " << mesh.next_halfedge_handle(mhf).idx() << " " << mesh.next_halfedge_handle(mesh.next_halfedge_handle(mhf)) << endl;

			fh_iter = mesh.fh_begin(fhandle[2]);
			mhf = mesh.halfedge_handle(fh_iter->idx());
			cout << "arount face 2 half:" << mhf.idx() << " " << mesh.next_halfedge_handle(mhf).idx() << " " << mesh.next_halfedge_handle(mesh.next_halfedge_handle(mhf)) << endl;

			 fh_iter = mesh.fh_begin(fhandle[4]);
			mhf = mesh.halfedge_handle(fh_iter->idx());
			cout << "arount face 4 half:" << mhf.idx() << " " << mesh.next_halfedge_handle(mhf).idx() << " " << mesh.next_halfedge_handle(mesh.next_halfedge_handle(mhf)) << endl;

			 fh_iter = mesh.fh_begin(fhandle[7]);
			mhf = mesh.halfedge_handle(fh_iter->idx());
			cout << "arount face 7 half:" << mhf.idx() << " " << mesh.next_halfedge_handle(mhf).idx() << " " << mesh.next_halfedge_handle(mesh.next_halfedge_handle(mhf)) << endl;

			fh_iter = mesh.fh_begin(fhandle[0]);
			mhf = mesh.halfedge_handle(fh_iter->idx());
			cout << "arount face 0 half:" << mhf.idx() << " " << mesh.next_halfedge_handle(mhf).idx() << " " << mesh.next_halfedge_handle(mesh.next_halfedge_handle(mhf)) << endl;

			fh_iter = mesh.fh_begin(fhandle[5]);
			mhf = mesh.halfedge_handle(fh_iter->idx());
			cout << "arount face 5 half:" << mhf.idx() << " " << mesh.next_halfedge_handle(mhf).idx() << " " << mesh.next_halfedge_handle(mesh.next_halfedge_handle(mhf)) << endl;

			

			cout << "Arround Vertex Face  3: ";
			for (MyMesh::VertexFaceIter vf_iter = mesh.vf_begin(vhandle[3]); vf_iter.is_valid();vf_iter++) {
				cout << vf_iter->idx() << " ";
			}
			cout << endl;
			cout << "------------------" << endl;

			mesh.collapse(hh);

			cout << "Arround Vertex Face  3: ";
			for (MyMesh::VertexFaceIter vf_iter = mesh.vf_begin(vhandle[3]); vf_iter.is_valid(); vf_iter++) {
				cout << vf_iter->idx() << " ";
			}
			cout << endl;

			if (fhandle[3].is_valid()) {
				cout << "collapse 3:";
				for (fh_iter = mesh.fh_begin(fhandle[3]); fh_iter.is_valid(); fh_iter++) {
					cout << mesh.halfedge_handle(fh_iter->idx()).idx()<<" ";
				}
				cout << endl;
			}
				
			if (fhandle[2].is_valid()) {
				cout << "collapse 2:";
				for (fh_iter = mesh.fh_begin(fhandle[2]); fh_iter.is_valid(); fh_iter++) {
					cout << mesh.halfedge_handle(fh_iter->idx()).idx()<<" ";
				}
				cout << endl;
			}
			
			if (fhandle[4].is_valid()) {
				cout << "collapse 4:";
				for (fh_iter = mesh.fh_begin(fhandle[4]); fh_iter.is_valid(); fh_iter++) {
					cout << mesh.halfedge_handle(fh_iter->idx()).idx()<<" ";
				}
				cout << endl;
			}
			
			if (fhandle[7].is_valid()) {
				cout << "collapse 7:";
				for (fh_iter = mesh.fh_begin(fhandle[7]); fh_iter.is_valid(); fh_iter++) {
					cout << mesh.halfedge_handle(fh_iter->idx()).idx()<<" ";
				}
				cout << endl;
			}
			
			mesh.collapse(hn);

			cout << "Arround Vertex Face  3: ";
			for (MyMesh::VertexFaceIter vf_iter = mesh.vf_begin(vhandle[3]); vf_iter.is_valid(); vf_iter++) {
				cout << vf_iter->idx() << " ";
			}
			cout << endl;

			if (fhandle[4].is_valid()) {
				cout << "collapse 4:";
				for (fh_iter = mesh.fh_begin(fhandle[4]); fh_iter.is_valid(); fh_iter++) {
					cout << mesh.halfedge_handle(fh_iter->idx()).idx() << " ";
				}
				cout << endl;
			}

			if (fhandle[7].is_valid()) {
				cout << "collapse 7:";
				for (fh_iter = mesh.fh_begin(fhandle[7]); fh_iter.is_valid(); fh_iter++) {
					cout << mesh.halfedge_handle(fh_iter->idx()).idx() << " ";
				}
				cout << endl;
			}

			if (fhandle[0].is_valid()) {
				cout << "collapse 0:";
				for (fh_iter = mesh.fh_begin(fhandle[0]); fh_iter.is_valid(); fh_iter++) {
					cout << mesh.halfedge_handle(fh_iter->idx()).idx() << " ";
				}
				cout << endl;
			}

			if (fhandle[5].is_valid()) {
				cout << "collapse 5:";
				for (fh_iter = mesh.fh_begin(fhandle[5]); fh_iter.is_valid(); fh_iter++) {
					cout << mesh.halfedge_handle(fh_iter->idx()).idx() << " ";
				}
				cout << endl;
			}
			//OpenMesh::IO::write_mesh(mesh, "ppst2.obj");

			//if(!mesh.is_collapse_ok(hh)) { //是不是可以不用判断
			//	cout <<"yes"<< endl;
			//}
			break;
		}
	}

	//for (MyMesh::VertexOHalfedgeIter voh_iter = mesh.voh_begin(mesh.vertex_handle(3)); voh_iter.is_valid(); voh_iter++) {
	//	MyMesh::HalfedgeHandle hh = mesh.halfedge_handle(voh_iter->idx());
	//	MyMesh::HalfedgeHandle hnt = mesh.opposite_halfedge_handle(hh);
	//	cout << "4:" << mesh.to_vertex_handle(hh).idx() << endl;
	//	if (mesh.to_vertex_handle(hh).idx() == 0) {
	//		MyMesh::HalfedgeHandle hn = mesh.opposite_halfedge_handle(mesh.next_halfedge_handle(hh));
	//		cout << "fi:" << voh_iter->idx() << endl;
	//		mesh.collapse(hh);
	//		OpenMesh::IO::write_mesh(mesh, "ppst3.obj");
	//		mesh.collapse(hn);
	//		OpenMesh::IO::write_mesh(mesh, "ppst4.obj");
	//		//if(!mesh.is_collapse_ok(hh)) { //是不是可以不用判断
	//		//	cout <<"yes"<< endl;
	//		//}
	//		break;
	//	}
	//}
	//OpenMesh::IO::write_mesh(mesh, "ppst2.obj");

}

void main_test() {
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

	MyMesh mesh;
	//TriMyMesh mesh;

	// Add some vertices as in the illustration above
	MyMesh::VertexHandle vhandle[10];
	vhandle[0] = mesh.add_vertex(MyMesh::Point(0, 1, 0));
	vhandle[1] = mesh.add_vertex(MyMesh::Point(1, 0, 0));
	vhandle[2] = mesh.add_vertex(MyMesh::Point(7, 0, 0));
	vhandle[3] = mesh.add_vertex(MyMesh::Point(3, 5, 0));
	vhandle[4] = mesh.add_vertex(MyMesh::Point(2, 4, 0));
	vhandle[5] = mesh.add_vertex(MyMesh::Point(5, 1, 0));
	vhandle[6] = mesh.add_vertex(MyMesh::Point(5, -2, 0));
	vhandle[7] = mesh.add_vertex(MyMesh::Point(9, 3, 0));
	vhandle[8] = mesh.add_vertex(MyMesh::Point(8, -3, 0));
	//vhandle[9] = mesh.add_vertex(MyMesh::Point(5, 4, 0));

	// Add three quad faces
	std::vector<MyMesh::VertexHandle> face_vhandles;

	MyMesh::FaceHandle   fhandle[10];

	face_vhandles.push_back(vhandle[1]);  //0 5 1
	face_vhandles.push_back(vhandle[0]);
	face_vhandles.push_back(vhandle[5]);
	fhandle[0] = mesh.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[0]);  //0 4 5
	face_vhandles.push_back(vhandle[1]);
	face_vhandles.push_back(vhandle[7]);
	fhandle[1] = mesh.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[4]);//
	face_vhandles.push_back(vhandle[3]);
	face_vhandles.push_back(vhandle[5]);
	fhandle[2] = mesh.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[3]);
	face_vhandles.push_back(vhandle[2]);
	face_vhandles.push_back(vhandle[5]);
	fhandle[3] = mesh.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[5]);
	face_vhandles.push_back(vhandle[2]);
	face_vhandles.push_back(vhandle[1]);
	fhandle[4] = mesh.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[2]);
	face_vhandles.push_back(vhandle[6]);
	face_vhandles.push_back(vhandle[1]);
	fhandle[5] = mesh.add_face(face_vhandles);

	face_vhandles.clear();//
	face_vhandles.push_back(vhandle[2]);
	face_vhandles.push_back(vhandle[8]);
	face_vhandles.push_back(vhandle[6]);
	fhandle[6] = mesh.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[2]);
	face_vhandles.push_back(vhandle[7]);
	face_vhandles.push_back(vhandle[8]);
	fhandle[7] = mesh.add_face(face_vhandles);

	face_vhandles.clear();
	face_vhandles.push_back(vhandle[2]);
	face_vhandles.push_back(vhandle[3]);
	face_vhandles.push_back(vhandle[7]);
	fhandle[8] = mesh.add_face(face_vhandles);

	//face_vhandles.clear();//
	//face_vhandles.push_back(vhandle[4]);
	//face_vhandles.push_back(vhandle[1]);
	//face_vhandles.push_back(vhandle[2]);
	//fhandle[9] = mesh.add_face(face_vhandles);

	//mesh.delete_face(fhandle[7]);

	printf("face num:%d\n", mesh.n_faces());
	printf("vertex num:%d\n", mesh.n_vertices());

	mesh.request_vertex_status();
	mesh.request_edge_status();
	mesh.request_face_status();

	for (MyMesh::VertexIHalfedgeIter vi_iter = mesh.vih_begin(vhandle[1]); vi_iter.is_valid();vi_iter++) {
		MyMesh::HalfedgeHandle hf = mesh.halfedge_handle(vi_iter->idx());
		if (vhandle[2] == mesh.from_vertex_handle(hf)) {
			MyMesh::HalfedgeHandle nf = mesh.opposite_halfedge_handle(mesh.next_halfedge_handle(hf));
			mesh.collapse(hf);
			mesh.collapse(nf);
			
		}
	}
	mesh.garbage_collection();
	OpenMesh::IO::write_mesh(mesh, "fps3.stl", 0);

}