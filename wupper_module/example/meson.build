project('vfio_test', 'cpp')

# Define VFIO library
vfio = static_library('vfio', ['../../vfio_uapi.cpp', '../../data_buffer.cpp'], include_directories: '../..')
vfio_dep = declare_dependency(include_directories: '../..', link_with: vfio)

# Define Wupper module
wupper = library('wupper', '../wupper.cpp', include_directories: '..', dependencies: vfio_dep)

# Build example applications
executable('setup_test', 'setup_test.cpp', link_with: [wupper])
executable('dma_single_transfer', 'dma_single_transfer.cpp', link_with: [wupper])
executable('dma_multi_transfers', 'dma_multi_transfers.cpp', link_with: [wupper])
