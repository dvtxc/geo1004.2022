// some standard libraries that are helpfull for reading/writing text files
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

#include "Gmap.h"

bool read_obj(std::string filepath, std::vector<Vertex>& vertices);

int main(int argc, const char * argv[]) {
    std::string file_in = "/home/ravi/git/geo1004.2022/hw/01/data/torus.obj";
    std::string file_out_obj = "/home/ravi/git/geo1004.2022/hw/01/data/torus_triangulated.obj";
    std::string file_out_csv_d = "/home/ravi/git/geo1004.2022/hw/01/data/torus_darts.csv";
    std::string file_out_csv_0 = "/home/ravi/git/geo1004.2022/hw/01/data/torus_vertices.csv";
    std::string file_out_csv_1 = "/home/ravi/git/geo1004.2022/hw/01/data/torus_edges.csv";
    std::string file_out_csv_2 = "/home/ravi/git/geo1004.2022/hw/01/data/torus_faces.csv";
    std::string file_out_csv_3 = "/home/ravi/git/geo1004.2022/hw/01/data/torus_volume.csv";

    // ## Read OBJ file ##
    file_in = "C:\\dev\\geo\\geo1004\\geo1004.2022\\hw\\01\\data\\cube.obj";

    // Store loaded vertices in this vector
    std::vector<Vertex> vertices;

    // Read file
    if (!read_obj(file_in, vertices)) {
        std::cerr << "Could not read input file." << std::endl;
        return 1;
    }

    std::cout << "Reached this part" << "\n";

  // ## Construct generalised map using the structures from Gmap.h ##
  
  // ## Output generalised map to CSV ##

  // ## Create triangles from the darts ##

  // ## Write triangles to obj ##
  
  return 0;
}

/*
 * Function that reads the obj file and stores the vertices
 *
 * Input:
 *  filepath    Input file
 */
bool read_obj(std::string filepath, std::vector<Vertex>& vertices) {
    std::cout << "Reading file: " << filepath << std::endl;

    // Create input stream class and open file in read mode
    std::ifstream stream_in(filepath.c_str(), std::ifstream::in);
    if (!stream_in) {
        std::cout << "Input file not found or file could not be opened.\n";
        return false;
    }

    // File is opened, read file
    std::string line;
    while (std::getline(stream_in, line)) {
        std::istringstream iss(line);
        std::string word;

        iss >> word;
        if (word == "v") {
            std::vector<float> coordinates;
            while (iss >> word) coordinates.push_back(std::stof(word));
            if (coordinates.size() == 3) vertices.emplace_back(coordinates[0], coordinates[1], coordinates[2]);
            else vertices.push_back(Vertex());
        }

    }

    std::cout << "Number of vertices read: " << vertices.size() << std::endl;


    return true;

}