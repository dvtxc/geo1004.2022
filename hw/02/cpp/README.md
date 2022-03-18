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
$$\vec{n}= \vec{ab} \times \vec {ac} = \left( \vec{b} - \vec{a} \right) \times \left( \vec{c} - \vec{a} \right)$$

The first approach was to select three random points from the surface and computing the normal vector, based on these three vertices. However, since three randomly selected vertices from a polygon may be (almost) colinear, this method is prone to large errors.

Therefore, it is necessary to take **all** polygon vertices into account. For this, the aforementioned equation has to be generalised for an arbitrary number of vertices. Factorizing the formula for three vertices, we get:
$$\vec{n} = \vec{a} \times \vec{b} + \vec{b} \times \vec{c} + \vec{c} \times \vec{a}$$
This can be generalised for polygons consisting of $n$ vertices $\vec{p_i}$. In that case:
$$\sum_{i=0}^n \vec{p_i} + \vec{p}_{i+1} \qquad \textit{(assuming cyclicity)}$$

Implemented in C++, with ``pv`` as a ``std::vector<double3>``.

```c++
double3 compute_plane_from_polygon(std::vector<double3> pv) {

    double3 n = {0,0,0};

    // Expanded form of outer product
    for (int i = 1; i < pv.size(); i++) {
        n += linalg::cross(pv[i-1], pv[i]);
    }

    // Add last term for cyclicity
    n += linalg::cross(pv.back(), pv[0]);

    return linalg::normalize(n);
}
```

In $\mathbb{R}^3$ with cartesian coordinates, the first two vector components denote the $x$ and $y$ components of the normal vector. These two components can be used to extract the orientation of the plane:
$$\tan(\theta) = \frac{y}{x}$$
Using ``atan2(y,x)`` the correct sign will be preserved and we obtain angle $\theta$ in radians in counter-clockwise direction with respect to $[1, 0]$, i.e. $\hat{x}$.

The orientations ``"NW"``, ``"SE"``, ... can than be obtained using a concatenation of simple if-else statements.

### Storing the semantic information

Since every semantic object can only hold one direction, we have to create multiple semantic objects with ``type = "RoofSurface"``, each storing a different direction. Ideally, if we want to use this information to calculate the solar potential of every building, we store **every direction only once**. Later, when the surface area is calculated, every semantic object will hold the total surface area for every roof surface direction.

For this, the code in ``orientation.cpp`` checks whether the calculated orientation has already been stored. If not, it creates new semantic object and reassigns the semantic id of the current surface. If the orientation is found, the id belonging to that semantic object is assigned to the currently evaluated surface.

### Implementation notes

* Only the vertices of the first (outer) ring are used for the computation of the orientation. All consecutive rings (inner rings = holes) are coplanar.

## Total surface area

The surfaces were traversed as described previously. Only surfaces with semantic information ``type = "RoofSurface"`` were taken into consideration.

The calculation of the surface area is based on the triangular version of the shoelace formula. Briefly, the area of a triangle $abc$ is half of the parallelogram spanned by the vectors $\vec{ab}$ and $\vec{ac}$:
$$A = \frac{1}{2} \| \vec{ab} \times \vec {ac} \|$$
By taking the norm of the outer product, the resulting area is always positive. However, if we retain the orientation information of a triangle, it is possible to concatenate this formula and acquire a generalized form that works for every arbitrary polygon. A *positively oriented* sequence of points is counter clockwise, whereas a *negatively oriented* sequence of points is clockwise. Consider an additional point $O$, coplanar to all other points of the polygon $p_i$, the total surface area of a polygon can be obtained by expanding the formula for the area of a triangle in a similar fashion as previously:

$$A = \frac{1}{2} \left| \left| \sum_{i=0}^n \vec{Op_i} \times \vec{Op_{i+1}} \right| \right| \qquad \textit{(assuming cyclicity)}$$

By taking the norm *after* summing over all normal vectors, positively oriented triangles are added to the total polygon area, whereas negatively oriented triangles are subtracted. Instead of creating a new point $O$, it is also possible to take one of the polygon vertices instead, as it is already coplanar with the other vertices by definition of a surface. For convenience, it is possible to take the first polygon vertex, then the implementation in C++ becomes:
```C++
double calculate_polygon_area(std::vector<double3> polygon) {

    // Return zero when no triangle is provided
    if (polygon.size() < 3) return 0.0;

    double area = 0.0;
    double3 crossum = {0, 0, 0};

    // Shoelace formula, triangular form.
    // Take first vertex as origin.
    for (int p = 1; p < polygon.size() - 1; p++) {
        crossum += linalg::cross(polygon[p] - polygon[0], polygon[p+1] - polygon[0]);
    }
    area = 0.5 * linalg::length(crossum);

    return area;
}
```



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