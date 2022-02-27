// some standard libraries that are helpful for reading/writing text files
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

// custom includes from the standard library
#include <algorithm>

#include "Gmap.h"

// Function declaration of read_obj: this function will read and parse the obj file
bool read_obj(std::string filepath, std::vector<Vertex>& vertices, std::vector<Face>& faces);

// Function to construct darts from a single face
std::vector<Dart*> construct_darts_from_face(Face& face, std::unordered_map<std::string, Vertex*>& vertices_umap, std::vector<Edge*>& edges);

// Function to find half edge twins
void construct_unique_edges_vertexHash(Face& face,
                                       std::vector<Dart*> face_darts,
                                       std::unordered_map<std::string, Vertex*>& vertices_umap,
                                       std::vector<Edge*>& edges);

void construct_unique_edges_vertexHash2(Face& face,
                                        std::vector<Dart*> face_darts,
                                        std::unordered_map<std::string, Vertex*>& vertices_umap,
                                        std::vector<Edge*>& edges);

void write_darts(std::string filepath, std::vector<Dart*>& darts);
void write_vertices(std::string fpath, std::unordered_map<std::string, Vertex*>& vertices_umap);
void write_edges(std::string filepath, std::vector<Edge*>& edges);

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
    std::vector<Dart*> darts;
    size_t num_darts = 0;

    // Store constructed edges
    std::vector<Edge*> edges;

    // Store vertices in unordered map
    std::unordered_map<std::string, Vertex*> vertices_umap;

    // Construct darts
    // Go through every face
    for(Face& face : faces) {
        // Get darts belonging to face, including:
        // a0 and a1 involutions
        // 0-cell and 2-cell
        std::vector<Dart*> face_darts = construct_darts_from_face(face, vertices_umap, edges);

        // Append to darts
        darts.insert(darts.end(), face_darts.begin(), face_darts.end());

        // Number the darts nicely
        for (auto dart : face_darts) {
            dart->id = num_darts;
            num_darts++;
        }
    }

    // Construct unique edges
    //for (Dart *dart : darts) {
    //}

    std::string file_out = "C:\\dev\\geo\\geo1004\\geo1004.2022\\hw\\01\\data\\vertices.csv";
    write_vertices(file_out, vertices_umap);
    write_edges("C:\\dev\\geo\\geo1004\\geo1004.2022\\hw\\01\\data\\edges.csv", edges);
    write_darts("C:\\dev\\geo\\geo1004\\geo1004.2022\\hw\\01\\data\\darts.csv", darts);

  
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

    // Keep track of integer vertex indices
    int vertex_id, face_id = 0;

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

            // Assign vertex id
            vertices.back().id = vertex_id;
            vertex_id++;
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

            faces.back().id = face_id;
            face_id;
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
 *  darts   vector of pointers to darts belonging to a single face
 *
 */
std::vector<Dart*> construct_darts_from_face(
        Face& face,
        std::unordered_map<std::string, Vertex*>& vertices_umap,
        std::vector<Edge*>& edges
        ) {
    size_t num_vertices = face.points.size();

    // Darts of this face
    std::vector<Dart*> face_darts;//(num_vertices * 2, NULL);

    // Go through every vertex
    for (Vertex *vertex : face.points) {

        // Create two darts
        Dart* dart_prev = new Dart(); // Dart pointing to the "previous" (clockwise)
        Dart* dart_next = new Dart(); // Dart pointing to the "next" (counter-clockwise)

        // store 0-cell
        dart_prev->cell_0 = vertex;
        dart_next->cell_0 = vertex;

        // store 2-cell
        dart_prev->cell_2 = &face;
        dart_next->cell_2 = &face;

        // a1 involutions
        dart_prev->involutions[1] = dart_next;
        dart_next->involutions[1] = dart_prev;

        // Insert new darts into vector
        face_darts.push_back(dart_prev);
        face_darts.push_back(dart_next);
    }

    // Create a0 involutions, go through every vertex
    for (size_t i = 0; i < num_vertices; ++i) {
        face_darts[0]->involutions[0] = face_darts.back();

        std::rotate(face_darts.begin(), face_darts.begin() + 1, face_darts.end());
        face_darts[0]->involutions[0] = face_darts[1];

        std::rotate(face_darts.begin(), face_darts.begin() + 1, face_darts.end());
    }

    construct_unique_edges_vertexHash(face, face_darts, vertices_umap, edges);

    return face_darts;
}


