#include "pch.h"
#include "Graph.h"
#include <queue>
#include <iostream>
#include <iomanip>
#include <numeric>


// SalesmanTrackBranchAndBound1 ===================================================

class CBBNode1 {
public:
	vector<int> is;
	vector<bool> visitats;
	double dijkstraDist;
	double cotai;
	double cotas;

	CBBNode1(double dist, vector<int> indexes, int nVertex) :dijkstraDist(dist), is(indexes) {
		visitats.resize(nVertex, false);
		visitats[0] = true;
	}

	CBBNode1(double dist, vector<int> indexes, int nVertex, double cotaInferior, double cotaSuperior) :dijkstraDist(dist), is(indexes), cotai(cotaInferior), cotas(cotaSuperior) {
		visitats.resize(nVertex, false);
		visitats[0] = true;
	}

	CBBNode1(const CBBNode1& node) {
		is = node.is;
		visitats = node.visitats;
	}

};


struct comparator1 {
	bool operator()(const CBBNode1* s1, const CBBNode1* s2) {
		return s1->dijkstraDist > s2->dijkstraDist;
	}
};


struct comparator2 {
	bool operator()(const CBBNode1* s1, const CBBNode1* s2) {
		return s1->cotai > s2->cotai;
	}
};

class Tros {
public:
	double dist;
	list<CEdge*> cami;
};

int construeix_matriu(CGraph& graph, CVisits& visits, vector<vector<Tros>>& m) {

	int i = 0;
	for (auto it = visits.m_Vertices.begin(); it != next(visits.m_Vertices.begin(), visits.m_Vertices.size() - 1); it++) {
		DijkstraQueue(graph, *it);

		(*it)->m_iMatriu = i;

		for (CVertex* v : visits.m_Vertices) {

			Tros matrixCell;
			matrixCell.dist = v->m_DijkstraDistance;
			if (*it != v) {
				while (v != *it) {
					matrixCell.cami.push_front(v->m_pDijkstraPrevious);
					v = v->m_pDijkstraPrevious->m_pOrigin;
				}
			}
			m[i].push_back(matrixCell);
		}
		i++;
	}
	return i;
}

bool checkVisits(vector<bool> v) {
	for (bool visited : v) {
		if (!visited) {
			return false;
		}
	}
	return true;
}
CTrack SalesmanTrackBranchAndBound1(CGraph& g, CVisits& visits)
{
	vector<vector<Tros>> mat(visits.m_Vertices.size() - 1);
	visits.m_Vertices.back()->m_iMatriu = construeix_matriu(g, visits, mat);
	priority_queue<CBBNode1*, std::vector<CBBNode1*>, comparator1> pQ;
	CBBNode1* node;
	for (int i = 1; i < visits.m_Vertices.size(); i++) {
		vector<int> vIndex;
		vIndex.push_back(0);
		vIndex.push_back(i);
		CBBNode1* nou = new CBBNode1(mat[0][i].dist, vIndex, visits.m_Vertices.size());
		pQ.push(nou);
	}
	while (!pQ.empty()) {
		node = pQ.top();
		pQ.pop();
		node->visitats[node->is.back()] = true;
		if (node->is.back() == visits.m_Vertices.back()->m_iMatriu) {
			if (checkVisits(node->visitats)) {
				CTrack best = CTrack(&g, mat[node->is[0]][node->is[1]].cami);
				for (int i = 1; i < node->is.size() - 1; i++) {
					for (CEdge* e : mat[node->is[i]][node->is[i + 1]].cami) {
						best.m_Edges.push_back(e);
					}
				}
				return best;
			}
		}
		if (node->is.back() != visits.m_Vertices.back()->m_iMatriu) {
			for (int i = 0; i < visits.m_Vertices.size(); i++) {
				if (i != node->is.back() && !node->visitats[i]) {
					CBBNode1* nn = new CBBNode1(*node);
					nn->is.push_back(i);
					nn->dijkstraDist = node->dijkstraDist + mat[node->is.back()][i].dist;
					pQ.push(nn);
				}
			}
		}
	}
	return CTrack(&g);
}
CTrack SalesmanTrackBranchAndBound2(CGraph& g, CVisits& visits)
{
	return CTrack();
}

