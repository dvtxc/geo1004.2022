//
// Created by Dmitri on 08/03/2022.
//

#define _USE_MATH_DEFINES // to use pi

#include <iostream>
#include <fstream>
#include <string>
#include "json.hpp"

// To use trigonometry
#include <cmath>

using json = nlohmann::json;

#include "orientation.h"

int orientation_run(json & j) {
    visit_roofsurfaces(j);
    return 0;
}

void visit_roofsurfaces(json &cj) {

    // Go through all CityObjects
    for (auto& co : cj["CityObjects"].items()) {

        // Go through all solid geometry
        for (auto& g : co.value()["geometry"]) {
            if (g["type"] == "Solid") {

                std::cout << "---\n" << co.key() << " (" << co.value()["type"] << ")" << std::endl;

                // Go through all boundaries (shells), iterate with i
                for (int i = 0; i < g["boundaries"].size(); i++) {

                    // Go through all planar surfaces, iterate with j
                    for (int j = 0; j < g["boundaries"][i].size(); j++) {

                        // Get semantic index, retrieve with i,j
                        int sem_index = g["semantics"]["values"][i][j];

                        // Check if semantics > surfaces > [sem_index] > type == roofsurface
                        if (g["semantics"]["surfaces"][sem_index]["type"].get<std::string>().compare("RoofSurface") == 0) {
                            std::cout << "RoofSurface: " << g["boundaries"][i][j] << std::endl;

                            // take first three vertices of first ring
                            std::vector<double3> triangle(3);
                            for (size_t k = 0; k < 5; k += 2) {
                                std::cout << "  -vtx: " << g["boundaries"][i][j][0][k].get<int>();
                                triangle[k] = get_vtx_coordinates(cj, g["boundaries"][i][j][0][k].get<int>());
                                std::cout << " (" << triangle[k][0] << ", " << triangle[k][1] << ", " << triangle[k][2] << ")" << std::endl;
                            }

                            double4 plane = compute_plane(triangle[0], triangle[1], triangle[2]);

                            std::string orientation = compute_orientation(plane);

                            // Go through first ring
                            /*
                            for (auto& v : g["boundaries"][i][j][0]) {
                                auto vi = v.get<int>();
                                double3 vtxco = get_vtx_coordinates(cj, vi);
                                std::cout << vtxco[0] << "," << vtxco[1] << "," << vtxco[2] << std::endl;
                            }
                            */

                            if (g["semantics"]["surfaces"][sem_index].contains("orientation")) {
                                if (g["semantics"]["surfaces"][sem_index]["orientation"].get<std::string>().compare(orientation) == 0) {
                                    // Orientation is stored and corresponds to current value
                                    std::cout << " Orientation already stored for sem_index:" << sem_index << std::endl;

                                } else {
                                    // Orientation is stored, but current orientation differs. Create new semantic surface.
                                    sem_index = g["semantics"]["surfaces"].size();
                                    std::cout << " New orientation. New semantic surface with sem_index:" << sem_index << std::endl;

                                    json new_surf_obj = {{"type", "Roofsurface"}, {"orientation", orientation}};
                                    g["semantics"]["surfaces"].insert(g["semantics"]["surfaces"].end(), new_surf_obj);
                                    g["semantics"]["values"][i][j] = sem_index;

                                }
                            } else {
                                // Orientation is not set. Create new key orientation.

                                std::cout << " Orientation not set. New orientation." << std::endl;
                                g["semantics"]["surfaces"][sem_index]["orientation"] = orientation;
                            }

                            // Storing attribute orientation
                            //g["semantics"]["surfaces"][sem_index]["orientation"] = orientation;

                            //std::cout << g["semantics"]["surfaces"][sem_index]["orientation"].get<std::string>() << std::endl;

                        }
                    }
                }
            }
        }
    }
}

// Get vertex coordinates
double3 get_vtx_coordinates(json &j, int vertex) {
    double3 vtx_co;

    std::vector<int> vi = j["vertices"][vertex];
    vtx_co[0] = (vi[0] * j["transform"]["scale"][0].get<double>()) + j["transform"]["translate"][0].get<double>();
    vtx_co[1] = (vi[1] * j["transform"]["scale"][1].get<double>()) + j["transform"]["translate"][1].get<double>();
    vtx_co[2] = (vi[2] * j["transform"]["scale"][2].get<double>()) + j["transform"]["translate"][2].get<double>();

    return vtx_co;
}

// Get normal vector
double4 compute_plane(double3 a, double3 b, double3 c) {
    // Compute a plane that intersects three points in space.
    double3 n = linalg::cross(b - a, c - a);
    return {n, -linalg::dot(n, a)};
}

std::string compute_orientation(double4 plane) {
    std::string orientation;

    double3 plane_norm = {plane[0], plane[1], plane[2]};

    // Flip if upside down
    if (plane_norm[2] < 0) plane_norm = -plane_norm;

    // Normalize
    double3 normnorm = linalg::normalize(plane_norm);

    double3 up_vec = {0, 0, 1};
    double3 dir_vec = {plane[0], plane[1], 0};
    double3 north_vec = {0, 1, 0};

    //double dir_rad = atan2( linalg::dot(dir_vec, north_vec), linalg::dot(up_vec, linalg::cross(dir_vec, north_vec) ) );
    double dir_rad = atan2(normnorm[1], normnorm[0]);
    double dir_deg = dir_rad / 2.0 / M_PI * 360.0;

    if (normnorm[2] > 0.99) {
        orientation = "horizontal";
    } else {

        if (dir_deg > 0 && dir_deg <= 45) {
            orientation = "EN";
        } else if (dir_deg > 45 && dir_deg <= 90) {
            orientation = "NE";
        } else if (dir_deg > 90 && dir_deg <= 135) {
            orientation = "NW";
        } else if (dir_deg > 135 && dir_deg <= 180) {
            orientation = "WN";
        } else if (dir_deg > -180 && dir_deg <= -135) {
            orientation = "WS";
        } else if (dir_deg > -135 && dir_deg <= -90) {
            orientation = "SW";
        } else if (dir_deg > -90 && dir_deg <= -45) {
            orientation = "SE";
        } else if (dir_deg > -45 && dir_deg <= 0) {
            orientation = "ES";
        }
    }

    std::cout << " Plane norm: " << normnorm[0] << ", " << normnorm[1] << ", " << normnorm[2] << std::endl;;
    std::cout << " Direction (Deg): " << dir_deg << std::endl;
    std::cout << " Orientation: " << orientation << std::endl;

    return orientation;
}
