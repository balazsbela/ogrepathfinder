/******************************************************************************
 * File:   astar.h                                                            *
 * Author: Balázs Béla                                                        *
 *                                                                            *
 * Created on May 22, 2010, 12:24 PM                                          *
 *                                                                            *
 *                                                                            *
 * AStar PathFinding implementation class                                     *
 * Copyright (c) Balázs Béla 2010                                             *
 * balazsbela@gmail.com                                                       *
 * If you want to use this class in your code, please send me an e-mail.      *
 *                                                                            *
 ******************************************************************************/


#include "astar.h"
#include <iostream>
#include <algorithm>

//using namespace std;


#define DEBUG 0
#define GRIDDEBUG 0
#define SCOREDEBUG 0

PathFinder :: PathFinder() {
    grd = new Grid();
    exitPoints = NULL;
    readMatrix();
    this->start = grd->start;
    this->exitPoints = NULL;
    this->nrOfExitPoints = 0;
    detectExitPoints();

}


PathFinder :: PathFinder(const PathFinder &orig) {
    grd = orig.grd;
    this->start = orig.start;
    this->exitPoints = NULL;
    this->nrOfExitPoints = 0;
    detectExitPoints();    
}


PathFinder :: ~PathFinder() {
    if (exitPoints) {
        delete[] exitPoints;
    }

    if(grd) {
        if (grd->A) {
            for(int i=0;i<grd->n;i++) {
                delete[] grd->A[i];
            }
        }
        delete grd;
    }
}


PathFinder& PathFinder ::operator=(const PathFinder &orig) {
    grd = orig.grd;
    this->start = orig.start;
    this->exitPoints = NULL;
    this->nrOfExitPoints = 0;
    detectExitPoints();
    
    return *this;
}


void PathFinder :: detectExitPoints() {
    exitPoints = new coord[ grd->n*2 + grd->m*2];
    nrOfExitPoints = 0;
    coord temp;


    for(int i=0; i < grd->n; i++) {
        //left
        if( grd->A[i][0] == 0) {
            temp.x = i;
            temp.y = 0;
            exitPoints[nrOfExitPoints++] = temp;
        }
        //right
        if( grd->A[i][grd->m - 1] == 0 ) {
            temp.x = i;
            temp.y = grd->m - 1;
            exitPoints[nrOfExitPoints++] = temp;
        }
        
    }

    for(int j=0; j < grd->m; j++) {
        //top 
        if( grd->A[0][j] == 0) {
            temp.x = 0;
            temp.y = j;
            exitPoints[nrOfExitPoints++] = temp;
        }
        //bottom
        if( grd->A[grd->n-1][j] == 0 ) {
            temp.x = grd->n-1;
            temp.y = j;
            exitPoints[nrOfExitPoints++] = temp;
        }

    }

    if (DEBUG) {
        for(int i=0;i<nrOfExitPoints;i++) {
            std::cout << "Exit point:" << exitPoints[i].x <<" "<< exitPoints[i].y<<"\n";
        }
    }
}


bool PathFinder :: isExitPoint(coord end) {    
    for (int i=0;i<nrOfExitPoints;i++) {
        if( (exitPoints[i].x == end.x ) && (exitPoints[i].y == end.y) ) {
            return 1;
        }
    }
    return 0;
}


void PathFinder :: printGrid() {
    for(int i=0;i< grd -> n;i++) {
            for(int j=0;j<grd -> m;j++) {
               // if( grd -> A[i][j]<10 ) std::cout << "  ";
               // else std::cout << " ";
                switch(grd -> A[i][j]) {
                    case 1:
                        if((i==grd->start.x)&&(j==grd->start.y)) {
                            std::cout << "R";
                        }
                        else std::cout << "1";
                        break;
                    case 0:
                        std::cout <<"*";
                        break;
                    case -1:
                        std::cout <<"X";
                        break;
                    default:
                        std::cout << grd->A[i][j];
                        break;
                }
              
            }
            std::cout << "\n";
        }

}


