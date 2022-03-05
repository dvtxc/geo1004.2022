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

// Function to find unique edges with the help of the vertices umap
void construct_unique_edges_vertexHash(Face& face,
                                       std::vector<Dart*> face_darts,
                                       std::unordered_map<std::string, Vertex*>& vertices_umap,
                                       std::vector<Edge*>& edges);

void triangulate_face(Face& face,
                      std::vector<Vertex>& new_vertices,
                      std::vector<Face>& new_faces,
                      int& num_orig_vtx);

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
    std::vector<Dart*> face_darts;

    // Go through every face
    for(Face& face : faces) {
        // Get darts belonging to face, including:
        // a0 and a1 involutions
        // 0-cell and 2-cell
        face_darts = construct_darts_from_face(face, vertices_umap, edges);

        // Store all darts of this face. We will access it later again.
        face.darts = face_darts;

        // Append to darts
        darts.insert(darts.end(), face_darts.begin(), face_darts.end());

        // Number the darts nicely
        for (auto dart : face_darts) {
            dart->id = num_darts;
            num_darts++;
        }
    }


    // ## Output generalised map to CSV ##
    write_vertices(R"(..\data\vertices.csv)", vertices_umap);
    write_edges(R"(..\data\edges.csv)", edges);
    write_darts(R"(..\data\darts.csv)", darts);

    // -- Dimi code

    // Store new triangulated faces in new vector
    std::vector<Face> new_faces;

    // Store new vertices in a new vector, because if we would push new triangulated vertices to the original vector, we
    // might invalidate references to those vertices upon memory reallocation.
    std::vector<Vertex> new_vertices;
    int num_orig_vtx = vertices.size();

    for (Face& face : faces) {

        triangulate_face(face, new_vertices, new_faces, num_orig_vtx);

    }
    //


    // ## Create triangles from the darts ##
    // create vector "new_faces" where the triangles will be stored
    //std::vector<Face*> new_faces;
    /*std::vector<Vertex> new_vertices = vertices;

    for(Face& face : faces) {
        std::vector<Dart *> face_darts = construct_darts_from_face(face, vertices_umap, edges);

        // use function barycenter for the faces to compute the barycenter coordinates of the face
        //Point vp = face.barycenter(*face.points[0], *face.points[1], *face.points[2], *face.points[3]);
        const double vp_x =  vp.x;
        const double vp_y =  vp.y;
        const double vp_z =  vp.z;
        // store barycenter of the faces in vertex cp
        Vertex cp = Vertex(vp_x, vp_y, vp_z);
        new_vertices.push_back(cp);

        // store the id of the faces
        std::vector<int> face_edges_id;
        std::vector<Vertex> barycenter_edges;

        for(Dart*& dart: face_darts) {
            // store the darts of an edge
            Edge edge_dart = *dart->cell_1;
            // find id of the dart of the edge
            int id = edge_dart.id;

            if (std::find(face_edges_id.begin(), face_edges_id.end(), id) != face_edges_id.end()) {
                //pass
            } else {
                face_edges_id.push_back(id);
                // find the two vertices (vi and vj) of the edge
                Vertex vi = *dart->cell_1->points[0];
                Vertex vj = *dart->cell_1->points[1];
                // compute the barycenter point of the edges with vertices vi and vj
                Point vij = edge_dart.barycenter(vi, vj);

                // store the x,y,z values for the barycenter point at the edge
                const double vij_x =  vij.x;
                const double vij_y =  vij.y;
                const double vij_z =  vij.z;
                // convert the x,y,z values to a vertex cij which contains the barycenter point of the edge
                Vertex cij = Vertex(vij_x, vij_y, vij_z);
                new_vertices.push_back(cij); //Naar kijken!


                // create two triangles (face1 and face2)
                // face1 consists out of vi (original vertex of the face), cij (barycenter vertex of the edge), cp (barycenter vertex of the face)
                Face face1 = Face(vi, cij, cp);
                std::cout << vi.id << " " << cij << " " << cp << std::endl;
                // face2 consists out of vj (original vertex of the face), (barycenter vertex of the face), cij (barycenter vertex of the edge)
                Face face2 = Face(vj, cp, cij);
                std::cout << vj.id << " " << cp << " " << cij << std::endl;

                // store both created faces within the new_faces vector
                new_faces.push_back(&face1);
                new_faces.push_back(&face2);

            }

        }

    }
*/

    std::string filepath = R"(C:\dev\geo\geo1004\geo1004.2022\hw\01\data\triangulated.obj)";
    size_t num_triangles = new_faces.size();
    std::cout << "--- Writing " << num_triangles << " triangles. ---" << std::endl;
    std::cout << "# Output file path: " << filepath << std::endl;
    std::ofstream outfile("" + filepath,std::ofstream::out);
    std::cout << new_vertices.size() << std::endl;
    if (outfile.is_open()) {

        for (auto v: vertices) { outfile << "v " << v.point.x << " " << v.point.y << " " << v.point.z << std::endl; }

        for (auto v: new_vertices) {  outfile << "v " << v.point.x << " " << v.point.y << " " << v.point.z << std::endl;  }

        for (Face f: new_faces) {
            outfile << "v";

            // go through all points of the face
            for (Vertex* fp : f.points) {
                outfile << " " << fp->id + 1; // increment by 1, as .obj file refers to lines that start at 1
            }

            outfile << std::endl;
        }

    }

    //write_triangles("C:\\dev\\geo\\geo1004\\geo1004.2022\\hw\\01\\data\\triangulated.obj", new_vertices, new_faces);
  
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
    int vertex_id = 0;
    int face_id = 0;

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
            face_id++;
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
                    opposite_dart = candidate_dart;
                }
            }
        }

        if (opposite_dart) {
            Edge *common_edge = opposite_dart->cell_1;

            //std::cout << "Common edge e" << common_edge->id << " " << *common_edge << "" << std::endl;

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

        } else {

            // Create edge
            Edge *edge = new Edge(*vtx_curr, *vtx_next);
            edge->dart = dart_next;
            edges.push_back(edge);

            edge->id = edges.size() - 1;

            // Assign edge as 1-cell to darts
            dart_next->cell_1 = edge;
            dart_next->involutions[0]->cell_1 = edge;

            // Set incident dart of this vertex, CCW in the current face
            vtx_curr->darts.push_back(dart_next);

            // Add vertex to umap
            vertices_umap[vtx_curr->gen_key()] = vtx_curr;

        }

    }
}

