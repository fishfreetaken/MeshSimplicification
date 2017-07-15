#include "stdafx.h"
#include "MyOpenMesh.h"

/*===========================================================================*\
*                                                                           *
*   $Revision$		v1.0                                                    *
*   $Date			2017.6.6												*
*   $Auther			Mr.chen												*
*                                                                           *
\*===========================================================================*/

MyOpenMesh::MyOpenMesh()
{
}

MyOpenMesh::~MyOpenMesh()
{
}

void MyOpenMesh::Readfile(const char * argg)
{
	// read mesh from stdin
	if (!OpenMesh::IO::read_mesh(mesh, argg, opt))
	{
		std::cerr << "Error: Cannot read mesh from " << std::endl;
		return;
	}
	//vertex_normals init!
	mesh.request_vertex_normals();

	// assure we have vertex normals
	if (!mesh.has_vertex_normals())
	{
		std::cerr << "ERROR: Standard vertex property 'Normals' not available!\n";
		return;
	}
	//if (!opt.check(OpenMesh::IO::Options::VertexNormal))
	//{
	//	cout << "Init Surface normal" << endl;
		//// we need face normals to update the vertex normals
		//mesh.request_face_normals();
		//// let the mesh update the normals
		//mesh.update_normals();
		//// dispose the face normals, as we don't need them anymore
		////mesh.release_face_normals();
	//}
	 //init for mesh collapse
	
	mesh.request_face_normals();
	// let the mesh update the normals
	mesh.update_normals();

	mesh.request_vertex_status();
	mesh.request_edge_status();
	mesh.request_face_status();

}

void MyOpenMesh::Writefile(const char *argg, int i) {
	if (!OpenMesh::IO::write_mesh(mesh, argg, i)) //0 ascii 1 binary
	{
		std::cerr << "Error: cannot write mesh to " << argg << std::endl;
	}
}

//-----------------------------------------------------------------------------

int MyOpenMesh::MeshSimplification(float dest)
{
	if ((dest >= 1) || (dest <= 0)) {
		cout << "Your input is out of range, MainLoop" << endl;
		return 1;
	}
	int idest = mesh.n_vertices()*dest;//有个转换

	FillVertexPointMap();
	InitFillEdgePair();

	printf("顶点数目统计 : 简化前:%d  目标:%d  差值:%d\n", mesh.n_vertices(), mesh.n_vertices() - idest, idest);

	//int temp =0;
	while (idest--) {
		MeshCollapseIterate3();
	}
	PointEdgeRelease();
	return 0;
}

void MyOpenMesh::FillVertexPointMap() 
{
	for (MyMesh::VertexIter v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it)
	{
		m_VertexPoint_Map[v_it->idx()] = new VertexPoint(mesh.vertex_handle(v_it->idx()),mesh);
		/*if (vp->QMSurfaceCoe()) {
			continue;
		}*/
		//m_VertexPoint_Map[v_it->idx()]->QMSurfaceCoe();
		m_VertexPoint_Map[v_it->idx()]->UpdateVertexQ();
	}
}

void MyOpenMesh::InitFillEdgePair()
{
	for (MyMesh::EdgeIter e_it = mesh.edges_begin(); e_it != mesh.edges_end(); ++e_it)
	{
		m_EdgePair_Map[e_it->idx()] = new EdgePair(mesh.edge_handle(e_it->idx()),mesh);

		m_EdgePair_Map[e_it->idx()]->UpdatePair(m_VertexPoint_Map);

		m_EdgePair_Set.insert(m_EdgePair_Map[e_it->idx()]->ReturnEdgeSet());
	}
}

