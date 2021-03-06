#pragma once
#include "IncludeHeader.h"

class TriMeshSimplify
{
public:
	TriMeshSimplify(MyMesh&a, MyMesh::FaceHandle b);

	~TriMeshSimplify();

	void UpdateTriQ();

	int TriCollapse(vector<int>&a,vector<int>&b, map<int, TriMeshSimplify*>&c);
	int TriCollapse2(vector<int>&a, vector<int>&b);
	int TriCollapse3(vector<int>&a, vector<int>&b);

	bool is_TriMesh();//判断该三角形是否有三个点
	int is_TriMesh(MyMesh::VertexHandle vh);
	bool is_Three_Tri(MyMesh::FaceHandle cc, MyMesh::HalfedgeHandle &hh); //判断该三角形是否是特殊的顶点周围只有三个点的三角形；

	//debug;
	bool DebugInfo2();
	void DebugInfo();

	bool UpdateHalfEdge(MyMesh::HalfedgeHandle cc);

	//double ReturnMx() { return m_x; }
	MESHSETEDGE ReturnTriSet() { return m_QX_Set; }
	//INT_INT_MAP ReturnTriMap() { return m_Face_Arround;}

	void SetHalfEdge() {
		m_Collapse_Half = mesh.halfedge_handle(mesh.fh_begin(m_Mesh_Face)->idx());
	}

	int ReturnObtuse() { return m_Obtuse; }

	bool ReturnObtuseHalf(MyMesh::HalfedgeHandle &hf) {
		if (m_Obtuse == -1) {
			return 0;
		}
		MyMesh::VertexHandle v = mesh.vertex_handle(m_Obtuse);
		MyMesh::HalfedgeHandle cc;
		for (MyMesh::FaceHalfedgeIter fh_iter = mesh.fh_begin(m_Mesh_Face); fh_iter.is_valid(); fh_iter++) {
			cc = mesh.halfedge_handle(fh_iter->idx());
			if (mesh.from_vertex_handle(cc)==v) {
				return 1;
			}
		}
		return 0;
	}
	int JudgeShortEdge(MyMesh::HalfedgeHandle &hf, map<int, TriMeshSimplify*>&cc) {
		if (m_Obtuse == -1) {
			return 0;
		}
		MyMesh::HalfedgeHandle ohf = mesh.opposite_halfedge_handle(hf);
		MyMesh::VertexHandle vv = mesh.vertex_handle(m_Obtuse);
		if ((mesh.from_vertex_handle(ohf)==vv) || (mesh.to_vertex_handle(ohf)==vv)) {
			return 1;
		}

		MyMesh::HalfedgeHandle nahf = mesh.opposite_halfedge_handle(mesh.next_halfedge_handle(ohf));
		if (cc[mesh.face_handle(nahf).idx()]->EdgeObtuse(nahf)) {
			hf = mesh.next_halfedge_handle(ohf);
			return - 1;
		}

		MyMesh::HalfedgeHandle nbhf = mesh.opposite_halfedge_handle(mesh.prev_halfedge_handle(ohf));
		if (cc[mesh.face_handle(nbhf).idx()]->EdgeObtuse(nbhf)) {
			hf = mesh.prev_halfedge_handle(ohf);
		}

		return 2;
	}
	bool EdgeObtuse(MyMesh::HalfedgeHandle &a) {
		if (m_Obtuse == -1) {
			return 0;
		}
		MyMesh::VertexHandle vv = mesh.vertex_handle(m_Obtuse);
		if ((mesh.from_vertex_handle(a) == vv) || (mesh.to_vertex_handle(a) == vv)) {
			return 1;
		}
		return 0;
	}

private:
	MyMesh &mesh;

	MyMesh::FaceHandle m_Mesh_Face;
	MyMesh::HalfedgeHandle m_Collapse_Half;//指定需要collapse的半边结构；
	//MyMesh::HalfedgeHandle m_Collapse_Opposit_Half;//指定需要collapse的半边结构；

	//Matrix4d m_Face_Q, m_Q_Equation;

	MyMesh::Point m_v_mesh;

	//double m_x;//误差

	MESHSETEDGE m_QX_Set;

	int m_Obtuse = -1;//判断三角形是不是钝角，如果是该值保存钝角顶点位置索引
	
	void is_Obtuse_Tri();

	//INT_INT_MAP m_Face_Arround;//用来记录周围的三角形，用来比较变形前后缺少的三角形

	Matrix4d ComputeSurfaceQ(MyMesh::Point *vp); //计算三角形的Q值

	double ComputeMaxPoint(Matrix4d &x); //计算最优点中心点

};
typedef map<int, TriMeshSimplify*>	INT_MAP_TRIMESH;
typedef vector<TriMeshSimplify*>	VECTOR_TRIMESH;

class MyTriOpenMesh
{
public:
	MyTriOpenMesh();
	MyTriOpenMesh(MyMesh a,float b):mesh(a), m_dest(b){}
	~MyTriOpenMesh();

	MyMesh mesh; //Public;

	/*
		文件的读写
	*/
	int Readfile(const char * argg);

	void Writefile(const char *argg, int i); //参数int i：1 表示二进制，0 表示ASCII
	void Writefile(const char *argg);		 //使用默认构造输出文件

	/*
		主进程入口函数；
		参数为简化量:0<dest<1；
	*/
	int MeshSimplification(float dest);

	/*
		删除已经无效的collapse点，同时释放内存
	*/
	void Release() {
		cout << "Release RAM" << endl;
		//mesh.garbage_collection();
		INT_MAP_TRIMESH::iterator it_edge(m_TriMesh_Map.begin());
		for (; it_edge != m_TriMesh_Map.end(); it_edge++) {
			delete it_edge->second;//释放new 创建的内存
		}
		cout << "Release RAM OVER!" << endl;
	}

	/*
		两个指定的接口函数；
	*/
	void InterFacePort(string in,string out,float dest); //这个适用给定输入和输出文件夹的情况；
	/*
		举例：
			MyTriOpenMesh ims;
			ims.InterFacePort("123.stl","456.stl",0.6);
	*/

	void InterFacePort();//这个适用使用mesh构造类型，并指定简化量的简化情况；
	/*
		举例：
			MyMesh mesh;
			MyTriOpenMesh ims(mesh,0.6);
			ims.InterFacePort();
			mesh=ims.mesh;
	*/

	void IterartoAll();
	void IterartoAllc();
	void IterartoAll2();

private:
	float m_dest=0;

	OpenMesh::IO::Options opt;

	INT_MAP_TRIMESH m_TriMesh_Map;

	//VECTOR_TRIMESH m_TriMesh_Vector;


	void FillTriMeshMap();

	STRUCT_SET_EDGEPAIR m_TriMesh_Set;

	/*
		主迭代简化过程；
	*/
	int CollapseIterator();
	//int CollapseIterator(int imf,int saw,vector<int>&a,vector<int> &b);

	float ObtuseTri(MyMesh::Point a, MyMesh::Point b, MyMesh::Point c) {
		Vector3f af(a[0], a[1], a[2]);
		Vector3f bf(b[0], b[1], b[2]);
		Vector3f cf(c[0], c[1], c[2]);
		
		if ((bf - af).norm() == 0) {
			cout << "sup" << endl;
		}
		if ((cf - af).norm() == 0) {
			cout << "sup2" << endl;
		}

		return  (bf - af).dot(cf - af);
	}
};
