// some standard libraries that are helpful for reading/writing text files
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

#include "Gmap.h"

// Function declaration of read_obj: this function will read and parse the obj file
bool read_obj(std::string filepath, std::vector<Vertex>& vertices, std::vector<Face>& faces);

// Function to construct darts from a single face
std::vector<Dart*> construct_darts_from_face(Face& face);

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

    // Store loaded faces in this vector
    std::vector<Face> faces;

    // Read file
    if (!read_obj(file_in, vertices, faces)) {
        std::cerr << "Could not read input file." << std::endl;
        return 1;
    }

  // ## Construct generalised map using the structures from Gmap.h ##

    // Store constructed darts
    std::vector<Dart> darts;

    // Construct darts
    // Go through every face
    for(Face& face : faces) {

        std::vector<Dart*> face_darts = construct_darts_from_face(face);

    }

  
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
 *  & vertices  (reference) vector of vertices
 *  & faces     (reference) vector of faces
 *
 * Output:
 *  bool, success
 *
 */
bool read_obj(std::string filepath, std::vector<Vertex>& vertices, std::vector<Face>& faces) {
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

        // Handle lines starting with v (= vertices)
        if (word == "v") {
            std::vector<float> coordinates;
            while (iss >> word) coordinates.push_back(std::stof(word));
            if (coordinates.size() == 3) vertices.emplace_back(coordinates[0], coordinates[1], coordinates[2]);
            else vertices.push_back(Vertex());
        }

        // Handle lines starting with f (= faces)
        if (word == "f") {
            std::vector<Vertex*> vertex_pointers;
            while (iss >> word) {
                // Read vertex index (an integer) with std::stoi
                // Notice that obj files start counting at 1, whereas our vector starts at 0. So, subtract 1 to correctly refer to the vertex.
                int vertex_index = std::stoi(word) - 1;

                // Check whether we can actually access this vertex
                if (vertex_index >= vertices.size()) {
                    std::cerr << "Found face with vertex reference = " << vertex_index << ", which is out of bounds." << std::endl;
                    continue;
                }

                // Convert vertex index to actual vertex and store the reference
                vertex_pointers.push_back(&vertices[vertex_index]);
            }
            if (vertex_pointers.size() == 3) faces.emplace_back(*vertex_pointers[0], *vertex_pointers[1], *vertex_pointers[2]);
            else if (vertex_pointers.size() == 4) faces.emplace_back(*vertex_pointers[0], *vertex_pointers[1], *vertex_pointers[2], *vertex_pointers[3]);
            else faces.push_back(Face());
        }

    }

    std::cout << "Number of vertices read: " << vertices.size() << std::endl;
    std::cout << "Number of faces read:    " << faces.size() << std::endl;

    return true;

}



/*
 * Function that constructs edges based on a vector of faces and vertices
 *
 * Input:
 *  face    (reference) to a face
 *
 * Output:
 *  darts   vector of darts belonging to a single face
 *
 */
std::vector<Dart*> construct_darts_from_face(Face& face) {
    size_t num_vertices = face.points.size();

    // Darts of this face
    std::vector<Dart*> face_darts(num_vertices * 2, NULL);

    // Go through every vertex
    for (size_t i = 0; i < num_vertices; ++i) {

        // Create two darts
        Dart* dart_prev = new Dart(); // Dart pointing to the "previous" (clockwise)
        Dart* dart_next = new Dart(); // Dart pointing to the "next" (counter-clockwise)

        // store 0-cell
        dart_prev->cells[0] = face.points[i];
        dart_next->cells[0] = face.points[i];

        // store 2-cell
        dart_prev->cells[2] = &face;
        dart_next->cells[2] = &face;

        //a1 involutions
        dart_prev->involutions[1] = dart_next;
        dart_next->involutions[1] = dart_prev;
    }

    /*
    for (const Vertex *vertex : face.points) {
        //std::cout << vertex->point << std::endl; // Uncomment for debug

    }
    */

    return face_darts;
}