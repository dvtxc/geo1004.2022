/*
Dmitri Visser
*/

#include <iostream>
#include <fstream>
#include <string>
#include "json.hpp"

#include "orientation.h"
#include "surfarea.h"

using json = nlohmann::json;

int   get_no_roof_surfaces(json &j);
void  list_all_vertices(json& j);


int read_input(std::string inputfile, json& j);

int main(int argc, const char * argv[]) {

    // Hold the loaded city json object in cj
    json cj;

    // Read the city json file
    std::string inputfile = R"(../../data/clean.city.json)";
    std::string outfile = R"(../../data/output.city.json)";
    read_input(inputfile, cj);

    // Go through buildings and list h_dak props
    for (auto &bldg : cj["CityObjects"]) {
        if (bldg["type"] == "Building") {
            std::cout << "Building " << (std::string) bldg["attributes"]["identificatie"];

            if (bldg["children"].size() == 0) {
                std::cout << " has no child geometry." << std::endl;
                continue;
            }

            // Has child geometry
            auto h_maaiveld = (double) bldg["attributes"]["h_maaiveld"];
            auto h_dak_min = (double) bldg["attributes"]["h_dak_min"];
            auto h_dak_max = (double) bldg["attributes"]["h_dak_max"];

            // Calculate number of floors
            double height = (0.7 * h_dak_max + 0.3 * h_dak_min) - h_maaiveld;
            int num_floors = (int) round(height / 3.0);

            std::cout << " - height: " << height << ", no_floor: " << num_floors << std::endl;
        }
    }


    // Compute roof orientations
    orientation_run(cj);

    // Compute areas for roof surfaces in the same orientation
    area_walk(cj);


    //-- write to disk the modified city model (myfile.city.json)
    std::cout << "\n\n---\n\nWriting output to: " << outfile << std::endl;
    std::ofstream o(outfile);
    o << cj.dump(2) << std::endl;
    o.close();


  return 0;
}


int read_input(std::string inputfile, json& cj) {
    //-- reading the file with nlohmann json: https://github.com/nlohmann/json

    std::cout << "Reading input file: " << inputfile << std::endl;

    std::ifstream input(inputfile);
    input >> cj;
    input.close();

    return 0;
}