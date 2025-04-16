from cache import Cache

# Create a cache
cache = Cache("FIFO", 1000000)  # 1MB cache

# Insert items
cache.insert("key1", 100)
cache.insert("key2", 200)

# Check if items exist
print(cache.get("key1"))  # True
print(cache.get("key3"))  # False

# Remove an item
cache.remove("key1")

# Evict an item
evicted_key = cache.evict()
