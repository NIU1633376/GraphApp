#include "pch.h"
#include "Graph.h"
#include <queue>
// SalesmanTrackGreedy =========================================================

struct comparator {
    bool operator()(CVertex* vE1, CVertex* vE2) {
        return vE1->m_DijkstraDistance > vE2->m_DijkstraDistance;
    }
};

CTrack SalesmanTrackGreedy(CGraph& graph, CVisits& visits) {
    CTrack t(&graph);
    if (visits.m_Vertices.empty()) { return t; }
    CVertex* v = *(visits.m_Vertices.begin());
    CVertex* vf = *(prev(visits.m_Vertices.end()));

    priority_queue<CVertex*, vector<CVertex*>, comparator> p_queue;

    DijkstraQueue(graph, v);

    for (auto it = next(visits.m_Vertices.begin()); it != prev(visits.m_Vertices.end()); it++) {
        p_queue.push(*it);
    }

    while (!p_queue.empty()) {
        CVertex* v1 = p_queue.top();

        CVertex* v2 = v1;
        CTrack p(t.m_pGraph);
        while (v2 != v) {
            p.AddFirst(v2->m_pDijkstraPrevious);
            v2 = v2->m_pDijkstraPrevious->m_pOrigin;
        }
        t.Append(p);
        
        p_queue.pop();
        
        v = v1;

        DijkstraQueue(graph, v);

        vector<CVertex*> vec;
        while (!p_queue.empty()) {
            vec.push_back(p_queue.top());
            p_queue.pop();
        }
        for (CVertex* v : vec) {
            p_queue.push(v);
        }

    }

    CTrack p1(t.m_pGraph);
    while (vf != v) {
        p1.AddFirst(vf->m_pDijkstraPrevious);
        vf = vf->m_pDijkstraPrevious->m_pOrigin;
    }
    t.Append(p1);

    return t;
}
