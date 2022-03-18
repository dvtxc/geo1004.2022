/*
Dmitri Visser
*/

#include <iostream>
#include <fstream>
#include <string>
#include "json.hpp"

#include "numfloors.h"
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
    std::string inputfile = R"(/mnt/c/Dev/geo/geo1004.2022/hw/02/data/clean.city.json)";
    std::string outfile = R"(/mnt/c/Dev/geo/geo1004.2022/hw/02/data/output_nminb.city.json)";
    read_input(inputfile, cj);

    // Compute number of floors
    numfloors_walk(cj);

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