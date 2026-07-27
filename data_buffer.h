#include <cstddef>
#include <iostream>
#include <sys/mman.h>

class DataBuffer {
public:
    /**
     * @brief Allocate a buffer on the process address space
     * @description Receives mmap's arguments, calls mmap with them,
     * stores the returned pointer in the "data" class attribute. Consult mmap's
     * manual page for expected values on each argument. By default, it assumes
     * an anonymous mapping
     * @param[in] size Size of the region
     * @param[in] prot Desired memory protection of the mapping
     * @param[in] flags Determines wheter updates to the mapping are visible to
     * other processes mapping the same region. Defaults to MAP_ANONYMOUS
     * @param[in] fd File descriptor reference, in case of a file mapping.
     * Defaults to -1
     * @param[in] offset Offset in the file, in case of a file a mapping.
     * Defaults to 0
     */
    DataBuffer(size_t size, int prot, int flags = MAP_ANONYMOUS, int fd = -1, off_t offset = 0);
    ~DataBuffer();

    /**
     * @brief Reallocates the class buffer on the process address space
     * @description Does the same thing as the class constructor, giving the
     * user the possibility to reallocate the buffer with different parameters
     * @param[in] n_size New size of the region
     * @param[in] n_prot Desired memory protection of the new mapping
     * @param[in] n_flags Determines wheter updates to the new mapping are visible to
     * other processes mapping the same region. Defaults to MAP_ANONYMOUS
     * @param[in] n_fd New file descriptor reference, in case of a file mapping.
     * Defaults to -1
     * @param[in] n_offset New offset in the file, in case of a file a mapping.
     * Defaults to 0
     */
    void realloc_buffer(
        size_t n_size, int n_prot, int n_flags = MAP_ANONYMOUS, int n_fd = -1, off_t n_offset = 0);

    volatile void *data = nullptr;
    size_t size = 0;
    int prot;
    int flags;
    int fd;
    off_t offset;
};
