#ifndef _MEMORY_ALLOCATOR_H_
#define _MEMORY_ALLOCATOR_H_

class MemoryAllocator
{
    public:
 
        /**
        * initialize memory handler
        * @return OK on success , error on failure
        */
        UStatus initialize() = 0;
 
        /**
        * terminate memory handler
        * @return OK on success , error on failure
        */
        UStatus terminate() = 0;
         
        /**
        * allocate shared memory buffer (ref counter will be increased)
        * @return pointer to a shared memory buffer, nullptr in case of failure
        */
        uint8_t* allocate() = 0;
         
        /**
        * free shared memory buffer in case that the memory was not used  (ref counter will be decreased)
        * @param addr - address to free
        * @return OK on success , error on failure
        */
        UStatus free(uint8_t &addr) = 0;
 
        /**
        * ged addresses of all memory addresses (ref counter will not be increased)
        * @return vector on success , nullopt on failure
        */
        std::optional<std::vector<uint8_t*>> getAddresses() noexcept;
}

#endif /* _MEMORY_ALLOCATOR_H_ */