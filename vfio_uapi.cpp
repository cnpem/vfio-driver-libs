#include "vfio_uapi.h"
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <fcntl.h>
#include <linux/vfio.h>
#include <stdexcept>
#include <sys/mman.h>
#include <unistd.h>

VFIO::VFIO(const std::string &device_name, int32_t iommu_group_id)
    : device_name(device_name)
    , iommu_group_id(iommu_group_id)
{
    iova = 0x1000;

    set_container();

    check_iommu();

    set_group();

    check_group_viability();

    check_cache_coherence();

    set_group_to_container();
}

void VFIO::set_container()
{
    container = open("/dev/vfio/vfio", O_RDWR);
    if (container < 0)
        throw std::runtime_error("open(): Failed to open container");
}

void VFIO::set_group()
{
    std::string group_path = "/dev/vfio/";
    group_path.append(std::to_string(iommu_group_id));

    group = open(group_path.c_str(), O_RDWR);
    if (group < 0)
        throw std::runtime_error("open(): Failed to open IOMMU group");
}

void VFIO::set_group_to_container()
{
    if (ioctl(group, VFIO_GROUP_SET_CONTAINER, &container))
        throw std::runtime_error("ioctl(): Failed to set group to container");
}

void VFIO::set_device()
{
    device = ioctl(group, VFIO_GROUP_GET_DEVICE_FD, device_name.c_str());
    if (device < 0)
        throw std::runtime_error("ioctl(): Failed to set device");
}

void VFIO::check_iommu()
{
    if (!ioctl(container, VFIO_CHECK_EXTENSION, VFIO_TYPE1_IOMMU))
        throw std::runtime_error("ioctl(): IOMMU type 1 unsupported");
}

void VFIO::check_group_viability()
{
    struct vfio_group_status group_status = { .argsz = sizeof(group_status) };
    if (ioctl(group, VFIO_GROUP_GET_STATUS, &group_status))
        throw std::runtime_error("ioctl(): Failed to get group status");

    if (!(group_status.flags & VFIO_GROUP_FLAGS_VIABLE))
        throw std::runtime_error("Group not viable");
}

void VFIO::check_cache_coherence()
{
    if (!ioctl(container, VFIO_CHECK_EXTENSION, VFIO_DMA_CC_IOMMU))
        throw std::runtime_error(
            "Cache coherence mechanisms unsupported by IOMMU");
}

void VFIO::dma_map_buffer(DataBuffer buffer)
{
    if (buffer.data == NULL || buffer.size == 0)
        throw std::runtime_error("Uninitialized buffer");

    struct vfio_iommu_type1_dma_map dma = { .argsz = sizeof(dma),
        .flags = VFIO_DMA_MAP_FLAG_READ | VFIO_DMA_MAP_FLAG_WRITE,
        .vaddr = (uintptr_t)buffer.data,
        .iova = iova,
        .size = (uint64_t)buffer.size };

    iova += buffer.size;

    if (ioctl(container, VFIO_IOMMU_MAP_DMA, &dma))
        throw std::runtime_error("ioctl(): Failed to map DMA region");
}

DataBuffer VFIO::map_mem_region(uint32_t index)
{
    // Get region info
    struct vfio_region_info reg = { .argsz = sizeof(reg), .index = index };

    if (ioctl(device, VFIO_DEVICE_GET_INFO, &reg))
        throw std::runtime_error("ioctl(): Failed to get memory region info");

    DataBuffer bar = DataBuffer(reg.size, PROT_READ | PROT_WRITE, MAP_SHARED, device, reg.offset);
    if (bar.data == MAP_FAILED)
        throw std::runtime_error("mmap(): Failed to mmap buffer");
    return bar;
}

VFIO::~VFIO()
{
    close(device);
    close(container);
    close(group);
}
