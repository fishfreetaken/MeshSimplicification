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

	void			StraightenSquare(); //基于表面积投影

	void			MeshSmoothing(); //这个平滑貌似有点问题，不能输出结果，所以暂时先不考虑；

	void			StraightenBottomNormal();		//基于底板法向量；
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
		该指针用来进行flann查找最近点；
	*/
	flann::Index< L2<float>>	*m_flann_index; //全局mesh的搜索；
	flann::Matrix<float>		*m_dataset;

	void					FlannIndexFind(vector<MyMesh::Point>&arr_query, int fd_num, vector<vector<int>>&indices);

	/*
		计算三角形面积的返回以及返回一个重心
	*/
	MyMesh::Point			TriSqureCompute(MyMesh::FaceHandle fh,float &s);
	bool					FindThreePoint();

	void					TwoIterGarvityPoint(MyMesh::Point origin,MyMesh::Point& toward);//给出两个方向来求其朝向下的界面部分重心坐标；

	Vector3f				InitTwoIterGarvityPoint( int i);//ini from gravity;
	/*
		模型截面
	*/
	vector<float>				m_coe;

	void					CrossSectionModel();
	float					DistSurface(MyMesh::Point &a);

	/*
		工具
	*/
	void					OutTriMesh(string name);//输出点云文件
	MyMesh::VertexHandle	FindNearestPoint(MyMesh::Point cc);//找最近点；
	MyMesh::VertexHandle	FindXYZSmallestPoint(int i);
	MyMesh::VertexHandle	FindXYZBigestPoint(int i);
	void					AxisAlongMove(int i,float l);

	MyMesh::Point			NormalMoveAlong(float m);
	void					NormalMoveAlong(vector<MyMesh::Point>&outline, MyMesh::Normal m);


	/*
		摆正
	*/
	void					Straighten();
	void					StraightenTest();

	void					SingleVectorTransfer(Vector3f a, Vector3f t);
	void					SingleVectorTransfer(Vector3f axi, float spin);

	void					TwoVectorTransfer(Quaternionx &x);

	void					MeshQuaternionSpin(Quaternionx x);
	void					SinglePointSpin(MyMesh::Point &p, Quaternionx &q);

	void					MeshOffsetMove();//移动到合适的位置；
	void					ProduceTwoSpinAxi(Vector3f &s1, Vector3f &s2);

	bool					ZCutShoe();//在摆正好的情况下，从z轴进行截取一个面，来判断上下；
	
	/*
		由于三个重心之间的微小差异，其摆向角度差异变化不是很明显，所以导致朝向错误；（小数点后第四位）
		具体可以参考EigenQuaternSpinTest里面给出的例子；
		发现不同坐标系下计算重心坐标的位置不同；
		放弃不用；
	*/
	void					Straighten2();

	/*
		基于表面积投影的摆正；
	*/
	double					CaculateSquare(MyMesh::FaceHandle fh, float fjudge);//返回面积
	double					StraightenSquareAngle(double a); //沿着对称轴进行旋转；

	/*
		基于底板法向量的摆正；
	*/
	void					ExtratOutline(vector<MyMesh::Point>& arr); //m_coe 平面方程； 截取平面的一个轮廓点
	bool					EdgeSurfaceCrossPoint(MyMesh::EdgeHandle e,MyMesh::Point &m);
	bool					HalfedgeSurfaceCrossPoint(MyMesh::HalfedgeHandle e, MyMesh::Point &m,MyMesh::Normal &n);
	void					SequenceArrayPointFlann(vector<MyMesh::Point>& arr);		//可以基于flann将点进行排序；

	/*
		前脚掌最小截取平面的摆正；
	*/
	void					SequeceExtratOutline(vector<MyMesh::Point>& arr, vector<MyMesh::Normal>&arrnormal);

	MyMesh::Normal			FindZVector(vector<MyMesh::Point> &vecsmall, vector<MyMesh::Point> &vecbig,MyMesh::Point origin);
	MyMesh::Normal			CrossPointSurface(MyMesh::Point big, MyMesh::Point origin, vector<MyMesh::Point>&sm);

	MyMesh::Normal			FindNearestZNormal(vector<MyMesh::Point> &sm,MyMesh::Point &origin);//脚尖的距离重心的最近点作为Z轴向；

	void					IterationHalfEdge(MyMesh::VertexHandle start,MyMesh::HalfedgeHandle heh, vector<MyMesh::Point>&outline, vector<MyMesh::Normal>&normaloutline);
};

