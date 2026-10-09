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

        // Map BAR2 to use special register to trigger interrupt
        DataBuffer bar2 = DataBuffer();
        xupp3r.interface.bar_map_buffer(VFIO_PCI_BAR2_REGION_INDEX, bar2);
        unsigned long* bar2_regs = (unsigned long*)bar2.vaddr;

        // Test interrupts
        struct timeval timeout = {
            .tv_sec = 1,
            .tv_usec = 0,
        };

        // Intialize interrupts
        for (int i = 0; i < xupp3r.interface.efds.size(); i++) {
            xupp3r.enable_irq(i);

            int ret;
            eventfd_t efd;
            fd_set fds;

            FD_ZERO(&fds);
            FD_SET(xupp3r.interface.efds[i], &fds);

            // Trigger interrupt via register
            // The write of the irq number triggers the interrupt
            bar2_regs[96] = i; /* 0x1800 - INT_TEST register */

            ret = select(1, &fds, NULL, NULL, &timeout);
            if (ret < 0)
                throw std::runtime_error("IRQ test timeout");
            else if (ret)
                if (read(xupp3r.interface.efds[i], &efd, sizeof(efd)) < 0)
                    throw std::runtime_error("EFD with invalid value");
                else
                    printf("IRQ [%d, %d] success", VFIO_PCI_MSIX_IRQ_INDEX, i);
        }

    } catch (std::runtime_error &e) {
        std::perror(e.what());
        exit(1);
    }

    return 0;
}

void sig_int_handler(int)
{
    exit(1);
}