MyMesh::EdgeHandle MyOpenMesh::FindMiniestEdge()
{
	INT_MAP_EDGEPAIR::iterator it(m_EdgePair_Map.begin());

	double xmin = it->second->ReturnQX(); double xtemp;
	MyMesh::EdgeHandle ehmin= it->second->ReturnEdge();
	for (it++; it != m_EdgePair_Map.end(); it++) {
		xtemp = it->second->ReturnQX();
		if (xmin > xtemp) {
			xmin = xtemp;
			ehmin = it->second->ReturnEdge();
		}
	}
	return ehmin;
}

int MyOpenMesh::MeshCollapseIterate() //主循环，迭代去除最小值
{//一次迭代删除一个点，但是有可能又多条边删除；
	MyMesh::EdgeHandle e = FindMiniestEdge();
	/*if (e.idx() == -1) {
		if (m_EdgePair_Map[e.idx()] == NULL) {
			cout << "NULL" << endl;
		}
		delete m_EdgePair_Map[e.idx()];
		m_EdgePair_Map.erase(e.idx());
		return 3;
	}*/
	MyMesh::VertexHandle emb = m_EdgePair_Map[e.idx()]->ReturnMb();
	MyMesh::VertexHandle ema = m_EdgePair_Map[e.idx()]->ReturnMa();

	//vector<MyMesh::VertexHandle>		vmv;
	map<int, int> s_medge;

#ifdef OUTDEBUG
	cout << "This edge id:" << e.idx() << " for Delete;" << endl;
#endif // OUTDEBUG

	for (MyMesh::VertexVertexIter vv_iter = mesh.vv_begin(emb); vv_iter.is_valid(); ++vv_iter)
	{
		MyMesh::VertexHandle vtemp = mesh.vertex_handle(vv_iter->idx());//遍历b点周围的顶点；
		//vmv.push_back(vtemp);
		if (vtemp == ema) {
			continue;
		}

		for (MyMesh::VertexOHalfedgeIter voh_iter = mesh.voh_begin(vtemp); voh_iter.is_valid(); ++voh_iter)
		{
			MyMesh::HalfedgeHandle htemp = mesh.halfedge_handle(voh_iter->idx());
			MyMesh::VertexHandle vbtemp = mesh.to_vertex_handle(*voh_iter);
			if ((vbtemp == emb)||(vbtemp==ema)) {
				continue;
			}
			MyMesh::EdgeHandle  etemp = mesh.edge_handle(htemp);
			s_medge[etemp.idx()] = 0;// vtemp.idx();
		}
	}
	//cout << count << endl;
	map<int, int>::iterator set_it;
	int count1 = 0,count2=0;
	//for (MyMesh::VertexVertexIter vv_iter = mesh.vv_begin(ema); vv_iter.is_valid(); ++vv_iter)
	//{
	//	MyMesh::VertexHandle vtemp = mesh.vertex_handle(vv_iter->idx());//遍历a点周围的顶点；

		for (MyMesh::VertexOHalfedgeIter voh_iter = mesh.voh_begin(ema); voh_iter.is_valid(); ++voh_iter)
		{ //196 :1
			MyMesh::HalfedgeHandle htemp = mesh.halfedge_handle(voh_iter->idx());
			MyMesh::EdgeHandle  etemp = mesh.edge_handle(htemp);
			count1++;
			s_medge[etemp.idx()] = -1;//vtemp.idx();
		}
		for (MyMesh::VertexOHalfedgeIter voh_iter = mesh.voh_begin(emb); voh_iter.is_valid(); ++voh_iter)
		{
			MyMesh::HalfedgeHandle htemp = mesh.halfedge_handle(voh_iter->idx());
			MyMesh::EdgeHandle  etemp = mesh.edge_handle(htemp);
			count2++;
			s_medge[etemp.idx()] = -1;//vtemp.idx();
		}
	//}
		//if (s_medge.size()<=2)
		if((count1<2)||(count2<2))
		{
			//cout << "Edge after collapse is zero!" << endl;
			m_EdgePair_Map[e.idx()]->PairCollapse();
			delete m_EdgePair_Map[e.idx()];
			m_EdgePair_Map.erase(e.idx());
			return 1;// mesh.n_vertices();
		}

	//先collapse，然后再看看边的另一个是不是符合原来的点；

		if (m_EdgePair_Map[e.idx()]->PairCollapse())
		{
			cout << "Collapse error, Remove the top!" << endl;
		}
	
	for (MyMesh::VertexOHalfedgeIter voh_iter = mesh.voh_begin(ema); voh_iter.is_valid(); ++voh_iter)
	{
		MyMesh::HalfedgeHandle htemp = mesh.halfedge_handle(voh_iter->idx());
		MyMesh::EdgeHandle  etemp = mesh.edge_handle(htemp);
		//count++;
		s_medge[etemp.idx()] = 0;//vtemp.idx();
	}

	//delete m_VertexPoint_Map[emb.idx()];	//一次迭代只删除一个点
	//delete m_EdgePair_Map[e.idx()];
	//m_VertexPoint_Map.erase(emb.idx());	//当前collapse点删除
	//m_EdgePair_Map.erase(e.idx());		//当前边删除；
	//cout << "Delete 2:" << e.idx() << endl;

	//for (int i = 0; i < vmv.size(); i++) {
	//	m_VertexPoint_Map[vmv[i].idx()]->UpdateVertexQ(); //更新B点周围的每个点的Q
	//}
	m_VertexPoint_Map[ema.idx()]->UpdateVertexQ();
	for (MyMesh::VertexVertexIter vv_iter = mesh.vv_begin(ema); vv_iter.is_valid(); ++vv_iter)
	{
		m_VertexPoint_Map[vv_iter->idx()]->UpdateVertexQ();//更新A点周围的每个点Q值
	}

	for (set_it = s_medge.begin(); set_it != s_medge.end(); set_it++) {
		if (set_it->second == -1) {

#ifdef OUTDEBUG
	cout << "Delete :" << set_it->first << endl;
#endif // OUTDEBUG

			delete m_EdgePair_Map[set_it->first];
			m_EdgePair_Map.erase(set_it->first);
			continue;
		}

		if (m_EdgePair_Map[set_it->first] == NULL) {
#ifdef OUTDEBUG
			cout << "NULL :" << set_it->first << endl;
#endif // OUTDEBUG
			m_EdgePair_Map.erase(set_it->first);
			continue;
		}
		m_EdgePair_Map[set_it->first]->UpdatePair(m_VertexPoint_Map); //更新每个变的最优解
	}

	//cout << "````````````````````````````````" << endl;

	return 2;// mesh.n_vertices();
}

