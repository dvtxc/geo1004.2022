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

We traverse the surfaces, as given per example:

* first, traverse through every CityObject
* traverse through every ``geometry``, when ``type = "Solid"``
* traverse through all ``boundaries`` (i.e. shells)
* traverse through all ``surfaces``, when the surface is associated with the semantic property ``type = "RoofSurface"``

The computation of the roof surface orientation is based on the premise, that all CityJson surfaces are planar.
This enables the calculation of a plane equation, which intersects all points (vertices) in a polygon. 
In principle, only three points are needed to construct a plane equation, which comprises a normal vector and support vector.
Given the points $a$, $b$, and $c$, which intersect with plane $P$, then the plane normal vector $\vec{n}$ is given by:
$$\vec{n}= \left( \vec{b} - \vec{a} \right) \times \left( \vec{c} - \vec{a} \right)$$

The first approach was to select three random points from the surface and computing the normal vector, based on these three vertices. However, since three randomly selected vertices from a polygon may be (almost) colinear, this method is prone to large errors.

Therefore, it is necessary to take **all** polygon vertices into account. For this, the aforementioned equation has to be generalised for an arbitrary number of vertices. Factorizing the formula for three vertices, we get:
$$\vec{n} = \vec{a} \times \vec{b} + \vec{b} \times \vec{c} + \vec{c} \times \vec{a}$$
This can be generalised for polygons consisting of $n$ vertices $\vec{p_i}$. In that case:
$$\sum_{i=0}^n \vec{p_i} + \vec{p}_{i+1} \qquad \text{(assuming cyclicity)}$$

Implemented in C++, with ``pv`` as a ``std::vector<double3>``.

```c++
double3 n = {0,0,0};
for (int i = 1; i < pv.size(); i++) {
    n += linalg::cross(pv[i-1], pv[i]);
}
n += linalg::cross(pv.back(), pv[0]);
```

In $\mathbb{R}^3$ with cartesian coordinates, the first two vector components denote the $x$ and $y$ components of the normal vector. These two components can be used to extract the orientation of the plane:
$$\tan(\theta) = \frac{y}{x}$$
Using ``atan2(y,x)`` the correct sign will be preserved and we obtain angle $\theta$ in radians in counter-clockwise direction with respect to $[1, 0]$, i.e. $\hat{x}$.

The orientations ``"NW"``, ``"SE"``, ... can than be obtained using a concatenation of simple if-else statements.

### Storing the semantic information

Since every semantic object can only hold one direction, we have to create multiple semantic objects with ``type = "RoofSurface"``, each storing a different direction. Ideally, if we want to use this information to calculate the solar potential of every building, we store **every direction only once**. Later, when the surface area is calculated, every semantic object will hold the total surface area for every roof surface direction.

For this, the code in ``orientation.cpp`` checks whether the calculated orientation has already been stored. If not, it creates new semantic object and reassigns the semantic id of the current surface. If the orientation is found, the id belonging to that semantic object is assigned to the currently evaluated surface.

## Total surface area

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