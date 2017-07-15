#include "stdafx.h"
#include "TriMeshSimplify.h"


TriMeshSimplify::TriMeshSimplify(MyMesh&m, MyMesh::FaceHandle b):
	mesh(m),
	m_Mesh_Face(b)
{
}

TriMeshSimplify::~TriMeshSimplify()
{
}

void TriMeshSimplify::UpdateTriQ() {
	Matrix4d m_Face_Q;
	m_Face_Q << 0, 0, 0, 0,
				0, 0, 0, 0,
				0, 0, 0, 0,
				0, 0, 0, 0;

	//m_Face_Arround.clear();

	set<int> atface;
	//m_Collapse_Half = mesh.halfedge_handle(mesh.fh_begin(m_Mesh_Face)->idx()); //先给一个，以后不用的话再删除，用来调试； ？？？

	for (MyMesh::FaceVertexIter fv_iter = mesh.fv_begin(m_Mesh_Face); fv_iter.is_valid(); ++fv_iter)
	{
		MyMesh::VertexHandle vhp = mesh.vertex_handle(fv_iter->idx());
		for (MyMesh::VertexFaceIter vf_iter = mesh.vf_begin(vhp); vf_iter.is_valid(); ++vf_iter)
		{
			atface.insert(vf_iter->idx());
		}
	}
	
	MyMesh::Point arv[3];
	set<int>::iterator sit(atface.begin());
	for (; sit != atface.end(); sit++) {
		//m_Face_Arround[*sit] = 1; //record
		int i = 0;
		for (MyMesh::FaceVertexIter fv_iter = mesh.fv_begin(mesh.face_handle(*sit)); fv_iter.is_valid(); ++fv_iter)
		{
			/*if (i >= 3) {
				cout << "Up three:"<<i<<" "<<fv_iter->idx() << endl;
				break;
			}*/
			arv[i] = mesh.point(*fv_iter);
			i++;
		}
		m_Face_Q += ComputeSurfaceQ(arv);
	}

	m_QX_Set.a = ComputeMaxPoint(m_Face_Q);
	m_QX_Set.i = m_Mesh_Face.idx();
}

bool TriMeshSimplify::UpdateHalfEdge(MyMesh::HalfedgeHandle cc) {
	//m_Collapse_Half = cc; //mesh.halfedge_handle(mesh.fh_begin(m_Mesh_Face)->idx());
	for (MyMesh::FaceHalfedgeIter fh_iter = mesh.fh_begin(mesh.face_handle(cc)); fh_iter.is_valid(); fh_iter++) {
		MyMesh::HalfedgeHandle ah = mesh.halfedge_handle(fh_iter->idx());

		MyMesh::HalfedgeHandle ah_a = mesh.next_halfedge_handle(mesh.opposite_halfedge_handle(ah));//opposit next

		MyMesh::HalfedgeHandle ah_b = mesh.prev_halfedge_handle(mesh.opposite_halfedge_handle(mesh.prev_halfedge_handle(ah)));

		if (ah_a.idx() == mesh.opposite_halfedge_handle(ah_b).idx()) {
			//m_Collapse_Half = ah;
			//break;
			return true;
		}
	}
	return false;
}

bool TriMeshSimplify::is_Three_Tri(MyMesh::FaceHandle cc,MyMesh::HalfedgeHandle &hh) {

	for (MyMesh::FaceHalfedgeIter fh_iter = mesh.fh_begin(cc); fh_iter.is_valid(); fh_iter++) {
		MyMesh::HalfedgeHandle ah = mesh.halfedge_handle(fh_iter->idx());

		MyMesh::HalfedgeHandle ah_a = mesh.next_halfedge_handle(mesh.opposite_halfedge_handle(ah));//opposit next

		MyMesh::HalfedgeHandle ah_b = mesh.prev_halfedge_handle(mesh.opposite_halfedge_handle(mesh.prev_halfedge_handle(ah)));

		if (ah_a.idx() == mesh.opposite_halfedge_handle(ah_b).idx()) {
			hh = ah;
			return true;
		}
	}
	return false;
}

