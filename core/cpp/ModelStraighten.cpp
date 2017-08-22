#include "stdafx.h"
#include "ModelStraighten.h"

ModelStraighten::ModelStraighten(MyMesh&a) :
	mesh(a)
{
}

ModelStraighten::ModelStraighten(MyMesh&m,bool flann):
	mesh(m)
{
	if (!mesh.n_vertices())
	{
		cout << "Mesh no vertex flann no init!" << endl;
		return;
	}

	int row = mesh.n_vertices();
	int nn = 3;
	m_dataset=new flann::Matrix<float>(new float[row*nn], row, nn);
	cout <<"Model Straighten init matrix: "<<m_dataset->rows << " " << m_dataset->cols << " " << endl;

	int ii = 0;
	MyMesh::Point p;
	for (MyMesh::VertexIter vit = mesh.vertices_begin(); vit != mesh.vertices_end(); vit++)
	{
		p = mesh.point(*vit);
		(*m_dataset)[ii][0] = p[0];
		(*m_dataset)[ii][1] = p[1];
		(*m_dataset)[ii][2] = p[2];
		ii++;
	}

	m_flann_index = new flann::Index< L2<float> >(*m_dataset, flann::KDTreeSingleIndexParams(10));//new 创建可以全局使用
	m_flann_index->buildIndex();
}


ModelStraighten::~ModelStraighten()
{
	delete m_flann_index;
	delete m_dataset;
	m_flann_index = nullptr;
	m_dataset = nullptr;
}

void ModelStraighten::MainTest()
{

	//StraightenTest();
	//return;

	//string a = "befor.ply";
	//ZCutShoe();

	FindThreePoint();
	//OutTriMesh(a);

	//Straighten2();

	Straighten();
	MeshOffsetMove();
	
	//a = "after.ply";
	//utTriMesh(a);
	//Straighten();
}

void ModelStraighten::MeshSmoothing()
{
	// Initialize smoother with input mesh
	OpenMesh::Smoother::JacobiLaplaceSmootherT<MyMesh> smoother(mesh);

	smoother.initialize(smoother.Tangential_and_Normal,smoother.C0);       //Smooth direction               //Continuity

	smoother.smooth(3); // Execute 3 smooth steps
	cout << "Mesh Smoother!" << endl;
}

void ModelStraighten::EigenQuaternSpinTest()
{

	Vector3f vv(-1,-1,-1);
	Quaternionx q1 = Quaternionx::FromTwoVectors(vv,Vector3f(0,1,0));
	Quaternionx q2 = Quaternionx::FromTwoVectors(Vector3f(0, 1, 0), Vector3f(1, 0, 0));
	Quaternionx spin = q2 *q1;
	Quaternionx ispin = spin.inverse();
	Quaternionx ss(0, vv[0], vv[1], vv[2]);
	Quaternionx out;
	out = spin*ss*ispin;
	MyMesh::Point p = MyMesh::Point(out.x(), out.y(), out.z());
	cout << p << endl;

	Vector3f a1(120.908577, 78.5950012, -57.8131676);
	Vector3f a2(64.7377777, 78.5950012, - 57.8131371);
	Vector3f a3(10.7367659, 78.5950012, - 57.8216248);

	Vector3f af1(120.908791, 78.5956039, -57.8139877);
	Vector3f af2(64.7390900, 78.5957184, -57.8184547);
	Vector3f af3(10.7362871, 78.5959778, -57.8212242);

	Vector3f f = (a1 - a2).cross(a3 - a2);
	Vector3f f2 = (af1 - af2).cross(af3 - af2);
	f.normalize();
	f2.normalize();
	cout << "ff: " << f << endl;
	cout << "ff2: " << f2 << endl;

}

bool ModelStraighten::FindThreePoint()
{
	if (!mesh.n_vertices())
	{
		cout << "Mesh no vertex!" << endl;
		return 0;
	}
	MyMesh::Point zero_point = MyMesh::Point(0, 0, 0);
	MyMesh::Point temp_point;
	
	float areatotal = 0;
	float area_temp = 0;
	for (MyMesh::FaceIter fit = mesh.faces_begin(); fit != mesh.faces_end(); fit++) 
	{
		temp_point = TriSqureCompute(mesh.face_handle(fit->idx()), area_temp);
		zero_point += area_temp*temp_point;
		areatotal += area_temp;
	}
	m_centreGarvity = zero_point / areatotal;
	
	float d_nearest = 9999;
	float d_farthest = 0;
	float f_temp;
	MyMesh::Point nearest, farthest;

	for (MyMesh::VertexIter vit = mesh.vertices_begin(); vit != mesh.vertices_end(); vit++)
	{
		zero_point = mesh.point(*vit);
		f_temp = (m_centreGarvity - zero_point).norm();
		/*if (d_nearest > f_temp) {
			nearest = zero_point;
			d_nearest = f_temp;
		}*/
		if (d_farthest < f_temp) {
			farthest = zero_point;
			d_farthest = f_temp;
		}
	}

	m_headPoint = farthest;//这个肯定是鞋楦头部位置；
	//m_endPoint = nearest;

	CrossSectionModel();

	/*Vector3f surface_cross_tri;
	Vector3f headvect;

	ProduceTwoSpinAxi(surface_cross_tri, headvect);*/

	//InitTwoIterGarvityPoint(m_centreGarvity_ahead,4); //bestest 4;
	
	return 1;
}

