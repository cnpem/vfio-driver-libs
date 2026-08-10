#include "../wupper.h"
#include <stdexcept>
#include <sys/mman.h>

#define MAX_TRANSFER 2084065280UL
int32_t read_region(void *region, uint64_t offset, uint64_t length);

int main(int argc, char **argv)
{
    try {
        if (argc != 4)
            throw std::runtime_error(
                "Expected arguments: <Device DBDF> <IOMMU group ID> "
                "<Transfer size in number of pages>");

        // Initialize device interface
        Wupper xupp3r = Wupper(argv[1], std::stoi(argv[2]));

        // Initialize region to receive data
        size_t transfer_size = 4096 * std::stoi(argv[3]);
        DataBuffer buffer = DataBuffer(transfer_size, PROT_READ | PROT_WRITE);
        buffer.iova = 0x1000;

        // Map buffer
        xupp3r.interface.dma_map_buffer(buffer);

        // Do DMA transfer
        xupp3r.dma_to_host(transfer_size, buffer.iova, DMA_DESC_0, false);
        while (!xupp3r.dma_is_done(DMA_DESC_0)); // Wait for conclusion

        // Read BAR0
        // The registers are 16 bytes long, so add 0x10 to the end address
        // to print the content of the register on that address
        // 0x4A0 is the address of the last register on BAR0, even
        // though the region size is much bigger
        read_region((void *)xupp3r.bar0.vaddr, 0, 0x4A0 + 0x10);

        // Read transfered data
        read_region((void *)buffer.vaddr, 0, buffer.size + 0x10);
    } catch (std::runtime_error &e) {
        std::perror(e.what());
        exit(1);
    }

    return 0;
}

int32_t read_region(void *region, uint64_t offset, uint64_t length)
{
    volatile uint8_t *ptr = (uint8_t *)((uintptr_t)region + offset);
    printf("0x%04x # ", (unsigned int)offset);
    for (int i = 1; i <= length; i++) {
        printf("%02x ", ptr[i - 1]);
        if (i % 16 == 0)
            printf("\n0x%04x # ", i);
    }
    printf("\n");

    return 0;
}