Matrix4d TriMeshSimplify::ComputeSurfaceQ(MyMesh::Point *vp) //三个面的顶点坐标计算Q值
{
	Matrix4d m_Face_Q;
	Vector3d a((*vp)[0], (*vp)[1], (*vp)[2]);
	Vector3d b((*(vp + 1))[0], (*(vp + 1))[1], (*(vp + 1))[2]);
	Vector3d c((*(vp + 2))[0], (*(vp + 2))[1], (*(vp + 2))[2]);

	Vector3d ab = a - b;
	ab = (a - c).cross(ab);
	ab.normalize();
	double df = ab.dot(Vector3d(0, 0, 0) - a);

	Vector4d coe(ab[0], ab[1], ab[2], df); //surface equation cofficient!

	m_Face_Q = coe*coe.transpose(); //update Q
	return m_Face_Q;
}

double TriMeshSimplify::ComputeMaxPoint(Matrix4d& m_Face_Q)//(MyMesh::VertexHandle tmpa, MyMesh::VertexHandle tmpb)
{
	Matrix4d m_Q_Equation;
	m_Q_Equation = m_Face_Q;
	m_Q_Equation(3, 0) = 0;
	m_Q_Equation(3, 1) = 0;
	m_Q_Equation(3, 2) = 0;
	m_Q_Equation(3, 3) = 1;
	double m_x;

	Vector4d ori(0, 0, 0, 1);
	//Vector4d m_v=m_Q_Equation.fullPivHouseholderQr().solve(ori); //改成局部变量；
	Vector4d m_v = m_Q_Equation.colPivHouseholderQr().solve(ori); //使用col会不会提速一点（这两个没啥区别，速度都差不多！）
																  //check;
	double temp = (m_Q_Equation*m_v - ori).norm();
	if ((!temp) || (temp < SOLUTIONACCURANCY)) { //解出来方程的情况下
		m_v_mesh = MyMesh::Point(m_v[0], m_v[1], m_v[2]); //无论如何访问这个最优点
		m_x = m_v.transpose()*m_Face_Q*m_v;
		return m_x;
	}

	//对于没有解出来方程的情况下，需要进行七个点的测试取其中的误差最小值
	vector<Vector4d> ptemp;
	STRUCT_SET_EDGEPAIR iter;
	int i = 0;
	for (MyMesh::FaceVertexIter fv_iter = mesh.fv_begin(m_Mesh_Face); fv_iter.is_valid(); ++fv_iter)
	{
		if (i == 3) {
			cout << "3 break;" << endl;
			break;
		}
		MyMesh::Point p= mesh.point(*fv_iter);
		ptemp.push_back(Vector4d(p[0], p[1], p[2], 1));
		i++;
	}
	ptemp.push_back( (ptemp[0] + ptemp[1]) / 2);
	ptemp.push_back( (ptemp[1] + ptemp[2]) / 2);
	ptemp.push_back( (ptemp[0] + ptemp[2]) / 2);
	ptemp.push_back( (ptemp[0] + ptemp[1] + ptemp[2]) / 3);

	for (i = 0; i < ptemp.size();i++) {
		MESHSETEDGE git;
		git.a=ptemp[i].transpose()*m_Face_Q*ptemp[i];
		git.i = i;
		iter.insert(git);
	}
	STRUCT_SET_EDGEPAIR::iterator it(iter.begin());
	m_x = it->a;
	m_v_mesh = MyMesh::Point(ptemp[it->i][0], ptemp[it->i][1], ptemp[it->i][2]);
	return m_x;
}

int TriMeshSimplify::TriCollapse(vector<int>&a, vector<int>&b)
{
	//UpdateHalfEdge();//每次都需要进行检查更新一下，看看是否出现错误的点；
	if (m_Mesh_Face.idx() == 36461) {
		cout << "find it" << endl;
	}

	if (!is_Three_Tri(m_Mesh_Face, m_Collapse_Half)) {
		for (MyMesh::FaceHalfedgeIter fh_iter = mesh.fh_begin(m_Mesh_Face); fh_iter.is_valid(); fh_iter++) {
			m_Collapse_Half = mesh.halfedge_handle(fh_iter->idx());
			if (UpdateHalfEdge(mesh.opposite_halfedge_handle(m_Collapse_Half))) {//相邻两个都不应该是特殊结构；
				return mesh.face_handle(mesh.opposite_halfedge_handle(m_Collapse_Half)).idx();
			}
		}
	}
	if (!DebugInfo2()) {
		return m_Mesh_Face.idx();
	}
	if (m_Collapse_Half.idx()==-1) {
		cout << "error -1" << endl;
	}
	a.push_back(mesh.face_handle(m_Collapse_Half).idx());
	a.push_back(mesh.face_handle(mesh.opposite_halfedge_handle(m_Collapse_Half)).idx());

	MyMesh::HalfedgeHandle halfn = mesh.opposite_halfedge_handle(mesh.next_halfedge_handle(m_Collapse_Half));

	

	MyMesh::VertexHandle vp = mesh.to_vertex_handle(m_Collapse_Half);

	mesh.collapse(m_Collapse_Half);

	a.push_back(mesh.face_handle(halfn).idx());
	a.push_back(mesh.face_handle(mesh.opposite_halfedge_handle(halfn)).idx());

	mesh.collapse(halfn);
	
	mesh.set_point(vp, m_v_mesh);

	for (MyMesh::VertexFaceIter vf_iter = mesh.vf_begin(vp); vf_iter.is_valid(); ++vf_iter) {
		//m_Face_Arround[vf_iter->idx()]++;
		b.push_back(vf_iter->idx());
	}
	return -1;
}