Vector3f ModelStraighten::InitTwoIterGarvityPoint( int iter_n)
{
	MyMesh::Point origin_ahead, toward_ahead, midtransfer_ahead;
	origin_ahead = m_centreGarvity;
	toward_ahead = m_centreGarvity_ahead;

	MyMesh::Point origin_back, toward_back, midtransfer_back;
	origin_back = m_centreGarvity;
	toward_back = m_centreGarvity_back;

	Vector3f ss(m_centreGarvity[0], m_centreGarvity[1], m_centreGarvity[2]);

	for (int i = 0; i < iter_n; i++)
	{
		cout << "origin ahead: " << origin_ahead << endl;
		cout << "toward ahead: " << toward_ahead << endl;
		
		midtransfer_ahead = toward_ahead;
		TwoIterGarvityPoint(origin_ahead, toward_ahead);
		origin_ahead = midtransfer_ahead;

		cout << endl;

		cout << "origin back: " << origin_back << endl;
		cout << "toward back: " << toward_back << endl;

		midtransfer_back = toward_back;
		TwoIterGarvityPoint(origin_back, toward_back);
		origin_back = midtransfer_back;
		cout << "------------------------" << endl;

		Vector3f ma(toward_ahead[0], toward_ahead[1], toward_ahead[2]);
		Vector3f mb(toward_back[0], toward_back[1], toward_back[2]);
		Vector3f re=(mb - ss).cross(ma - ss);
		re.normalize();
		cout << "Vect normal ：" << re[0] << " " << re[1] << " " << re[2] << endl;
		cout << endl;
		if (i == (iter_n - 1))
		{
			return re;
		}
	}
	return Vector3f(0, 0, 0);
}

MyMesh::Point ModelStraighten::TriSqureCompute(MyMesh::FaceHandle fh, float &s)
{
	MyMesh::Point p[3];
	int i = 0;
	for (MyMesh::FaceVertexIter fv = mesh.fv_begin(fh); fv.is_valid(); fv++) 
	{
		p[i] = mesh.point(*fv);
		i++;
	}

	float line[3];
	line[0] = (p[0] - p[1]).norm();
	line[1] = (p[1] - p[2]).norm();
	line[2] = (p[0] - p[2]).norm();
	float cir = (line[0] + line[1] + line[2]) / 2;

	s = sqrt(cir*(cir-line[0])*(cir-line[1])*(cir-line[2]));//三角形面积；

	MyMesh::Point m = (p[0] + p[1] + p[2]) / 3;
	return m;
}

void ModelStraighten::CrossSectionModel()
{
	/*
		算出截面方程
	*/
	MyMesh::Normal aximesh = m_headPoint - m_centreGarvity;
	aximesh.normalize();
	//MyMesh::Point thirdpoint = m_headPoint - aximesh * 10; //offset 10mm

	float d = aximesh | m_centreGarvity;

	m_coe.clear();
	m_coe.push_back(aximesh[0]); 
	m_coe.push_back(aximesh[1]);
	m_coe.push_back(aximesh[2]);
	m_coe.push_back(0-d);//4

	/*
		后半段的重心；
	*/
	float headpoint= DistSurface(m_headPoint);
	
	MyMesh::Point zero_point = MyMesh::Point(0, 0, 0);
	MyMesh::Point zero_point_ahead = MyMesh::Point(0, 0, 0);
	float areatotal = 0;
	float areatotal_ahead = 0;

	MyMesh::Point temp_point;
	float area_temp = 0;
	float surface_judge = 0;
	for (MyMesh::FaceIter fit = mesh.faces_begin(); fit != mesh.faces_end(); fit++)
	{
		temp_point = TriSqureCompute(mesh.face_handle(fit->idx()), area_temp);
		surface_judge=DistSurface(temp_point);
		if ((surface_judge*headpoint) > 0)
		{
			areatotal_ahead += area_temp;
			zero_point_ahead += area_temp*temp_point;
			continue;
		}
		zero_point += area_temp*temp_point;
		areatotal += area_temp;
	}
	m_centreGarvity_back = zero_point / areatotal;
	m_centreGarvity_ahead = zero_point_ahead/ areatotal_ahead;

	cout << "m_centreGarvity_ahead 1 :"<<m_centreGarvity_ahead << endl;

	/*
		再取前半段的重心
	*/
	/*AheadPartGrGarvity();
	cout << "m_centreGarvity_ahead 2 :" << m_centreGarvity_ahead << endl;
	AheadPartGrGarvity();
	cout << "m_centreGarvity_ahead 3 :" << m_centreGarvity_ahead << endl;*/

	cout << "m_centreGarvity: " << m_centreGarvity << endl;
	cout << "m_centreGarvity_back: " << m_centreGarvity_back << endl;
	cout << m_headPoint << endl;

	cout << endl;

	return ;
}

void ModelStraighten::TwoIterGarvityPoint(MyMesh::Point origin, MyMesh::Point& toward)
{
	m_coe.clear();


	MyMesh::Normal sec_nomrmal = (toward - origin);
	sec_nomrmal.normalize();

	cout << "sec_nomramal: " << sec_nomrmal << endl;

	float d = sec_nomrmal | toward;
	m_coe.push_back(sec_nomrmal[0]);
	m_coe.push_back(sec_nomrmal[1]);
	m_coe.push_back(sec_nomrmal[2]);
	m_coe.push_back(0 - d);

	MyMesh::Point testPoint = toward + sec_nomrmal * 10;

	float ftestpoint = DistSurface(testPoint);
	MyMesh::Point temp_point = MyMesh::Point(0, 0, 0);
	MyMesh::Point zero_point_ahead = MyMesh::Point(0, 0, 0);
	float areatotal_ahead = 0;
	float area_temp, surface_judge;

	zero_point_ahead = MyMesh::Point(0, 0, 0);
	areatotal_ahead = 0;
	for (MyMesh::FaceIter fit = mesh.faces_begin(); fit != mesh.faces_end(); fit++)
	{
		temp_point = TriSqureCompute(mesh.face_handle(fit->idx()), area_temp);
		surface_judge = DistSurface(temp_point);
		if ((surface_judge*ftestpoint) < 0)
		{
			continue;
		}
		zero_point_ahead += area_temp*temp_point;
		areatotal_ahead += area_temp;
	}
	toward = (zero_point_ahead / areatotal_ahead);
	return;
}

bool ModelStraighten::ZCutShoe()
{
	float squre_up = 0;
	float squre_down = 0;
	MyMesh::Point z_min = mesh.point(FindXYZSmallestPoint(2));
	MyMesh::Point z_max = mesh.point(FindXYZBigestPoint(2));
	float z = (z_min[2] + z_max[2]) / 2;

	MyMesh::Point temp_point;
	float area_temp = 0;
	for (MyMesh::FaceIter fit = mesh.faces_begin(); fit != mesh.faces_end(); fit++)
	{
		temp_point = TriSqureCompute(mesh.face_handle(fit->idx()), area_temp);
		if (temp_point[2] > z) 
		{
			squre_up += area_temp;
			continue;
		}
		squre_down += area_temp;
	}

	cout << "squre diide: " << squre_down << "   " << squre_up << endl;
	
	if (squre_down<squre_up)
	{
		if ((squre_down / squre_up) < (2 / 3))
		{

		}
		return 1;
	}
	else {
		if ((squre_up / squre_down) < (2 / 3))
		{

		}
	}
	return 0;
}

