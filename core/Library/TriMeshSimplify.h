#pragma once
#include "IncludeHeader.h"

class TriMeshSimplify
{
public:
	TriMeshSimplify(MyMesh&a, MyMesh::FaceHandle b);

	~TriMeshSimplify();

	void UpdateTriQ();

	int TriCollapse();
	int TriCollapse3();

	bool is_TriMesh();//判断该三角形是否有三个点
	int is_TriMesh(MyMesh::VertexHandle vh);
	bool is_Three_Tri(MyMesh::FaceHandle cc, MyMesh::HalfedgeHandle &hh); //判断该三角形是否是特殊的顶点周围只有三个点的三角形；

	//debug;
	bool DebugInfo2();
	void DebugInfo();

	bool UpdateHalfEdge(MyMesh::HalfedgeHandle cc);

	double ReturnMx() { return m_x; }
	MESHSETEDGE ReturnTriSet() { return m_QX_Set; }
	INT_INT_MAP ReturnTriMap() { return m_Face_Arround;}

private:
	MyMesh &mesh;

	MyMesh::FaceHandle m_Mesh_Face;
	MyMesh::HalfedgeHandle m_Collapse_Half;//指定需要collapse的半边结构；
	//MyMesh::HalfedgeHandle m_Collapse_Opposit_Half;//指定需要collapse的半边结构；

	Matrix4d m_Face_Q, m_Q_Equation;

	MyMesh::Point m_v_mesh;

	double m_x;//误差

	MESHSETEDGE m_QX_Set;

	INT_INT_MAP m_Face_Arround;//用来记录周围的三角形，用来比较变形前后缺少的三角形

	void ComputeSurfaceQ(MyMesh::Point *vp); //计算三角形的Q值

	void ComputeMaxPoint(); //计算最优点中心点

};
typedef map<int, TriMeshSimplify*>	INT_MAP_TRIMESH;

class MyTriOpenMesh
{
public:
	MyTriOpenMesh();
	MyTriOpenMesh(MyMesh a):mesh(a){}
	~MyTriOpenMesh();

	MyMesh mesh;

	int Readfile(const char * argg);

	void Writefile(const char *argg, int i);
	void Writefile(const char *argg);

	int MeshSimplification(float dest);

	void Release() { //删除已经无效的collapse点
		cout << "Release RAM" << endl;
		mesh.garbage_collection();
		INT_MAP_TRIMESH::iterator it_edge(m_TriMesh_Map.begin());
		for (; it_edge != m_TriMesh_Map.end(); it_edge++) {
			delete it_edge->second;//释放new 创建的内存
		}
		cout << "Release RAM OVER!" << endl;
	}

	int CollapseIterator();
	int CollapseIterator(int imf);

private:

	OpenMesh::IO::Options opt;

	INT_MAP_TRIMESH m_TriMesh_Map;

	void FillTriMeshMap();

	STRUCT_SET_EDGEPAIR m_TriMesh_Set;
};