void construct_unique_edges_vertexHash(Face& face,
                                       std::vector<Dart*> face_darts,
                                       std::unordered_map<std::string, Vertex*>& vertices_umap,
                                       std::vector<Edge*>& edges
) {

    // find half edges
    for (size_t i = 0; i < face.points.size() * 2; i += 2) {

        // Start in CCW direction
        Dart* dart_next = face_darts[i+1]; // forward (CCW)
        Vertex* vtx_curr = dart_next->cell_0;
        Vertex* vtx_next = dart_next->involutions[0]->cell_0;

        std::cout << "Start v" << vtx_curr->id << " " << *vtx_curr;
        std::cout << " --> To v" << vtx_next->id << " " << *vtx_next << std::endl;

        Dart* opposite_dart = nullptr;

        if (vertices_umap.find(vtx_next->gen_key()) != vertices_umap.end()) {
            for (Dart *candidate_dart: vtx_next->darts) {
                if (candidate_dart->involutions[0]->cell_0->gen_key() == vtx_curr->gen_key()) {
                    // found
                    std::cout << "Back pointing dart found." << std::endl;
                    opposite_dart = candidate_dart;

                }
            }
        }

        if (opposite_dart) {
            Edge *common_edge = opposite_dart->cell_1;

            std::cout << "Common edge e" << common_edge->id << " " << *common_edge << "" << std::endl;

            // Assign common edge as 1-cell to darts
            dart_next->cell_1 = common_edge;
            dart_next->involutions[0]->cell_1 = common_edge;

            // Assign a2 involutions;
            dart_next->involutions[2] = opposite_dart->involutions[0];
            dart_next->involutions[2]->involutions[2] = dart_next;
            dart_next->involutions[0]->involutions[2] = opposite_dart;
            opposite_dart->involutions[2] = dart_next->involutions[0];

            // add
            vtx_curr->darts.push_back(dart_next);
            std::cout << "added vtx to umap, umap size: " << vertices_umap.size() << "" << std::endl;
            std::cout << "added v" << vtx_curr->id << " pointing to ";
            for (auto vd: vtx_curr->darts) {
                std::cout << "->v" << vd->involutions[0]->cell_0->id << ", ";
            }
            std::cout << "\n" << std::endl;
        } else {

            std::cout << "next vtx not found" << std::endl;

            // Create edge
            Edge *edge = new Edge(*vtx_curr, *vtx_next);
            edge->dart = dart_next;
            edges.push_back(edge);
            // TODO: debug
            edge->id = edges.size() - 1;
            std::cout << "created edge e" << edge->id << "" << std::endl;

            // Assign edge as 1-cell to darts
            dart_next->cell_1 = edge;
            dart_next->involutions[0]->cell_1 = edge;

            // Set incident dart of this vertex, CCW in the current face
            vtx_curr->darts.push_back(dart_next);

            // Add vertex to umap
            vertices_umap[vtx_curr->gen_key()] = vtx_curr;

            std::cout << "added vtx to umap, umap size: " << vertices_umap.size() << "" << std::endl;
            std::cout << "added v" << vtx_curr->id << " pointing to ";
            for (auto vd: vtx_curr->darts) {
                std::cout << "->v" << vd->involutions[0]->cell_0->id << ", ";
            }
            std::cout << "\n" << std::endl;
        }

    }
}


