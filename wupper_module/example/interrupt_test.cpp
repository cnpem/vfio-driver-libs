#include "../wupper.h"
#include <csignal>
#include <cstdio>
#include <linux/vfio.h>
#include <stdexcept>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/mman.h>

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

        // Setup epoll instance
        struct epoll_event ev, ev_ret;
        int epollfd = epoll_create1(EPOLL_CLOEXEC);
        if (epollfd == -1)
            throw std::runtime_error(
                "epoll_create1(): Error opening epoll instance");

        // Intialize interrupts
        for (int i = 0; i < xupp3r.interface.efds.size(); i++) {
            xupp3r.enable_irq(i);
            printf("IRQ [%d, %d] enabled\n", VFIO_PCI_MSIX_IRQ_INDEX, i);

            // Add eventfd to epoll interest list
            ev.data.fd = xupp3r.interface.efds[i];
            ev.events = EPOLLIN;
            if (epoll_ctl(epollfd, EPOLL_CTL_ADD, xupp3r.interface.efds[i], &ev)
                == -1)
                throw std::runtime_error(
                    "epoll_ctl(): Error adding fd to interest list");

            // Trigger interrupt via VFIO interface
            xupp3r.interface.set_irq(
                VFIO::TRIGGER_IRQ, VFIO_PCI_MSIX_IRQ_INDEX, i);

            int ret;
            eventfd_t efd;
            ret = epoll_wait(epollfd, &ev_ret, 1, 1000);
            if (ret < 0)
                throw std::runtime_error("epoll(): Error reading eventfd");
            else if (ret)
                if (read(xupp3r.interface.efds[i], &efd, sizeof(efd)) < 0)
                    throw std::runtime_error("read(): Error reading eventfd");
                else {
                    printf("Interrupt correctly captured!\n efd counter: %lu\n",
                        efd);
                    printf(
                        "Returned event struct:\n data_fd: %d\n events: %04x\n",
                        ev_ret.data.fd, ev_ret.events);
                }
            else
                printf("epoll(): Timeout\n\n");
            printf("epoll_wait for IRQ %d ret = %d\n\n", i, ret);

            if (epoll_ctl(
                    epollfd, EPOLL_CTL_DEL, xupp3r.interface.efds[i], NULL)
                == -1)
                throw std::runtime_error(
                    "epoll_ctl(): Error deleting fd from interest list");
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
