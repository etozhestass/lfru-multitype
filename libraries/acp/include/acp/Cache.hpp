#ifndef ACP_CACHE_HPP
#define ACP_CACHE_HPP

#include <cstddef>
#include <list>
#include <new>
#include <ostream>
#include <queue>

template <class Key, class KeyProvider, class Allocator>
class Cache {
public:
    template <class... AllocArgs>
    [[maybe_unused]] explicit Cache(const std::size_t cache_size, AllocArgs &&...alloc_args)
        : m_max_top_size(cache_size), m_max_low_size(cache_size), m_alloc(std::forward<AllocArgs>(alloc_args)...) {}

    [[nodiscard]] std::size_t size() const { return m_lru.size() + m_queue.size(); }

    [[nodiscard]] bool empty() const { return size() == 0; }

    template <class T>
    T &get(const Key &key);

    std::ostream &print(std::ostream &strm) const;

    friend std::ostream &operator<<(std::ostream &strm, const Cache &cache) { return cache.print(strm); }

private:
    const std::size_t m_max_top_size;
    const std::size_t m_max_low_size;
    Allocator m_alloc;
    std::list<KeyProvider *> m_lru;
    std::list<KeyProvider *> m_queue;
    auto find_index(std::list<KeyProvider *> *list, const Key &key);
};
template <class Key, class KeyProvider, class Allocator>
auto Cache<Key, KeyProvider, Allocator>::find_index(std::list<KeyProvider *> *list, const Key &key) {
    return std::find_if(list->begin(), list->end(), [&key](const KeyProvider *ptr) { return *ptr == key; });
}

template <class Key, class KeyProvider, class Allocator>
template <class T>
inline T &Cache<Key, KeyProvider, Allocator>::get(const Key &key) {
    auto lru_index        = find_index(&m_lru, key);
    auto queue_index      = find_index(&m_queue, key);
    auto *choose_fragment = &m_lru;
    auto *choose_index    = &lru_index;
    if (lru_index == m_lru.end()) {
        if (queue_index == m_queue.end()) {
            if (m_queue.size() == m_max_low_size) {
                m_alloc.template destroy<KeyProvider>(m_queue.front());
                m_queue.erase(m_queue.begin());
            }
            m_queue.push_back(m_alloc.template create<T>(key));
            return *static_cast<T *>(m_queue.back());
        } else {
            if (m_lru.size() == m_max_top_size) {
                m_queue.push_back(m_lru.back());
                m_lru.pop_back();
            }
        }
        choose_fragment = &m_queue;
        choose_index    = &queue_index;
    }
    m_lru.splice(m_lru.begin(), *choose_fragment, *choose_index);
    return *static_cast<T *>(m_lru.front());
}

template <class Key, class KeyProvider, class Allocator>
inline std::ostream &Cache<Key, KeyProvider, Allocator>::print(std::ostream &strm) const {
    std::vector<std::list<KeyProvider *>> type = {m_lru, m_queue};
    for (std::size_t count = 0; count < 2; count++) {
        for (const auto i : type[count]) {
            strm << *i << ' ';
        }
    }
    return strm;
}

#endif  // ACP_CACHE_HPP
