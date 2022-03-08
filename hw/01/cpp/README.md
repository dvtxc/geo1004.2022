# HW01

**Group members:**
* Dmitri Visser, **4279913**
* Pam ...
* Maren Hengelmolen, **4294068**


# General Instructions

To compile and run on Linux:

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ ./hw01 [DATA_ROOT_DIR]

Provide ``ROOT_DIR`` with trailing slash. If no root directory is provided, the input files will be retrieved from ``../../data``.

You can also open the `cpp` folder directly in CLion.

# Implementation details

_See for more detailed information the comment lines._

## Reading the .obj file
The .obj file is read in ``read_obj()``.
This function reads the vertices and stores them into a vector ``vertices``.
It also reads the faces and creates the faces with references to the vertices and stores the faces in the vector ``faces``.

The torus obj file is normally read from the relative directory ``../../data/torus.obj``. All output files will be stored in this folder as well.

## Creating g-map
1. Loop through every face
2. Construct darts from a single face. Convention: darts are numbered and constructed in CCW direction.
3. Vertices are added to an unordered map.
4. Unique edges are constructed, when the vertices are not found in the unordered map. ``Vertex::gen_key()`` is used to generate a key for the unordered map.
``set_precision(4)`` ensures that floating point coordinates can be reliably compared.

## Triangulation
1. First, every edge is bisected, using ``barycenter()``. The barycentric point is stored within the edge, so it can be retrieved later on.
An unordered map is used here as well to prevent pointer invalidation, as vertices are added on the fly.
2. Loop through every face and calculate the barycentric point of every face.
3. Triangulize every face, by looping through the darts and retrieving the bisection vertices from the edges.

## Data structure
The data in ``Dart`` is subdivided into a **combinatorial** and an **embedding** part:

* Combinatorial part

    This part is implemented with the ``involutions`` property. 
    It stores a vector of pointers to other darts.

* Embedding part
    
    This part is implemented with the ``cell_i`` properties.
