//
// Created by Dmitri on 08/03/2022.
//

#ifndef HW02_ORIENTATION_H
#define HW02_ORIENTATION_H

#include "json.hpp"
using json = nlohmann::json;

// Makes dot and cross products a lot easier and tidier.
#include <linalg/linalg.h>
using double2 = linalg::aliases::double2;
using double3 = linalg::aliases::double3;
using double4 = linalg::aliases::double4;

int orientation_run(json &j);

void  visit_roofsurfaces(json &j);
//double3 get_vtx_coordinates(json &j, int vertex);
double4 compute_plane(double3 a, double3 b, double3 c);
double4 compute_plane_from_polygon(std::vector<double3> pv);
std::string compute_orientation(double4 plane);
//void write_orientation(json& g, std::string orientation, int sem_index, int i, int j)

#endif //HW02_ORIENTATION_H
