#include "stdafx.h"
#include "EdgePair.h"


EdgePair::EdgePair(MyMesh&cc) :
	cmesh(cc)
{
	/*m_Q_a << 0, 0, 0, 0,
			 0, 0, 0, 0,
			 0, 0, 0, 0,
			 0, 0, 0, 0;

	m_Q_b << 0, 0, 0, 0,
			 0, 0, 0, 0,
			 0, 0, 0, 0,
			 0, 0, 0, 0;*/
}

EdgePair::EdgePair(MyMesh::EdgeHandle e,MyMesh&c) :
	m_edge_h(e),
	cmesh(c)
{
}

EdgePair::~EdgePair()
{
}

//int EdgePair::AddVertexSurfaceCoe() {
//	MyMesh::Point pp = cmesh.point(m_a);
//	int ii[2] = {0,0};
//	Matrix4d Q_a, Q_b;
//	Q_a << 0, 0, 0, 0,
//		0, 0, 0, 0,
//		0, 0, 0, 0,
//		0, 0, 0, 0;
//
//	Q_b << 0, 0, 0, 0,
//		0, 0, 0, 0,
//		0, 0, 0, 0,
//		0, 0, 0, 0;
//	for (MyMesh::VertexFaceIter vf_it = cmesh.vf_begin(m_a); vf_it.is_valid(); ++vf_it) {
//		MyMesh::Normal nf= cmesh.normal(*vf_it);
//		nf.normalize();
//		double temp = 0 - (nf[0]* pp[0]+ nf[1] * pp[1]+ nf[2] * pp[2]);
//
//		Vector4d vfd = Vector4d(nf[0], nf[1], nf[2], temp);
//		Matrix4d tempm = vfd*vfd.transpose();
//		Q_a += tempm;
//
//		//m_equ_surf_a.push_back(Vector4d(nf[0],nf[1],nf[2],temp));
//		ii[0]++;
//	}
//
//	pp = cmesh.point(m_b);
//	for (MyMesh::VertexFaceIter vf_it = cmesh.vf_begin(m_b); vf_it.is_valid(); ++vf_it) {
//		MyMesh::Normal nf = cmesh.normal(*vf_it);
//		nf.normalize();
//		double temp = 0 - (nf[0] * pp[0] + nf[1] * pp[1] + nf[2] * pp[2]);
//
//		Vector4d vfd = Vector4d(nf[0], nf[1], nf[2], temp);
//		Matrix4d tempm = vfd*vfd.transpose();
//		Q_b += tempm;
//
//		//m_equ_surf_a.push_back(Vector4d(nf[0], nf[1], nf[2], temp));
//		ii[1]++;
//	}
//
//	if (!(ii[0] && ii[1])) 
//	{
//		cout << "Vertex Surface Either a zero!" << endl;
//		return 1;
//	}
//
//	m_Q = Q_a + Q_b;
//
//	if (ii[0] != ii[1]) 
//	{
//		printf("The number around vertex not equal a:%d b:%d\n",ii[0],ii[1]);
//		return 2;
//	}
//
//	return 0;
//}

