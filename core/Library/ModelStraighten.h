#pragma once
#include "IncludeHeader.h"

#define HEADSTEPSURFACE 2

#define MAXIUMDISTANCE 9999
typedef Quaternion<float, 0> Quaternionx;

class ModelStraighten
{
public:
	ModelStraighten(MyMesh&a,bool f);
	ModelStraighten(MyMesh&a);
	~ModelStraighten();


	/*
		test library;
	*/
	void			MainTest();
	void			FlannTest();
	void			FlannTest2();
	
	void			EigenQuaternSpinTest();

	void			StraightenSquare(); //���ڱ����ͶӰ

	void			MeshSmoothing(); //���ƽ��ò���е����⣬������������������ʱ�Ȳ����ǣ�

	void			StraightenBottomNormal();		//���ڵװ巨������
	void			StraightenBottomNormalSingle();

	void			OutlineOBJfile(string outfilename, vector<MyMesh::Point>&outline);
private :
	MyMesh&						mesh;

	MyMesh::Point				m_centreGarvity;
	MyMesh::Point				m_centreGarvity_back;
	MyMesh::Point				m_centreGarvity_ahead;
	MyMesh::Point				m_headPoint;
	//MyMesh::Point				m_endPoint;

	/*
		��ָ����������flann��������㣻
	*/
	flann::Index< L2<float>>	*m_flann_index; //ȫ��mesh��������
	flann::Matrix<float>		*m_dataset;

	void					FlannIndexFind(vector<MyMesh::Point>&arr_query, int fd_num, vector<vector<int>>&indices);

	/*
		��������������ķ����Լ�����һ������
	*/
	MyMesh::Point			TriSqureCompute(MyMesh::FaceHandle fh,float &s);
	bool					FindThreePoint();

	void					TwoIterGarvityPoint(MyMesh::Point origin,MyMesh::Point& toward);//�����������������䳯���µĽ��沿���������ꣻ

	Vector3f				InitTwoIterGarvityPoint( int i);//ini from gravity;
	/*
		ģ�ͽ���
	*/
	vector<float>				m_coe;

	void					CrossSectionModel();
	float					DistSurface(MyMesh::Point &a);

	/*
		����
	*/
	void					OutTriMesh(string name);//��������ļ�
	MyMesh::VertexHandle	FindNearestPoint(MyMesh::Point cc);//������㣻
	MyMesh::VertexHandle	FindXYZSmallestPoint(int i);
	MyMesh::VertexHandle	FindXYZBigestPoint(int i);
	void					AxisAlongMove(int i,float l);

	MyMesh::Point			NormalMoveAlong(float m);
	void					NormalMoveAlong(vector<MyMesh::Point>&outline, MyMesh::Normal m);


	/*
		����
	*/
	void					Straighten();
	void					StraightenTest();

	void					SingleVectorTransfer(Vector3f a, Vector3f t);
	void					SingleVectorTransfer(Vector3f axi, float spin);

	void					TwoVectorTransfer(Quaternionx &x);

	void					MeshQuaternionSpin(Quaternionx x);
	void					SinglePointSpin(MyMesh::Point &p, Quaternionx &q);

	void					MeshOffsetMove();//�ƶ������ʵ�λ�ã�
	void					ProduceTwoSpinAxi(Vector3f &s1, Vector3f &s2);

	bool					ZCutShoe();//�ڰ����õ�����£���z����н�ȡһ���棬���ж����£�
	
	/*
		������������֮���΢С���죬�����ǶȲ���仯���Ǻ����ԣ����Ե��³�����󣻣�С��������λ��
		������Բο�EigenQuaternSpinTest������������ӣ�
		���ֲ�ͬ����ϵ�¼������������λ�ò�ͬ��
		�������ã�
	*/
	void					Straighten2();

	/*
		���ڱ����ͶӰ�İ�����
	*/
	double					CaculateSquare(MyMesh::FaceHandle fh, float fjudge);//�������
	double					StraightenSquareAngle(double a); //���ŶԳ��������ת��

	/*
		���ڵװ巨�����İ�����
	*/
	void					ExtratOutline(vector<MyMesh::Point>& arr); //m_coe ƽ�淽�̣� ��ȡƽ���һ��������
	bool					EdgeSurfaceCrossPoint(MyMesh::EdgeHandle e,MyMesh::Point &m);
	bool					HalfedgeSurfaceCrossPoint(MyMesh::HalfedgeHandle e, MyMesh::Point &m,MyMesh::Normal &n);
	void					SequenceArrayPointFlann(vector<MyMesh::Point>& arr);		//���Ի���flann�����������

	/*
		ǰ������С��ȡƽ��İ�����
	*/
	void					SequeceExtratOutline(vector<MyMesh::Point>& arr, vector<MyMesh::Normal>&arrnormal);

	MyMesh::Normal			FindZVector(vector<MyMesh::Point> &vecsmall, vector<MyMesh::Point> &vecbig,MyMesh::Point origin);
	MyMesh::Normal			CrossPointSurface(MyMesh::Point big, MyMesh::Point origin, vector<MyMesh::Point>&sm);

	MyMesh::Normal			FindNearestZNormal(vector<MyMesh::Point> &sm,MyMesh::Point &origin);//�ż�ľ������ĵ��������ΪZ����

	void					IterationHalfEdge(MyMesh::VertexHandle start,MyMesh::HalfedgeHandle heh, vector<MyMesh::Point>&outline, vector<MyMesh::Normal>&normaloutline);
};

