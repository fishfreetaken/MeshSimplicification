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
	//is_Obtuse_Tri();//判断是否为钝角三角形
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

void TriMeshSimplify::is_Obtuse_Tri() {
	MyMesh::Point cc[3];
	MyMesh::Point vec[2];
	MyMesh::VertexHandle vh[3];
	float t = 0;
	int i = 0;
	for (MyMesh::FaceVertexIter fv_iter = mesh.fv_begin(m_Mesh_Face); fv_iter.is_valid();fv_iter++) {
		vh[i] = mesh.vertex_handle(fv_iter->idx());
		cc[i] = mesh.point(vh[i]);
		i++;
	}
	vec[0] = cc[1] - cc[0]; vec[1] = cc[2] - cc[0];
	for (i = 0; i < 3; i++) {
		t += vec[0][i] * vec[1][i];
	}
	if (t < 0) {
		m_Obtuse = vh[0].idx();
		return;
	}

	t = 0;
	vec[0] = cc[0] - cc[1]; vec[1] = cc[2] - cc[1];
	for (i = 0; i < 3; i++) {
		t += vec[0][i] * vec[1][i];
	}
	if (t < 0) {
		m_Obtuse = vh[1].idx();
		return;
	}

	t = 0;
	vec[0] = cc[0] - cc[2]; vec[1] = cc[1] - cc[2];
	for (i = 0; i < 3; i++) {
		t += vec[0][i] * vec[1][i];
	}
	if (t < 0) {
		m_Obtuse = vh[2].idx();
		return;
	}
	m_Obtuse = -1;
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
		/*if (i == 3) {
			cout << "3 break;" << endl;
			break;
		}*/
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

int TriMeshSimplify::TriCollapse(vector<int>&a, vector<int>&b, INT_MAP_TRIMESH&cs)
{
	//UpdateHalfEdge();//每次都需要进行检查更新一下，看看是否出现错误的点；

	if (!DebugInfo2()) {
		DebugInfo(); //1304380
		return m_Mesh_Face.idx();
	}
	
	MyMesh::VertexHandle vp;

	if (is_Three_Tri(m_Mesh_Face, m_Collapse_Half)) {
		a.push_back(mesh.face_handle(m_Collapse_Half).idx());
		a.push_back(mesh.face_handle(mesh.opposite_halfedge_handle(m_Collapse_Half)).idx());

		vp = mesh.to_vertex_handle(m_Collapse_Half);

		mesh.collapse(m_Collapse_Half);

		for (MyMesh::VertexFaceIter vf_iter = mesh.vf_begin(vp); vf_iter.is_valid(); ++vf_iter) {
			b.push_back(vf_iter->idx());
		}
		return -1;
	}

	MyMesh::HalfedgeHandle halfc[3];
	MyMesh::HalfedgeHandle halfn;//另一个需要collapse的地方
	MyMesh::HalfedgeHandle ohf;
	MyMesh::HalfedgeHandle ccf;

	vector<MyMesh::HalfedgeHandle> vmh;

	halfc[0] = mesh.halfedge_handle(mesh.fh_begin(m_Mesh_Face)->idx());
	halfc[1] = mesh.next_halfedge_handle(halfc[0]);
	halfc[2] = mesh.prev_halfedge_handle(halfc[0]);

	int gg;
	for (int i = 0; i < 3; i++) {
		ohf = mesh.opposite_halfedge_handle(halfc[i]);
		ccf = halfc[i];
		gg=cs[mesh.face_handle(ohf).idx()]->JudgeShortEdge(ccf,cs);
		if (gg < 0) {
			vmh.push_back(ccf);
		}
	}
	if (!vmh.size()) {
		m_Collapse_Half = halfc[0];
		halfn = mesh.opposite_halfedge_handle(halfc[1]);

		a.push_back(mesh.face_handle(m_Collapse_Half).idx());
		a.push_back(mesh.face_handle(mesh.opposite_halfedge_handle(m_Collapse_Half)).idx());

		vp = mesh.to_vertex_handle(m_Collapse_Half);

		mesh.collapse(m_Collapse_Half);

		a.push_back(mesh.face_handle(halfn).idx());
		a.push_back(mesh.face_handle(mesh.opposite_halfedge_handle(halfn)).idx());

		mesh.collapse(halfn);

		mesh.set_point(vp, m_v_mesh);

		for (MyMesh::VertexFaceIter vf_iter = mesh.vf_begin(vp); vf_iter.is_valid(); ++vf_iter) {
			b.push_back(vf_iter->idx());
		}
		return -1;
	}
	else if (vmh.size() == 1) {
		m_Collapse_Half = vmh[0];
		//halfn = mesh.opposite_halfedge_handle(mesh.next_halfedge_handle(m_Collapse_Half));
		a.push_back(mesh.face_handle(m_Collapse_Half).idx());
		a.push_back(mesh.face_handle(mesh.opposite_halfedge_handle(m_Collapse_Half)).idx());

		vp = mesh.to_vertex_handle(m_Collapse_Half);

		mesh.collapse(m_Collapse_Half);
		for (MyMesh::VertexFaceIter vf_iter = mesh.vf_begin(vp); vf_iter.is_valid(); ++vf_iter) {
			b.push_back(vf_iter->idx());
		}
		return -1;

	}
	else {
		m_Collapse_Half = vmh[0];
		halfn = vmh[1];

		a.push_back(mesh.face_handle(m_Collapse_Half).idx());
		a.push_back(mesh.face_handle(mesh.opposite_halfedge_handle(m_Collapse_Half)).idx());

		vp = mesh.to_vertex_handle(m_Collapse_Half);
		MyMesh::VertexHandle vp2 = mesh.to_vertex_handle(halfn);

		mesh.collapse(m_Collapse_Half);

		a.push_back(mesh.face_handle(halfn).idx());
		a.push_back(mesh.face_handle(mesh.opposite_halfedge_handle(halfn)).idx());

		mesh.collapse(halfn);

		for (MyMesh::VertexFaceIter vf_iter = mesh.vf_begin(vp); vf_iter.is_valid(); ++vf_iter) {
			b.push_back(vf_iter->idx());
		}
		for (MyMesh::VertexFaceIter vf_iter = mesh.vf_begin(vp2); vf_iter.is_valid(); ++vf_iter) {
			b.push_back(vf_iter->idx());
		}
		return -1;
	}
}

int TriMeshSimplify::TriCollapse2(vector<int>&a, vector<int>&b)
{
	if (!DebugInfo2()) {
		DebugInfo();
		return m_Mesh_Face.idx();
	}
	
	MyMesh::HalfedgeHandle halfn;
	MyMesh::VertexHandle vp;

	//bool ifs=is_Three_Tri(m_Mesh_Face, m_Collapse_Half); //不管是不是，都要验证对面的是否是特殊结构

	if (is_Three_Tri(m_Mesh_Face, m_Collapse_Half)) {

		//halfn = mesh.opposite_halfedge_handle(mesh.next_halfedge_handle(m_Collapse_Half));
		//MyMesh::HalfedgeHandle ohf = mesh.opposite_halfedge_handle(mesh.next_halfedge_handle(m_Collapse_Half));
		//if (is_Three_Tri(mesh.face_handle(ohf), halfn)) {
			a.push_back(mesh.face_handle(m_Collapse_Half).idx());
			a.push_back(mesh.face_handle(mesh.opposite_halfedge_handle(m_Collapse_Half)).idx());

		vp = mesh.to_vertex_handle(m_Collapse_Half);
		// MyMesh::VertexHandle vp2= mesh.to_vertex_handle(halfn);

			mesh.collapse(m_Collapse_Half);

		//	a.push_back(mesh.face_handle(halfn).idx());
		//	a.push_back(mesh.face_handle(mesh.opposite_halfedge_handle(halfn)).idx());

		//	mesh.collapse(halfn);

		//	//mesh.set_point(vp, m_v_mesh);

			for (MyMesh::VertexFaceIter vf_iter = mesh.vf_begin(vp); vf_iter.is_valid(); ++vf_iter) {
				b.push_back(vf_iter->idx());
			}
		//	for (MyMesh::VertexFaceIter vf_iter = mesh.vf_begin(vp2); vf_iter.is_valid(); ++vf_iter) {
		//		b.push_back(vf_iter->idx());
		//	}
			return -1;
		//}
	}
	else {
		MyMesh::HalfedgeHandle ohf;
		for (MyMesh::FaceHalfedgeIter fh_iter = mesh.fh_begin(m_Mesh_Face); fh_iter.is_valid(); fh_iter++) {
			m_Collapse_Half = mesh.halfedge_handle(fh_iter->idx());
			ohf = mesh.opposite_halfedge_handle(m_Collapse_Half);
			//if (UpdateHalfEdge(ohf)) {
			if(is_Three_Tri(mesh.face_handle(ohf), halfn)){
				//return mesh.face_handle(ohf).idx();
				a.push_back(mesh.face_handle(halfn).idx());
				a.push_back(mesh.face_handle(mesh.opposite_halfedge_handle(halfn)).idx());
				vp = mesh.to_vertex_handle(halfn);

				mesh.collapse(halfn);

				for (MyMesh::VertexFaceIter vf_iter = mesh.vf_begin(vp); vf_iter.is_valid(); ++vf_iter) {
					b.push_back(vf_iter->idx());
				}
				return -1;
			}
		}

		halfn = mesh.opposite_halfedge_handle(mesh.next_halfedge_handle(m_Collapse_Half));
	}

	a.push_back(mesh.face_handle(m_Collapse_Half).idx());
	a.push_back(mesh.face_handle(mesh.opposite_halfedge_handle(m_Collapse_Half)).idx());

	vp = mesh.to_vertex_handle(m_Collapse_Half);

	mesh.collapse(m_Collapse_Half);

	a.push_back(mesh.face_handle(halfn).idx());
	a.push_back(mesh.face_handle(mesh.opposite_halfedge_handle(halfn)).idx());

	mesh.collapse(halfn);

	mesh.set_point(vp, m_v_mesh);


	for (MyMesh::VertexFaceIter vf_iter = mesh.vf_begin(vp); vf_iter.is_valid(); ++vf_iter) {
		b.push_back(vf_iter->idx());
	}
	return -1;
}

int TriMeshSimplify::TriCollapse3(vector<int>&a, vector<int>&b)
{
	if (!DebugInfo2()) {
		DebugInfo();
		return m_Mesh_Face.idx();
	}

	MyMesh::HalfedgeHandle halfn;
	MyMesh::VertexHandle vp;

	if (is_Three_Tri(m_Mesh_Face, m_Collapse_Half)) {
		a.push_back(mesh.face_handle(m_Collapse_Half).idx());
		a.push_back(mesh.face_handle(mesh.opposite_halfedge_handle(m_Collapse_Half)).idx());

		vp = mesh.to_vertex_handle(m_Collapse_Half);

		mesh.collapse(m_Collapse_Half);
		mesh.set_point(vp, m_v_mesh);
		for (MyMesh::VertexFaceIter vf_iter = mesh.vf_begin(vp); vf_iter.is_valid(); ++vf_iter) {
			b.push_back(vf_iter->idx());
		}
		return -1;
	}
	else {
		MyMesh::HalfedgeHandle ohf;
		for (MyMesh::FaceHalfedgeIter fh_iter = mesh.fh_begin(m_Mesh_Face); fh_iter.is_valid(); fh_iter++) {
			m_Collapse_Half = mesh.halfedge_handle(fh_iter->idx());
			ohf = mesh.opposite_halfedge_handle(m_Collapse_Half);
			MyMesh::FaceHandle  mfh = mesh.face_handle(ohf);
			if (is_Three_Tri(mfh, halfn)) {
				a.push_back(mesh.face_handle(halfn).idx());
				a.push_back(mesh.face_handle(mesh.opposite_halfedge_handle(halfn)).idx());
				vp = mesh.to_vertex_handle(halfn);

				mesh.collapse(halfn);

				for (MyMesh::VertexFaceIter vf_iter = mesh.vf_begin(vp); vf_iter.is_valid(); ++vf_iter) {
					b.push_back(vf_iter->idx());
				}
				return -1;
			}
			else {
				MyMesh::FaceHandle lfh;
				for (MyMesh::FaceFaceIter ff_iter = mesh.ff_begin(mfh); ff_iter.is_valid();ff_iter++) {
					lfh = mesh.face_handle(ff_iter->idx());
					if (is_Three_Tri(lfh, halfn)) {
						a.push_back(mesh.face_handle(halfn).idx());
						a.push_back(mesh.face_handle(mesh.opposite_halfedge_handle(halfn)).idx());
						vp = mesh.to_vertex_handle(halfn);

						mesh.collapse(halfn);

						for (MyMesh::VertexFaceIter vf_iter = mesh.vf_begin(vp); vf_iter.is_valid(); ++vf_iter) {
							b.push_back(vf_iter->idx());
						}
						return -1;
					}
				}
			}
		}
	}

	halfn = mesh.opposite_halfedge_handle(mesh.next_halfedge_handle(m_Collapse_Half));
	

	a.push_back(mesh.face_handle(m_Collapse_Half).idx());
	a.push_back(mesh.face_handle(mesh.opposite_halfedge_handle(m_Collapse_Half)).idx());

	vp = mesh.to_vertex_handle(m_Collapse_Half);

	mesh.collapse(m_Collapse_Half);

	if (UpdateHalfEdge(halfn)) {
		mesh.set_point(vp, m_v_mesh);
		for (MyMesh::VertexFaceIter vf_iter = mesh.vf_begin(vp); vf_iter.is_valid(); ++vf_iter) {
			b.push_back(vf_iter->idx());
		}
		return -1;
	}
	else {
		MyMesh::FaceHandle lfh;
		MyMesh::HalfedgeHandle hhf;
		for (MyMesh::FaceFaceIter ff_iter = mesh.ff_begin(mesh.face_handle(halfn)); ff_iter.is_valid(); ff_iter++) {
			lfh = mesh.face_handle(ff_iter->idx());
			if (is_Three_Tri(lfh, hhf)) {
				mesh.set_point(vp, m_v_mesh);
				for (MyMesh::VertexFaceIter vf_iter = mesh.vf_begin(vp); vf_iter.is_valid(); ++vf_iter) {
					b.push_back(vf_iter->idx());
				}
				return -1;
			}
		}
	}

	a.push_back(mesh.face_handle(halfn).idx());
	a.push_back(mesh.face_handle(mesh.opposite_halfedge_handle(halfn)).idx());

	mesh.collapse(halfn);

	mesh.set_point(vp, m_v_mesh);


	for (MyMesh::VertexFaceIter vf_iter = mesh.vf_begin(vp); vf_iter.is_valid(); ++vf_iter) {
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
		return i;
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
		cout << "Face Vertex: " << fv_iter->idx()<<" "<<mesh.point(mesh.vertex_handle(fv_iter->idx())) << endl;
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
		/*if (i == 3) {
			cc++;
		}
		else if (i<3) {
			return false;
		}*/
		if (i < 3) {
			return false;
		}
	}
	/*if (cc>1) {
		return false;
	}*/
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
	mesh.request_vertex_normals();

	if (!mesh.has_vertex_normals())
	{
		cout << "Normals  not available!" << endl;
		std::cerr << "ERROR: Standard vertex property 'Normals' not available!\n";
	}

	if (!OpenMesh::IO::read_mesh(mesh, argg, opt))
	{
		std::cerr << "Error: Cannot read mesh from " << std::endl;
		return 1;
	}

	mesh.request_face_normals();

	// let the mesh update the normals
	mesh.update_normals();

	//if (!opt.check(OpenMesh::IO::Options::VertexNormal))
	//{
	//	cout << "Normals  not available````````!" << endl;
	//	// we need face normals to update the vertex normals
	//	mesh.request_face_normals();

	//	// let the mesh update the normals
	//	mesh.update_normals();

	//	// dispose the face normals, as we don't need them anymore
	//	mesh.release_face_normals();
	//}

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

	//MyMesh::Point pp;
	//for (MyMesh::VertexIter vi = mesh.vertices_begin(); vi != mesh.vertices_end(); vi++) {
	//	pp = mesh.point(*vi);
	//	/*if ((pp[0] == NAN)||(pp[1]==NAN)||(pp[2]==NAN)) {
	//	cout << "NAN : " << vi->idx() << endl;
	//	}*/
	//	if (vi->idx() == 817706) {
	//		cout << pp << endl;
	//	}
	//}

	FillTriMeshMap();
	printf("顶点数目统计 : 简化前:%d  目标:%d  差值:%d\n", mesh.n_faces(), mesh.n_faces() - idest, idest);

	while (idest>0) {
		idest -=CollapseIterator();
	}

	//IterartoAllc();
	//IterartoAll();

	mesh.garbage_collection();

	Release();

	//IterartoAllc();

	IterartoAll2();
	//mesh.garbage_collection();
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

		m_TriMesh_Map[i]->SetHalfEdge();

		/*if (i==512373) {
			m_TriMesh_Map[i]->DebugInfo();
		}
		if (i == 506721) {
			m_TriMesh_Map[i]->DebugInfo();
		}*/
	}
	//MyMesh::Point pp;
	//for (MyMesh::VertexIter vi = mesh.vertices_begin(); vi != mesh.vertices_end(); vi++) {
	//	pp = mesh.point(*vi);
	//	/*if ((pp[0] == NAN)||(pp[1]==NAN)||(pp[2]==NAN)) {
	//		cout << "NAN : " << vi->idx() << endl;
	//	}*/
	//	if (vi->idx() == 817706) {
	//		cout << pp << endl;
	//	}
	//}
}

