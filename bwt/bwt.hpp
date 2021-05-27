#pragma once

#include "interfaces.hpp"

template<typename SuffixArrayAccessor, typename BWTBuilder>
inline static void BWT_TT(const std::string& text, const SuffixArrayAccessor& sa, BWTBuilder& bwt) {
    const size_t n = text.size();
    for(size_t i = 0; i < n; i++) {
        const auto j = sa[i];
        bwt.push_back(text[j > 0 ? j - 1 : n - 1]);
    }
}

template<typename BWTBuilder>
inline static void BWT_IT(const std::string& text, const ISuffixArrayAccess* sa, BWTBuilder& bwt) {
    const size_t n = text.size();
    for(size_t i = 0; i < n; i++) {
        const auto j = (*sa)[i];
        bwt.push_back(text[j > 0 ? j - 1 : n - 1]);
    }
}

template<typename SuffixArrayAccessor>
inline static void BWT_TI(const std::string& text, const SuffixArrayAccessor& sa, IBWTBuilder* bwt) {
    const size_t n = text.size();
    for(size_t i = 0; i < n; i++) {
        const auto j = sa[i];
        bwt->push_back(text[j > 0 ? j - 1 : n - 1]);
    }
}

inline static void BWT_II(const std::string& text, const ISuffixArrayAccess* sa, IBWTBuilder* bwt) {
    const size_t n = text.size();
    for(size_t i = 0; i < n; i++) {
        const auto j = (*sa)[i];
        bwt->push_back(text[j > 0 ? j - 1 : n - 1]);
    }
}
