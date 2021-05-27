#pragma once

#include "interfaces.hpp"

struct BWTBuilder_Inline {
    std::string bwt;
    
    inline void push_back(const char_t c) { bwt.push_back(c); }
    inline size_t length() const { return bwt.size(); }
};

struct BWTBuilder_Interface : public IBWTBuilder {
    std::string bwt;
    
    virtual void push_back(const char_t c) override { bwt.push_back(c); }
    virtual size_t length() const override { return bwt.size(); }
};

struct BWTBuilder_Dummy : public IBWTBuilder {
    virtual void push_back(const char_t c) override { }
    virtual size_t length() const override { return 0; }
};
