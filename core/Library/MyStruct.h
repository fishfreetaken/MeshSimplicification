#pragma once

#define SOLUTIONACCURANCY  0.0000000001  //小数点后十位,用来判断是否解出来方程；
//专门用来定义struct结构
struct MESHSETEDGE {
	int i;
	double a;
	bool operator < (const struct MESHSETEDGE &m)const {
		if (a == m.a) {
			return i < m.i; //肯定是按照从小到大的顺序排列的，如有需要，相反
		}
		else {
			return a < m.a;
		}
	}
};
typedef set<MESHSETEDGE>     STRUCT_SET_EDGEPAIR;

typedef map<int, int>   INT_INT_MAP;
//
//struct agw {
//	int a;
//	float b;
//	bool operator < (const struct agw &m)const {
//		if (b == m.b) {
//			return a < m.a;
//		}
//		else {
//			return b < m.b;
//		}
//	}
//};
//set<struct agw> ct;
//set<struct agw>::iterator it;
//agw fw1{ 5,2.63 };
//agw fw2{ 3,2.63 };
//agw fw3{ 6,2.63 };
//agw fw4{ 7,1.63 };
//agw fw5{ 3,1.63 };
//agw fw6{ 3,1.63 };
//ct.insert(fw1);
//ct.insert(fw2);
//ct.insert(fw3);
//ct.insert(fw4);
//ct.insert(fw5);
//ct.insert(fw6);
//ct.erase(fw4);
//
//vector<set<struct agw>::iterator> vsi;
//for (it = ct.begin(); it != ct.end(); it++) {
//	cout << it->a << " " << it->b << endl;
//	vsi.push_back(it);
//}
//
//for (auto i : vsi) {
//	//cout << i->a << " " << i->b << endl;
//}
