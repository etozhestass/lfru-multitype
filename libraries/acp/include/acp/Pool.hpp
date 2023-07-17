#ifndef ACP_POOL_HPP
#define ACP_POOL_HPP

#include <cstddef>
#include <initializer_list>
#include <new>
#include <ranges>
#include <vector>

class PoolAllocator {
public:
    PoolAllocator(std::size_t const block_size, std::initializer_list<std::size_t> sizes)
        : m_block_size(block_size)
        , m_block_count(sizes.size())
        , m_blocks(m_block_count)
        , m_storage(m_block_size * m_block_count)
        , m_sizes(sizes) {
        m_sizes.reserve(m_block_count);
        m_blocks.reserve(m_block_count);
        for (std::size_t i = 0; std::size_t size : m_sizes) {
            m_blocks[i++].resize(m_block_size / size);
        }
    }

    void* allocate(std::size_t n);

    void deallocate(void const* ptr);

private:
    const std::size_t m_block_size;
    const std::size_t m_block_count;
    std::vector<std::vector<bool>> m_blocks;
    std::vector<std::byte> m_storage;
    std::vector<std::size_t> m_sizes;
};

#endif  // ACP_POOL_HPP
