// Graph.c ... implementation of Graph ADT
// Written by John Shepherd, May 2013
//implementation by Han ZHAO

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "Graph.h"
#include "Queue.h"

// graph representation (adjacency matrix)
typedef struct GraphRep {
	int    nV;    // #vertices
	int    nE;    // #edges
	int  **edges; // matrix of weights (0 == no edge)
} GraphRep;

// check validity of Vertex
int validV(Graph g, Vertex v)
{
	return (g != NULL && v >= 0 && v < g->nV);
}

// make an edge
Edge mkEdge(Graph g, Vertex v, Vertex w, int weight)
{
	assert(g != NULL && validV(g,v) && validV(g,w));
	Edge new = {v,w,weight}; // struct assignment
	return new;
}

// insert an Edge
// - sets (v,w) and (w,v)
void insertEdge(Graph g, Vertex v, Vertex w, int wt)
{
	assert(g != NULL && validV(g,v) && validV(g,w));
	if (g->edges[v][w] == 0) {
		g->edges[v][w] = wt;
		g->edges[w][v] = wt;
		g->nE++;
	}
}

// remove an Edge
// - unsets (v,w) and (w,v)
void removeEdge(Graph g, Vertex v, Vertex w)
{
	assert(g != NULL && validV(g,v) && validV(g,w));
	if (g->edges[v][w] != 0) {
		g->edges[v][w] = 0;
		g->edges[w][v] = 0;
		g->nE--;
	}
}

// create an empty graph
Graph newGraph(int nV)
{
	assert(nV > 0);
	int i, j;
   int **edges = malloc(nV * sizeof(int *));
   assert(edges != NULL);
   for (i = 0; i < nV; i++) {
      edges[i] = malloc(nV * sizeof(int));
      assert(edges[i] != NULL);
      for (j = 0; j < nV; j++)
         edges[i][j] = 0;
   }
   Graph g = malloc(sizeof(GraphRep));
   assert(g != NULL);
   g->nV = nV;  g->nE = 0;  g->edges = edges;
   return g; // dummy ... doesn't create an empty graph
}

// free memory associated with graph
void dropGraph(Graph g)
{
	assert(g != NULL);
	int i;
	for (i=0; i<(g->nV); i++){
		free (g->edges[i]);
	}
	free(g->edges);
	g->nV = 0;
	g->nE = 0;
	free(g);
}

// display graph, using names for vertices
void showGraph(Graph g, char **names)
{
	assert(g != NULL);
	printf("#vertices=%d, #edges=%d\n\n",g->nV,g->nE);
	int v, w;
	for (v = 0; v < g->nV; v++) {
		printf("%d %s\n",v,names[v]);
		for (w = 0; w < g->nV; w++) {
			if (g->edges[v][w]) {
				printf("\t%s (%d)\n",names[w],g->edges[v][w]);
			}
		}
		printf("\n");
	}
}

// find a path between two vertices using breadth-first traversal
// only allow edges whose weight is less than "max"
int findPath(Graph g, Vertex src, Vertex dest, int max, int *path){
	assert(g != NULL && validV(g,src) && validV(g,dest));

    Edge e = mkEdge(g, src, src,-1);
    int i, order = 0;
    int *pre = malloc(g->nV*sizeof(int));//create an array for the prev
    int *next = malloc(g->nV*sizeof(int));//create an array
    for (i = 0; i < g->nV; i++) pre[i] = next[i] = -1;//make the shortest is the direct distance
    Queue q = newQueue();//create a new queue to store edges
    QueueJoin(q,e); pre[e.w] = order++;
    //traverse from every path in the queue
    while (!QueueIsEmpty(q)) {
        Edge e = QueueLeave(q);
        next[e.w] = e.v;//get the next queue item
        Vertex v;
        for (v = 0; v < g->nV; v++) {
            if (g->edges[e.w][v] <= 0 || g->edges[e.w][v] >= max) continue;//verify the path is valid and not too long
            if (pre[v] == -1) {//if there is no path 
                QueueJoin(q, mkEdge(g,e.w,v,-1));//the vertex has been visited
                pre[v] = order++;
            }
        }
    }
    //counte the number of hops in the shortest journey
    Vertex curr;
    curr = dest;
    int counter = 0;
    while (curr != src) {
        path[counter++] = curr;
        curr = next[curr];//move the curr
    }
    path[counter++] = src;//plus the src
    
    for (i = 0; i < counter / 2; i++) {
        int temp = path[i];
        path[i] = path[counter - i - 1];
        path[counter - i - 1] = temp;
    }//reverse the asign value in the array to make it correct position

    free(pre);//free memory
    free(next);
    return counter; 
}