void ModelStraighten::StraightenTest()
{
	Vector3f tri_a(228.0642, 26.6170, -19.4296);
	Vector3f tri_b(163.0059, -17.8065, 18.2487);
	Vector3f tri_c(90.3861, -80.9891, 68.2130);

	Vector3f yaxi, xaxi;
	xaxi = tri_a - tri_b;
	yaxi = (tri_c - tri_b).cross(xaxi);

	//TwoVectorTransfer(xaxi,yaxi);
	SingleVectorTransfer(xaxi, Vector3f(1, 0, 0));
	SingleVectorTransfer(yaxi, Vector3f(0, 1, 0));
}

void ModelStraighten::Straighten()
{
	Vector3f surface_cross_tri;
	Vector3f headvect;

	ProduceTwoSpinAxi(surface_cross_tri,headvect);

	//SingleVectorTransfer(headvect, Vector3f(1, 0, 0));
	SingleVectorTransfer(headvect, Vector3f(1, 0, 0));
	FindThreePoint();

	Vector3f surface_cross_tri2;
	surface_cross_tri2 = InitTwoIterGarvityPoint(4);

	//ProduceTwoSpinAxi(surface_cross_tri, headvect);

	SingleVectorTransfer(surface_cross_tri2, Vector3f(0, 1, 0));
	FindThreePoint();

	ProduceTwoSpinAxi(surface_cross_tri, headvect);
	if (ZCutShoe()) {
		cout << endl;
		cout << "The model is headstand" << endl;
		
		SingleVectorTransfer(surface_cross_tri, Vector3f(0, -1, 0));
	}
	FindThreePoint();
}


void ModelStraighten::Straighten2()
{
	Vector3f surface_cross_tri(0,0,0);
	Vector3f headvect(0,0,0);
	ProduceTwoSpinAxi(surface_cross_tri, headvect);

	Quaternionx q1;
	q1 = Quaternionx::FromTwoVectors(headvect, Vector3f(1, 0, 0));

	/*headvect.normalize();
	cout << headvect << endl;*/

	Quaternionx s1(0, m_centreGarvity[0], m_centreGarvity[1], m_centreGarvity[2]);
	Quaternionx s2(0, m_centreGarvity_ahead[0], m_centreGarvity_ahead[1], m_centreGarvity_ahead[2]);
	Quaternionx s3(0, m_centreGarvity_back[0], m_centreGarvity_back[1], m_centreGarvity_back[2]);

	Quaternionx out;
	out = q1*s1*q1.inverse();

	cout << s1.x() << endl;

	m_centreGarvity.data()[0] = out.x();
	m_centreGarvity.data()[1] = out.y();
	m_centreGarvity.data()[2] = out.z();

	out = q1*s2*q1.inverse();

	m_centreGarvity_ahead.data()[0] = out.x();
	m_centreGarvity_ahead.data()[1] = out.y();
	m_centreGarvity_ahead.data()[2] = out.z();

	out = q1*s3*q1.inverse();

	m_centreGarvity_back.data()[0] = out.x();
	m_centreGarvity_back.data()[1] = out.y();
	m_centreGarvity_back.data()[2] = out.z();

	ProduceTwoSpinAxi(surface_cross_tri, headvect);

	TwoVectorTransfer(q1);

	cout << "--------------------------------------" << endl;
	Quaternionx q2;
	q2 = Quaternionx::FromTwoVectors(surface_cross_tri, Vector3f(0, 1, 0));

	//cout << "q1: " << q1.x() << " " << q1.y() << " " << q1.z() << endl;

	Quaternionx as1(0, m_centreGarvity[0], m_centreGarvity[1], m_centreGarvity[2]);
	Quaternionx as2(0, m_centreGarvity_ahead[0], m_centreGarvity_ahead[1], m_centreGarvity_ahead[2]);
	Quaternionx as3(0, m_centreGarvity_back[0], m_centreGarvity_back[1], m_centreGarvity_back[2]);

	out = q2*as1*q2.inverse();

	m_centreGarvity.data()[0] = out.x();
	m_centreGarvity.data()[1] = out.y();
	m_centreGarvity.data()[2] = out.z();

	out = q2*as2*q2.inverse();

	m_centreGarvity_ahead.data()[0] = out.x();
	m_centreGarvity_ahead.data()[1] = out.y();
	m_centreGarvity_ahead.data()[2] = out.z();

	out = q2*as3*q2.inverse();

	m_centreGarvity_back.data()[0] = out.x();
	m_centreGarvity_back.data()[1] = out.y();
	m_centreGarvity_back.data()[2] = out.z();

	//TwoVectorTransfer(q2);

	ProduceTwoSpinAxi(surface_cross_tri, headvect);

	Quaternionx spin;
	spin = q2*q1;

	out = spin*s1*spin.inverse();
	cout << "out s1: " << out.x() << " " << out.y() << " " << out.z() << endl;
	out = spin*s2*spin.inverse();
	cout << "out s2: " << out.x() << " " << out.y() << " " << out.z() << endl;
	out = spin*s3*spin.inverse();
	cout << "out s2: " << out.x() << " " << out.y() << " " << out.z() << endl;

	TwoVectorTransfer(spin);

	/*Quaternionx xinverse = spin.inverse();
	MyMesh::Point p;

	for (MyMesh::VertexIter it = mesh.vertices_begin(); it != mesh.vertices_end(); it++)
	{
		p = mesh.point(*it);
		Quaternionx ss(0, p[0], p[1], p[2]);
		out = spin*ss*xinverse;
		p = MyMesh::Point(out.x(), out.y(), out.z());
		mesh.set_point(*it, p);
	}*/
	ProduceTwoSpinAxi(surface_cross_tri, headvect);

	MeshQuaternionSpin(spin);

	FindThreePoint();

	ProduceTwoSpinAxi(surface_cross_tri, headvect);
}