int TriMeshSimplify::is_TriMesh(MyMesh::VertexHandle vh) { //用来判断一个点周围是否仅仅有三个三角形；
	int i = 1;
	for (MyMesh::VertexFaceIter vf_iter = mesh.vf_begin(vh); vf_iter.is_valid(); vf_iter++) {
		i++;
	}
	if (i == 3) {
		return 1;
	}
	if (i < 3) {
		return 2;
	}
	return 0;
}

bool TriMeshSimplify::is_TriMesh() {
	int i = 0;
	for (MyMesh::FaceVertexIter fv_iter = mesh.fv_begin(m_Mesh_Face); fv_iter.is_valid(); ++fv_iter)
	{
		i++;
	}
	if (i < 3) {
		return false;
	}
	return true;
}

void TriMeshSimplify::DebugInfo()
{
	cout << "Debug mesh Tri "<< m_Mesh_Face.idx()<<" :" << endl;
	cout<< "This collapse edge: " << m_Collapse_Half.idx() << "-"<<mesh.opposite_halfedge_handle(m_Collapse_Half)<<" "<<mesh.next_halfedge_handle(m_Collapse_Half)<<"-"<<mesh.opposite_halfedge_handle(mesh.next_halfedge_handle(m_Collapse_Half))<<" "<<mesh.next_halfedge_handle(mesh.next_halfedge_handle(m_Collapse_Half))<<"-"<<mesh.opposite_halfedge_handle(mesh.next_halfedge_handle(mesh.next_halfedge_handle(m_Collapse_Half))) << endl;
	for (MyMesh::FaceVertexIter fv_iter = mesh.fv_begin(m_Mesh_Face); fv_iter.is_valid(); fv_iter++) {
		cout << "Face Vertex: " << fv_iter->idx() << endl;
		for (MyMesh::VertexFaceIter vf_iter = mesh.vf_begin(mesh.vertex_handle(fv_iter->idx())); vf_iter.is_valid(); vf_iter++) {
			cout << "Face " << vf_iter->idx() << " halfedge: ";
			for (MyMesh::FaceHalfedgeCWIter fh_iter = mesh.fh_cwbegin(mesh.face_handle(vf_iter->idx())); fh_iter.is_valid(); fh_iter++) {
				cout << fh_iter->idx() << "-" << mesh.opposite_halfedge_handle(mesh.halfedge_handle(fh_iter->idx())) << " ";
			}
			cout << endl;
		}
	}
	cout << endl;
}

bool TriMeshSimplify::DebugInfo2() {
	int cc = 0;
	for (MyMesh::FaceVertexIter fv_iter = mesh.fv_begin(m_Mesh_Face); fv_iter.is_valid(); fv_iter++) {
		int i = 0;
		for (MyMesh::VertexFaceIter vf_iter = mesh.vf_begin(mesh.vertex_handle(fv_iter->idx())); vf_iter.is_valid(); vf_iter++) {
			i++;
		}
		if (i == 3) {
			cc++;
		}
		else if (i<3) {
			return false;
		}
	}
	if (cc > 1) {
		return false;
	}
	return true;
}

MyTriOpenMesh::MyTriOpenMesh()
{
}
MyTriOpenMesh::~MyTriOpenMesh()
{
}

int MyTriOpenMesh::Readfile(const char * argg)
{
	// read mesh from stdin
	cout << "正在读取网格文件..." << endl;
	if (!OpenMesh::IO::read_mesh(mesh, argg, opt))
	{
		std::cerr << "Error: Cannot read mesh from " << std::endl;
		return 1;
	}

	mesh.request_vertex_status();
	mesh.request_edge_status();
	mesh.request_face_status();
	cout << "Read File Over!" << endl;
	return 0;
}