CTrack SalesmanTrackBranchAndBound3(CGraph& g, CVisits& visits)
{
	vector<vector<Tros>> matriu(visits.m_Vertices.size() - 1);
	int index = 0;

	index = 0;

	//if we only have orgin and destination with return Dijkstra way
	if (visits.m_Vertices.size() == 2) {

		//we create the CTrack with th first section already addes
		return CTrack(&g, matriu[0][1].cami);


	}
	else {

		//we put the index of the column that represents the destination vertex in the matrix
		visits.m_Vertices.back()->m_iMatriu = index;

		//-----------------------------------------

		priority_queue<CBBNode1*, std::vector<CBBNode1*>, comparator2> allPossibleSolution;


		//creation of cotas


		//each element of next vectors is the DujkstraDistance to one node that we have to visited.
		vector <double> min;
		vector <double> max;
		min.resize(visits.m_Vertices.size(), numeric_limits<double>::max());

		max.resize(visits.m_Vertices.size(), 0.0);

		for (int row = 0; row < visits.m_Vertices.size() - 1; row++)
		{
			//we check if we find another max or min DijkstraDistance in the new row to nodes
			for (int col = 0; col < visits.m_Vertices.size(); col++)
			{
				if (row != col) {
					if (matriu[row][col].dist < min[col])
					{
						min[col] = matriu[row][col].dist;
					}

					if (matriu[row][col].dist > max[col])
					{
						max[col] = matriu[row][col].dist;
					}
				}

			}
		}

		//summation of all the minimums and maximums ways to each vertex
		double cotaInferior = accumulate(min.begin(), min.end(), 0.0);
		double cotaSuperior = accumulate(max.begin(), max.end(), 0.0) + pow(10, -6);

		double globalCotaSuperior = cotaSuperior;

		//It is created the first node with the initial cotas
		vector<int> vIndex;
		vIndex.push_back(0);
		allPossibleSolution.push(new CBBNode1(0, vIndex, visits.m_Vertices.size(), cotaInferior, cotaSuperior));


		//we are going to iterate until find a solution,  the first one it's the optimal in B&B, or not get any solution
		while (!allPossibleSolution.empty())
		{

		noSolution:
			//we take the tram with less weight(DijkstraDistance) and we compare if the las node in this is the destination
			CBBNode1* topNode = allPossibleSolution.top();
			allPossibleSolution.pop();

			//we mark the next vertex in the list of index as visited
			topNode->visitats[topNode->is.back()] = true;

			//if the last index is equal to the index of the destination that means that we are in a solution
			if (topNode->is.back() == visits.m_Vertices.back()->m_iMatriu)
			{
				//we check if the possible solution it's right
				/*
				for (bool visitVertex : topNode->m_visited)
				{
					if (!visitVertex)
					{
						goto noSolution;
					}
				}
				*/

				//we get the unique vertex that we visited
				auto ip = std::unique(topNode->is.begin(), topNode->is.begin() + topNode->is.size());

				topNode->is.resize(distance(topNode->is.begin(), ip));

				//we check if we have the same number of unique values as the number of vertex we have to visited
				if (topNode->is.size() != visits.m_Vertices.size())
					goto noSolution;

				//we create the CTrack with th first section already addes
				CTrack solution = CTrack(&g, matriu[topNode->is[0]][topNode->is[1]].cami);

				//merge al de indexes trams that we created with Dijkstra and we had saved in the matrix
				for (int element = 1; element < topNode->is.size() - 1; element++) {

					for (CEdge* e : matriu[topNode->is[element]][topNode->is[element + 1]].cami)
						solution.m_Edges.push_back(e);
					//solution.m_Edges.merge(matriu[topNode->m_indexes[element]][topNode->m_indexes[element] + 1].tram);
				}


				//we release the memory

				return solution;

			}





			//if we don't get a solution we push all the new possible section of the solution way plus the current lenght

			for (int row = 0; row < visits.m_Vertices.size() - 1; row++)
			{
				//It's not consider the rows of visited vertex
				//bool a1 = topNode->m_visited[row] == false;
				//bool a2 = row == topNode->m_indexes.back();
				if (!topNode->visitats[row]) {

					//we check if we find another max or min DijkstraDistance in the new row to nodes
					for (int col = 0; col < visits.m_Vertices.size(); col++)
					{
						//It is not considere the column of the last visited vertex

						if (row != col && !topNode->visitats[col]) {
							if (matriu[row][col].dist < min[col])
							{
								min[col] = matriu[row][col].dist;
							}

							if (matriu[row][col].dist > max[col])
							{
								max[col] = matriu[row][col].dist;
							}
						}




					}
				}

			}


			//loop to check the row of the last vertex in the way, It is not considered going to the final vertex
			for (int col = 0; col < visits.m_Vertices.size() - 1; col++) {

				//It is not considered the diagonal of the matrix because it is 0, we don't consider column of visited vertex
				if (topNode->is.back() != col && !topNode->visitats[col]) {

					if (matriu[topNode->is.back()][col].dist < min[col])
					{
						min[col] = matriu[topNode->is.back()][col].dist;
					}

					if (matriu[topNode->is.back()][col].dist > max[col])
					{
						max[col] = matriu[topNode->is.back()][col].dist;
					}
				}
			}

			for (int col = 0; col < visits.m_Vertices.size(); col++)
			{

				//we control that we don't go to the same vertex, if we don't put this conditional, we will always in the same vertex because Dijkstra distance to it is 0
				//It is controled that we are not goint to go to visited vertex and we are not going to create a way with only the origin and the destination origin->destination
				if (col != topNode->is.back() && !topNode->visitats[col] && (col != visits.m_Vertices.size() - 1 || topNode->is.size() != 1))
				{


					//calculation of the new cotaSuperior to knwo if it is interesting to continous exploring this solutions
					cotaInferior = topNode->cotai - min[col] + matriu[topNode->is.back()][col].dist;

					if (globalCotaSuperior > cotaInferior)
					{


						//cota inferior calculation
						cotaSuperior = topNode->cotas - max[col] + matriu[topNode->is.back()][col].dist + pow(10, -6);

						//when it is found a smaller cotaSuperior we take it as the new global

						if (globalCotaSuperior > cotaSuperior)
						{
							globalCotaSuperior = cotaSuperior;
						}


						//Creation of the new solution or node
						CBBNode1* newNode = new CBBNode1(*topNode);
						newNode->cotai = cotaInferior;
						newNode->cotas = cotaSuperior;

						newNode->is.push_back(col);


						newNode->dijkstraDist = topNode->dijkstraDist + matriu[topNode->is.back()][col].dist;

						allPossibleSolution.push(newNode);
					}


				}

			}



		}

	}

	return CTrack(&g);
}