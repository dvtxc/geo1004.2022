# HW01

**Group members:**
* Dmitri Visser, **4279913**
* Pam ...
* Maren Hengelmolen


# General Instructions
To compile and run:

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ ./hw01

You can also open the `cpp` folder directly in CLion.

# Implementation details

## Reading the .obj file
The .obj file is read in ``read_obj()``.
This function reads the vertices and stores them into a vector ``vertices``.
It also reads the faces and creates the faces with references to the vertices and stores the faces in the vector ``faces``.

The torus obj file is normally read from the relative directory ``../../data/torus.obj``. All output files will be stored in this folder as well.

## Creating g-map
1. Loop through every face
2. Construct darts from a single face
3. Vertices are added to an unordered map.
4. Unique edges are constructed.

## Data structure
The data in ``Dart`` is subdivided into a **combinatorial** and an **embedding** part:

* Combinatorial part

    This part is implemented with the ``involutions`` property. 
    It stores a vector of pointers to other darts.

* Embedding part
    
    This part is implemented with the ``cell_i`` properties.
