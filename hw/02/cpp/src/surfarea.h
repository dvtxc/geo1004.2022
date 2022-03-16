//
// Created by Dmitri on 16/03/2022.
//

#ifndef HW02_SURFAREA_H
#define HW02_SURFAREA_H

#include "json.hpp"
using json = nlohmann::json;

// Makes dot and cross products a lot easier and tidier.
#include <linalg/linalg.h>
using double2 = linalg::aliases::double2;
using double3 = linalg::aliases::double3;
using double4 = linalg::aliases::double4;

int area_walk(json& cj);
double calculate_polygon_area(std::vector<double3> polygon);

#endif //HW02_SURFAREA_H