void MyOpenMesh::PointArround(MyMesh::VertexHandle m_vh) {
	cout << "m_vh:" << m_vh.idx() << endl;
	
	for (MyMesh::VertexOHalfedgeIter  voh_iter = mesh.voh_begin(m_vh); voh_iter.is_valid(); ++voh_iter) {
		cout << "the OH NO. is : " << voh_iter->idx() << endl;
		cout << "the OH NEXT NO. is: " << mesh.next_halfedge_handle(mesh.halfedge_handle(voh_iter->idx())) << endl;;

	}
	for (MyMesh::VertexIHalfedgeIter  vih_iter = mesh.vih_begin(m_vh); vih_iter.is_valid(); ++vih_iter) {
		cout << "the IH NO. is : " << vih_iter->idx() << endl;
		cout << "the OH NEXT NO. is: " << mesh.next_halfedge_handle(mesh.halfedge_handle(vih_iter->idx())) << endl;;
	}
	for (MyMesh::VertexVertexIter  vv_iter = mesh.vv_begin(m_vh); vv_iter.is_valid(); ++vv_iter) {
		cout << "the VV NO. is : " << vv_iter->idx() << endl;
	}
	for (MyMesh::VertexEdgeIter ve_iter = mesh.ve_begin(m_vh); ve_iter.is_valid(); ++ve_iter) {
		cout << "the edge NO. is : " << ve_iter->idx() << endl;
	}
}

