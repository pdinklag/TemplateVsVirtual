#pragma once

#include <util/typedefs.hpp>

class ILZ78Consumer {
public:
    virtual void consume(const index_t ref, const char_t c) = 0;
    virtual size_t num_factors() const = 0;
};

class ILZ78Trie {
public:
    virtual index_t root() const = 0;
    virtual index_t get_child(const index_t v, const char_t c) = 0; // nb: not const, may change child order (MTF)
    virtual index_t insert_child(const index_t v, const char_t c) = 0;
    virtual size_t size() const = 0;
};
