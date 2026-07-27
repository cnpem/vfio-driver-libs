#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <linux/vfio.h>
#include <stdexcept>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

struct data_buffer {
    volatile void *data = NULL;
    size_t size = 0;
};

class VFIO {
public:
    VFIO(const std::string &device_name, int32_t iommu_group_id);
    ~VFIO();

    // VFIO user-defined properties
    const std::string device_name; // DBDF
    int iommu_group_id; // IOMMU group number
    uint64_t iova; // Device virtual address space for DMA operations

    // Memory management utils
    /**
     * @brief Allocate a buffer on the process address space
     * @description Receives mmap's arguments, initializes a data_buffer struct,
     * stores in it a pointer to an mmaped region with the given parameters and
     * its size. Consult mmap's manual page for expected values on each argument
     * @param[in] size Size of the region
     * @param[in] prot Desired memory protection of the mapping
     * @param[in] flags Determines wheter updates to the mapping are visible to
     * other processes mapping the same region
     * @param[in] fd File descriptor reference, in case of a file mapping.
     * Defaults for -1 for anonymous mappings
     * @param[in] offset Offset in the file, in case of a file a mapping.
     * Defaults for -1 for anonymous mappings
     * @return Struct with reference to region
     */
    data_buffer alloc_data_buffer(
        size_t size, int prot, int flags, int fd = -1, off_t offset = 0);

    /**
     * @brief Unallocate buffer on the process address space
     * @description Receives an initialized data_buffer struct, unmaps its
     * referenced memory region and returns its fields to empty values
     * @param[in] buffer Reference to buffer
     */
    void unalloc_data_buffer(data_buffer &buffer);

    // VFIO user interface setup methods
    //
    /**
     * @brief Maps an empty memory region in the device IOAS for DMA transfers
     * @description Receives an initialized data_buffer struct, maps its
     * referenced memory region
     * @param[in] buffer Reference to data_buffer struct to be mapped
     */
    void dma_map_buffer(data_buffer &buffer);

    /**
     * @brief Maps an indexed memory region in the device VFIO file descriptor
     * @description Gets information of the region on the correspondent index
     * via IOCTL and initializes a data_buffer with its base address and size
     * @param[in] index Index of memory region in the file descriptor, expected
     * to be VFIO's interface BAR region index enum region
     * @return Struct with reference to region
     */
    data_buffer map_mem_region(uint32_t index);

private:
    // VFIO interface setup properties
    int container, group, device; // VFIO File descriptors

    // VFIO interface internal setup methods
    /**
     * @brief Opens VFIO container
     */
    void set_container();

    /**
     * @brief Opens IOMMU group
     */
    void set_group();

    /**
     * @brief Includes an open IOMMU group in an open VFIO container
     */
    void set_group_to_container();

    /**
     * @brief Opens a device VFIO file descriptor
     */
    void set_device();

    /**
     * @brief Checks type1 IOMMU support
     */
    void check_iommu();

    /**
     * @brief Checks IOMMU group viability for operation with VFIO
     */
    void check_group_viability();

    /**
     * @brief Checks support for cache coherence mechanisms
     */
    void check_cache_coherence();
};