void ModelStraighten::ProduceTwoSpinAxi(Vector3f &s1,Vector3f &s2)
{
	MyMesh::Normal tri_a = m_centreGarvity_ahead - m_centreGarvity;

	//x zhou (1,0,0)
	MyMesh::Normal tri_b = m_centreGarvity_back - m_centreGarvity;

	Vector3f tri_a_vec(tri_a[0], tri_a[1], tri_a[2]);
	Vector3f tri_b_vec(tri_b[0], tri_b[1], tri_b[2]);
	Vector3f surface_cross_tri = tri_a_vec.cross(tri_b_vec);//y zhou (0,1,0)

	Vector3f headvect(tri_a[0], tri_a[1], tri_a[2]);

	s1.data()[0] = surface_cross_tri[0];
	s1.data()[1] = surface_cross_tri[1];
	s1.data()[2] = surface_cross_tri[2];

	s2.data()[0] = headvect[0];
	s2.data()[1] = headvect[1];
	s2.data()[2] = headvect[2];

	surface_cross_tri.normalize();
	headvect.normalize();

	cout <<"surface_cross_tri: "<<surface_cross_tri << endl;
	cout <<"headvect: "<<headvect << endl;
	cout << endl;
}

void ModelStraighten::TwoVectorTransfer(Quaternionx &x)
{	
	//SinglePointSpin(m_centreGarvity_ahead, x);
	//SinglePointSpin(m_centreGarvity, x);
	//SinglePointSpin(m_centreGarvity_back, x);

	cout << endl;
	cout << "m_centreGarvity_ahead-Two:" << m_centreGarvity_ahead << endl;
	cout << "m_centreGarvity-Two: " << m_centreGarvity << endl;
	cout << "m_centreGarvity_back-Two: " << m_centreGarvity_back << endl;
	cout << endl;
}

void ModelStraighten::SinglePointSpin(MyMesh::Point &p, Quaternionx &q)
{
	Quaternionx qi = q.inverse();
	Quaternionx ss(0, p[0], p[1], p[2]);
	Quaternionx out;
	out = q*ss*qi;
	
	p[0] = out.x();
	p[1] = out.y();
	p[2] = out.x();
}

void ModelStraighten::SingleVectorTransfer(Vector3f a,Vector3f t)
{
	if ((a.norm() == 0) || (t.norm() == 0)) {
		cout << "axi zero error!" << endl;
		return;
	}

	Quaternionx mXTransfer;

	mXTransfer = Quaternionx::FromTwoVectors(a, t);

	MeshQuaternionSpin(mXTransfer);
}

void ModelStraighten::SingleVectorTransfer(Vector3f axi,float spin)
{
	Quaternionx mXTransfer(spin,axi[0],axi[1],axi[2]);
	MeshQuaternionSpin(mXTransfer);
}

void ModelStraighten::MeshQuaternionSpin(Quaternionx x)
{
	Quaternionx xinverse = x.inverse();
	MyMesh::Point p;
	Quaternionx out;
	for (MyMesh::VertexIter it = mesh.vertices_begin(); it != mesh.vertices_end(); it++)
	{
		p = mesh.point(*it);
		Quaternionx ss(0, p[0], p[1], p[2]);
		out = x*ss*xinverse;
		p = MyMesh::Point(out.x(), out.y(), out.z());
		mesh.set_point(*it, p);
	}
}

void ModelStraighten::MeshOffsetMove()
{
	MyMesh::Point minvhx, minvhz;
	minvhx = mesh.point(FindXYZSmallestPoint(0));//x
	minvhz = mesh.point(FindXYZSmallestPoint(2));//z

	MyMesh::Point offset;
	offset[0] = 5 - minvhx[0];//移到某个位置
	offset[1] = -1.5-m_centreGarvity[1];
	offset[2] = 5 - minvhz[2];

	MyMesh::Point p;
	for (MyMesh::VertexIter it = mesh.vertices_begin(); it != mesh.vertices_end(); it++)
	{
		p = mesh.point(*it);
		p += offset;
		mesh.set_point(*it, p);
	}

}
void ModelStraighten::AxisAlongMove(int i, float l) //300
{
	MyMesh::Point minvhy;
	minvhy = mesh.point(FindXYZSmallestPoint(i));//x

	MyMesh::Point offset(0,0,0);
	offset[i] = (l + minvhy[i]);//移动一定距离

	MyMesh::Point p;
	for (MyMesh::VertexIter it = mesh.vertices_begin(); it != mesh.vertices_end(); it++)
	{
		p = mesh.point(*it);
		p += offset;
		mesh.set_point(*it, p);
	}
}

float ModelStraighten::DistSurface(MyMesh::Point &a) {
	return (a[0] * m_coe[0] + a[1] * m_coe[1] + a[2] * m_coe[2] + m_coe[3]);
};


MyMesh::VertexHandle	ModelStraighten::FindNearestPoint(MyMesh::Point cc)
{
	MyMesh::VertexIter v_it = mesh.vertices_begin();
	MyMesh::Point p = mesh.point(*v_it);
	float dist = (cc - p).norm();
	float temp;
	MyMesh::VertexHandle vh = mesh.vertex_handle(v_it->idx());
	v_it++;
	for (; v_it != mesh.vertices_end(); v_it++)
	{
		p = mesh.point(*v_it);
		temp = (cc - p).norm();
		if (temp < dist)
		{
			dist = temp;
			vh = mesh.vertex_handle(v_it->idx());
		}
	}
	return vh;
}

MyMesh::VertexHandle	ModelStraighten::FindXYZSmallestPoint(int i)
{
	MyMesh::VertexIter v_it = mesh.vertices_begin();
	MyMesh::Point p = mesh.point(*v_it);
	float dist = p[i];
	//float temp;
	MyMesh::VertexHandle vh = mesh.vertex_handle(v_it->idx());
	v_it++;
	for (; v_it != mesh.vertices_end(); v_it++)
	{
		p = mesh.point(*v_it);
		//temp = p[i];
		if (p[i] < dist)
		{
			dist = p[i];
			vh = mesh.vertex_handle(v_it->idx());
		}
	}
	return vh;
}

