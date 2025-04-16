import pylibCacheSim

class Cache:
    def __init__(self, algorithm: str, cache_size: int):
        """Initialize cache with specified algorithm and size
        
        Args:
            algorithm: One of 'FIFO', 'FIFO_Reinsertion', 'LFU', 'LRU', 'S3FIFO', 'Sieve'
            cache_size: Size of cache in bytes
        """
        self._cache = pylibCacheSim.Cache(algorithm, cache_size)

    def get(self, key: str) -> bool:
        """Check if key exists in cache
        
        Args:
            key: String key to check
            
        Returns:
            bool: True if key exists in cache, False otherwise
        """
        return self._cache.get(key)

    def insert(self, key: str, size: int) -> bool:
        """Insert key with size into cache
        
        Args:
            key: String key to insert
            size: Size of the object in bytes
            
        Returns:
            bool: True if insertion was successful, False if key already exists
        """
        return self._cache.insert(key, size)

    def evict(self) -> str:
        """Evict an item from cache
        
        Returns:
            str: The key of the evicted item
            
        Raises:
            RuntimeError: If cache is empty
        """
        return self._cache.evict()

    def remove(self, key: str) -> bool:
        """Remove specific key from cache
        
        Args:
            key: String key to remove
            
        Returns:
            bool: True if key was removed, False if key was not found
        """
        return self._cache.remove(key)
