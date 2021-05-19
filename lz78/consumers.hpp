#pragma once

#include "interfaces.hpp"

struct LZ78Consumer_Inline {
    std::vector<index_t> refs;
    std::vector<char_t> chars;
    
    inline void consume(const index_t ref, const char_t c) {
        refs.emplace_back(ref);
        chars.emplace_back(c);
    }
    
    inline size_t num_factors() const {
        return refs.size();
    }
};

struct LZ78Consumer_Interface : public ILZ78Consumer {
    std::vector<index_t> refs;
    std::vector<char_t> chars;
    
    virtual void consume(const index_t ref, const char_t c) override {
        refs.emplace_back(ref);
        chars.emplace_back(c);
    }
    
    virtual size_t num_factors() const override {
        return refs.size();
    }
};

struct LZ78Consumer_Dummy : public ILZ78Consumer {
    virtual void consume(const index_t ref, const char_t c) override { }
    virtual size_t num_factors() const override { return 0; }
};
