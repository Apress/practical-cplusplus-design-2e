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
#include <algorithm>
#include <ranges>
#include <memory>
#include <fstream>
module pdCalc_commandDispatcher:StoredProcedure;

import pdCalc_utilities;
import :CommandInterpreter;

namespace ranges = std::ranges;
namespace views = std::views;

using std::string;

namespace pdCalc {

StoredProcedure::StoredProcedure(UserInterface& ui, const string& filename)
: filename_{filename}
{
    ci_ = std::make_unique<CommandInterpreter>(ui);
}

void StoredProcedure::checkPreconditionsImpl() const
{
    if(first_)
    {
        try
        {
            ifs_.open( filename_.c_str() );
            if(!ifs_)
                throw Exception{"Could not open procedure"};

            tokenizer_ = std::make_unique<LazyTokenizer>(ifs_);
        }
        catch(...)
        {
            throw Exception{"Could not open procedure"};
        }
    }

    return;
}

void StoredProcedure::executeImpl() noexcept
{
    if(first_)
    {
        ranges::for_each(*tokenizer_, [this](auto c){ci_->commandEntered(c);});
        first_ = false;
    }
    else
    {
        for(auto i = 0u; i < tokenizer_->nTokens(); ++i)
            ci_->commandEntered("redo");
    }

    return;
}

void StoredProcedure::undoImpl() noexcept
{
    for(auto i = 0u; i < tokenizer_->nTokens(); ++i)
        ci_->commandEntered("undo");

    return;
}

Command* StoredProcedure::cloneImpl() const noexcept
{
    return 0;
}

const char* StoredProcedure::helpMessageImpl() const noexcept
{
    return "Executes a stored procedure from disk";
}

}