MyMesh::VertexHandle ModelStraighten::FindXYZBigestPoint(int i)
{
	MyMesh::VertexIter v_it = mesh.vertices_begin();
	MyMesh::Point p = mesh.point(*v_it);
	float dist = p[i];
	MyMesh::VertexHandle vh = mesh.vertex_handle(v_it->idx());
	v_it++;
	for (; v_it != mesh.vertices_end(); v_it++)
	{
		p = mesh.point(*v_it);
		if (p[i] > dist)
		{
			dist = p[i];
			vh = mesh.vertex_handle(v_it->idx());
		}
	}
	return vh;
}

double ModelStraighten::StraightenSquareAngle(double a)//a 角度
{
	if (a != 0)
	{
		//Vector3f surface_cross_tri;
		// headvect;

		Quaternionx quaternspin(a, m_coe[0], m_coe[1], m_coe[2]);
		MeshQuaternionSpin(quaternspin);
		mesh.update_normals();
	}

	float fjudge = m_headPoint[0] * m_coe[0] + m_headPoint[1] * m_coe[1] + m_headPoint[2] * m_coe[2] + m_coe[3];

	double  projectArea = 0;
	for (MyMesh::FaceIter fit = mesh.faces_begin(); fit != mesh.faces_end(); fit++)
	{
		projectArea += CaculateSquare(mesh.face_handle(fit->idx()), fjudge);
	}

	return projectArea;
}


void ModelStraighten::StraightenSquare()
{

	if (!FindThreePoint())
	{
		cout << " Init error no vertex!" << endl;
		return;
	}
	MyMesh::Point meshHeadvect = m_headPoint - m_centreGarvity;

	Vector3f headvect(meshHeadvect[0], meshHeadvect[1], meshHeadvect[2]);

	SingleVectorTransfer(headvect, Vector3f(1, 0, 0));

	FindThreePoint();

	AxisAlongMove(1, 300); //沿y轴移动300个单位；

	int circle = 16;
	float pi = M_PI / circle; //每次旋转45度

	double square=0;

	//cout << pi << endl;
	square = StraightenSquareAngle(0);
	cout << 0 << " ：" << square << endl;

	for (int i = 1; i <=3; i++)
	{
		square=StraightenSquareAngle(pi);
		cout << i*22.5 << " ：" << square << endl;
	}
}

double ModelStraighten::CaculateSquare(MyMesh::FaceHandle fh,float fjudge)
{
	MyMesh::Point p[3];
	int i = 0;

	for (MyMesh::FaceVertexIter fv = mesh.fv_begin(fh); fv.is_valid(); fv++)
	{
		p[i] = mesh.point(*fv);
		i++;
	}
	MyMesh::Point centergravity = (p[0] + p[1] + p[2]) / 3;

	if ((centergravity[0] * m_coe[0] + centergravity[1] * m_coe[1] + centergravity[2] * m_coe[2] + m_coe[3])*fjudge >= 0)
	{
		return 0;
	}

	float line[3];
	line[0] = (p[0] - p[1]).norm();
	line[1] = (p[1] - p[2]).norm();
	line[2] = (p[0] - p[2]).norm();
	float cir = (line[0] + line[1] + line[2]) / 2;

	double s = sqrt(cir*(cir - line[0])*(cir - line[1])*(cir - line[2]));//三角形面积；

	MyMesh::Normal surfaceNormal = mesh.normal(fh);

	//surfaceNormal.normalize(); //用不着进行归一化了，再归一化可能会出现被除数是零的情况，导致最后结果错误；
	
	//float g = surfaceNormal[0] * m_coe[0] + surfaceNormal[1] * m_coe[1] + surfaceNormal[2] * m_coe[2];//y轴向的(0,1,0)
	if (surfaceNormal[1] <= 0)
	{
		return 0;
	}


	return surfaceNormal[1]*s;
}
void ModelStraighten::StraightenBottomNormalSingle()
{
	if (!FindThreePoint()) //给出截面
	{
		cout << " Init error no vertex!" << endl;
		return;
	}
	MyMesh::Point meshHeadvect = m_headPoint - m_centreGarvity;

	Vector3f headvect(meshHeadvect[0], meshHeadvect[1], meshHeadvect[2]);

	SingleVectorTransfer(headvect, Vector3f(1, 0, 0)); 

	FindThreePoint();

	MyMesh::Point origin = m_centreGarvity;

	origin = NormalMoveAlong(0.8);//往前走

	vector<MyMesh::Point> outlinesmall ;
	ExtratOutline(outlinesmall);

	MyMesh::Normal z = FindNearestZNormal(outlinesmall, origin); //方法之一求其最近的距离为Z轴；

	cout << "neast z axi: " << z << endl;
	z.normalize();
	cout << z << endl;
	cout << endl;

	if (ZCutShoe())
	{
		SingleVectorTransfer(Vector3f(z[0], z[1], z[2]), Vector3f(0, 0, -1));
	}
	else {
		SingleVectorTransfer(Vector3f(z[0], z[1], z[2]), Vector3f(0, 0, 1));
	}
	

	FindThreePoint();
	meshHeadvect = m_headPoint - m_centreGarvity;


	if (ZCutShoe()) {
		cout << endl;
		cout << "The model is headstand" << endl;

		SingleVectorTransfer(Vector3f(meshHeadvect[0], meshHeadvect[1], meshHeadvect[2]), M_PI/2);
	}
	FindThreePoint();

	MeshOffsetMove();
}

