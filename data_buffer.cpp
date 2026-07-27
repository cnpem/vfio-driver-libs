#include "data_buffer.h"
#include <stdexcept>
#include <sys/mman.h>

DataBuffer::DataBuffer(size_t size, int prot, int flags, int fd, off_t offset)
    : size(size)
    , prot(prot)
    , flags(flags)
    , fd(fd)
    , offset(offset)
{
    data = mmap(NULL, size, prot, flags, fd, offset);
    if (data == MAP_FAILED)
        throw std::runtime_error("mmap(): Failed to mmap buffer");
}

void DataBuffer::realloc_buffer(
    size_t size, int prot, int flags, int fd, off_t offset)
{
    if (data != nullptr) {
        if (munmap((void *)data, size) == -1)
            throw std::runtime_error("munmap(): Failed to munmap buffer");
    }

    data = mmap(NULL, size, prot, flags, fd, offset);
    if (data == MAP_FAILED)
        throw std::runtime_error("mmap(): Failed to mmap buffer");
}

DataBuffer::~DataBuffer()
{
    if (munmap((void *)data, size) == -1)
        throw std::runtime_error("munmap(): Failed to munmap buffer");
}
