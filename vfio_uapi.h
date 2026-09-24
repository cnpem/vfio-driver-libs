#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <linux/vfio.h>
#include <stdexcept>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <vector>

#include "data_buffer.h"

class VFIO {
public:
    VFIO(const std::string &device_name, int32_t iommu_group_id);
    ~VFIO();

    enum irq_operation { UNMASK_IRQ, SET_EFD_IRQ };

    // VFIO user-defined properties
    const std::string device_name; // DBDF
    int iommu_group_id; // IOMMU group number

    // IRQ related properties
    std::vector<int> efds; // eventfd's

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
     * @description Initializes the passed vfio_device_info struct with
     * VFIO_DEVICE_GET_INFO ioctl
     * @param[out] dev_info Unitialized vfio_device_info struct
     */
    void get_device_info(vfio_device_info &dev_info);

    /**
     * @description Initializes the passed vfio_region_info struct with
     * VFIO_DEVICE_GET_REGION_INFO ioctl
     * @param[out] region_info Unitialized vfio_region_info struct
     * @param[in] index Index of memory region in the file descriptor, expected
     * to be VFIO's interface BAR region index enum region
     */
    void get_region_info(vfio_region_info &region_info, uint32_t index);

    /**
     * @description Initializes the passed vfio_irq_info struct with
     * VFIO_DEVICE_GET_IRQ_INFO ioctl
     * @param[out] irq_info Unitialized vfio_irq_info struct
     * @param[in] index IRQ index
     */
    void get_irq_index_info(vfio_irq_info &irq_info, uint32_t index);

    /**
     * @brief Resize the eventfd vector according to a given IRQ index
     * @description Calls get_irq_index_info to get the IRQ index interrupt
     * count, then resizes the vector to that value
     * @param[in] index
     */
    void resize_efds(uint32_t index);

    /**
     * @brief Perform the given operation on an specified interrupt
     * @description Initialize an irq_set struct with the given index and
     * sub_index to identify the interrupt, count equal to 1 to operate on a
     * single interrupt and then set data and flags according to the operation
     * @param[in] operation Must be one of the values in the irq_operation enum:
     * SET_EFD_IRQ: Creates a new eventfd, stores it on the member efds in the
     * position equal to the sub_index parameter and set data field to it.
     * flags = VFIO_IRQ_SET_DATA_EVENTFD | VFIO_IRQ_SET_ACTION_TRIGGER
     *
     * UNMASK_INT: Unmask the given interrupt
     * flags = VFIO_IRQ_SET_DATA_NONE | VFIO_IRQ_SET_ACTION_UNMASK
     * @param[in] index IRQ index
     * @param[in] sub_index Sub index of the interrupt on the IRQ
     */
    void set_irq(int operation, int index, int sub_index);

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
