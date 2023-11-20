#include "pch.h"
#include "Graph.h"
#include <algorithm>
#include <queue>
#include <utility>
#include <unordered_set>

// =============================================================================
// Dijkstra ====================================================================
// =============================================================================

void Dijkstra(CGraph& graph, CVertex* pStart) {
	if (graph.GetNVertices() != 0) {
		struct comparator {
			bool operator()(CVertex* v1, CVertex* v2) {
				return v1->m_DijkstraDistance > v2->m_DijkstraDistance;
			}
		};

		for (CVertex& v : graph.m_Vertices) {
			v.m_DijkstraDistance = numeric_limits<double>::max();
			v.m_DijkstraVisit = false;
		}

		CVertex* pActual = pStart;
		pActual->m_DijkstraDistance = 0.0;

		vector<CVertex*> minHeap;
		minHeap.push_back(pActual);
		
		while (!minHeap.empty()) {
			make_heap(minHeap.begin(), minHeap.end(), comparator());

			pActual = minHeap.front();
			pop_heap(minHeap.begin(), minHeap.end());
			minHeap.pop_back();

			if (!pActual->m_DijkstraVisit) {
				pActual->m_DijkstraVisit = true;
				for (CEdge* e : pActual->m_Edges) {
						double distV = e->m_pDestination->m_DijkstraDistance;
						double distA = pActual->m_DijkstraDistance + e->m_Length;
						if (distV > distA) {
							e->m_pDestination->m_DijkstraDistance = distA;
							e->m_pDestination->m_pDijkstraPrevious = e;
							minHeap.push_back(e->m_pDestination);
						}
				}
			}
		}
	}
}

// =============================================================================
// DijkstraQueue ===============================================================
// =============================================================================

struct comparator {
	bool operator()(pair<CVertex*, double> v1, pair<CVertex*, double> v2) {
		return v1.second > v2.second;
	}
};

void DijkstraQueue(CGraph& graph, CVertex* pStart)
{
	priority_queue<pair<CVertex*, double>, vector<pair<CVertex*, double>>, comparator> pQ;

	for (CVertex& v : graph.m_Vertices) {
		v.m_DijkstraDistance = numeric_limits<double>::max();
	}

	pQ.push({ pStart, pStart->m_DijkstraDistance = 0 });

	if (pStart->m_Edges.empty()) {
	}
	else {
		while (!pQ.empty()) {
			CVertex* pActual = pQ.top().first;
			pQ.pop();


			for (CEdge* e : pActual->m_Edges) {

				if (pActual->m_DijkstraDistance + e->m_Length < e->m_pDestination->m_DijkstraDistance) {
					e->m_pDestination->m_DijkstraDistance = pActual->m_DijkstraDistance + e->m_Length;
					e->m_pDestination->m_pDijkstraPrevious = e;
					pQ.push({ e->m_pDestination, pActual->m_DijkstraDistance + e->m_Length });
				}
			}
		}
	}
}