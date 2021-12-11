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

#include <any>
#include <string>

export module pdCalc_commandDispatcher:AppObservers;

import :CommandInterpreter;
import pdCalc_utilities;

using std::string;
using std::any;

namespace pdCalc {

export class CommandIssuedObserver : public Observer
{
public:
    explicit CommandIssuedObserver(CommandInterpreter& ci);

private:
    void notifyImpl(const any&) override;

    CommandInterpreter& ci_;
};

export class StackUpdatedObserver : public Observer
{
public:
    explicit StackUpdatedObserver(UserInterface& ui);

private:
    void notifyImpl(const any&) override;

    UserInterface& ui_;
};

CommandIssuedObserver::CommandIssuedObserver(CommandInterpreter& ci)
: Observer{"CommandIssued"}
, ci_{ci}
{ }

void CommandIssuedObserver::notifyImpl(const any& data)
{
    try
    {
        const auto& d = any_cast<string>(data);
        ci_.commandEntered(d);
    }
    catch(const std::bad_any_cast&)
    {
        throw Exception("Could not convert CommandIssued data to a string");
    }

    return;
}

StackUpdatedObserver::StackUpdatedObserver(UserInterface& ui)
: Observer{"StackUpdated"}
, ui_{ui}
{ }

void StackUpdatedObserver::notifyImpl(const any&)
{
    ui_.stackChanged();

    return;
}

}