void MyTriOpenMesh::Writefile(const char *argg, int i) {
	cout << "Now is Writing File..." << endl;
	if (!OpenMesh::IO::write_mesh(mesh, argg, i)) //0 ascii 1 binary
	{
		std::cerr << "Error: cannot write mesh to " << argg << std::endl;
	}
}

void MyTriOpenMesh::Writefile(const char *argg) {
	if (!OpenMesh::IO::write_mesh(mesh, argg)) //obj file
	{
		std::cerr << "Error: cannot write mesh to " << argg << std::endl;
	}
}

int MyTriOpenMesh::MeshSimplification(float dest)
{
	if ((dest >= 1) || (dest <= 0)) {
		cout << "Your input is out of range, MainLoop" << endl;
		return 1;
	}
	int idest = mesh.n_faces()*dest;//有个转换
	FillTriMeshMap();
	printf("顶点数目统计 : 简化前:%d  目标:%d  差值:%d\n", mesh.n_faces(), mesh.n_faces() - idest, idest);

	while (idest>0) {
		idest -=CollapseIterator();
	}
	Release();
}

void MyTriOpenMesh::FillTriMeshMap()
{
	cout << "Fill the Map :"<<mesh.n_faces() << endl;
	int i = 0;
	for (MyMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it)
	{
		i = f_it->idx();
		m_TriMesh_Map[i] = new TriMeshSimplify(mesh,mesh.face_handle(i));

		m_TriMesh_Map[i]->UpdateTriQ();

		m_TriMesh_Set.insert(m_TriMesh_Map[i]->ReturnTriSet());

	}
}

int MyTriOpenMesh::CollapseIterator()
{
	STRUCT_SET_EDGEPAIR::iterator it(m_TriMesh_Set.begin());
	//cout << it->i<<":"<< endl;

	//309493
	vector<int> delet, reserve;
	int judge = m_TriMesh_Map[it->i]->TriCollapse(delet,reserve);
	if (judge != -1) {
		//m_TriMesh_Set.erase(m_TriMesh_Map[it->i]->ReturnTriSet());//应该有两个方案，不删除，或者删除它；
		//cout << "-1 collapse : " << it->i << " " << judge << endl;
		if (it->i == judge) {
			m_TriMesh_Set.erase(m_TriMesh_Map[it->i]->ReturnTriSet());
			return 0;
		}

		delet.clear();
		reserve.clear();
		judge = m_TriMesh_Map[judge]->TriCollapse(delet, reserve);
		if (judge != -1) {
			//m_TriMesh_Set.erase(m_TriMesh_Map[imf]->ReturnTriSet());//应该有两个方案，不删除，或者删除它；
			m_TriMesh_Set.erase(m_TriMesh_Map[it->i]->ReturnTriSet());
			cout << "-2 collapse : " << judge << " saw:" << it->i << endl;
			return 0;
		}
		//return CollapseIterator(judge,it->i);
	}

	for (auto i : delet) {
		m_TriMesh_Set.erase(m_TriMesh_Map[i]->ReturnTriSet());
	}
	for (auto i : reserve) {
		m_TriMesh_Set.erase(m_TriMesh_Map[i]->ReturnTriSet());
		m_TriMesh_Map[i]->UpdateTriQ();
		m_TriMesh_Set.insert(m_TriMesh_Map[i]->ReturnTriSet());
	}

	return delet.size();
}

void MyTriOpenMesh::InterFacePort(string inputfilename, string outputfilename, float dest) {
	if ((dest < 0)||(dest>=1)) {
		cout << "简化量输入不合法！" << endl;
		return;
	}
	//cout << "简化输出文件 ：" << outputfilename << endl;
	cout << "目标简化量：" << dest << endl;
	//string outputfilename = "DIT2-4-simplify-60.ply";
	Readfile(inputfilename.c_str());
	cout << "读取文件完毕，开始网格简化..." << endl;
	MeshSimplification(dest);
	cout << "简化完毕，正在生成目标简化网格文件..." << endl;
	Writefile(outputfilename.c_str());
}

void MyTriOpenMesh::InterFacePort() { 
	if ((m_dest < 0) || (m_dest >= 1)) {
		cout << "简化量输入不合法！" << endl;
		return;
	}
	MeshSimplification(m_dest);
}