from pylibCacheSim import Cache

def test_eviction_comparison():
    print('--- LRU vs FIFO Eviction Test ---')
    # Small cache for demonstration
    cache_size = 3
    lru_cache = Cache("LRU", cache_size)
    fifo_cache = Cache("FIFO", cache_size)

    # Insert three items
    keys = ['C', 'B', 'A']
    for k in keys:
        lru_cache.insert(k, 1)
        fifo_cache.insert(k, 1)

    # Access 'A' in LRU to make it most recently used
    lru_cache.get('A')

    # Insert another item to trigger eviction
    lru_cache.insert('D', 1)
    fifo_cache.insert('D', 1)

    # Check which key would be evicted next
    print('LRU to_evict():', lru_cache.to_evict())
    print('FIFO to_evict():', fifo_cache.to_evict())
    print('Expected: LRU evicts least recently used (B), FIFO evicts first inserted (A)')

if __name__ == "__main__":
    test_eviction_comparison()