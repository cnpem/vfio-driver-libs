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

    // VFIO user interface setup methods
    /**
     * @brief Maps an empty memory region in the device IOAS for DMA transfers
     * @description Initializes a data_buffer struct, maps a memory region of
     * the proccess address space for DMA transfers and stores the pointer and
     * the region size in the struct
     * @param[in] size Desired size of the buffer
     * region
     * @return Struct with reference to region
     */
    data_buffer map_dma_buffer(size_t size);

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
