#pragma once

#include <util/typedefs.hpp>

class ISuffixArrayAccess {
public:
    virtual index_t operator[](const size_t i) const = 0;
};

class IBWTBuilder {
public:
    virtual void push_back(const char_t c) = 0;
    virtual size_t length() const = 0;
};