int MyOpenMesh::MeshCollapseIterate(float dest) //主循环，迭代去除最小值
{//一次迭代删除一个点，但是有可能又多条边删除；
	int agdest = (int)dest;
	MyMesh::EdgeHandle e = mesh.edge_handle(agdest);
	MyMesh::VertexHandle emb = m_EdgePair_Map[e.idx()]->ReturnMb();
	MyMesh::VertexHandle ema = m_EdgePair_Map[e.idx()]->ReturnMa();

	//vector<MyMesh::VertexHandle>		vmv;
	map<int, int> s_medge;
	cout << "This edge id:" << e.idx() << " for Delete;" << endl;

	for (MyMesh::VertexVertexIter vv_iter = mesh.vv_begin(emb); vv_iter.is_valid(); ++vv_iter)
	{
		MyMesh::VertexHandle vtemp = mesh.vertex_handle(vv_iter->idx());//遍历b点周围的顶点；
		//vmv.push_back(vtemp);
		if (vtemp == ema) {
			continue;
		}

		for (MyMesh::VertexOHalfedgeIter voh_iter = mesh.voh_begin(vtemp); voh_iter.is_valid(); ++voh_iter)
		{
			MyMesh::HalfedgeHandle htemp = mesh.halfedge_handle(voh_iter->idx());
			MyMesh::VertexHandle vbtemp = mesh.to_vertex_handle(*voh_iter);
			if ((vbtemp == emb) || (vbtemp == ema)) {
				continue;
			}
			MyMesh::EdgeHandle  etemp = mesh.edge_handle(htemp);
			s_medge[etemp.idx()] = 0;// vtemp.idx();
		}
	}
	map<int, int>::iterator set_it;
	int count = 0;

	for (MyMesh::VertexOHalfedgeIter voh_iter = mesh.voh_begin(ema); voh_iter.is_valid(); ++voh_iter)
	{
		MyMesh::HalfedgeHandle htemp = mesh.halfedge_handle(voh_iter->idx());
		MyMesh::EdgeHandle  etemp = mesh.edge_handle(htemp);
		count++;
		s_medge[etemp.idx()] = -1;//vtemp.idx();
	}
	for (MyMesh::VertexOHalfedgeIter voh_iter = mesh.voh_begin(emb); voh_iter.is_valid(); ++voh_iter)
	{
		MyMesh::HalfedgeHandle htemp = mesh.halfedge_handle(voh_iter->idx());
		MyMesh::EdgeHandle  etemp = mesh.edge_handle(htemp);
		count++;
		s_medge[etemp.idx()] = -1;//vtemp.idx();
	}

	if (s_medge.size() <= 2)
	{
		cout << "Edge after collapse is zero!" << endl;
		m_EdgePair_Map[e.idx()]->PairCollapse();
		delete m_EdgePair_Map[e.idx()];
		m_EdgePair_Map.erase(e.idx());
		return mesh.n_vertices();
	}

	//先collapse，然后再看看边的另一个是不是符合原来的点；
	if (m_EdgePair_Map[e.idx()]->PairCollapse())
	{
		cout << "Collapse error, Remove the top!" << endl;
	}

	for (MyMesh::VertexOHalfedgeIter voh_iter = mesh.voh_begin(ema); voh_iter.is_valid(); ++voh_iter)
	{
		MyMesh::HalfedgeHandle htemp = mesh.halfedge_handle(voh_iter->idx());
		MyMesh::EdgeHandle  etemp = mesh.edge_handle(htemp);
		count++;
		s_medge[etemp.idx()] = 0;//vtemp.idx();
	}

	m_VertexPoint_Map[ema.idx()]->UpdateVertexQ();
	for (MyMesh::VertexVertexIter vv_iter = mesh.vv_begin(ema); vv_iter.is_valid(); ++vv_iter)
	{
		m_VertexPoint_Map[vv_iter->idx()]->UpdateVertexQ();//更新A点周围的每个点Q值
	}

	for (set_it = s_medge.begin(); set_it != s_medge.end(); set_it++) {
		if (set_it->second == -1) {
			cout << "Delete :" << set_it->first << endl;
			delete m_EdgePair_Map[set_it->first];
			m_EdgePair_Map.erase(set_it->first);
			continue;
		}
		if (m_EdgePair_Map[set_it->first] == NULL) {
			cout << "NULL :" << set_it->first << endl;
			m_EdgePair_Map.erase(set_it->first);
			continue;
		}
		m_EdgePair_Map[set_it->first]->UpdatePair(m_VertexPoint_Map); //更新每个变的最优解
	}

	return mesh.n_vertices();
}

