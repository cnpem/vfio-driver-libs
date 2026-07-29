#include "../wupper.h"
#include <array>
#include <cstdio>
#include <stdexcept>
#include <sys/mman.h>

#define MAX_TRANSFER 2084065280UL
#define REGION_SIZE (4 * 1024 * 1024)
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
        #define NUM_BUFFERS 4
        size_t transfer_size = REGION_SIZE/NUM_BUFFERS;
        std::array<DataBuffer, NUM_BUFFERS> bufs;
        for (int i = 0; i < NUM_BUFFERS; i++)
            bufs[i] = DataBuffer(transfer_size, PROT_READ | PROT_WRITE);

        // Do DMA transfers
        int num_cycles = 3;
        for (int j = 0; j < num_cycles; j++)
        {
            printf("############### CYCLE %d ###############\n\n", j);
            for (int i = 0; i < NUM_BUFFERS; i++)
            {
                printf("## REGION %d ##\n", i);
                xupp3r.dma_to_host(transfer_size, DMA_DESC_0, bufs[i]);
                read_region((void *)bufs[i].vaddr, 0, bufs[i].size + 0x10);
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
