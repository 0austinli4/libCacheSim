#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>
#include <stdexcept>
#include <memory>

#include "../../libCacheSim/include/libCacheSim/cache.h"
#include "../../libCacheSim/include/libCacheSim/evictionAlgo.h"

class CacheWrapper {
private:
    cache_t* cache;
    request_t* request;
    std::unordered_map<std::string, uint64_t> key_to_id;
    std::unordered_map<uint64_t, std::string> id_to_key;
    uint64_t next_id;

    cache_t* create_cache(const std::string& algorithm, uint64_t cache_size);

public:
    CacheWrapper(const std::string& algorithm, uint64_t cache_size);
    ~CacheWrapper();
    
    bool get(const std::string& key);
    bool insert(const std::string& key, uint64_t size);
    std::string evict();
    bool remove(const std::string& key);
}; 