void ModelStraighten::StraightenBottomNormal()
{
	if (!FindThreePoint()) //给出截面
	{
		cout << " Init error no vertex!" << endl;
		return;
	}
	string smallfile = "smalloutline.obj";
	string bigfile = "bigoutline.obj";
	MyMesh::Point origin=m_centreGarvity;
	MyMesh::Normal z;

	float movea = 0.7, moveb = -0.4;

	origin=NormalMoveAlong(movea);//往前走
	cout << origin << endl;

	vector<MyMesh::Point> outlinesmall,outlinebig;
	vector<MyMesh::Normal> noutlinesmall, noutlinebig;
	//ExtratOutline(outlinesmall);

	SequeceExtratOutline(outlinesmall, noutlinesmall);

	OutlineOBJfile(smallfile,outlinesmall);

	//MyMesh::Normal z = FindNearestZNormal(outlinesmall, origin); //方法之一求其最近的距离为Z轴；
	//cout << "neast z axi: " << z << endl;
	//z.normalize();
	//cout << z << endl;
	//cout << endl;

	//SequenceArrayPointFlann(outline);//这个方法不行

	origin = NormalMoveAlong(moveb);
	cout << origin << endl;

	//ExtratOutline(outlinebig);
	SequeceExtratOutline(outlinebig, noutlinebig);

	OutlineOBJfile(bigfile, outlinebig);

	NormalMoveAlong(outlinesmall, (moveb+movea)*(m_headPoint - m_centreGarvity));

	z= FindZVector(outlinesmall,outlinebig,origin);

	cout << "z axi: " << z << endl;


}

MyMesh::Normal ModelStraighten::FindNearestZNormal(vector<MyMesh::Point> &sm,MyMesh::Point &origin)
{
	float a = (sm[0] - origin).norm();
	float t;
	int  s=0;
	for (int i = 1; i < sm.size(); i++)
	{
		t=(sm[i] - origin).norm();
		if (t < a)
		{
			a = t;
			s = i;
		}
	}
	cout << "origin : " << origin << endl;
	cout << "small s: " << sm[s] << endl;
	return (sm[s] - origin);//Z轴的正向；
}

MyMesh::Point ModelStraighten::NormalMoveAlong(float m)
{
	MyMesh::Normal aximesh = m_headPoint - m_centreGarvity;
	//aximesh.normalize();
	//MyMesh::Point thirdpoint = m_headPoint - aximesh * 10; //offset 10mm

	MyMesh::Point s = m_centreGarvity + m*aximesh;
	aximesh.normalize();
	float d = aximesh | s;

	m_coe[0] = aximesh[0];
	m_coe[1] = aximesh[1];
	m_coe[2] = aximesh[2];
	m_coe[3] = 0 - d;
	return s;
}

void ModelStraighten::NormalMoveAlong(vector<MyMesh::Point>&outline, MyMesh::Normal m)
{
	for (int i = 0; i < outline.size(); i++)
	{
		outline[i] += m;
	}
}

MyMesh::Normal ModelStraighten::FindZVector(vector<MyMesh::Point> &vecsmall,vector<MyMesh::Point> &vecbig,MyMesh::Point origin)
{
	float min = 0;
	float temp;
	MyMesh::Point pmax;
	for (int i = 0; i < vecbig.size(); i++)
	{
		temp = CrossPointSurface(vecbig[i], origin, vecsmall).norm();
		if(temp>min)
		{
			min = temp;
			pmax = vecbig[i];
		}
	}
	cout << "bigest: "<<pmax << endl;
	cout << "origin : " << origin << endl;
	return (pmax - origin);
}

MyMesh::Normal ModelStraighten::CrossPointSurface(MyMesh::Point big,MyMesh::Point origin,vector<MyMesh::Point>&sm)
{
	Vector3f vbig(big[0], big[1], big[2]);
	Vector3f vorigin(origin[0], origin[1], origin[2]);

	Vector3f recnorml(m_coe[0], m_coe[1], m_coe[2]);

	Vector3f t = recnorml.cross(vbig - vorigin);

	float d = 0 - t.dot(vorigin);

	int size = sm.size();

	MyMesh::Point m;

	for (int j = 0; j < size; j++)
	{
		if (j != (size-1))
		{
			Vector3f a(sm[j][0], sm[j][1], sm[j][2]);
			Vector3f b(sm[j + 1][0], sm[j + 1][1], sm[j + 1][2]);
			
			if (((a.dot(t) + d)*(b.dot(t) + d))<=0)
			{
				MyMesh::Point coe(t[0], t[1], t[2]);
				MyMesh::Point v = sm[j] - sm[j+1];
				float tf = (0 - ((coe | v) + d)) / (coe | v);

				m = MyMesh::Point(v[0] * tf + a[0], v[1] * tf + a[1], v[2] * tf + a[2]);
				break;
			}

		}
		else 
		{
			Vector3f a(sm[j][0], sm[j][1], sm[j][2]);
			Vector3f b(sm[0][0], sm[0][1], sm[0][2]);
			if (((a.dot(t) + d)*(b.dot(t) + d)) <= 0)
			{
				MyMesh::Point coe(t[0], t[1], t[2]);
				MyMesh::Point v = sm[j] - sm[j + 1];
				float tf = (0 - ((coe | v) + d)) / (coe | v);

				m = MyMesh::Point(v[0] * tf + a[0], v[1] * tf + a[1], v[2] * tf + a[2]);
				break;
			}
		}
	}
	return (origin - m);
}

void ModelStraighten::OutlineOBJfile(string outfilename,vector<MyMesh::Point>&outline)
{
	FILE *fp;
	fopen_s(&fp, outfilename.c_str(), "w");
	fprintf(fp, "# Studio\n");
	fprintf(fp, "g Point_Model_1\n");
	for (unsigned int i = 0; i < outline.size(); i++) {
		fprintf(fp, "v %f %f %f\n", outline[i][0], outline[i][1], outline[i][2]);
		fprintf(fp, "p %d\n", (i + 1));
	}
	fprintf(fp, "# end of file\n");
	fclose(fp);
}

void ModelStraighten::ExtratOutline(vector<MyMesh::Point>& arr)
{
	arr.clear();

	int i = 0;
	MyMesh::Point m;
	
	for (MyMesh::EdgeIter e_iter = mesh.edges_begin(); e_iter != mesh.edges_end(); e_iter++)
	{
		if (EdgeSurfaceCrossPoint(mesh.edge_handle(e_iter->idx()),m))
		{
			arr.push_back(m);
		}
	}
}