int MyOpenMesh::MeshCollapseIterate2() //主循环，迭代去除最小值
{//一次迭代删除一个点，但是有可能又多条边删除；
	
	MyMesh::EdgeHandle e = mesh.edge_handle(m_EdgePair_Set.begin()->i); //最小点索引

	MyMesh::VertexHandle emb = m_EdgePair_Map[e.idx()]->ReturnMb();
	MyMesh::VertexHandle ema = m_EdgePair_Map[e.idx()]->ReturnMa();

	map<int, int> s_medge;

#ifdef OUTDEBUG2
	cout << "This edge id:" << e.idx() << " for Delete;" << endl;
#endif // OUTDEBUG

	for (MyMesh::VertexVertexIter vv_iter = mesh.vv_begin(emb); vv_iter.is_valid(); ++vv_iter)
	{
		MyMesh::VertexHandle vtemp = mesh.vertex_handle(vv_iter->idx());//遍历b点周围的顶点；
		if (vtemp == ema) {
			continue;
		}
		for (MyMesh::VertexOHalfedgeIter voh_iter = mesh.voh_begin(vtemp); voh_iter.is_valid(); ++voh_iter)
		{
			MyMesh::HalfedgeHandle htemp = mesh.halfedge_handle(voh_iter->idx());
			MyMesh::VertexHandle vbtemp = mesh.to_vertex_handle(*voh_iter);
			if ((vbtemp == emb) || (vbtemp == ema)) {
				continue;
			}
			MyMesh::EdgeHandle  etemp = mesh.edge_handle(htemp);
			s_medge[etemp.idx()] = 0;// vtemp.idx();
		}
	}
	//cout << count << endl;
	map<int, int>::iterator set_it;
	int count1 = 0, count2 = 0;

	for (MyMesh::VertexOHalfedgeIter voh_iter = mesh.voh_begin(ema); voh_iter.is_valid(); ++voh_iter)
	{ //196 :1
		MyMesh::HalfedgeHandle htemp = mesh.halfedge_handle(voh_iter->idx());
		MyMesh::EdgeHandle  etemp = mesh.edge_handle(htemp);
		count1++;
		s_medge[etemp.idx()] = -1;//vtemp.idx();
	}
	for (MyMesh::VertexOHalfedgeIter voh_iter = mesh.voh_begin(emb); voh_iter.is_valid(); ++voh_iter)
	{
		MyMesh::HalfedgeHandle htemp = mesh.halfedge_handle(voh_iter->idx());
		MyMesh::EdgeHandle  etemp = mesh.edge_handle(htemp);
		count2++;
		s_medge[etemp.idx()] = -1;//vtemp.idx();
	}

	if ((count1<2) || (count2<2))
	{
		//cout << "Edge after collapse is zero!" << endl;
		m_EdgePair_Map[e.idx()]->PairCollapse();
		m_EdgePair_Set.erase(m_EdgePair_Map[e.idx()]->ReturnEdgeSet());

		delete m_EdgePair_Map[e.idx()];
		m_EdgePair_Map.erase(e.idx());
		return 1;// mesh.n_vertices();
	}

	//先collapse，然后再看看边的另一个是不是符合原来的点；

	if (m_EdgePair_Map[e.idx()]->PairCollapse())
	{
		cout << "Collapse error, Remove the top!" << endl;
	}

	for (MyMesh::VertexOHalfedgeIter voh_iter = mesh.voh_begin(ema); voh_iter.is_valid(); ++voh_iter)
	{
		MyMesh::HalfedgeHandle htemp = mesh.halfedge_handle(voh_iter->idx());
		MyMesh::EdgeHandle  etemp = mesh.edge_handle(htemp);
		//count++;
		s_medge[etemp.idx()] = 0;//vtemp.idx();
	}

	//delete m_EdgePair_Map[e.idx()];
	//m_EdgePair_Map.erase(e.idx());		//当前边删除；
	//cout << "Delete 2:" << e.idx() << endl;

	m_VertexPoint_Map[ema.idx()]->UpdateVertexQ();
	for (MyMesh::VertexVertexIter vv_iter = mesh.vv_begin(ema); vv_iter.is_valid(); ++vv_iter)
	{
		m_VertexPoint_Map[vv_iter->idx()]->UpdateVertexQ();//更新A点周围的每个点Q值
	}

	for (set_it = s_medge.begin(); set_it != s_medge.end(); set_it++) {
		if (m_EdgePair_Map[set_it->first] == NULL) {
#ifdef OUTDEBUG2
			cout << "NULL :" << set_it->first << endl;
#endif // OUTDEBUG
			m_EdgePair_Map.erase(set_it->first);
			continue;
		}
		m_EdgePair_Set.erase(m_EdgePair_Map[set_it->first]->ReturnEdgeSet());

		if (set_it->second == -1) {
#ifdef OUTDEBUG2
			cout << "Delete :" << set_it->first << endl;
#endif // OUTDEBUG
			delete m_EdgePair_Map[set_it->first];
			m_EdgePair_Map.erase(set_it->first);
			continue;
		}

		m_EdgePair_Map[set_it->first]->UpdatePair(m_VertexPoint_Map); //更新每个变的最优解
		m_EdgePair_Set.insert(m_EdgePair_Map[set_it->first]->ReturnEdgeSet());
	}

	return 2;// mesh.n_vertices();
}