int PathFinder :: manhattan_distance(coord start,coord end) {
    return abs(end.x - start.x) + abs(end.y-start.y);
}


int PathFinder :: score (coord start) {
    int mdist = 0;
    int mindist = -1;
    if (SCOREDEBUG) {
        std::cout << "\n===Scoring: "<<start.x<<" "<<start.y << "\n";
    }
    for (int i=0;i<nrOfExitPoints;i++) {
        mdist = manhattan_distance(start,exitPoints[i]);

        if(SCOREDEBUG) {
        std::cout <<"---Distance from " << exitPoints[i].x << " "
             << exitPoints[i].y << " is "<< mdist<<"\n";
        }

        if ( (mdist < mindist) || (mindist == -1) ) {
            mindist = mdist;
        }
    }

    if(SCOREDEBUG) { std::cout << "\n===Final score for:"<< start.x <<" "<<start.y
                     << " is " << mindist <<" "; }
    return mindist;
}


template <class T>
PathFinder::node PathFinder :: searchNode(T closed, node nd) {
    node cnt;
    while(!closed.empty()) {
        cnt = closed.top();
        closed.pop();
        if ( ( cnt.current.x == nd.current.x ) &&
             ( cnt.current.y == nd.current.y ) ) {
            return cnt;
        }
    }

    node a;
    a.current.x = -1;
    return a;
}

template <class T>
void PathFinder :: actualize(T &open,node nd,node parent) {
    /*we search for this node in open
     * if we don't find it we add it to open, if we find it
     * but this one has a lower score then we need to actualize it*/

    T aux;
    node cnt;
    bool found=0;
    while((!open.empty())&&(!found)) {
        cnt = open.top();
        //open.dequeue();
        open.pop();
        
        if ( ( cnt.current.x == nd.current.x ) &&
             ( cnt.current.y == nd.current.y ) ) {

            if(DEBUG) std::cout <<"Found it in open!\n";

            found = 1;
            //we found it
            if(cnt.score > nd.score) {
                nd.previous.x = parent.current.x;
                nd.previous.y = parent.current.y;
                
                if(DEBUG) std::cout<< "With a lower score";
                //aux.enqueue(nd);
                aux.push(nd);
            }
            else {
                //aux.enqueue(cnt);
                aux.push(cnt);
            }
        }
        else {
            //aux.enqueue(cnt);
            aux.push(cnt);
        }

    }

    if(!found) {

        if(DEBUG) std::cout<<"Didn't find it in open, adding\n";
        nd.previous.x = parent.current.x;
        nd.previous.y = parent.current.y;

        //aux.enqueue(nd);
        aux.push(nd);
    }

    while(!aux.empty()) {
        //open.enqueue(aux.top());
        open.push(aux.top());
        //aux.dequeue();
        aux.pop();
    }
}