bool ModelStraighten::EdgeSurfaceCrossPoint(MyMesh::EdgeHandle e,MyMesh::Point &m)
{
	MyMesh::HalfedgeHandle hf = mesh.halfedge_handle(e, 0);
	MyMesh::Point pfrom	=mesh.point( mesh.from_vertex_handle(hf));
	MyMesh::Point pto = mesh.point(mesh.to_vertex_handle(hf));

	float ffrom = DistSurface(pfrom);
	float fto = DistSurface(pto);
	if (ffrom*fto > 0)
	{
		return 0;
	}
	else 
	{
		if (ffrom == 0)
		{
			m = pfrom;
			return 1;
		}
		if (fto == 0)
		{
			m = pto;
			return 1;
		}

		MyMesh::Point coe(m_coe[0], m_coe[1], m_coe[2]);
		MyMesh::Point v = pfrom - pto;
		//cout << "v : "<<v << endl;
		float t;
		t = (0 - ((coe| pfrom) + m_coe[3])) / (coe|v);

		m = MyMesh::Point(v[0] * t + pfrom[0], v[1] * t + pfrom[1], v[2] * t + pfrom[2]);
		return 1;
	}
	return 0;
}

bool ModelStraighten::HalfedgeSurfaceCrossPoint(MyMesh::HalfedgeHandle e, MyMesh::Point &m,MyMesh::Normal& n)
{
	MyMesh::Point pfrom = mesh.point(mesh.from_vertex_handle(e));
	MyMesh::Point pto = mesh.point(mesh.to_vertex_handle(e));

	float ffrom = DistSurface(pfrom);
	float fto = DistSurface(pto);
	if (ffrom*fto > 0)
	{
		return 0;
	}
	else
	{
		float s1 = (pfrom - m).norm();
		float s2 = (pto - m).norm();
		n = mesh.normal(mesh.from_vertex_handle(e))*(s2 / (s1 + s2)) + mesh.normal(mesh.to_vertex_handle(e))*(s1 / (s1 + s2));//平面法向量投影
		
		if (ffrom == 0)
		{
			m = pfrom;
			return 2;
		}else if (fto == 0)
		{
			m = pto;
			return 2;
		}
		else {
			MyMesh::Point coe(m_coe[0], m_coe[1], m_coe[2]);
			MyMesh::Point v = pfrom - pto;
			float t;
			t = (0 - ((coe | pfrom) + m_coe[3])) / (coe | v);

			m = MyMesh::Point(v[0] * t + pfrom[0], v[1] * t + pfrom[1], v[2] * t + pfrom[2]);
		}

		return 1;
	}

	return 0;
}

void ModelStraighten::SequenceArrayPointFlann(vector<MyMesh::Point>& arr)
{
	int row = arr.size();
	int nn = 3;

	flann::Matrix<float>dataset(new float[row*nn], row, nn);
	for (int i = 0; i < row; i++)
	{
		dataset[i][0] = arr[i][0];
		dataset[i][1] = arr[i][1];
		dataset[i][2] = arr[i][2];
	}

	flann::Index< L2<float> > flann_index(dataset, flann::KDTreeSingleIndexParams(10));

	vector<vector<int>>indices;
	vector<vector<float>>dists;

	flann_index.buildIndex();

	flann_index.knnSearch(dataset, indices, dists, 3, flann::SearchParams(128)); //自己搜自己比较临近的三个点，应该包括自己一个点，临近的两个点

	vector<int> afterarr;
	afterarr.push_back(indices[0][0]);
	int up= indices[0][1],temp;
	for (int i=1;i<indices.size();i++)
	{
		temp = indices[up][1];
		cout << temp << endl;
		if (temp == afterarr[i - 1]) {
			up = indices[afterarr[i-1]][2];
		}
		cout << up << endl;
		cout << endl;
		up = temp;
		afterarr.push_back(up);
	}
	arr.clear();
	for (auto i : afterarr)
	{
		arr.push_back(MyMesh::Point(dataset[i][0], dataset[i][1], dataset[i][2]));
	}
}

void ModelStraighten::SequeceExtratOutline(vector<MyMesh::Point>& arr,vector<MyMesh::Normal>&arrnormal)
{
	arrnormal.clear();
	arr.clear();
	float max = 9999;
	float ft = 0;
	MyMesh::VertexHandle HandleBegin;
	for (MyMesh::VertexIter vit = mesh.vertices_begin(); vit != mesh.vertices_end(); vit++)
	{
		ft=abs(DistSurface(mesh.point(mesh.vertex_handle(vit->idx()))));
		if (ft < max)
		{
			max = ft;
			HandleBegin = mesh.vertex_handle(vit->idx());
		}
	}

	MyMesh::Point p;
	MyMesh::Normal n;
	for (MyMesh::VertexOHalfedgeIter voh_it = mesh.voh_iter(HandleBegin); voh_it.is_valid(); ++voh_it) {
		MyMesh::HalfedgeHandle heh = mesh.halfedge_handle(voh_it->idx());
		
		if (HalfedgeSurfaceCrossPoint(heh,p,n)==1)
		{
			arr.push_back(p);
			arrnormal.push_back(n);
			heh = mesh.next_halfedge_handle(heh);
			
			IterationHalfEdge(HandleBegin,heh, arr,arrnormal);
			break;
			//init = 1;
		}
	}
}