void triangulate_face(Face& face,
                      std::vector<Vertex>& new_vertices,
                      std::vector<Face>& new_faces,
                      int& num_orig_vtx) {

    std::vector<Dart*> face_darts = face.darts;

    // Calculate face barycenter
    Point vp = face.barycenter();
    Vertex cp = Vertex(vp.x, vp.y, vp.z);
    new_vertices.push_back(cp);
    new_vertices.back().id = num_orig_vtx + new_vertices.size(); // size() does end() - begin(), so it should be fast, right?

    for (size_t i = 0; i < face_darts.size(); i += 2) {
        // Take the dart that is pointing in CCW direction, thus [1], [3], etc ...
        Dart *dart;
        dart = face_darts[i + 1];

        // Edge barycenter
        Point edge_bc = dart->cell_1->barycenter();
        Vertex edge_bc_vtx = Vertex(edge_bc.x, edge_bc.y, edge_bc.z);
        edge_bc_vtx.id = num_orig_vtx + new_vertices.size();
        new_vertices.push_back(edge_bc_vtx);

        // Create triangle 1
        Face face1 = Face(*dart->cell_0, edge_bc_vtx, cp);
        //face1.id = new_faces.size();
        new_faces.push_back(face1);

        // Create triangle 2
        Face face2 = Face(edge_bc_vtx, *dart->involutions[0]->cell_0, cp);
        //face2.id = new_faces.size();
        new_faces.push_back(face2);
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
            outfile << d->involutions[2]->id << ";";
            outfile << ";";
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
