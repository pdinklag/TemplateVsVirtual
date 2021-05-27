#pragma once

#include "interfaces.hpp"

struct SuffixArrayAccessor_Inline {
    const index_t* sa;
    
    inline index_t operator[](const size_t i) const { return sa[i]; }
};

struct SuffixArrayAccessor_Interface : public ISuffixArrayAccess {
    const index_t* sa;
    
    inline SuffixArrayAccessor_Interface(const index_t* _sa) : sa(_sa) {}
    virtual index_t operator[](const size_t i) const override { return sa[i]; }
};

struct SuffixArrayAccessor_Dummy : public ISuffixArrayAccess {
    virtual index_t operator[](const size_t i) const override { return 0; }
};