void construct_unique_edges_vertexHash2(Face& face,
                                       std::vector<Dart*> face_darts,
                                       std::unordered_map<std::string, Vertex*>& vertices_umap,
                                       std::vector<Edge*>& edges
                                       ) {

    // find half edges
    for (size_t i = 0; i < face.points.size() * 2; i += 2) {

        // Start in CCW direction
        Dart* dart_next = face_darts[i+1]; // forward (CCW)
        Vertex* vtx_curr = dart_next->cell_0;
        Vertex* vtx_next = dart_next->involutions[0]->cell_0;

        std::cout << "Start v" << vtx_curr->id << " " << *vtx_curr;
        std::cout << ", --> To v" << vtx_next->id << " " << *vtx_next << std::endl;

        //std::string vertex_key = vtx_next->gen_key();
        if (vertices_umap.find(vtx_next->gen_key()) == vertices_umap.end()) {
            // not found

            std::cout << "next vtx not found" << std::endl;

            // Create edge
            Edge* edge = new Edge(*vtx_curr, *vtx_next);
            edge->dart = dart_next;
            edges.push_back(edge);
            // TODO: debug
            edge->id = edges.size() - 1;
            std::cout << "created edge e" << edge->id << "" << std::endl;

            // Assign edge as 1-cell to darts
            dart_next->cell_1 = edge;
            dart_next->involutions[0]->cell_1 = edge;

            // Set incident dart of this vertex, CCW in the current face
            if (vertices_umap.find(vtx_curr->gen_key()) == vertices_umap.end()) vtx_curr->dart_next = dart_next;

            // Add vertex to umap
            vertices_umap[vtx_curr->gen_key()] = vtx_curr;

            std::cout << "constructed edge, ";
            std::cout << "added v" << vtx_curr->id << " pointing to v" << vtx_curr->dart_next->involutions[0]->cell_0->id << ", added vtx to umap. umap size: " << vertices_umap.size() << "\n" << std::endl;

        } else {
            // The "next" vertex is found in the umap. Check if it its curretly stored incident dart points back to our
            // current vertex and call this the candidate current vertex.
            // Recall: in other face also CCW.
            Vertex* candidate_vtx_curr = vtx_next->dart_next->involutions[0]->cell_0;

            std::cout << "Other vtx found in umap, pointing back to v" << candidate_vtx_curr->id << " = " << *candidate_vtx_curr << std::endl;

            // Check if candidate vertex matches current vertex
            if (vtx_curr->gen_key() == candidate_vtx_curr->gen_key()) {
                // The found next vertex should already have an incident dart on the other side of the edge
                Dart* opposite_dart = vtx_next->dart_next;
                Edge* common_edge = opposite_dart->cell_1;

                std::cout << "Common edge e" << common_edge->id << " " << *common_edge << "" << std::endl;

                // Assign common edge as 1-cell to darts
                dart_next->cell_1 = common_edge;
                dart_next->involutions[0]->cell_1 = common_edge;

                // Assign a2 involutions;
                dart_next->involutions[2] = opposite_dart->involutions[0];
                dart_next->involutions[2]->involutions[2] = dart_next;
                dart_next->involutions[0]->involutions[2] = opposite_dart;
                opposite_dart->involutions[2] = dart_next->involutions[0];

                // Reassign incident dart, to point to the next boundary edge, e.g. turn "outwards"
                std::cout << "v" << vtx_next->id << " dart points to v" << vtx_next->dart_next->involutions[0]->cell_0->id;
                Dart* start = vtx_next->dart_next;
                while (vtx_next->dart_next->involutions[2]) {
                    vtx_next->dart_next = vtx_next->dart_next->involutions[2]->involutions[1];
                    if (vtx_next->dart_next == start) break; // went around vtx
                }
                std::cout << ", after turning check: points to v" << vtx_next->dart_next->involutions[0]->cell_0->id << "" << std::endl;

                std::cout << "v" << vtx_curr->id << " dart points to v" << vtx_curr->dart_next->involutions[0]->cell_0->id;
                start = vtx_curr->dart_next;
                while (vtx_curr->dart_next->involutions[2]) {
                    vtx_curr->dart_next = vtx_curr->dart_next->involutions[2]->involutions[1];
                    if (vtx_curr->dart_next == start) break;
                }
                std::cout << ", after turning check: points to v" << vtx_curr->dart_next->involutions[0]->cell_0->id << "\n" << std::endl;


            } else {

                std::cout << "pointing back somewhere else";

                // Create edge
                Edge* edge = new Edge(*vtx_curr, *vtx_next);
                edge->dart = dart_next;
                edges.push_back(edge);
                // TODO: debug
                edge->id = edges.size() - 1;
                std::cout << "created edge e" << edge->id << "" << std::endl;

                // Assign edge as 1-cell to darts
                dart_next->cell_1 = edge;
                dart_next->involutions[0]->cell_1 = edge;

                vtx_curr->dart_next = dart_next;
                //vertices_umap[vtx_curr->gen_key()] = vtx_curr;

                std::cout << "v" << vtx_curr->id << " points to v" << vtx_curr->dart_next->involutions[0]->cell_0->id << ", v umap size: " << vertices_umap.size() << "\n" << std::endl;
            }

        }

    }
}

