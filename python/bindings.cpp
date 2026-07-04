#include <nanobind/nanobind.h>
#include "../include/tokenizer.hpp"

namespace nb = nanobind;
using namespace nb::literals;

NB_MODULE(Scribe, m) {
    nb::class_<Scribe::BytePairEncoding>(m, "BytePairEncoding")
        .def(nb::init<>());
}
