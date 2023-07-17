#include "acp/Allocator.hpp"

void* PoolAllocator::allocate(std::size_t const n) {
    auto index = std::find(m_sizes.begin(), m_sizes.end(), n) - m_sizes.begin();
    for (std::size_t i = 0; i < m_blocks[index].size(); i++) {
        auto&& block = m_blocks[index][i];
        if (!block) {
            block = true;
            return &m_storage[m_sizes[index] * i + m_block_size * index];
        }
    }
    throw std::bad_alloc{};
}

void PoolAllocator::deallocate(void const* ptr) {
    const auto offset = static_cast<std::size_t>(static_cast<const std::byte*>(ptr) - &m_storage[0]);
    if (offset < m_block_size * m_block_count) {
        std::size_t size      = offset / m_block_size;
        std::size_t index     = (offset % m_block_size) / m_sizes[size];
        m_blocks[size][index] = false;
    }
}
