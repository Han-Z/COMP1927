// crawl.c ... build a graph of part of the web
// Written by John Shepherd, September 2015
// Uses the cURL library and functions by Vincent Sanders <vince@kyllikki.org>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <curl/curl.h>
#include "stack.h"
#include "set.h"
#include "graph.h"
#include "url.h"
#include "url_file.h"
#include <assert.h>

#define BUFSIZE 1024

typedef struct GraphRep *Graph;
typedef unsigned char Num;

typedef struct GraphRep {
    int   nV;
    int   maxV;
    char  **vertex;
    Num   **edges;
} GraphRep;

void setFirstURL(char *, char *);
void normalise(char *, char *, char *, char *, int);
void strlower(char *);
static int isallowed(char *url);

int main(int argc, char **argv)
{
	URL_FILE *handle;
	char buffer[BUFSIZE];
	char baseURL[BUFSIZE];
	char firstURL[BUFSIZE];
	char next[BUFSIZE];
	int  maxURLs;
    char baseC[BUFSIZE];
	if (argc > 2) {
		strcpy(baseURL,argv[1]);
		setFirstURL(baseURL,firstURL);
		maxURLs = atoi(argv[2]);
		if (maxURLs < 40) maxURLs = 40;
	}
	else {
		fprintf(stderr, "Usage: %s BaseURL MaxURLs\n",argv[0]);
		exit(1);
	}
		
	// You need to modify the code below to implement:
	//
	// add firstURL to the ToDo list
	// initialise Graph to hold up to maxURLs
	// initialise set of Seen URLs
	// while (ToDo list not empty and Graph not filled) {
	//    grab Next URL from ToDo list
	//    if (not allowed) continue
	//    foreach line in the opened URL {
	//       foreach URL on that line {
	//          if (Graph not filled or both URLs in Graph)
	//             add an edge from Next to this URL
	//          if (this URL not Seen already) {
	//             add it to the Seen set
	//             add it to the ToDo list
	//          }
	//       }
    //    }
	//    close the opened URL
	//    sleep(1)
	// }
    Graph g = newGraph(maxURLs);
    assert(g != NULL);
    
    Set visited = newSet();
    assert(visited != NULL);
    insertInto(visited, firstURL);
    
    Stack s = newStack();
    assert(s != NULL);
    pushOnto(s, firstURL);
    char *u;
while(!emptyStack(s) &&(g->nV < g->maxV)){
        u = popFrom(s);
        strcpy(baseC, baseURL);
        if (strstr(u, "/A/index") != NULL){
            strcat(baseC, "/A");
        }
        if (strstr(u, "/B/index") != NULL){
            strcat(baseC, "/B");
        }
    	if (!(handle = url_fopen(u, "r"))) {
    		fprintf(stderr,"Couldn't open %s\n", next);
    		//exit(1);
            continue;
    	}
	while(!url_feof(handle)) {
		url_fgets(buffer,sizeof(buffer),handle);
		//fputs(buffer,stdout);
		//strlower(buffer);
		char *cur, link[BUFSIZE], full_link[BUFSIZE];
		cur = buffer;
		while ((cur = nextURL(cur)) != NULL) {
			getURL(cur, link, BUFSIZE-1);
			
            normalise(link, next, baseC, full_link, BUFSIZE-1);
            strcpy(next, baseURL);
            
            if(isallowed(full_link) && g->nV <maxURLs &&isConnected(g,u,full_link)!=1){
                addEdge(g, u, full_link);    
                g->nV++;
            }
            if(!isElem(visited,full_link)){
                insertInto(visited, full_link);
                pushOnto(s,full_link);
                printf("Found %s\n", full_link);
            }
                
            cur +=strlen(link);
        }
    }
}
    url_fclose(handle);
    sleep(1);
    showGraph(g, 0);
    showGraph(g, 1);
    free(visited);
    disposeStack(s);
    disposeGraph(g);
    
	return 0;
}

// normalise(In,From,Base,Out,N)
// - converts the URL in In into a regularised version in Out
// - uses the base URL to handle URLs like "/..."
// - uses From (the URL used to reach In) to handle "../"
//   - assumes that From has already been normalised
void normalise(char *in, char *from, char *base, char *out, int n)
{
    if (strstr(in, "http") != NULL){
        strcpy(out, in);
        
    } else if (in[0] == '/') {
        strcpy(out, base);
        strcat(out, in);
    } else if (in[0] == '.'){
        
        strcpy(out, from);
        in++;
        strcat(out, ++in);
        
    }  else {
        strcpy(out, base);
        strcat(out, "/");
        
        strcat(out, in);
    }
}

// setFirstURL(Base,First)
// - sets a "normalised" version of Base as First
// - modifies Base to a "normalised" version of itself
void setFirstURL(char *base, char *first)
{
	char *c;
	if ((c = strstr(base, "/index.html")) != NULL) {
		strcpy(first,base);
		*c = '\0';
	}
	else if (base[strlen(base)-1] == '/') {
		strcpy(first,base);
		strcat(first,"index.html");
		base[strlen(base)-1] = '\0';
	}
	else {
		strcpy(first,base);
		strcat(first,"/index.html");
	}
}

// strlower(Str)
// - set all chars in Str to lower-case
void strlower(char *s)
{
	for ( ; *s != '\0'; s++)
		*s = tolower(*s);
}


static int isallowed(char *url){
    assert(url != NULL);
    char *substr1= "unsw";
    char *substr2 = "html";
    char *result;
    result = strstr(url, substr1);
    if(result == NULL) return 0;
    
    result = strstr(url, substr2);
    if(result == NULL) return 0;
    
    return 1;
}