int EdgePair::UpdatePair(INT_MAP_VERTEXP &tmap)
{
	MyMesh::HalfedgeHandle h = cmesh.halfedge_handle(m_edge_h, 0);
	/*MyMesh::VertexHandle a[2];
	a[0] = cmesh.to_vertex_handle(h);
	a[1] = cmesh.from_vertex_handle(h);*/
	m_halfedge_h = h;//决定点向哪个方向倒

	m_a = cmesh.to_vertex_handle(h);
	m_b = cmesh.from_vertex_handle(h);

	UpdateQEquation(tmap[m_a.idx()]->ReturnMatrix() + tmap[m_b.idx()]->ReturnMatrix());
	ComputeMaxPoint();//核心最优解以及x生成算法函数；

	m_QX_Idx.a = m_x; //用来索引排序查找；
	m_QX_Idx.i = m_edge_h.idx();
	return 0;
}
//
//int EdgePair::ComputeQArroundSurface(MyMesh::VertexHandle m)//
//{
//	m_Q << 0, 0, 0, 0,
//		0, 0, 0, 0,
//		0, 0, 0, 0,
//		0, 0, 0, 0;
//	int ini=0;
//	for (MyMesh::VertexFaceIter  vf_iter = cmesh.vf_begin(m); vf_iter.is_valid(); vf_iter++) {
//		MyMesh::FaceHandle arf = cmesh.face_handle(vf_iter->idx());
//		MyMesh::Point arv[3];
//		int i = 0;
//		for (MyMesh::FaceVertexIter fv_iter = cmesh.fv_begin(arf); fv_iter.is_valid(); ++fv_iter)
//		{
//			arv[i] = cmesh.point(*fv_iter);
//			i++;
//		}
//		if (i != 3) {
//			cout << "Arround Num is ERROR!" << endl;
//			continue;
//		}
//		ini++;
//		ComputeSurfaceQ(arv);
//	}
//	return ini;
//}
//
//void EdgePair::ComputeSurfaceQ(MyMesh::Point *vp) //三个面的顶点坐标计算Q值
//{
//	Vector3d a((*vp)[0], (*vp)[1], (*vp)[2]);
//	Vector3d b((*(vp+1))[0], (*(vp+1))[1], (*(vp+1))[2]);
//	Vector3d c((*(vp + 2))[0], (*(vp + 2))[1], (*(vp + 2))[2]);
//
//	Vector3d ab = a - b;
//	ab = (a - c).cross(ab);
//	ab.normalize();
//	double df = ab.dot(Vector3d(0, 0, 0) - a);
//
//	Vector4d coe(ab[0],ab[1],ab[2],df); //surface equation cofficient!
//
//	m_Q += coe*coe.transpose(); //update Q
//}
//
//

void EdgePair::UpdateQEquation(Matrix4d f)
{
	m_Q = f;
	m_Q_Equation = m_Q;
	m_Q_Equation(3, 0) = 0;
	m_Q_Equation(3, 1) = 0;
	m_Q_Equation(3, 2) = 0;
	m_Q_Equation(3, 3) = 1;
}

void EdgePair::ComputeMaxPoint()//(MyMesh::VertexHandle tmpa, MyMesh::VertexHandle tmpb)
{
	Vector4d ori(0,0,0,1);
	//Vector4d m_v=m_Q_Equation.fullPivHouseholderQr().solve(ori); //改成局部变量；
	Vector4d m_v = m_Q_Equation.colPivHouseholderQr().solve(ori); //使用col会不会提速一点（这两个没啥区别，速度都差不多！）
	//check;
	double temp = (m_Q_Equation*m_v - ori).norm();
	if ((!temp) || (temp < SOLUTIONACCURANCY)) {
		m_v_mesh = MyMesh::Point(m_v[0], m_v[1], m_v[2]); //无论如何访问这个最优点
		m_x = m_v.transpose()*m_Q*m_v;
		return;
	}

	MyMesh::Point pa, pb, pc;
	pa = cmesh.point(m_a);
	pb = cmesh.point(m_b);
	pc = (pa + pb) / 2;

	Vector4d vpa(pa[0], pa[1], pa[2],1);
	Vector4d vpb(pb[0], pb[1], pb[2],1);
	Vector4d vpc(pc[0], pc[1], pc[2],1);

	double dpa, dpb, dpc, comp;
	dpa = vpa.transpose()*m_Q*vpa;
	dpb = vpb.transpose()*m_Q*vpb;
	dpc = vpc.transpose()*m_Q*vpc;

	comp = dpa;
	m_v_mesh = pa;
	if (dpb > comp) {
		comp = dpb;
		m_v_mesh = pb;
	}
	if (dpc > comp) {
		comp = dpc;
		m_v_mesh = pc;
	}
	m_x = comp;
}

