#include "../wupper.h"
#include <array>
#include <cstdio>
#include <stdexcept>
#include <sys/mman.h>

#define MAX_TRANSFER 2084065280UL
#define REGION_SIZE 4096
#define NUM_BUFFERS 2
int32_t read_region(void *region, uint64_t offset, uint64_t length);

int main(int argc, char **argv)
{
    try {
        // Verify command line arguments
        if (argc != 3)
            throw std::runtime_error("Command usage <Device DBDF> <IOMMU group ID>");

        // Initialize device interface
        Wupper xupp3r = Wupper(argv[1], std::stoi(argv[2]));

        // Initialize regions to receive data
        size_t transfer_size = REGION_SIZE/NUM_BUFFERS;
        std::array<DataBuffer, NUM_BUFFERS> bufs;
        for (int i = 0; i < NUM_BUFFERS; i++) {
            bufs[i].realloc_buffer(transfer_size, PROT_READ | PROT_WRITE);
            bufs[i].iova = 0x1000;
        }

        // Application loop
        int num_cycles = 2;

        // Variables to track DMA wrap around
        bool even_dma, current_even_dma;

        for (int i = 0; i < num_cycles; i++) {
            // Map buffer
            xupp3r.interface.dma_map_buffer(bufs[i % NUM_BUFFERS]);

            if (i == 0)
                // Start endless DMA
                xupp3r.dma_to_host(transfer_size, (unsigned long)bufs[0].iova,
                    DMA_DESC_0, true);

            // Wait for wrap around
            even_dma = xupp3r.dma_get_even_addr(DMA_DESC_0);
            current_even_dma = even_dma;
            while (even_dma == current_even_dma) {
                std::cout << "DMA current addr = "
                          << xupp3r.dma_get_current_addr(DMA_DESC_0) << '\n';

                xupp3r.dma_update_read_ptr(DMA_DESC_0);
                current_even_dma = xupp3r.dma_get_even_addr(DMA_DESC_0);
                std::cout << "even_dma = " << current_even_dma << '\n';
            }
        }
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
