//
// Created by Dmitri on 16/03/2022.
//
#include <iostream>
#include <fstream>
#include <string>
#include "json.hpp"

// To use trigonometry
#include <cmath>

using json = nlohmann::json;

#include "helper.h"
#include "surfarea.h"



// START
int area_walk(json &cj) {

    // Go through all CityObjects
    for (auto &co: cj["CityObjects"].items()) {

        // Go through all solid geometry
        for (auto &g: co.value()["geometry"]) {
            if (g["type"] == "Solid") {

                std::cout << "---\n" << co.key() << " (" << co.value()["type"] << ")" << std::endl;

                // Go through all boundaries (shells), iterate with i
                for (int i = 0; i < g["boundaries"].size(); i++) {

                    // Go through all planar surfaces, iterate with j
                    for (int j = 0; j < g["boundaries"][i].size(); j++) {

                        // Get semantic index, retrieve with i,j
                        int sem_index = g["semantics"]["values"][i][j];

                        // Check if semantics > surfaces > [sem_index] > type == roofsurface
                        if (g["semantics"]["surfaces"][sem_index]["type"].get<std::string>().compare("RoofSurface") ==
                            0) {
                            std::cout << "Boundary plane index: " << j << " = RoofSurface: " << g["boundaries"][i][j]
                                      << std::endl;

                            auto &surf = g["boundaries"][i][j];

                            // Get semantic index, retrieve with i,j
                            int sem_index = g["semantics"]["values"][i][j];

                            std::cout << " Calculating orientation for sem_index: " << sem_index;
                            if (g["semantics"]["surfaces"][sem_index].contains("orientation")) {
                                std::cout << " with orientation: " << g["semantics"]["surfaces"][sem_index]["orientation"].get<std::string>();
                            }
                            std::cout << std::endl;

                            // construct polygons as vector of double3 with actual coordinates (of first ring only)
                            std::vector<double3> polygon = construct_poly(cj, surf[0]);

                            // calc area
                            double poly_area = calculate_polygon_area(polygon);
                            std::cout << " Poly ring 0 area: " << poly_area << std::endl;

                            // Calculate area of holes. Assume all other rings are first-level holes (and not holes of other holes)
                            // Subtract these areas of the area of the outer ring
                            for (int h = 1; h < surf.size(); h++) {
                                std::vector<double3> inner_polygon = construct_poly(cj, surf[h]);
                                double inner_area = calculate_polygon_area(inner_polygon);

                                std::cout << " subtracting inner ring " << h << ", area: " << inner_area << std::endl;
                                poly_area -= inner_area;


                            }
                            std::cout << " Surface area: " << poly_area << std::endl;

                            // Write orientation --------------------------------------------------------
                            // Check if orientation has already semantically been stored
                            if (!g["semantics"]["surfaces"][sem_index].contains("area")) {
                                // Orientation is not set. Create new key orientation.

                                std::cout << " Area not set in this orientation. Writing new area." << std::endl;
                                g["semantics"]["surfaces"][sem_index]["area"] = poly_area;

                            } else {
                                // Area is already set for this semantic obj. Add area.

                                double prev_area = g["semantics"]["surfaces"][sem_index]["area"].get<double>();

                                std::cout << " Area for this orientation (sem_index " << sem_index << ") was: " << prev_area << ", new area = " << prev_area + poly_area << std::endl;
                                g["semantics"]["surfaces"][sem_index]["area"] = prev_area + poly_area;

                            }

                        }
                    }
                }
            }
        }
    }
}

double calculate_polygon_area(std::vector<double3> polygon) {

    if (polygon.size() < 3) return 0.0;

    double area = 0.0;
    double3 crossum = {0, 0, 0};

    // Shoelace formula, triangular form. Take first vertex as origin.
    for (int p = 1; p < polygon.size() - 1; p++) {
        crossum += linalg::cross(polygon[p] - polygon[0], polygon[p+1] - polygon[0]);
    }
    area = 0.5 * linalg::length(crossum);

    return area;
}