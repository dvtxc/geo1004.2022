# Report HW02

Dmitri Visser, 4279913

## Preparation

First the file ``3dbag_v210908_fd2cee53_5910.json`` was downloaded and preprocessed.
As the preprocessing requires *cjio*, this was installed in a virtual environment:

    python -m venv .pyenv
    python -m pip install cjio

The city.json file was preprocessed with the following command:

    cjio 3dbag_v210908_fd2cee53_5910.json upgrade vertices_clean lod_filter 2.2 metadata_remove save clean.city.json

This made sure the resulting city.json file

* is upgraded to the latest city.json version, *i.e. 1.1*
* only LOD 2.2 geometries are present

For small size testing, additional files where generated with ``cjio [input_file] subset`` command, which extracts a small subset of the city.json tile.

The project driver code is contained by ``main.cpp``. The individual sub assignments are contained within separate .cpp / .h files. Some helper functions are written in ``helper.h``.

## Calculating the number of floors

## Roof Surface Orientation

The computation of the roof surface orientation is based on the premise, that all CityJson surfaces are planar.
This enables the calculation of a plane equation, which intersects all points (vertices) in a polygon. 
In principle, only three points are needed to construct a plane equation, which comprises a normal vector and support vector.
Given the points $a$, $b$, and $c$, which intersect with plane $P$, then the plane normal vector $\vec{n}$ is given by:
$$\vec{n}= \left( \vec{b} - \vec{a} \right) \times \left( \vec{b} - \vec{a} \right)$$


Downloaded json file.

Installed cjio:

    python -m venv .pyenv
    python -m pip install cjio

Preprocessed city.json file:

    cjio data\3dbag_v210908_fd2cee53_5910.json upgrade vertices_clean lod_filter 2.2 metadata_remove save data\clean.city.json




To compile and run:

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ ./cjdemo

# Tips given by hugoledoux

Use ``cjio`` to explore the json file. For example:

    cjio myfile.city.json info --long

gives information on semantics surfaces.

Subtract subset from json:

    cjio myfile.city.json subset --random 2 save twobuildings.jso


## Code snippets

````c++
for (auto& co : j["CityObjects"].items()) {
    // return key + value
}
````