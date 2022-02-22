# HW01

**Group members:**
* Dmitri Visser
* Pam ...
* Maren ...

## Reading the .obj file
The .obj file is read in ``read_obj()``.
This function reads the vertices and stores them into a vector ``vertices``.
It also reads the faces and creates the faces with references to the vertices and stores the faces in the vector ``faces``.

## Creating g-map
1. Loop through every face
2. Construct darts from a single face
3. to implement: create ``std::unordered_map`` for vertices and create 0 cell table
4. to implement: create unique edges

## Data structure
The data in ``Dart`` is subdivided into a **combinatorial** and an **embedding** part:

* Combinatorial part

    This part is implemented with the ``involutions`` property. 
    It stores a vector of pointers to other darts.

* Embedding part
    
    This part is implemented with the ``cells``


# Tips given by *rypeters*
* Constructing the gmap itself does not require you to triangulate anything or compute any barycenters.
* Only for the triangulated output (which comes after you have a complete gmap), you need to start worrying about the barycenters.
* Also notice you do not need to create tetrahedra. We just triangulate the surface of the mesh.
* alpha_3 is always the null pointer

# General Instructions
To compile and run:

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ ./hw01

You can also open the `cpp` folder directly in CLion.