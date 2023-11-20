#include "pch.h"
#include "Graph.h"
#include "GraphApplicationDlg.h"
#include <set>


// =============================================================================
// CONVEX HULL =================================================================
// =============================================================================

// QuickHull ===================================================================

double PosicioRespeteRecta(CGPoint& a, CGPoint& b, CGPoint& c) {
	return (a.m_Y - b.m_Y) * (c.m_X - b.m_X) - (a.m_X - b.m_X) * (c.m_Y - b.m_Y);
}

double AreaTriangle(CGPoint& a, CGPoint& b, CGPoint c) {
	return abs((a.m_Y - b.m_Y) * (c.m_X - b.m_X) - (a.m_X - b.m_X) * (c.m_Y - b.m_Y)) / 2.0;
}

CConvexHull recursiveHull(CVertex* A, CVertex* B, list<CVertex*> llist, CGraph& graph)
{
	CConvexHull c(&graph);
	c.m_Vertices.push_back(A);
	c.m_Vertices.push_back(B);
	if (llist.size() == 0) { return c; } //Si la llista està buida retornem [A,B]
	double maxC = 0.0;
	CVertex* C = nullptr;

	for (CVertex* v : llist) { //Trobem el punt C que maximitza l'Area
		if (AreaTriangle(A->m_Point, v->m_Point, B->m_Point) > maxC) {
			maxC = AreaTriangle(A->m_Point, v->m_Point, B->m_Point);
			C = v;
		}
	}

	list<CVertex*> left;
	list<CVertex*> right;

	for (CVertex* v : llist) { //For que separa el conjunt de punts en dos subconjunts perr tal d'aplicar l'algorisme de redueix i venceràs
		if (v->m_Name != C->m_Name) {
			if (PosicioRespeteRecta(A->m_Point, C->m_Point, v->m_Point) > 0) {
				left.push_back(v);
			}
			if (PosicioRespeteRecta(C->m_Point, B->m_Point, v->m_Point) > 0) {
				right.push_back(v);
			}
		}
	}

	CConvexHull CHLeft(&graph), CHRight(&graph);
	CHLeft = recursiveHull(A, C, left, graph);
	CHRight = recursiveHull(C, B, right, graph);

	CHRight.m_Vertices.remove(C);
	CHLeft.m_Vertices.insert(CHLeft.m_Vertices.end(), CHRight.m_Vertices.begin(), CHRight.m_Vertices.end());
	return CHLeft;
}


CConvexHull QuickHull(CGraph& graph)
{	//Casos especials
	CConvexHull c(&graph);
	if (graph.m_Vertices.size() == 0) { return CConvexHull(&graph); }
	if (graph.m_Vertices.size() == 1) {
		c.m_Vertices.push_back(&graph.m_Vertices.front());
		return c;
	}
	if (graph.m_Vertices.size() == 2) {
		CGPoint p1, p2;
		p1 = graph.m_Vertices.front().m_Point;
		p2 = graph.m_Vertices.back().m_Point;
		if (p1 == p2) {
			c.m_Vertices.push_back(&graph.m_Vertices.front());
			return c;
		}
		c.m_Vertices.push_back(&graph.m_Vertices.front());
		c.m_Vertices.push_back(&graph.m_Vertices.back());
		return c;
	}

	double minX = numeric_limits<double>::max(), maxX = 0.0;
	CVertex* B = nullptr, *A = nullptr;
	for (CVertex& v : graph.m_Vertices) { //For en el que busquem els vertexs mes externs del graf per tal de començar desde alli la cerca de la hull
		if (v.m_Point.m_X > maxX) {
			maxX = v.m_Point.m_X;
			B = &v;
		}
		if (v.m_Point.m_X < minX) {
			minX = v.m_Point.m_X;
			A = &v;
		}
	}
	
	list<CVertex*> up;
	list<CVertex*> down;

	for (CVertex& v : graph.m_Vertices) { //For que separa el conjunt de punts en dos subconjunts perr tal d'aplicar l'algorisme de redueix i venceràs
		if (PosicioRespeteRecta(A->m_Point, B->m_Point, v.m_Point) > 0) {
			up.push_back(&v);
		}
		else if (PosicioRespeteRecta(A->m_Point, B->m_Point, v.m_Point) < 0){
			down.push_back(&v);
		}
	}

	//Part recursiva en que calculem les convexHull dels subconjunts
	CConvexHull CHUp = recursiveHull(A, B, up, graph);
	CConvexHull CHDown = recursiveHull(B, A, down, graph);

	CHUp.m_Vertices.remove(A);
	CHUp.m_Vertices.remove(B);
	CHDown.m_Vertices.insert(CHDown.m_Vertices.end(), CHUp.m_Vertices.begin(), CHUp.m_Vertices.end());

	return CHDown;
}
