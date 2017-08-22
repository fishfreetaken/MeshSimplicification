#pragma once
#include "IncludeHeader.h"

#define SEARCHRADIUS				3 //mm �������������Χ�ڽ��е�����

#define BILATERAFILETERPARA			0.05 //�����˫���˲������㷽��ʱ�򣬹���k��һ���̶������������ֶ����ţ���ϵ����Ե��ѡ��

class BilateralFilter
{
public:
	BilateralFilter(MyMesh&a);
	~BilateralFilter();

	/*
		�˲�
	*/
	void					updateVertexPosition(bool fixed_boundary, int iteration_number); //����ƽ�������˲���,���������У��������飬���߰��㣻
	void					updateBilateralFilter(int i, float radius, double sigma);
	void					updateBilateralFilter(); //�����˲�

private:
	MyMesh&						mesh;

	/*
		mesh ����
	*/
	void					getFaceCentroid(std::vector<MyMesh::Point> &centroid);
	void					getRadiusAverageNormal(vector<MyMesh::Normal>&average_normals); //vertex face

	MyMesh::Point			radiusNormalVarience(int sourcepoint,vector<int> vit,double &max, double &min,double sima); //��Ĭ�ϵķ�Χ�ڼ��㷨��ķ���

	MyMesh::Point			GaussionArroundVertex(vector<MyMesh::Point>& pGray, float sigma); //��˹�˲�����

	/*
		�˲�
	*/
	//void					updateBilateralFilter();

};

