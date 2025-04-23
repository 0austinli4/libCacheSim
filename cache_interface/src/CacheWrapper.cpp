#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <stdexcept>
#include <iostream>
#include <functional>  
#include <chrono>

extern "C" {
#include "../../libCacheSim/include/libCacheSim/cache.h"
#include "../../libCacheSim/include/libCacheSim/evictionAlgo.h"
#include "../../libCacheSim/include/libCacheSim/request.h"
}

namespace py = pybind11;

class CacheWrapper {
public:
    CacheWrapper(const std::string& algo, int cache_size) {
        common_cache_params_t params;
        params.cache_size = cache_size;
        params.default_ttl = 0;
        params.hashpower = 0;
        params.consider_obj_metadata = false;

        if (algo == "LRU") {
            cache = LRU_init(params, nullptr);
        } else if (algo == "FIFO") {
            cache = FIFO_init(params, nullptr);
        } else if (algo == "LFU") {
            cache = LFU_init(params, nullptr);
        } else if (algo == "S3FIFO") {
            cache = S3FIFO_init(params, nullptr);
        } else if (algo == "Sieve") {
            cache = Sieve_init(params, nullptr);
        } else {
            throw std::invalid_argument("Unknown algorithm: " + algo);
        }
        if (!cache) throw std::runtime_error("Failed to initialize cache");
    }

    ~CacheWrapper() {
        if (cache) {
            cache->cache_free(cache);
        }
    }

    bool get(const std::string& key) {
        request_t* req = create_request(key, 0);
        int result = cache->get(cache, req);
        free_request(req);
        return result; 
    }

    bool insert(const std::string& key, int size) {
        // std::cerr << "[DEBUG][CacheWrapper::insert] Inserting key: '" << key << "', size: " << size << std::endl;
        request_t* req = nullptr;

        try {
            req = create_request(key, size);
        } catch (const std::exception& e) {
            // std::cerr << "[ERROR][CacheWrapper::insert] Failed to create request for key: '" << key << "', error: " << e.what() << std::endl;
            return false;
        }
        cache_obj_t* result = nullptr;
        if (cache && req) {
            result = cache->insert(cache, req);
            if (result) {
                // Store mapping from obj_id to original key
                std::hash<std::string> hasher;
                objid_to_key[static_cast<uint64_t>(hasher(key))] = key;
            }
            // std::cerr << "[DEBUG][CacheWrapper::insert] Insert result: " << (result ? "success" : "failure") << std::endl;
        } else {
            std::cerr << "[ERROR][CacheWrapper::insert] Cache or request is null." << std::endl;
        }
        free_request(req);
        return result != nullptr;
    }

    bool remove(const std::string& key) {
        request_t* req = create_request(key, 0);
        int result = cache->remove(cache, req->obj_id);
        free_request(req);
        return result == 1;
    }

    pybind11::dict find(const std::string& key, bool update_cache = false) {
        request_t* req = create_request(key, 0);
        pybind11::dict result;
        if (cache && cache->find) {
            cache_obj_t* obj = cache->find(cache, req, update_cache);
            if (obj) {
                result["found"] = true;
                result["obj_id"] = +obj->obj_id;
                result["obj_size"] = +obj->obj_size;
            } else {
                result["found"] = false;
            }
        } else {
            result["found"] = false;
        }
        free_request(req);
        return result;
    }

public:
    // Return the original string key of the object to be evicted
    std::string to_evict() {
        if (cache && cache->to_evict) {
            cache_obj_t* obj = cache->to_evict(cache, nullptr);
            if (obj) {
                uint64_t obj_id = obj->obj_id;
                auto it = objid_to_key.find(obj_id);
                if (it != objid_to_key.end()) {
                    return it->second;
                } else {
                    // fallback: return as string
                    return std::to_string(obj_id);
                }
            }
        }
        return "";
    }

    // Evict the object as determined by the cache's eviction policy
    void evict() {
        if (cache && cache->evict) {
            cache->evict(cache, nullptr);
        }
    }

private:
    cache_t* cache;
    std::unordered_map<uint64_t, std::string> objid_to_key;

    // Helper to create a request_t for a string key and size
    request_t* create_request(const std::string& key, int size) {
        request_t* req = new_request();
        try {
            // std::cerr << "[DEBUG] Attempting to convert key to obj_id: '" << key << "'" << std::endl;
            std::hash<std::string> hasher;
            req->obj_id = static_cast<unsigned long long>(hasher(key));
        } catch (const std::exception& e) {
            // std::cerr << "[ERROR] std::stoull failed for key: '" << key << "', error: " << e.what() << std::endl;
            throw;
        }
        req->obj_size = size;
        req->clock_time = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
        return req;
    }
};

PYBIND11_MODULE(pylibCacheSim, m) {
    py::class_<CacheWrapper>(m, "Cache")
        .def(py::init<const std::string&, int>())
        .def("get", &CacheWrapper::get)
        .def("insert", &CacheWrapper::insert)
        .def("remove", &CacheWrapper::remove)
        .def("find", &CacheWrapper::find,
             py::arg("key"), py::arg("update_cache") = false,
             R"pb(
                 find(key: str, update_cache: bool = False) -> dict
                 Returns a dict with keys: found (bool), obj_id (int), obj_size (int) if found
             )pb")
        .def("to_evict", &CacheWrapper::to_evict,
             R"pb(
                 to_evict() -> str
                 Returns the original string key of the object that should be evicted according to the current policy.
             )pb")
        .def("evict", &CacheWrapper::evict,
             R"pb(
                 evict() -> None
                 Evicts the object as determined by the cache's eviction policy.
             )pb");
}