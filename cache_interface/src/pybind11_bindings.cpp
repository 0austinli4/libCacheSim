#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "CacheWrapper.hpp"

namespace py = pybind11;

PYBIND11_MODULE(pylibCacheSim, m) {
    m.doc() = "Python interface for libCacheSim";

    py::class_<CacheWrapper>(m, "Cache")
        .def(py::init<const std::string&, uint64_t>())
        .def("get", &CacheWrapper::get)
        .def("insert", &CacheWrapper::insert)
        .def("evict", &CacheWrapper::evict)
        .def("remove", &CacheWrapper::remove);
} 