int MyOpenMesh::MeshCollapseIterate3() //主循环，迭代去除最小值
{//一次迭代删除一个点，但是有可能又多条边删除；
	
	int edge_int = m_EdgePair_Set.begin()->i;

	MyMesh::VertexHandle emb = m_EdgePair_Map[edge_int]->ReturnMb();
	MyMesh::VertexHandle ema = m_EdgePair_Map[edge_int]->ReturnMa();

#ifdef OUTDEBUG2
	cout << "This edge id:" << e.idx() << " for Delete;" << endl;
#endif // OUTDEBUG

	map<int, int> s_medge;
	int count1 = 0, count2 = 0;
	for (MyMesh::VertexOHalfedgeIter voh_iter = mesh.voh_begin(ema); voh_iter.is_valid(); ++voh_iter)
	{ //196 :1
		MyMesh::HalfedgeHandle htemp = mesh.halfedge_handle(voh_iter->idx());
		MyMesh::EdgeHandle  etemp = mesh.edge_handle(htemp);
		count1++;
		s_medge[etemp.idx()] = 0;//vtemp.idx();
	}
	if (count1<2)
	{
		//cout << "Edge after collapse is zero!" << endl;
		m_EdgePair_Map[edge_int]->PairCollapse();
		m_EdgePair_Set.erase(m_EdgePair_Map[edge_int]->ReturnEdgeSet());

		//delete m_EdgePair_Map[e.idx()];
		//m_EdgePair_Map.erase(e.idx());
		return 1;// mesh.n_vertices();
	}

	for (MyMesh::VertexVertexIter vv_iter = mesh.vv_begin(emb); vv_iter.is_valid(); ++vv_iter)
	{
		count2++;
		MyMesh::VertexHandle vtemp = mesh.vertex_handle(vv_iter->idx());//遍历b点周围的顶点；
		if (vtemp == ema) {
			continue;
		}
		for (MyMesh::VertexOHalfedgeIter voh_iter = mesh.voh_begin(vtemp); voh_iter.is_valid(); ++voh_iter)
		{
			MyMesh::HalfedgeHandle htemp = mesh.halfedge_handle(voh_iter->idx());
			MyMesh::EdgeHandle  etemp = mesh.edge_handle(htemp);
			s_medge[etemp.idx()] = 0;// vtemp.idx();
		}
	}

	if (count2<2)
	{
		//cout << "Edge after collapse is zero!" << endl;
		m_EdgePair_Map[edge_int]->PairCollapse();
		m_EdgePair_Set.erase(m_EdgePair_Map[edge_int]->ReturnEdgeSet());

		//delete m_EdgePair_Map[e.idx()];
		//m_EdgePair_Map.erase(e.idx());
		return 2;// mesh.n_vertices();
	}

	map<int, int>::iterator set_it;

	//vector<MyMesh::EdgeHandle> v_del_half;
	vector<int> v_del_half;
	v_del_half.push_back(edge_int);
	MyMesh::HalfedgeHandle s_half = m_EdgePair_Map[edge_int]->ReturnHalfEdge();
	MyMesh::HalfedgeHandle o_half = mesh.opposite_halfedge_handle(s_half);
	MyMesh::HalfedgeHandle s_del_half = mesh.next_halfedge_handle(mesh.next_halfedge_handle(s_half));
	MyMesh::HalfedgeHandle o_del_half = mesh.next_halfedge_handle(o_half);
	if (s_half == mesh.next_halfedge_handle(s_del_half)) {
		v_del_half.push_back(mesh.edge_handle(s_del_half).idx());
	}
	if (o_half == mesh.next_halfedge_handle(mesh.next_halfedge_handle(o_del_half))) {
		v_del_half.push_back(mesh.edge_handle(o_del_half).idx());
	}

	//先collapse，然后再看看边的另一个是不是符合原来的点；

	if (m_EdgePair_Map[edge_int]->PairCollapse())
	{
		cout << "Collapse error, Remove the top!" << endl;
	}

	m_VertexPoint_Map[ema.idx()]->UpdateVertexQ();
	for (MyMesh::VertexVertexIter vv_iter = mesh.vv_begin(ema); vv_iter.is_valid(); ++vv_iter)
	{
		m_VertexPoint_Map[vv_iter->idx()]->UpdateVertexQ();//更新A点周围的每个点Q值
	}

	for (auto i : v_del_half) {
		m_EdgePair_Set.erase(m_EdgePair_Map[i]->ReturnEdgeSet());

#ifdef OUTDEBUG2
			cout << "Delete :" << i.idx() << endl;
#endif // OUTDEBUG
		//delete m_EdgePair_Map[i.idx()];
		//m_EdgePair_Map.erase(i.idx());
		s_medge.erase(i);
	}

	for (set_it = s_medge.begin(); set_it != s_medge.end(); set_it++) {
		if (m_EdgePair_Map[set_it->first] == NULL) {
#ifdef OUTDEBUG2
			cout << "NULL :" << set_it->first << endl;
#endif // OUTDEBUG
			m_EdgePair_Map.erase(set_it->first);
			continue;
		}
		m_EdgePair_Set.erase(m_EdgePair_Map[set_it->first]->ReturnEdgeSet());

		m_EdgePair_Map[set_it->first]->UpdatePair(m_VertexPoint_Map); //更新每个变的最优解
		m_EdgePair_Set.insert(m_EdgePair_Map[set_it->first]->ReturnEdgeSet());
	}
	return 3;// mesh.n_vertices();
}