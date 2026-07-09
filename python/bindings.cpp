#include <nanobind/nanobind.h>
#include "../include/byte_pair_encoder.hpp"

#include <nanobind/stl/string.h>
#include <nanobind/stl/unordered_map.h>
#include <nanobind/stl/vector.h>
#include <nanobind/ndarray.h>

namespace nb = nanobind;
using namespace nb::literals;

NB_MODULE(Scribe, m) {
    m.doc() = "A fast and simple module to tokenize text data.";

    nb::class_<Scribe::BytePairEncoder>(m, "BytePairEncoder", "A module that applies BPE algorithm on the given text to generate word/sub-word tokens."
    ).def(nb::init<>()
    ).def("train", &Scribe::BytePairEncoder::train, nb::arg("filename"), nb::arg("cycles"), nb::arg("verbose"),
        "Trains the tokenizer on text data contained in `filename` file.\n\n"
        "Args:\n"
        "   filename (str): Path to the training data (taken relative from the directory the function will be called in).\n"
        "   cycles (int): Number of cycles to train the tokenizer for, total vocab size will be 256 + `cycles`.\n"
        "   verbose (bool): If true, prints log of every merge cycle to standard output.\n"
        "Returns:\n"
        "   None"
    ).def("encode", &Scribe::BytePairEncoder::encode, nb::arg("data"),
        "Encoder function to convert text data into tokens.\n\n"
        "Args:\n"
        "   data (str): Text data to encode.\n"
        "Returns:\n"
        "   list[int]"
    ).def("decode", &Scribe::BytePairEncoder::decode, nb::arg("tokens"),
        "Decoder function to convert tokens into string data.\n\n"
        "Args:\n"
        "   tokens (list[int]): List of tokens to decode.\n"
        "Returns:\n"
        "   list[int]"
    );
}
