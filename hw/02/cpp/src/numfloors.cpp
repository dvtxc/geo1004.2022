//
// Created by VisserDmitri on 18/03/2022.
//

#include <iostream>
#include <fstream>
#include <string>
#include "json.hpp"
using json = nlohmann::json;

#include "numfloors.h"


void numfloors_walk(json& cj) {
    // Go through buildings and list h_dak props
    for (auto &bldg : cj["CityObjects"]) {
        if (bldg["type"] == "Building") {
            std::cout << "Building " << (std::string) bldg["attributes"]["identificatie"];

            if (bldg["children"].size() == 0) {
                std::cout << " has no child geometry." << std::endl;
                continue;
            }

            // Has child geometry
            double h_maaiveld = bldg["attributes"]["h_maaiveld"].get<double>();
            double h_dak_min = bldg["attributes"]["h_dak_min"].get<double>();
            double h_dak_max = bldg["attributes"]["h_dak_max"].get<double>();

            // Calculate number of floors
            double height = (0.7 * h_dak_max + 0.3 * h_dak_min) - h_maaiveld;
            int num_floors = (int) round(height / 3.0);

            std::cout << " - height: " << height << ", no_floor: " << num_floors << std::endl;
        }
    }
}