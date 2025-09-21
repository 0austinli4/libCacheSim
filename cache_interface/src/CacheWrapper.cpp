#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <stdexcept>
#include <iostream>
#include <functional>  
#include <chrono>
#include <sstream>
#include <optional>

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
        } else if (algo == "ARC") {
            cache = ARC_init(params, nullptr);
        } else if (algo == "Clock") {
            cache = Clock_init(params, nullptr);
        } else if (algo == "FIFO_Reinsertion") {
            cache = FIFO_Reinsertion_init(params, nullptr);
        } else if (algo == "GDSF") {
            cache = GDSF_init(params, nullptr);
        } else if (algo == "Cacheus") {
            cache = Cacheus_init(params, nullptr);
        } else if (algo == "LeCaR") {
            cache = LeCaR_init(params, nullptr);
        } else if (algo == "Hyperbolic") {
            cache = Hyperbolic_init(params, nullptr);
        } else if (algo == "MRU") {
            cache = MRU_init(params, nullptr);
        } else if (algo == "SLRU") {
            cache = SLRU_init(params, nullptr);
        } else if (algo == "LIRS") {
            cache = LIRS_init(params, nullptr);
        } else {
            throw std::runtime_error("Unknown cache algorithm: " + algo);
        }

        if (!cache) {
            std::cerr << "[ERROR] Failed to initialize cache for algo=" << algo << std::endl;
            throw std::runtime_error("Failed to initialize cache");
        }
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
        request_t* req = nullptr;

        try {
            req = create_request(key, size);
        } catch (const std::exception& e) {
            std::cerr << "[ERROR][CacheWrapper::insert] Failed to create request for key: '" << key << "', error: " << e.what() << std::endl;
            return false;
        }
        cache_obj_t* result = nullptr;
        if (cache && req) {
            result = cache->insert(cache, req);
            if (result) {
                // Store mapping from obj_id to original key
                std::hash<std::string> hasher;
                uint64_t hash_val = static_cast<uint64_t>(hasher(key));
                objid_to_key[hash_val] = key;
            }
        } else {
            std::cerr << "[ERROR][CacheWrapper::insert] Cache or request is null." << std::endl;
        }
        free_request(req);
        return result != nullptr;
    }

    bool remove(const std::string& key) {
        request_t* req = create_request(key, 0);
        // std::cerr << "[DEBUG][CacheWrapper::remove] key: '" << key << "', obj_id: " << req->obj_id << std::endl;
        int result = cache->remove(cache, req->obj_id);
        // std::cerr << "[DEBUG][CacheWrapper::remove] remove result: " << result << std::endl;
        free_request(req);
        return result == 1;
    }

    pybind11::dict find(const std::string& key, bool update_cache = false) {
        // std::cout << "Called: CacheWrapper::find" << std::endl;
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
        // std::cout << "Called: CacheWrapper::to_evict" << std::endl;
        if (cache && cache->to_evict) {
            // std::cerr << "[DEBUG][CacheWrapper::to_evict] About to call cache->to_evict" << std::endl;
            
            // if (!cache) {
            //     std::cerr << "[ERROR] cache is null before to_evict" << std::endl;
            // }
            // if (!cache->to_evict) {
            //     std::cerr << "[ERROR] cache->to_evict is null before call!" << std::endl;
            // }
            // std::cerr << "[DEBUG] cache pointer: " << cache << std::endl;
            // std::cerr << "[DEBUG] cache->to_evict pointer: " << (void*)cache->to_evict << std::endl;
            
            cache_obj_t* obj = cache->to_evict(cache, nullptr);
            // std::cerr << "[DEBUG][CacheWrapper::to_evict] Returned from cache->to_evict" << std::endl;
            // if (!cache) {
            //     std::cerr << "[ERROR] cache is null in to_evict" << std::endl;
            // }
            // if (!cache->to_evict) {
            //     std::cerr << "[ERROR] cache->to_evict is null!" << std::endl;
            // }
            if (obj) {
                uint64_t obj_id = obj->obj_id;
                // std::cerr << "[DEBUG][CacheWrapper::to_evict] obj_id: " << obj_id << std::endl;
                auto it = objid_to_key.find(obj_id);
                if (it != objid_to_key.end()) {
                    // std::cerr << "[DEBUG][CacheWrapper::to_evict] Found obj_id in objid_to_key. key: '" << it->second << "'" << std::endl;
                    return it->second;
                } else {
                    // std::cerr << "[DEBUG][CacheWrapper::to_evict] obj_id NOT found in objid_to_key. Returning string version of obj_id." << std::endl;
                    // fallback: return as string
                    return std::to_string(obj_id);
                }
            } else {
                // std::cerr << "[DEBUG][CacheWrapper::to_evict] cache->to_evict returned nullptr obj." << std::endl;
            }
        } else {
            // std::cerr << "[DEBUG][CacheWrapper::to_evict] cache or cache->to_evict is null." << std::endl;
        }
        return "";
    }

    // Evict the object as determined by the cache's eviction policy
    void evict() {
        // std::cout << "Called: CacheWrapper::evict" << std::endl;
        if (cache && cache->evict) {
            cache->evict(cache, nullptr);
        }
    }

private:
    cache_t* cache;
    std::unordered_map<uint64_t, std::string> objid_to_key;

    request_t* create_request(const std::string& key, int size) {
        request_t* req = new_request();
        try {
            // First, try to find the key in objid_to_key (if you have access here)
            // If not found, check if key is an integer string
            uint64_t obj_id = 0;
            std::istringstream iss(key);
            if ((iss >> obj_id) && iss.eof()) {
                req->obj_id = obj_id;
                // std::cerr << "[DEBUG][CacheWrapper::create_request] Key '" << key << "' interpreted as integer obj_id: " << obj_id << std::endl;
            } else {
                // Fallback to hashing
                std::hash<std::string> hasher;
                req->obj_id = static_cast<unsigned long long>(hasher(key));
                // std::cerr << "[DEBUG][CacheWrapper::create_request] Key '" << key << "' hashed to obj_id: " << req->obj_id << std::endl;
            }
        } catch (const std::exception& e) {
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