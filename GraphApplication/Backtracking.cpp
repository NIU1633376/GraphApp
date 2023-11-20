#include "pch.h"
#include "Graph.h"
#include <set>
#include <algorithm>
#include <stack>
constexpr auto INFINI = 1.79769e+308;


// =============================================================================
// SalesmanTrackBacktracking ===================================================
// =============================================================================

/*
Funció solució: Comprovem que un comptador de visites intermèdies == total de visites
i estem en el vector destí (ultim vector del camí)

Lògica BTracking:
No es pot passar dos cops per la mateixa aresta en el mateix sentit, ens ho indica el booleà m_processed
per tal de saber si hem passat per aquella aresta. (Pensar amb l'atribut m_reverseEdge).

Farem un backtrack quan:
- No tinguem arestes a explorar
- Si la longitud actual és més gran que la longitud òptima d'abans
- Quan trobem una solució (per tal de comprovar la seva longitud)
*/

CTrack SalesmanTrackBacktracking(CGraph& graph, CVisits& visits)
{
    CTrack best(&graph);
    CVertex* start = visits.m_Vertices.front();
    CVertex* desti = visits.m_Vertices.back();
    double shortest = numeric_limits<double>::max();
    do {
        list<CVertex*> permuta = visits.m_Vertices;
        permuta.push_front(start);
        permuta.push_back(desti);
        CTrack t(&graph);
        for (auto it = permuta.begin(); it != permuta.end(); it++) {
            auto current = *it;
            auto next = *std::next(it);
            if (next) {
                CTrack currentTrack = BackTrack(graph, current, next);
                t.Append(currentTrack);
            }
        }
        double currentL = t.Length();
        if (currentL < shortest) {
            best = t;
            shortest = currentL;
        }
    } while (std::next_permutation(visits.m_Vertices.begin(), visits.m_Vertices.end()));

    return best;
}



// =============================================================================
// SalesmanTrackBacktrackingGreedy =============================================
// =============================================================================
vector<vector<CTrack>> path;
CTrack camiCurt(CGraph& graph, CVertex* begin, CVertex* end) {
    CTrack t(&graph);
    if (graph.MemberP(begin, end)) {
        t.AddFirst(graph.FindEdge(begin, end));
    }
    else {
        CVertex* vActual = end;
        while (vActual != begin) {
            for (CEdge* e : vActual->m_Edges) {
                CVertex* vSeg = (vActual == e->m_pDestination) ? e->m_pOrigin : e->m_pDestination;
                if (abs(vActual->m_DijkstraDistance - (e->m_Length + vSeg->m_DijkstraDistance)) < 0.0001) {
                    t.AddFirst(e->m_pReverseEdge);
                    vActual = vSeg;
                    break;
                }
            }
        }
    }
    return t;
}
void bakitraki(CGraph& graph, CTrack& cami, CVisits& visits, int iActual, CVertex* pActual, CTrack& CamiMesCurt, double& LongitudCamiMesCurt, int visitCount)
{
	if (cami.Length() >= LongitudCamiMesCurt)
		return;

	CVertex* pDesti = visits.m_Vertices.back();

	CTrack cami_original = cami;

	if (pActual->m_VertexToVisit)
		visitCount++;

	if (pActual == pDesti && visitCount == visits.GetNVertices()) {
		CamiMesCurt = cami;
		LongitudCamiMesCurt = cami.Length();
		return;
	}

	bool anticJaHePassat = pActual->m_JaHePassat;
	pActual->m_JaHePassat = true;

	int i = 0;
	for (CTrack tram : path[iActual])
	{
		if (i != iActual && !tram.m_Edges.back()->m_pDestination->m_JaHePassat)
		{
			cami.Append(tram);	// push
			bakitraki(graph, cami, visits, i, tram.m_Edges.back()->m_pDestination, CamiMesCurt, LongitudCamiMesCurt, visitCount);
			cami = cami_original;	// pop
		}
		i++;
	}

	pActual->m_JaHePassat = anticJaHePassat;
}
void CalculateOptimalPaths(CGraph& graph, CVisits& visits, vector<vector<CTrack>>& path)
{
	for (CVertex* v1 : visits.m_Vertices)
	{
		DijkstraQueue(graph, v1);
		vector<CTrack> optimal_p;
		for (CVertex* v2 : visits.m_Vertices)
		{
			CTrack actual_path(&graph);
			if (v1 != v2)
			{
				CEdge* e = v2->m_pDijkstraPrevious;
				actual_path.AddFirst(e);
				while (e->m_pOrigin != v1)
				{
					e = e->m_pOrigin->m_pDijkstraPrevious;
					actual_path.AddFirst(e);
				}
			}
			optimal_p.push_back(actual_path);
		}
		path.push_back(optimal_p);
	}

	//ya esta fet ; 
}
CTrack SalesmanTrackBacktrackingGreedy(CGraph& graph, CVisits& visits)
{
	

	CVertex* pInici = visits.m_Vertices.front();
	double LonShortestPath;
	CTrack n_path(&graph);
	CTrack CamiMesCurt(nullptr);
	int a = 0;
	CamiMesCurt.Clear();
	
	LonShortestPath = numeric_limits<double>::max();
	for (CVertex& v : graph.m_Vertices)
	{
		v.m_JaHePassat = false;
		v.m_VertexToVisit = false;
	}
	for (CVertex* v : visits.m_Vertices) {
		v->m_VertexToVisit = true;
	}
	for (CEdge& e : graph.m_Edges)
	{
		e.m_Used = false;
	}

	

	CalculateOptimalPaths(graph, visits, path);

	

	bakitraki(graph, n_path, visits, 0, pInici, CamiMesCurt, LonShortestPath, a);
	
	return CamiMesCurt;
}


