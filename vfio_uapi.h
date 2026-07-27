#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <linux/vfio.h>
#include <stdexcept>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "data_buffer.h"

class VFIO {
public:
    VFIO(const std::string &device_name, int32_t iommu_group_id);
    ~VFIO();

    // VFIO user-defined properties
    const std::string device_name; // DBDF
    int iommu_group_id; // IOMMU group number
    uint64_t iova; // Device virtual address space for DMA operations

    // VFIO user interface setup methods
    //
    /**
     * @brief Maps an empty memory region in the device IOAS for DMA transfers
     * @description Receives an initialized DataBuffer object, maps its
     * referenced memory region
     * @param[in] buffer Reference to DataBuffer object to be mapped
     */
    void dma_map_buffer(DataBuffer &buffer);

    /**
     * @brief Maps an indexed memory region in the device VFIO file descriptor
     * @description Gets information of the region on the correspondent index
     * via IOCTL and initializes a DataBuffer with its base address and size
     * @param[in] index Index of memory region in the file descriptor, expected
     * to be VFIO's interface BAR region index enum region
     * @param[in] buffer Reference to DataBuffer object to be mapped
     */
    void bar_map_buffer(uint32_t index, DataBuffer &buffer);

    /**
     * @brief Prints info of the device acquired through ioctl
     */
    void get_device_info();

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
     * @brief Sets IOMMU to IOMMU type1
     */
    void set_iommu_type();

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