void ModelStraighten::IterationHalfEdge(MyMesh::VertexHandle start,MyMesh::HalfedgeHandle heh,vector<MyMesh::Point>&outline,vector<MyMesh::Normal>&normaloutline)
{
	//偏移至下一条半边，以vertex为基点索引该边对应顶点

	MyMesh::Point p;
	MyMesh::Point n;
	MyMesh::HalfedgeHandle heh_next = heh;//=mesh.next_halfedge_handle(heh);
	MyMesh::VertexHandle vertex_i, vertex_s;
	int ac = 0;
	//cout << heh.idx() << endl;
	int ii = 0;

	while (((vertex_i != start) && (vertex_s != start))||(ii<10)) {

		ac = HalfedgeSurfaceCrossPoint(heh_next, p, n);
		if (ac==1)
		{
			//cout << "erro 1" << endl;
			outline.push_back(p);
			normaloutline.push_back(n);
			heh_next = mesh.opposite_halfedge_handle(heh_next);
			//cout << heh_next.idx() << endl;
			heh_next = mesh.next_halfedge_handle(heh_next);
		}
		else if (ac == 2)
		{
			outline.push_back(p);
			normaloutline.push_back(n);
			heh_next = mesh.opposite_halfedge_handle(heh_next);
			heh_next = mesh.next_halfedge_handle(heh_next);

			if (HalfedgeSurfaceCrossPoint(heh_next, p, n))
			{
				heh_next = mesh.opposite_halfedge_handle(heh_next);
				heh_next = mesh.next_halfedge_handle(heh_next);
				heh_next = mesh.next_halfedge_handle(heh_next);
			}
			else {
				heh_next = mesh.next_halfedge_handle(heh_next);
				heh_next = mesh.opposite_halfedge_handle(heh_next);
				heh_next = mesh.next_halfedge_handle(heh_next);
			}
			
		}
		else {
			//cout << "erro 0" << endl;
			heh_next = mesh.next_halfedge_handle(heh_next);
		}
		vertex_i = mesh.to_vertex_handle(heh_next);	//指向点
		vertex_s = mesh.from_vertex_handle(heh_next);  //出发点
		//cout << heh_next.idx() << endl;
		ii++;
		
	}
};


void ModelStraighten::OutTriMesh(string name)
{
	MyMesh meshout;

	MyMesh::VertexHandle vhandle[3];
	vhandle[0] = meshout.add_vertex(m_centreGarvity);

	vhandle[1] = meshout.add_vertex(m_centreGarvity_ahead);
	vhandle[2] = meshout.add_vertex(m_centreGarvity_back);

	vector<MyMesh::VertexHandle> face_vhandles;
	MyMesh::FaceHandle   fhandle[1];

	face_vhandles.push_back(vhandle[0]);
	face_vhandles.push_back(vhandle[1]);
	face_vhandles.push_back(vhandle[2]);
	fhandle[0] = meshout.add_face(face_vhandles);

	OpenMesh::IO::write_mesh(meshout, name.c_str());
}

void ModelStraighten::FlannIndexFind(vector<MyMesh::Point>&arr_query,int fd_num, vector<vector<int>>&indices)
{
	int nn = 3;
	int querysize = arr_query.size();

	//vector<vector<int>>indices;
	vector<vector<float>>dists;
	
	flann::Matrix<float>query(new float[querysize * nn], querysize, nn);

	for (int i = 0; i < querysize; i++)
	{
		query[i][0] = arr_query[i][0];
		query[i][1] = arr_query[i][1];
		query[i][2] = arr_query[i][2];
	}
	m_flann_index->knnSearch(query, indices, dists, fd_num, flann::SearchParams(128));//默认只搜一个结果吧

	/*
		indices[i][j]:i--表示第几个的query查询；j--表示该次的query返回最近的第j个结果
	*/
	//for (int i = 0; i < querysize; i++) 
	//{
	//	for (int j = 0; j < fd_num; j++) {
	//		out_query.push_back(MyMesh::Point((*m_dataset)[indices[i][j]][0], (*m_dataset)[indices[i][j]][1], (*m_dataset)[indices[i][j]][2]));

	//		/*cout << indices[i][j] << " : ";
	//		cout << (*m_dataset)[indices[i][j]][0] << " " << (*m_dataset)[indices[i][j]][1] << " " << (*m_dataset)[indices[i][j]][2] << endl;
	//		cout << mesh.point(mesh.vertex_handle(indices[i][j])) << endl;
	//		//这个寻找出来的matrix整数是可以直接在mesh中进行索引的
	//		*/
	//	}
	//}
	return;
}

void ModelStraighten::FlannTest()
{
	int row = mesh.n_vertices();
	int nn = 3;
	flann::Matrix<float>dataset(new float[row*nn], row, nn);
	cout << dataset.rows << " " << dataset.cols <<" " << endl;

	int ii = 0;
	MyMesh::Point p;
	for (MyMesh::VertexIter vit = mesh.vertices_begin(); vit != mesh.vertices_end(); vit++)
	{
		p = mesh.point(*vit);
		dataset[ii][0] = p[0];
		dataset[ii][1] = p[1];
		dataset[ii][2] = p[2];
		ii++;
	}

	flann::Index< L2<float> > *ptr_flann_index;
	ptr_flann_index = new flann::Index< L2<float> >(dataset, flann::KDTreeSingleIndexParams(10));//new 创建可以全局使用

	ptr_flann_index->buildIndex();


	flann::Index< L2<float> > flann_index(dataset,flann::KDTreeSingleIndexParams(10));
	
	flann_index.buildIndex();
	
	//flann_index.addPoints(dataset,2);
	vector<vector<int>>indices;
	vector<vector<float>>dists;

	flann::Matrix<float>query(new float[1 * nn], 1, nn);
	//flann::Matrix<float>dists(new float[1 * nn], row, nn);
	//flann::Matrix<int>indicei(new int[1 * nn], row, nn);
	
	query[0][0] = 92.5049;
	query[0][1] = -21.6531;
	query[0][2] = 73.8194;

	ptr_flann_index->knnSearch(query, indices, dists, 2, flann::SearchParams(128));
	cout << indices.size() << endl;
	for (auto i : indices[0]) {
		cout << i << " : ";
		cout << dataset[i][0] << " " << dataset[i][1] << " " << dataset[i][2] << endl;

	}

	flann_index.knnSearch(query, indices, dists, 2, flann::SearchParams(128));
	cout << indices.size() << endl;
	for (auto i : indices[0]) {
		cout << i << " : ";
		cout << dataset[i][0] << " " << dataset[i][1] << " " << dataset[i][2] << endl;
	}
	cout << endl;
	//flann_index.knnSearch()
	return;
}

void ModelStraighten::FlannTest2() 
{
	if (m_flann_index == NULL) {
		cout << "flann no init!" << endl;
		return;
	}
	MyMesh::Point t1(92.5049,-21.6531,73.8194);
	MyMesh::Point t2(162.0809,33.2874,28.2474);
	vector<MyMesh::Point> s1, s2;
	s1.push_back(t1);
	s1.push_back(t2);
	vector<vector<int>>si;
	FlannIndexFind(s1, 2, si);
}