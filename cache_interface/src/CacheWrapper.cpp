#include "CacheWrapper.hpp"

cache_t* CacheWrapper::create_cache(const std::string& algorithm, uint64_t cache_size) {
    common_cache_params_t params = {
        .cache_size = cache_size,
        .default_ttl = 86400 * 300,  // 300 days
        .hashpower = 24,
        .consider_obj_metadata = false
    };

    if (algorithm == "FIFO") {
        return FIFO_init(params, NULL);
    } else if (algorithm == "FIFO_Reinsertion") {
        return FIFO_Reinsertion_init(params, NULL);
    } else if (algorithm == "LFU") {
        return LFU_init(params, NULL);
    } else if (algorithm == "LRU") {
        return LRU_init(params, NULL);
    } else if (algorithm == "S3FIFO") {
        return S3FIFO_init(params, NULL);
    } else if (algorithm == "Sieve") {
        return Sieve_init(params, NULL);
    } else {
        throw std::runtime_error("Unsupported cache algorithm: " + algorithm);
    }
}

CacheWrapper::CacheWrapper(const std::string& algorithm, uint64_t cache_size) 
    : cache(nullptr), request(nullptr), next_id(1) {
    cache = create_cache(algorithm, cache_size);
    request = new_request();
    if (!cache || !request) {
        throw std::runtime_error("Failed to initialize cache");
    }
}

CacheWrapper::~CacheWrapper() {
    if (cache) {
        cache->cache_free(cache);
    }
    if (request) {
        free_request(request);
    }
}

bool CacheWrapper::get(const std::string& key) {
    auto it = key_to_id.find(key);
    if (it == key_to_id.end()) {
        return false;
    }

    request->obj_id = it->second;
    return cache->get(cache, request);
}

bool CacheWrapper::insert(const std::string& key, uint64_t size) {
    // Check if key already exists
    if (key_to_id.find(key) != key_to_id.end()) {
        return false;
    }

    // Assign new ID
    uint64_t id = next_id++;
    key_to_id[key] = id;
    id_to_key[id] = key;

    // Prepare request
    request->obj_id = id;
    request->obj_size = size;

    // Insert into cache
    return cache->insert(cache, request) != nullptr;
}

std::string CacheWrapper::evict() {
    cache_obj_t* obj = cache->to_evict(cache, request);
    if (!obj) {
        throw std::runtime_error("No items to evict");
    }

    uint64_t id = obj->obj_id;
    std::string key = id_to_key[id];

    // Remove from our mappings
    key_to_id.erase(key);
    id_to_key.erase(id);

    // Evict from cache
    cache->evict(cache, request);

    return key;
}

bool CacheWrapper::remove(const std::string& key) {
    auto it = key_to_id.find(key);
    if (it == key_to_id.end()) {
        return false;
    }

    uint64_t id = it->second;
    bool success = cache->remove(cache, id);

    if (success) {
        key_to_id.erase(key);
        id_to_key.erase(id);
    }

    return success;
} 