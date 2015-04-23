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



#ifndef _ASTAR_H
#define	_ASTAR_H

#include <cstdlib>
#include <queue>
#include <stack>
#include <fstream>
//#include "PriorityQueue.h"
//#include "Stack.h"
#include <vector>

class PathFinder;          

//using namespace std;

class PathFinder {

   public:
        struct coord {
            int x,y;
        };

        struct node {
                PathFinder :: coord current; //coords of current node;
                PathFinder :: coord previous; //coords of previous node of path
                int distance;
                int score; //the score , F = G+H , H is the heuristic
        };

        struct Grid {
            int n,m;
            int** A;
            coord start;
        };

        enum Direction { Up,Down,Left,Right };
        std::vector<Direction> directions;
        std::vector<coord> obstacles;
        Grid* grd;

    private:

        coord start;        

        std::priority_queue< node, std::vector<node> , std::less<node> > open;
        //PriorityQueue<node> open;
        std::stack<node> closed;

        coord* exitPoints;
        int nrOfExitPoints;
   public:

        PathFinder();
        PathFinder(const PathFinder &orig);
        ~PathFinder();
        PathFinder& operator=(const PathFinder &orig);

        void detectExitPoints();
        bool isExitPoint(coord end);
        int AStar();
        int manhattan_distance(coord start,coord end);
        int score(coord start);
        void printGrid();
        void reconstructPath();
        int getLength();
        void readMatrix();
        void deleteDuplicateDirections();

    private:
        template <class T>
        node searchNode(T closed, node nd);

        template <class T>
        void actualize(T &open,node nd,node parent);

};


inline bool operator<(const PathFinder :: node &node1,const PathFinder :: node &node2) {
    return node1.score > node2.score;
}



#endif	/* _ASTAR_H */

