#include "../wupper.h"
#include <csignal>
#include <cstdio>
#include <linux/vfio.h>
#include <stdexcept>
#include <sys/eventfd.h>
#include <sys/mman.h>
#include <sys/select.h>

#define MAX_TRANSFER 2084065280UL
#define REGION_SIZE 4096
#define NUM_BUFFERS 2
void sig_int_handler(int);
int32_t read_region(void *region, uint64_t offset, uint64_t length);

int main(int argc, char **argv)
{
    try {
        // Verify command line arguments
        if (argc != 3)
            throw std::runtime_error(
                "Command usage <Device DBDF> <IOMMU group ID>");

        int sa_stat;
        static struct sigaction sa;

        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sa.sa_handler = sig_int_handler;
        if (sigaction(SIGINT, &sa, NULL) < 0)
            throw std::runtime_error("Failed to install signal handler");

        // Initialize device interface
        Wupper xupp3r = Wupper(argv[1], std::stoi(argv[2]));

        // Get interrupts info
        struct vfio_device_info dev;
        xupp3r.interface.get_device_info(dev);

        printf("IRQ count: %lu\n\n", xupp3r.interface.efds.size());
        for (int i = 0; i < dev.num_irqs; i++) {
            struct vfio_irq_info irq;

            xupp3r.interface.get_irq_index_info(irq, i);
            printf("IRQ %d\nFlags: %04b\nCount: %d\n", irq.index, irq.flags,
                irq.count);
        }
        printf("\n");

        // Timeout for the select call
        struct timeval timeout = {
            .tv_sec = 1,
            .tv_usec = 0,
        };

        // Intialize interrupts
        for (int i = 0; i < xupp3r.interface.efds.size(); i++) {
            xupp3r.enable_irq(i);
            printf("IRQ [%d, %d] enabled\n", VFIO_PCI_MSIX_IRQ_INDEX, i);

            int ret;
            eventfd_t efd;
            fd_set fds;

            FD_ZERO(&fds);
            FD_SET(xupp3r.interface.efds[i], &fds);

            // Trigger interrupt via VFIO interface
            xupp3r.interface.set_irq(VFIO::TRIGGER_IRQ, VFIO_PCI_MSIX_IRQ_INDEX, i);

            ret = select(xupp3r.interface.efds[i]+1, &fds, NULL, NULL, &timeout);
            if (ret < 0)
                throw std::runtime_error("IRQ test error");
            else if (ret)
                if (read(xupp3r.interface.efds[i], &efd, sizeof(efd)) < 0)
                    throw std::runtime_error("EFD with invalid value");
                else {
                    printf("IRQ [%d, %d] success\n", VFIO_PCI_MSIX_IRQ_INDEX, i);
                    printf("Counter = %lu\n", efd);
		}
            else 
                printf("IRQ test timeout\n\n");
            printf("Eventfd %d ret = %d\n\n", i, ret);
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

void sig_int_handler(int) { exit(1); }