int MyTriOpenMesh::CollapseIterator()
{
	STRUCT_SET_EDGEPAIR::iterator it(m_TriMesh_Set.begin());

	if (it->i == 1640747) {
		m_TriMesh_Map[it->i]->DebugInfo();
		//m_TriMesh_Map[196572]->DebugInfo();
		//m_TriMesh_Map[1772774]->DebugInfo();
	}
	if (it->i == 1640744) {
		m_TriMesh_Map[it->i]->DebugInfo();
		//m_TriMesh_Map[502319]->DebugInfo();
	}
	//if (it->i == 503915) {
	//	m_TriMesh_Map[it->i]->DebugInfo();
	//	//m_TriMesh_Map[209713]->DebugInfo();
	//}
	//309493
	int lin = it->i;

	vector<int> delet, reserve;
	//int judge = m_TriMesh_Map[lin]->TriCollapse(delet,reserve, m_TriMesh_Map);

	int judge = m_TriMesh_Map[lin]->TriCollapse3(delet, reserve);
	if (judge != -1) {
		//m_TriMesh_Set.erase(m_TriMesh_Map[it->i]->ReturnTriSet());//应该有两个方案，不删除，或者删除它；
		//cout << "-1 collapse : " << it->i << " " << judge << endl;
		//if (lin == judge) {
		m_TriMesh_Set.erase(m_TriMesh_Map[lin]->ReturnTriSet());
			//return 0;
		//}
		//delet.clear();
		//reserve.clear();
		
		//judge = m_TriMesh_Map[judge]->TriCollapse(delet, reserve);
		//if (judge != -1) {
		//	//m_TriMesh_Map[judge]->DebugInfo();
		//	//m_TriMesh_Map[it->i]->DebugInfo();
		//	//m_TriMesh_Set.erase(m_TriMesh_Map[it->i]->ReturnTriSet());//应该有两个方案，不删除，或者删除它；
		//	cout << "-2 collapse : " << judge << " saw:" << it->i << endl;
		//	return 0;
		//}
	}


	for (auto i : delet) {
		m_TriMesh_Set.erase(m_TriMesh_Map[i]->ReturnTriSet());
	}
	for (auto i : reserve) {
		m_TriMesh_Set.erase(m_TriMesh_Map[i]->ReturnTriSet());
		m_TriMesh_Map[i]->UpdateTriQ();
		m_TriMesh_Set.insert(m_TriMesh_Map[i]->ReturnTriSet());
		/*if (i == 1640747) {
			cout << "find-"<<i<<": "<< lin << endl;
			m_TriMesh_Map[i]->DebugInfo();
		}*/
		/*if (i == 395635) {
			cout << "find-"<<i<<": " << lin << endl;
			m_TriMesh_Map[i]->DebugInfo();
		}
		if (i == 408613) {
			cout << "find-" << i << ": " << lin << endl;
			m_TriMesh_Map[i]->DebugInfo();
		}
		if (i == 414928) {
			cout << "find2 : " << lin << endl;
			m_TriMesh_Map[i]->DebugInfo();
		}*/
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
void MyTriOpenMesh::IterartoAllc() {
	cout << "total vertex:" << mesh.n_vertices() << " " << mesh.n_faces() << endl;
	MyMesh::VertexHandle vhi;
	MyMesh::Point pp;
	for (MyMesh::VertexIter vit = mesh.vertices_begin(); vit != mesh.vertices_end(); vit++) {
		if (vit->idx() == 1172816) {
			vhi = mesh.vertex_handle(vit->idx());
			pp = mesh.point(vhi);
			cout << vit->idx() << " : " << pp << endl;
		}
		if (vit->idx() == 1172812) {
			vhi = mesh.vertex_handle(vit->idx());
			pp = mesh.point(vhi);
			cout << vit->idx() << " : " << pp << endl;
		}
		if (vit->idx() == 1172814) {
			vhi = mesh.vertex_handle(vit->idx());
			pp = mesh.point(vhi);
			cout << vit->idx() << " : " << pp << endl;
		}
	}

	
	MyMesh::Point cc[3];
	float ss[3];
	float st = 0;
	int ii = 0;
	int n[3] = { 0,0,0 };

	MyMesh::FaceHandle fhc;

	for (MyMesh::FaceIter it = mesh.faces_begin(); it != mesh.faces_end(); it++) {
		ii = 0;
		fhc = mesh.face_handle(it->idx());
		for (MyMesh::FaceVertexIter fv_iter = mesh.fv_begin(fhc); fv_iter.is_valid(); fv_iter++) {
			cc[ii] = mesh.point(mesh.vertex_handle(fv_iter->idx()));
			ii++;
		}
		ss[0] = (cc[0] - cc[1]).norm();
		ss[1] = (cc[1] - cc[2]).norm();
		ss[2] = (cc[0] - cc[2]).norm();
		//st = (ss[0] + ss[1] + ss[2]) ;
		if ((ss[0] < 1e-6) || (ss[1] < 1e-6) || (ss[2] < 1e-6)) {
			cout << "平面边长为0：" << it->idx() << endl;
			cout << ss[0] << " " << ss[1] << " " << ss[2] << " halfedge:" << endl;
			for (MyMesh::FaceHalfedgeIter fh_iter = mesh.fh_begin(fhc); fh_iter.is_valid(); fh_iter++) {
				cout << fh_iter->idx() << " ";
			}
			cout << endl;
			//MyMesh::HalfedgeHandle ccf= mesh.halfedge_handle(fh_iter->idx());
			//mesh.collapse(ccf);
			//mesh.delete_face(fhc);
			n[1]++;
		}
		else {
			n[2]++;
		}
		if (it->idx() == 390122) {
			cout << "find " << it->idx() << endl;
			cout << ss[0] << " " << ss[1] << " " << ss[2] << " halfedge:" << endl;
			for (MyMesh::FaceHalfedgeIter fh_iter = mesh.fh_begin(fhc); fh_iter.is_valid(); fh_iter++) {
				cout << fh_iter->idx() << " ";
			}
			cout << endl;
		}
	}
	cout << n[1] << " " << n[2] << endl;
}

void MyTriOpenMesh::IterartoAll() {
	int i = 0;
	int cc[3] = {0,0,0};
	for (MyMesh::VertexIter v_iter = mesh.vertices_begin(); v_iter != mesh.vertices_end(); v_iter++) {
		i = 0;
		for (MyMesh::VertexFaceIter vf_iter = mesh.vf_begin(*v_iter); vf_iter.is_valid(); vf_iter++) {
			i++;
		}
		if (i ==1) {
			cout <<"1:"<< v_iter->idx() <<" "<<i<< endl;
			cout << mesh.point(mesh.vertex_handle(v_iter->idx())) << endl;
			mesh.delete_face(mesh.face_handle(v_iter->idx()));
			cc[0]++;
		}
		if (i == 2) {
			cout << v_iter->idx() << " " << i << endl;
			cout << mesh.point(mesh.vertex_handle(v_iter->idx())) << endl;
			
			cout << "face : ";
			for (MyMesh::VertexFaceIter vf_iter = mesh.vf_begin(*v_iter); vf_iter.is_valid(); vf_iter++) {
				cout << vf_iter->idx()<< " : " << endl;;
				for (MyMesh::FaceVertexIter fv_iter = mesh.fv_begin(*vf_iter); fv_iter.is_valid(); fv_iter++) {
					cout <<"vertex "<<fv_iter->idx()<<" : "<< mesh.point(mesh.vertex_handle(fv_iter->idx())) << endl;
					for (MyMesh::VertexFaceIter vf_iter2 = mesh.vf_begin(*fv_iter); vf_iter2.is_valid(); vf_iter2++) {
						cout << vf_iter2->idx()<<" ";
					}
					cout << endl;
				}
			}
			/*MyMesh::HalfedgeHandle hf = mesh.halfedge_handle(mesh.voh_begin(mesh.vertex_handle(v_iter->idx()))->idx());
			mesh.collapse(hf);*/
			cc[1]++;
		}
	}
	cout << endl;

	cout << "cc : " << cc[0]<<" "<<cc[1] << endl;
}

void MyTriOpenMesh::IterartoAll2() {
	int i = 0;
	for (MyMesh::VertexIter v_iter = mesh.vertices_begin(); v_iter != mesh.vertices_end(); v_iter++) {
		i = 0;
		for (MyMesh::VertexFaceIter vf_iter = mesh.vf_begin(*v_iter); vf_iter.is_valid(); vf_iter++) {
			i++;
		}
		if (i == 1) {
			cout << v_iter->idx() << " " << i << endl;
			mesh.delete_face(mesh.face_handle(v_iter->idx()));
			//cout << mesh.point(mesh.vertex_handle(v_iter->idx())) << endl;
		}
		if (i == 2) {
			cout << v_iter->idx() << " " << i << endl;
			mesh.delete_face(mesh.face_handle(v_iter->idx()));
			//cout << mesh.point(mesh.vertex_handle(v_iter->idx())) << endl;
		}
	}
	cout << endl;
}