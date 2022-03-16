//
// Created by Dmitri on 16/03/2022.
//

#ifndef HW02_HELPER_H
#define HW02_HELPER_H

#include "json.hpp"
using json = nlohmann::json;

// Makes dot and cross products a lot easier and tidier.
#include <linalg/linalg.h>
using double2 = linalg::aliases::double2;
using double3 = linalg::aliases::double3;
using double4 = linalg::aliases::double4;

// Get vertex coordinates
inline double3 get_vtx_coordinates(json &j, int vertex) {
    double3 vtx_co;

    std::vector<int> vi = j["vertices"][vertex];
    vtx_co[0] = (vi[0] * j["transform"]["scale"][0].get<double>()) + j["transform"]["translate"][0].get<double>();
    vtx_co[1] = (vi[1] * j["transform"]["scale"][1].get<double>()) + j["transform"]["translate"][1].get<double>();
    vtx_co[2] = (vi[2] * j["transform"]["scale"][2].get<double>()) + j["transform"]["translate"][2].get<double>();

    return vtx_co;
}

inline std::vector<double3> construct_poly(json& cj, json& surf_ring) {
    std::vector<double3> polygon(surf_ring.size());
    int vi = 0;

    for (auto &v: surf_ring) {
        polygon[vi] = get_vtx_coordinates(cj, v);
        vi++;
    }

    return polygon;
}

#endif //HW02_HELPER_H
