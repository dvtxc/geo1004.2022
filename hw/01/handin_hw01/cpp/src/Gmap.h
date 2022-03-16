#pragma once

#include "Point.h"

// include iomanip to set precision --> needed to reliably compare the vertex keys
#include <iomanip>

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
  Volume* cell_3;

  int id;

  // Constructor
    Dart() {
    }


};


struct Vertex {
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

  // Make accessing a little tidier
  float x() const { return point.x; }
  float y() const { return point.y; }
  float z() const { return point.z; }

  int id;

};

std::ostream& operator<<(std::ostream& os, const Vertex v) {
    os << std::fixed << std::setprecision(3) << v.point;
    return os;
}

struct Edge {
    // Points that make up this edge, store as vector of pointers to vertices
    std::vector<Vertex*> points;

    // Constructor with Vertex0 and Vertex1 arguments to initialize the point member on this Edge
    Edge(Vertex &v1, Vertex &v2) {
        //std::cout << "Constructing Edge from v" << v1.id << ": " << v1.point << " to v" << v2.id << ": " << v2.point << std::endl;

        points.push_back(&v1);
        points.push_back(&v2);
    }

    // A dart incident to this edge
    Dart* dart;

    // Store barycenters
    Vertex* barycenter_vtx = NULL;

  // function to compute the barycenter for this Edge (needed for triangulation output):
  // Point barycenter() {}
    Vertex* barycenter() {
        if (points.empty()) {
            return new Vertex();
        }

        // was already calculated
        if (barycenter_vtx) {
            return  barycenter_vtx;
        }

        float px = (points[0]->x() + points[1]->x()) / 2;
        float py = (points[0]->y() + points[1]->y()) / 2;
        float pz = (points[0]->z() + points[1]->z()) / 2;

        // Set barycenter (so we don't have to calculate it again.)
        barycenter_vtx = new Vertex(px, py, pz);

        // return barycenter
        return barycenter_vtx;
    }

    int id;
};

// Make edge "printable" for easy debugging
std::ostream& operator<<(std::ostream& os, const Edge e) {
    os << "Edge: " << *(e.points[0]) << " - " << *(e.points[1]);
    return os;
}

struct Face {
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
    // Store all darts of the face in this vector, even though we only need one.
    std::vector<Dart*> darts;

  // function to compute the barycenter for this Face (needed for triangulation output):
  // Point barycenter() {}
    Point barycenter() {
        // Return empty point, when face is not fully initialized
        if (points.empty()) {
          return Point();
        }

        // Calculate barycenter, when face is initialized
        float Cx = (points[0]->x() + points[1]->x() + points[2]->x() + points[3]->x()) * 0.25;
        float Cy = (points[0]->y() + points[1]->y() + points[2]->y() + points[3]->y()) * 0.25;
        float Cz = (points[0]->z() + points[1]->z() + points[2]->z() + points[3]->z()) * 0.25;

        return Point(Cx, Cy, Cz);

    }


};

// Make face "printable" for easy debugging
std::ostream& operator<<(std::ostream& os, const Face f) {
    os << "Face f" << f.id << ", vertices:";
    for (Vertex* vtx : f.points) {
        os << "\t v" << vtx->id << " " << *vtx;
    }
    return os;
}

struct Volume {
    Dart* dart;

    int id;

};