int EdgePair::PairCollapse() 
{
	cmesh.collapse(m_halfedge_h);

	//if(cmesh.is_collapse_ok(m_halfedge_h)) { //是不是可以不用判断
	//	return 1;//collapse error!
	//}

	cmesh.set_point(m_a, m_v_mesh);
	return 0;  //对的情况只有一个，错的情况有很多1，2，3，4.。。。
}

int EdgePair::PairCollapseInverse() {
	MyMesh::HalfedgeHandle hf = cmesh.opposite_halfedge_handle(m_halfedge_h);
	cmesh.collapse(hf);
	if (cmesh.is_collapse_ok(hf)) {
		return 1;//collapse error!
	}
	cmesh.set_point(m_b, m_v_mesh);
	return 0;  //对的情况只有一个，错的情况有很多1，2，3，4.。。。
}

int EdgePair::PairCollapse2()
{
	cout << "wgewg" << endl;
	cmesh.collapse(m_halfedge_h);
	cout << "after" << endl;
	if (cmesh.is_collapse_ok(m_halfedge_h)) {
		return 1;//collapse error!
	}
	cmesh.set_point(m_a, m_v_mesh);
	return 0;  //对的情况只有一个，错的情况有很多1，2，3，4.。。。
}


int VertexPoint::QMSurfaceCoe() 
{
	MyMesh::Point pp = cmesh.point(m_vh);
	int ii = 0;
	for (MyMesh::VertexFaceIter vf_it = cmesh.vf_begin(m_vh); vf_it.is_valid(); ++vf_it) {
		MyMesh::Normal nf = cmesh.normal(*vf_it);
		nf.normalize();
		double temp = 0 - (nf[0] * pp[0] + nf[1] * pp[1] + nf[2] * pp[2]);

		Vector4d vfd(nf[0], nf[1], nf[2], temp);
		Matrix4d tempm = vfd*vfd.transpose();
		m_Q += tempm;
		ii++;
	}
	if (!ii) {
		cout << "VertexPoint "<<m_vh.idx()<< ": No Face!" << endl;
		
		/*vertex索引号：97 101 7880 对于该种没有边的孤立点的情况，测试显示，该点周围并没有连接任何边，半边，顶点等结构！*/
		return 1;
	}
	return 0;
}

int VertexPoint::UpdateVertexQ()
{
	m_Q << 0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0;
	//int ini = 0;
	for (MyMesh::VertexFaceIter vf_iter = cmesh.vf_begin(m_vh); vf_iter.is_valid(); vf_iter++) {
		MyMesh::FaceHandle arf = cmesh.face_handle(vf_iter->idx());
		MyMesh::Point arv[3];
		int i = 0;
		for (MyMesh::FaceVertexIter fv_iter = cmesh.fv_begin(arf); fv_iter.is_valid(); ++fv_iter)
		{
			arv[i] = cmesh.point(*fv_iter);
			i++;
		}
		if (i != 3) {
			cout << "Arround Num is ERROR!" << endl;
			//continue;
		}
		//ini++;
		ComputeSurfaceQ(arv);
	}
	return 0; //ini
}

void VertexPoint::ComputeSurfaceQ(MyMesh::Point *vp) //三个面的顶点坐标计算Q值
{
	Vector3d a((*vp)[0], (*vp)[1], (*vp)[2]);
	Vector3d b((*(vp + 1))[0], (*(vp + 1))[1], (*(vp + 1))[2]);
	Vector3d c((*(vp + 2))[0], (*(vp + 2))[1], (*(vp + 2))[2]);

	Vector3d ab = a - b;
	ab = (a - c).cross(ab);
	ab.normalize();
	double df = ab.dot(Vector3d(0, 0, 0) - a);

	Vector4d coe(ab[0], ab[1], ab[2], df); //surface equation cofficient!

	m_Q += coe*coe.transpose(); //update Q
}
