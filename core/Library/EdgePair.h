#pragma once
#include "IncludeHeader.h"
//#include "MyStruct.h"

class VertexPoint {
public:
	VertexPoint(MyMesh::VertexHandle a,MyMesh &c) :
		m_vh(a),
		cmesh(c)
	{
		m_Q << 0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0;
	}

	int QMSurfaceCoe();

	int UpdateVertexQ();//更新点的Q矩阵
	
	Matrix4d ReturnMatrix() {
		return m_Q;
	}
private:
	MyMesh&cmesh;
	MyMesh::VertexHandle m_vh;

	Matrix4d m_Q;

	//vector<MyMesh::EdgeHandle> m_arr_Edg; //该点周围的边

	void ComputeSurfaceQ(MyMesh::Point *vp);
};

typedef map<int, VertexPoint*>	INT_MAP_VERTEXP;

class EdgePair
{
public:
	EdgePair(MyMesh &c);
	EdgePair(MyMesh::EdgeHandle e, MyMesh &c);

	~EdgePair();

	//int AddVertexSurfaceCoe();//思路：每条边索引两个点，然后两个点再根据周围面进行索引，肯定有重复计算；

	int UpdatePair(INT_MAP_VERTEXP&a);//更新边的最优系数x,同时给出最优点Vmax
	//int ComputeQArroundSurface(MyMesh::VertexHandle a);
	//void ComputeSurfaceQ(MyMesh::Point *vp);

	double ReturnQX() {//返回该函数的Q权值
		return m_x;
	}

	//给出该边的两个点
	MyMesh::VertexHandle ReturnMa() {
		return m_a;
	}
	MyMesh::VertexHandle ReturnMb() { 
		return m_b;
	}

	//当前边给出
	MyMesh::EdgeHandle ReturnEdge() {
		return m_edge_h;
	}

	MESHSETEDGE ReturnEdgeSet() {
		return m_QX_Idx;
	}

	MyMesh::HalfedgeHandle ReturnHalfEdge() {
		return m_halfedge_h;
	}
	//删除b，移动a
	int PairCollapse();//调用来进行网格精简collapse
	int PairCollapse2();
	int PairCollapseInverse();

private:
	MyMesh&cmesh;

	MyMesh::VertexHandle m_a, m_b; //最终都是collapse到a点

	MyMesh::EdgeHandle m_edge_h;//一条边界

	MyMesh::HalfedgeHandle m_halfedge_h;

	//vector<MyMesh::VertexHandle> m_a_arr, m_b_arr;

	//vector<Vector4d> m_equ_surf_a, m_equ_surf_b;//存储平面方程参数

	//Matrix4d m_Q_a, m_Q_b;//两个Q矩阵

	//Vector4d m_v;//最优坐标点 (x,y,z,1)
	MyMesh::Point m_v_mesh;

	Matrix4d m_Q, m_Q_Equation;

	MESHSETEDGE m_QX_Idx;//与pair的id绑定的Q值的结构体

	double m_x = 0; //该边的合并系数权值

	void UpdateQEquation(Matrix4d x);
	void ComputeMaxPoint();// (MyMesh::VertexHandle a, MyMesh::VertexHandle b);//核心计算最优坐标以及x值的函数；
};

typedef map<int, EdgePair*>		INT_MAP_EDGEPAIR; //value - edge->idx : value - EdgePair* 

