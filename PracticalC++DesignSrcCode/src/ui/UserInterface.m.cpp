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
#include <string>
#include <string_view>
#include <memory>
#include <iostream>
export module pdCalc_userInterface;

import pdCalc_utilities;

using std::unique_ptr;
using std::string;
using std::string_view;
using std::istream;
using std::ostream;

namespace pdCalc {

export class UserInterface : protected Publisher
{
public:
    UserInterface() { registerEvent( CommandEntered() ); }
    virtual ~UserInterface() = default;

    // post a message to the user
    virtual void postMessage(string_view m) = 0;

    // notifies the interface that the stack has changed
    virtual void stackChanged() = 0;

    using Publisher::attach;
    using Publisher::detach;

    // defines the event this publisher can raise
    // note that the string is defined in main.cpp of the application since
    // class UserInterface has no implementation file (in test driver for same
    // reason)

    // static function instead of member to avoid manual Windows dllexport requirements
    static string CommandEntered(){ return "CommandIssued";};
};

export class Cli : public UserInterface
{
public:
    Cli(istream&, ostream&);
    ~Cli() = default;

    // start the cli run loop
    void execute(bool suppressStartupMessage = false, bool echo = false);

private:
    // posts a text message to the output
    void postMessage(string_view m) override;

    // updates the output when the stack is changed
    void stackChanged() override;

    void startupMessage();

    Cli(const Cli&) = delete;
    Cli(Cli&&) = delete;
    Cli& operator=(const Cli&) = delete;
    Cli& operator=(Cli&&) = delete;

    istream& in_;
    ostream& out_;
};

}
