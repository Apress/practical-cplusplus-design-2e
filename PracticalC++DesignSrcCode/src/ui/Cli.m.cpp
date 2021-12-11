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
#include <iterator>
#include <string>
#include <iostream>
#include <vector>
#include <format>
#include <ranges>
#include <any>
#include <string_view>
#include <sstream>
export module pdCalc_userInterface:Cli;

import pdCalc_stack;
import pdCalc_userInterface;
import pdCalc_utilities;

using std::string_view;
using std::string;
using std::istream;
using std::ostream;
using std::endl;
using std::format;
using std::istringstream;
namespace  views = std::views;

namespace pdCalc {

Cli::Cli(istream& in, ostream& out)
: in_(in)
, out_(out)
{ }

void Cli::startupMessage()
{
    out_ << "pdCalc v. " << PDCALC_VERSION << ", an RPN calculator\n"
         << "type 'help' for a list of commnads\n"
         << "'exit' to end program\n" << endl;

    return;
}

void Cli::execute(bool suppressStartupMessage, bool echo)
{
    if(!suppressStartupMessage) startupMessage();

    for(string line; std::getline(in_, line, '\n'); )
    {
        istringstream iss{line};
        LazyTokenizer tokenizer{iss};
        for(auto i : tokenizer)
        {
            if(echo) out_ << i << endl;
            if(i == "exit" || i == "quit")
            {
                return;
            }
            else
            {
                raise(UserInterface::CommandEntered(), i);
            }
        }
    }

    return;
}

void Cli::stackChanged()
{
    unsigned int nElements{4};
    auto v = Stack::Instance().getElements(nElements);
    string s{"\n"};
    auto bi = std::back_inserter(s);

    if(size_t size = Stack::Instance().size(); size == 0)
        std::format_to(bi, "Stack currently empty.\n");
    else if(size == 1)
        std::format_to(bi, "Top element of stack (size = {}):\n", size);
    else if(size > 1 && size <= nElements)
        std::format_to(bi, "Top {0} elements of stack (size = {0}):\n", size);
    else
        std::format_to(bi, "Top {} elements of stack (size = {}):\n", nElements, size);

    for( auto j = v.size(); auto i : views::reverse(v) )
    {
        std::format_to(bi, "{}:\t{:.12g}\n", j--, i);
    }

    postMessage(s);
}

void Cli::postMessage(string_view m)
{
    out_ << m << endl;
    return;
}

}
