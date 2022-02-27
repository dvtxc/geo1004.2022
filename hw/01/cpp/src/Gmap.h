#pragma once

#include "Point.h"

class Cell;
struct Point;
struct Dart;
struct Vertex;
struct Edge;
struct Face;
struct Volume;

/*
Below you find the basic elements that you need to build the generalised map.
The main thing you need to fill out are the links between the elements:
  * the involutions and cells on the Dart
  * the darts on the cells

One way to do this is by using pointers. eg. define a member on the dart struct like

  Struct Dart {
    // involutions:
    Dart* a0 = nullptr;
    // ...

    // cells:
    // ...
  
  };

Then you could create and link Darts like:
  
  Dart* dart_a = new Dart();
  Dart* dart_b = new Dart();

  dart_a->a0 = dart_b;
*/

class Cell {
    //...
};

struct Dart {
  // involutions:
  // Store the involutions as a vector of pointers to other darts
  std::vector<Dart*> involutions{4, NULL};

  // cells:
  // Store the cells as a vector of pointers to these cells (either a vertex, edge, face, or volume)
  //std::vector<Cell*> cells{4, NULL};

  Vertex* cell_0;
  Edge* cell_1;
  Face* cell_2;

  int id;

  // Constructor
    Dart() {
    }


};


struct Vertex : Cell {
  // the coordinates of this vertex:
  Point point;

  // constructor without arguments
  Vertex() : point(Point()) 
  {}

  // constructor with x,y,z arguments to immediately initialise the point member on this Vertex.
  Vertex(const double &x, const double &y, const double &z) : point(Point(x,y,z))
  {}

  // output as string
  std::string gen_key() {
      // use standard output
      std::stringstream ss;
      ss << this->point;
      return ss.str();
  }

  // a dart incident to this Vertex:
  // Convention: a dart incident to this Vertex in CCW direction
  Dart* dart_next;
  std::vector<Dart*> darts;

  int id;

};

std::ostream& operator<<(std::ostream& os, const Vertex v) {
    os << v.point;
    return os;
}

struct Edge : Cell {
    // TODO: We could hard-code the points that define this edge or infer it from the darts...
    // TODO: If hard-coded, store as property points.
    // Points that make up this edge, store as vector of pointers to vertices
    std::vector<Vertex*> points;

    // Constructor with Vertex0 and Vertex1 arguments to initialize the point member on this Edge
    Edge(Vertex &v1, Vertex &v2) {
        std::cout << "Constructing Edge from v" << v1.id << ": " << v1.point << " to v" << v2.id << ": " << v2.point << std::endl;

        points.push_back(&v1);
        points.push_back(&v2);
    }

    // A dart incident to this vertex
    Dart* dart;


  // a dart incident to this Edge:
  // ...

  // function to compute the barycenter for this Edge (needed for triangulation output):
  // Point barycenter() {}
    Point barycenter() {
        if (!points.empty()) {

        }
    }

    int id;
};

// Make edge "printable" for easy debugging
std::ostream& operator<<(std::ostream& os, const Edge e) {
    os << "Edge: " << *(e.points[0]) << " - " << *(e.points[1]);
    return os;
}

struct Face : Cell {
    // TODO: We could hard-code the points that define this face or infer it from the darts...
    // TODO: If hard-coded, store as property points.
    // Points that make up this face, store as vector of pointers to vertices.
    std::vector<Vertex*> points;

    // Constructor with 0 arguments
    Face() {
        // ...
    }

    // Constructor with 3 arguments (a triangular face)
    Face(Vertex &v0, Vertex &v1, Vertex &v2) {
        points.push_back(&v0);
        points.push_back(&v1);
        points.push_back(&v2);
    }

    // Constructor with 4 arguments (a face with 4 vertices)
    Face(Vertex &v0, Vertex &v1, Vertex &v2, Vertex &v3) {
        points.push_back(&v0);
        points.push_back(&v1);
        points.push_back(&v2);
        points.push_back(&v3);
    }

    int id;

  // a dart incident to this Face:
  // ...

  // function to compute the barycenter for this Face (needed for triangulation output):
  // Point barycenter() {}
    Point barycenter() {

    }

};

struct Volume : Cell {
  // a dart incident to this Volume:
  // ...

};