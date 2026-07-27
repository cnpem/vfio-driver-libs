#include "../wupper.h"
#include <stdexcept>

int main(int argc, char **argv)
{
    try {
        // Verify command line arguments
        if (argc != 3)
            throw std::runtime_error("Command usage <Device DBDF> <IOMMU group ID>");

        // Initialize device interface
        Wupper xupp3r = Wupper(argv[1], std::stoi(argv[2]));

        // Read device information
        xupp3r.interface.get_region_info(VFIO_PCI_BAR0_REGION_INDEX);
        xupp3r.interface.get_device_info();
    } catch (std::runtime_error &e) {
        std::perror(e.what());
        exit(1);
    }

    return 0;
}