int PathFinder :: AStar() {

    int dx[4] = {0,0,1,-1};
    int dy[4] = {1,-1,0,0};
    
    node first;
    first.current = start;
    first.previous.x=-1;
    first.previous.y=-1;
    first.score = score(start);
    first.distance = 0;

    //open.enqueue(first);
    open.push(first);

    while (!open.empty()) {
        node cnt = open.top();
        //open.dequeue();
        open.pop();

        closed.push(cnt);
        
        if(DEBUG) std::cout << "Current top is:"<<cnt.current.x<<" "<<cnt.current.y<<"\n";
        if(GRIDDEBUG) grd -> A[cnt.current.x][cnt.current.y] = 1;
        
        if( isExitPoint(cnt.current) ) {
            //We reached our destination
            if(DEBUG)std::cout << "Destination reached!! "<<cnt.current.x<<" "<<cnt.current.y<<"\n";
            return 1;
        }

        //closed.push(cnt);

        //We check it's neighbours
        node newNd;
        for(int i=0;i<4;i++) {
            newNd.current.x = cnt.current.x + dx[i];
            newNd.current.y = cnt.current.y + dy[i];
            
            if( ( (newNd.current.x>=0)&&(newNd.current.x<grd -> n)&&
                  (newNd.current.y>=0)&&(newNd.current.y<grd -> m) )&&
                (grd -> A[newNd.current.x][newNd.current.y] != -1) ) {
                //valid coordinates

                if(DEBUG)std::cout << "\n==Processing Neighbour:"<<newNd.current.x<<" "<<newNd.current.y;

                newNd.distance = cnt.distance+1;
                newNd.score = newNd.distance + score(newNd.current);                

                if(SCOREDEBUG) {std::cout << "-" << newNd.score << "\n\n"; };
                if(DEBUG&&!SCOREDEBUG) {std::cout << " heuristic:" << score(newNd.current) << "\n"; }

                
                if(searchNode(closed,newNd).current.x == -1) {
                    if(DEBUG) std::cout <<"It doesn't belong to closed!\n";
                    if(GRIDDEBUG) grd -> A[newNd.current.x][newNd.current.y] = 3;
                    actualize(open,newNd,cnt);
                }
            }
            
        }

        if(DEBUG) std::cout<<"\n";
        if(DEBUG) printGrid();
        if(DEBUG) std::cout << "\n";
    }

    return 0;

}


int PathFinder :: getLength() {
    if(!closed.empty()) {
        node last = closed.top();
        return last.distance;
    }
    return -1;
}


void PathFinder :: reconstructPath() {
    node last = closed.top();
    node prev;
    int length = last.distance;
   // closed.pop();

    int i=0;
    while( (last.current.x!=-1)&&(last.previous.x != -1) && (last.previous.y != -1)) {
        grd->A[last.current.x][last.current.y] = length - i;

        int lx = last.current.x;
        int ly = last.current.y;
        int px = last.previous.x;
        int py = last.previous.y;

        std::cout << "Stepping from " << px << "-" << py<< " to " << lx <<"-"<<ly << "\n";

        if ( lx == px+1) {
            directions.push_back(Down);
        }
        else
            if ( lx == px-1 ) {
                directions.push_back(Up);
            }
            else if (ly == py +1) {
                directions.push_back(Right);
            }
            else if(ly == py-1) directions.push_back(Left);


        i++;
        prev.current.x = last.previous.x;
        prev.current.y = last.previous.y;
        last = searchNode(closed,prev);
    }

    std::reverse(directions.begin(),directions.end());  
}


void PathFinder :: readMatrix() {
    std::ifstream infile;
    infile.open("labyrinth.in");
    if(infile) {
        //Read the size of the matrix;
        infile >> grd->n >> grd->m;
        //Allocate the matrix
        grd->A = new int*[grd->n];
        for (int i = 0;i<grd->n;i++) {
            grd->A[i] = new int[grd->m];
        }

        //Read the elements

        char element;
        for(int i=0;i<grd->n;i++) {
            for(int j=0;j<grd->m;j++) {
                infile >> element;
                switch(element) {
                    case 'R':
                        grd->A[i][j] = 1;
                        grd->start.x = i;
                        grd->start.y = j;
                        break;
                    case 'X':
                        grd->A[i][j] = -1;
                        coord obst;
                        obst.x = i;
                        obst.y = j;
                        obstacles.push_back(obst);
                        break;
                    default:
                        grd->A[i][j] = 0;
                        break;
                }
            }
        }

    }
    else {
        std::cout << "Could not open infile!\n";
        exit(1);
    }
}


void PathFinder :: deleteDuplicateDirections() {
    Direction last = directions[0];
    int nr = directions.size();
    int i=0;
    while (i<nr) {
        if(i>0) {
            if (directions[i] == last) {               
                directions.erase(directions.begin()+i);
                nr--;
            }
            else  { last = directions[i];
                    i++;
                  }
        }
        else i++;
    }
    
}
