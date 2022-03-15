/*
Dmitri Visser
*/

#include <iostream>
#include <fstream>
#include <string>
#include "json.hpp"

#include "orientation.h"

using json = nlohmann::json;

int   get_no_roof_surfaces(json &j);
void  list_all_vertices(json& j);


int read_input(std::string inputfile, json& j);

int main(int argc, const char * argv[]) {

    // Hold the loaded city json object in cj
    json cj;

    // Read the city json file
    std::string inputfile = R"(..\..\data\twobuildings.city.json)";
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

    //list_all_vertices(cj);

  // go into floors.cpp
  std::cout << "go into floors.cpp" << std::endl;
  orientation_run(cj);

  //-- get total number of RoofSurface in the file
  // int noroofsurfaces = get_no_roof_surfaces(j);
  // std::cout << "Total RoofSurface: " << noroofsurfaces << std::endl;

  // list_all_vertices(j);

  visit_roofsurfaces(cj);

  //-- print out the number of Buildings in the file
  int nobuildings = 0;
  for (auto& co : cj["CityObjects"]) {
    if (co["type"] == "Building") {
      nobuildings += 1;
    }
  }
  std::cout << "There are " << nobuildings << " Buildings in the file" << std::endl;

  //-- print out the number of vertices in the file
  std::cout << "Number of vertices " << cj["vertices"].size() << std::endl;

  //-- add an attribute "volume"
  for (auto& co : cj["CityObjects"]) {
    if (co["type"] == "Building") {
      co["attributes"]["volume"] = rand();
    }
  }

  //-- write to disk the modified city model (myfile.city.json)
  std::ofstream o("myfile.city.json");
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



// Visit every 'RoofSurface' in the CityJSON model and output its geometry (the arrays of indices)
// Useful to learn to visit the geometry boundaries and at the same time check their semantics.



// Returns the number of 'RooSurface' in the CityJSON model
int get_no_roof_surfaces(json &j) {
  int total = 0;
  for (auto& co : j["CityObjects"].items()) {
    for (auto& g : co.value()["geometry"]) {
      if (g["type"] == "Solid") {
        for (auto& shell : g["semantics"]["values"]) {
          for (auto& s : shell) {
            if (g["semantics"]["surfaces"][s.get<int>()]["type"].get<std::string>().compare("RoofSurface") == 0) {
              total += 1;
            }
          }
        }
      }
    }
  }
  return total;
}


// CityJSON files have their vertices compressed: https://www.cityjson.org/specs/1.1.1/#transform-object
// this function visits all the surfaces and print the (x,y,z) coordinates of each vertex encountered
void list_all_vertices(json& j) {
  for (auto& co : j["CityObjects"].items()) {
    std::cout << "= CityObject: " << co.key() << std::endl;
    for (auto& g : co.value()["geometry"]) {
      if (g["type"] == "Solid") {
        for (auto& shell : g["boundaries"]) {
          for (auto& surface : shell) {
            for (auto& ring : surface) {
              std::cout << "---" << std::endl;
              for (auto& v : ring) { 
                std::vector<int> vi = j["vertices"][v.get<int>()];
                double x = (vi[0] * j["transform"]["scale"][0].get<double>()) + j["transform"]["translate"][0].get<double>();
                double y = (vi[1] * j["transform"]["scale"][1].get<double>()) + j["transform"]["translate"][1].get<double>();
                double z = (vi[2] * j["transform"]["scale"][2].get<double>()) + j["transform"]["translate"][2].get<double>();
                std::cout << std::setprecision(2) << std::fixed << v << " (" << x << ", " << y << ", " << z << ")" << std::endl;                
              }
            }
          }
        }
      }
    }
  }
}