#ifndef _MEMORY_ALLOCATOR_H_
#define _MEMORY_ALLOCATOR_H_

#include <vector>
#include <include/utransport/datamodel/UStatus.h>

using namespace std;
using namespace uprotocol::utransport;

class MemoryAllocator
{
    public:
 
        /**
        * initialize memory handler
        * @return OK on success , error on failure
        */
        virtual UStatus initialize() = 0;
 
        /**
        * terminate memory handler
        * @return OK on success , error on failure
        */
        virtual UStatus terminate() = 0;
         
        /**
        * allocate shared memory buffer (ref counter will be increased)
        * @return pointer to a shared memory buffer, nullptr in case of failure
        */
        virtual uint8_t* allocate() = 0;
         
        /**
        * free shared memory buffer in case that the memory was not used  (ref counter will be decreased)
        * @param addr - address to free
        * @return OK on success , error on failure
        */
        virtual UStatus free(uint8_t *addr) = 0;
 
        /**
        * ged addresses of all memory addresses (ref counter will not be increased)
        * @return vector on success , nullopt on failure
        */
        virtual std::optional<std::vector<uint8_t*>> getAddresses() = 0;
};

#endif /* _MEMORY_ALLOCATOR_H_ */