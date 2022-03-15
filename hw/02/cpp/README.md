# Steps performed
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