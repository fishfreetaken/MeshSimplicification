#pragma once
#include "IncludeHeader.h"

#define SEARCHRADIUS				3 //mm 这个是在搜索范围内进行调整；

#define BILATERAFILETERPARA			0.05 //这个是双边滤波器计算方差时候，关于k的一个固定常量参数；手动调优；关系到边缘的选择；

class BilateralFilter
{
public:
	BilateralFilter(MyMesh&a);
	~BilateralFilter();

	/*
		滤波
	*/
	void					updateVertexPosition(bool fixed_boundary, int iteration_number); //沿着平均法向滤波；,试验结果不行，密密麻麻，乱七八糟；
	void					updateBilateralFilter(int i, float radius, double sigma);
	void					updateBilateralFilter(); //两向滤波

private:
	MyMesh&						mesh;

	/*
		mesh 工具
	*/
	void					getFaceCentroid(std::vector<MyMesh::Point> &centroid);
	void					getRadiusAverageNormal(vector<MyMesh::Normal>&average_normals); //vertex face

	MyMesh::Point			radiusNormalVarience(int sourcepoint,vector<int> vit,double &max, double &min,double sima); //在默认的范围内计算法向的方差

	MyMesh::Point			GaussionArroundVertex(vector<MyMesh::Point>& pGray, float sigma); //高斯滤波器；

	/*
		滤波
	*/
	//void					updateBilateralFilter();

};