void write_darts(std::string filepath, std::vector<Dart*>& darts) {
    size_t num_edges = darts.size();

    std::cout << "--- Writing " << num_edges << " darts. ---" << std::endl;
    std::cout << "# Output file path: " << filepath << std::endl;

    std::ofstream outfile("" + filepath,std::ofstream::out);
    if (outfile.is_open()) {
        outfile << "id;a0;a1;a2;a3;v;e;f" << std::endl;

        for (auto d : darts) {
            outfile << "d" << d->id << ";";
            outfile << d->involutions[0]->id << ";";
            outfile << d->involutions[1]->id << ";";
            outfile << d->involutions[2]->id << ";;";
            outfile << d->cell_0->id << ";";
            outfile << d->cell_1->id << ";";
            outfile << d->cell_2->id << std::endl;
        }

        outfile.close();

        std::cout << "# Finished writing." << std::endl;
    } else {
        std::cout << "## Unable to open file!" << std::endl;
    }
}

void write_vertices(std::string filepath, std::unordered_map<std::string, Vertex*>& vertices_umap) {
    size_t numVertexes = vertices_umap.size();

    std::cout << "--- Writing " << numVertexes << " vertices. ---" << std::endl;
    std::cout << "# Output file path: " << filepath << std::endl;

    std::ofstream outfile("" + filepath,std::ofstream::out);
    if (outfile.is_open()) {
        outfile << "id;dart;x;y;z" << std::endl;

        for (auto v : vertices_umap) {
            outfile << "v" << v.second->id << "; " << v.second->darts[0] << "; " << v.second->point.x << "; " << v.second->point.y << "; " << v.second->point.z;
            outfile << "; a dart going to " << v.second->darts[0]->involutions[0]->cell_0->id << std::endl;
        }

        outfile.close();

        std::cout << "# Finished writing." << std::endl;
    } else {
        std::cout << "## Unable to open file!" << std::endl;
    }
}

void write_edges(std::string filepath, std::vector<Edge*>& edges) {
    size_t num_edges = edges.size();

    std::cout << "--- Writing " << num_edges << " edges. ---" << std::endl;
    std::cout << "# Output file path: " << filepath << std::endl;

    std::ofstream outfile("" + filepath,std::ofstream::out);
    if (outfile.is_open()) {
        outfile << "id;dart;desc" << std::endl;

        for (auto e : edges) {
            outfile << "e" << e->id << "; " << e->dart << "; " << *e << std::endl;
        }

        outfile.close();

        std::cout << "# Finished writing." << std::endl;
    } else {
        std::cout << "## Unable to open file!" << std::endl;
    }
}