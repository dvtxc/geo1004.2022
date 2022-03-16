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

#include "helper.h"
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

                        auto &surf = g["boundaries"][i][j];

                        // Get semantic index, retrieve with i,j
                        int sem_index = g["semantics"]["values"][i][j];

                        // Check if semantics > surfaces > [sem_index] > type == roofsurface
                        if (g["semantics"]["surfaces"][sem_index]["type"].get<std::string>().compare("RoofSurface") == 0) {
                            std::cout << "Boundary plane index: " << j << " = RoofSurface: " << g["boundaries"][i][j] << std::endl;

                            // take first three vertices of first ring
                            /*
                            std::vector<double3> triangle(3);
                            for (size_t k = 0; k < 5; k += 2) {
                                std::cout << "  -vtx: " << g["boundaries"][i][j][0][k].get<int>();
                                triangle[k] = get_vtx_coordinates(cj, g["boundaries"][i][j][0][k].get<int>());
                                std::cout << " (" << triangle[k][0] << ", " << triangle[k][1] << ", " << triangle[k][2] << ")" << std::endl;
                            }
                            */

                            // construct polygons as vector of double3 with actual coordinates (of first ring only)
                            std::vector<double3> polygon = construct_poly(cj, surf[0]);

                            // Compute plane equation
                            double4 plane = compute_plane_from_polygon(polygon);

                            // Compute orientation of surface
                            std::string orientation = compute_orientation(plane);



                            // Write orientation --------------------------------------------------------
                            // Check if orientation has already semantically been stored
                            if (!g["semantics"]["surfaces"][sem_index].contains("orientation")) {
                                // Orientation is not set. Create new key orientation.

                                std::cout << " Orientation not set. New orientation." << std::endl;
                                g["semantics"]["surfaces"][sem_index]["orientation"] = orientation;

                            } else {

                                bool found_elsewhere = false;

                                // Check if orientation has already semantically been stored somewhere else
                                for (auto &sem_surface: g["semantics"]["surfaces"].items()) {
                                    if (sem_surface.value().contains("orientation")) {
                                        if (sem_surface.value()["orientation"].get<std::string>().compare(orientation) == 0) {

                                            // Semantic surface surface with same orientation has been found
                                            sem_index = std::stoi(sem_surface.key());

                                            std::cout << " Orientation " << orientation
                                                      << " already stored for sem_index: " << sem_index << std::endl;

                                            // Set current semantic reference to that object
                                            g["semantics"]["values"][i][j] = sem_index;

                                            found_elsewhere = true;
                                            break;

                                        }
                                    }
                                }

                                if (found_elsewhere == false) {

                                    // Current surface obj has different orientation. Same orientation has not been found elsewhere. Create new surface obj.
                                    sem_index = g["semantics"]["surfaces"].size();
                                    std::cout << " New orientation. New semantic surface with sem_index: " << sem_index
                                              << std::endl;

                                    json new_surf_obj = {{"type",        "RoofSurface"},
                                                         {"orientation", orientation}};
                                    g["semantics"]["surfaces"].insert(g["semantics"]["surfaces"].end(), new_surf_obj);
                                    g["semantics"]["values"][i][j] = sem_index;
                                }

                            }
                            // Write orientation END ----------------------------------------------------

                        }
                    }
                }
            }
        }
    }
}



// Construct plane equation (with normal vector) based on three vertices (triangle)
double4 compute_plane(double3 a, double3 b, double3 c) {
    // Compute a plane that intersects three points in space.
    double3 n = linalg::cross(b - a, c - a);

    n = linalg::normalize(n);

    return {n, -linalg::dot(n, a)};
}

// Construct plane equation (with normal vector) based on mostly planar polygon vertices
double4 compute_plane_from_polygon(std::vector<double3> pv) {

    double3 n = {0,0,0};
    for (int i = 1; i < pv.size(); i++) {
        n += linalg::cross(pv[i-1], pv[i]);
    }
    n += linalg::cross(pv.back(), pv[0]);

    n = linalg::normalize(n);

    return {n, -linalg::dot(n, pv[0])};
}

std::string compute_orientation(double4 plane) {
    std::string orientation;

    double3 plane_norm = {plane[0], plane[1], plane[2]};

    // Flip if upside down
    if (plane_norm[2] < 0) plane_norm = -plane_norm;

    // Normalize
    double3 normnorm = linalg::normalize(plane_norm);

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

//void write_orientation(json& g, std::string orientation, int sem_index, int i, int j) {
//}
