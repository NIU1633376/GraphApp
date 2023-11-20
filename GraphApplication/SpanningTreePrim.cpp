#include "pch.h"
#include "Graph.h"
#include <vector>
#include <queue>
using namespace std;

// =============================================================================
// SpanningTreePrim ============================================================
// =============================================================================


CSpanningTree SpanningTreePrim(CGraph& graph)
{
	if (graph.m_Edges.empty()) {
		return  CSpanningTree(&graph);
	}
	CSpanningTree t(&graph);
	struct comparator {
		bool operator()(CEdge* pE1, CEdge* pE2) {
			return pE1->m_Length > pE2->m_Length;
		}
	};

	priority_queue<CEdge*, vector<CEdge*>, comparator> p_queue;

	for (CVertex& v : graph.m_Vertices) { v.m_PrimInTree = false; }

	graph.m_Vertices.front().m_PrimInTree = true;

	for (auto it = t.m_pGraph->m_Vertices.front().m_Edges.begin(); it != t.m_pGraph->m_Vertices.front().m_Edges.end(); ++it) {
		p_queue.push(*it);
	}
	int nVert = 1;
		
	while (!p_queue.empty() && nVert < graph.m_Vertices.size()) {
		CEdge* newA = p_queue.top();
		p_queue.pop();	
		CVertex* newV = newA->m_pDestination;

		if (!newV->m_PrimInTree) {
			newV->m_PrimInTree = true;
			t.m_Edges.push_back(newA);
			nVert++;
			for (CEdge* e : newV->m_Edges) { p_queue.push(e); }
		}
	}
	return t;
}