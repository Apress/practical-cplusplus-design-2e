// Copyright 2016 Adam B. Singer
// Contact: PracticalDesignBook@gmail.com
//
// This file is part of pdCalc.
//
// pdCalc is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// pdCalc is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with pdCalc; if not, see <http://www.gnu.org/licenses/>.
module;

#include <sstream>
#include <string>
#include <vector>
#include <istream>
#include <compare>
#include "3rdParty/cppcoro/generator.hpp"
#include <iterator>
#include <algorithm>
#include <iostream>
#include <ranges>

export module pdCalc_utilities:Tokenizer;

using std::string;
using std::istream_iterator;
using std::back_inserter;
using std::vector;
using std::istream;
namespace ranges = std::ranges;
namespace views = std::views;

namespace pdCalc {

export class GreedyTokenizer
{
public:
    explicit GreedyTokenizer(istream&);
    ~GreedyTokenizer() = default;

    size_t nTokens() const { return tokens_.size(); }

    auto begin() { return tokens_.begin(); }
    auto end() { return tokens_.end(); }

private:
    void tokenize(istream&);

    GreedyTokenizer() = delete;
    GreedyTokenizer(const GreedyTokenizer&) = delete;
    GreedyTokenizer(GreedyTokenizer&&) = delete;
    GreedyTokenizer& operator=(const GreedyTokenizer&) = delete;
    GreedyTokenizer& operator=(GreedyTokenizer&&) = delete;

    vector<string> tokens_;
};

export class LazyTokenizer
{
public:
    explicit LazyTokenizer(istream&);
    ~LazyTokenizer() = default;

    auto begin() { return generator_.begin(); }
    auto end() { return generator_.end(); }

    size_t nTokens() const { return nTokens_; }

private:
    cppcoro::generator<string> tokenize(istream& is) noexcept;

    LazyTokenizer() = delete;
    LazyTokenizer(const LazyTokenizer&) = delete;
    LazyTokenizer(LazyTokenizer&&) = delete;
    LazyTokenizer& operator=(const LazyTokenizer&) = delete;
    LazyTokenizer& operator=(LazyTokenizer&&) = delete;

    size_t nTokens_;
    cppcoro::generator<string> generator_;
};

GreedyTokenizer::GreedyTokenizer(istream& is)
{
    tokenize(is);
}

void GreedyTokenizer::tokenize(istream& is)
{
    for(istream_iterator<string> i{is}; i != istream_iterator<string>{}; ++i)
    {
        string t;
        ranges::transform(*i, back_inserter<string>(t), ::tolower);
        tokens_.push_back( std::move(t) );
    }

    return;
}

LazyTokenizer::LazyTokenizer(istream& is)
: nTokens_{0}
, generator_{ tokenize(is) }
{ }

cppcoro::generator<string> LazyTokenizer::tokenize(istream& is) noexcept
{
    for(istream_iterator<string> i{is}; i != istream_iterator<string>{}; ++i)
    {
        string t;
        ranges::transform(*i, back_inserter<string>(t), ::tolower);
        ++nTokens_;
        co_yield t;
    }

    co_return;
}

}
