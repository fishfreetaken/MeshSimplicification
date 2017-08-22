#include "stdafx.h"
#include "BilateralFilter.h"


BilateralFilter::BilateralFilter(MyMesh&a)
	:mesh(a)
{
}


BilateralFilter::~BilateralFilter()
{
}


void BilateralFilter::getFaceCentroid(std::vector<MyMesh::Point> &centroid)
{
	centroid.clear();
	centroid.resize(mesh.n_faces(), MyMesh::Point(0.0, 0.0, 0.0));

	for (MyMesh::FaceIter f_it = mesh.faces_begin(); f_it != mesh.faces_end(); f_it++)
	{
		MyMesh::Point pt = mesh.calc_face_centroid(*f_it);
		centroid[(*f_it).idx()] = pt;
	}
}

void BilateralFilter::getRadiusAverageNormal(vector<MyMesh::Normal>&average_normals)
{
	average_normals.clear();
	average_normals.resize(mesh.n_faces(), MyMesh::Normal(0.0, 0.0, 0.0));

	for (MyMesh::VertexIter it=mesh.vertices_begin();it!=mesh.vertices_end();it++)
	{
		MyMesh::Normal tempNormal(0, 0, 0);
		float i = 0.0;
		for (MyMesh::VertexFaceIter vf_iter=mesh.vf_begin(*it);vf_iter.is_valid();vf_iter++)
		{
			tempNormal += mesh.normal(*vf_iter);
			i += 1;
		}
		average_normals[it->idx()] = tempNormal / i;
		average_normals[it->idx()].normalize();
	}
}

void BilateralFilter::updateVertexPosition(bool fixed_boundary, int iteration_number)
{
	cout << "平均法向的点的移动" << endl;
	vector<MyMesh::Normal> filtered_normals;
	vector<MyMesh::Point> face_centroid;

	vector<MyMesh::Point> new_points(mesh.n_vertices());

	for (int i = 0; i < iteration_number; i++)
	{
		cout <<"update:"<< i << endl;
		getRadiusAverageNormal(filtered_normals);
		getFaceCentroid(face_centroid);
		for (MyMesh::VertexIter it = mesh.vertices_begin(); it != mesh.vertices_end(); it++)
		{
			MyMesh::Point p = mesh.point(*it);
			MyMesh::VertexHandle vh = mesh.vertex_handle(it->idx());
			/*if (fixed_boundary && mesh.is_boundary(vh))
			{
				continue;
			}
			else
			{*/
				float face_num = 0.0;
				MyMesh::Point temp_point(0.0, 0.0, 0.0);

				for (MyMesh::VertexFaceIter vf_it = mesh.vf_iter(vh); vf_it.is_valid(); vf_it++)
				{
					MyMesh::Normal temp_normal = filtered_normals[vf_it->idx()];
					MyMesh::Point temp_centroid = face_centroid[vf_it->idx()];
					temp_point += temp_normal * (temp_normal | (temp_centroid - p));
					face_num++;
				}
				p += temp_point / face_num;
				new_points[it->idx()] = p;
			//}
		}
		for (int s=0;s<new_points.size();s++)
		{
			mesh.set_point(mesh.vertex_handle(s), new_points[s]);
		}
		
		mesh.update_normals();
	}
}

void BilateralFilter::updateBilateralFilter()
{
	updateBilateralFilter(1, 2, 0.8);

	updateBilateralFilter(1, 3, 1.3);
}

void BilateralFilter::updateBilateralFilter(int iteration_number,float radius,double sigma)
{
	mesh.update_normals(); 

	int row = mesh.n_vertices();
	int nn = 3;

	flann::Matrix<float>dataset(new float[row*nn], row, nn);
	MyMesh::Point p;

	for (MyMesh::VertexIter vit = mesh.vertices_begin(); vit != mesh.vertices_end(); vit++)
	{
		p = mesh.point(*vit);
		dataset[vit->idx()][0] = p[0];
		dataset[vit->idx()][1] = p[1];
		dataset[vit->idx()][2] = p[2];
	}
	
	flann::Index< L2<float> > flann_index(dataset, flann::KDTreeSingleIndexParams(10));
	flann_index.buildIndex();

	vector<vector<int>>indices;
	vector<vector<float>>dists;

	flann_index.radiusSearch(dataset, indices, dists, radius, flann::SearchParams(128)); //搜索该半径范围以内所有的点的索引；可以直接使用在mesh上；

	cout << "build flann for Bilateral Filter" << endl;
	double  maxf = 0;
	double minf = 9999;

	for (int i = 0; i < iteration_number; i++)
	{
		cout << "update: " << i << endl;
		vector<MyMesh::Point> vpm;

		for (int s = 0; s < row; s++)
		{
			vpm.push_back(radiusNormalVarience(s, indices[s], maxf, minf,sigma));
		}

		for (int s = 0; s < row; s++)
		{
			mesh.set_point(mesh.vertex_handle(s), vpm[s]);
		}
	}
	cout << "varence max : " << maxf << " min: " << minf << endl;
}

MyMesh::Point BilateralFilter::radiusNormalVarience(int sourceint,vector<int> radiuspoint,double &max,double &min,double sigma)
{
	MyMesh::Normal averageNormal(0,0,0);
	int size = radiuspoint.size();
	if (!size)
	{
		cout << "zero error!" << endl;
		return MyMesh::Point(0, 0, 0);
	}

	//double sigma = sigma; //init 0.8

	vector<MyMesh::Normal> mn;
	vector<MyMesh::Point> vp;
	for (int i = 0; i < size; i++)
	{
		MyMesh::VertexHandle vh = mesh.vertex_handle(radiuspoint[i]);
		vp.push_back(mesh.point(vh));
		averageNormal += mesh.normal(vh);
		mn.push_back(mesh.normal(vh));
	}

	averageNormal = averageNormal / size;

	averageNormal.normalize();

	double varience = 0;
	for (auto i : mn)
	{
		MyMesh::Normal n= (i - averageNormal);
		varience += (n | n);
	}

	varience = varience / size; //varience 0~4;

	//for tiaoshi
	if (varience > max)
	{
		max = varience;
	}
	if (varience < min) {
		min = varience;
	}

	double k = varience / (varience + BILATERAFILETERPARA); //这个参数还是需要看方差的范围的

	MyMesh::Point gausspoint;
	gausspoint = GaussionArroundVertex(vp, sigma);

	MyMesh::Point sourcepoint = mesh.point(mesh.vertex_handle(sourceint));

	return ((1 - k)*gausspoint + k*sourcepoint);

}

MyMesh::Point BilateralFilter::GaussionArroundVertex(vector<MyMesh::Point>& pGray, float sigma) {
	int sz = pGray.size();
	float dWeightSum = 0;//滤波系数总和  

	MyMesh::Point dDotMul(0, 0, 0);//高斯系数与图像数据的点乘  
	float dDis, dValue;

	for (int i = 0; i<sz; i++)
	{
		dDis = (pGray[0] - pGray[i]).norm();//距离
		dValue = exp(-(1 / 2)*dDis*dDis / (sigma*sigma)) / (sqrt(2 * 3.1415926)*sigma);

		dDotMul += dValue*pGray[i];//叠加的点坐标
		dWeightSum += dValue;
	}
	return dDotMul / dWeightSum;
}