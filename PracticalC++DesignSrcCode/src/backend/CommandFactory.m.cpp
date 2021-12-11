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
#include <memory>
#include <string>
#include <set>
#include <format>
#include <unordered_map>
#include <format>
#include <algorithm>
#include <ranges>
export module pdCalc_commandDispatcher:CommandFactory;

import pdCalc_utilities;
import pdCalc_userInterface;
import pdCalc_command;
import :CoreCommands;

using std::string;
using std::unordered_map;
using std::set;

namespace pdCalc {

export class CommandFactory
{
public:
    static CommandFactory& Instance();

    // register a new command for the factory: throws if a command with the
    // same name already exists...deregister first to replace a command
    void registerCommand(const string& name, CommandPtr c);

    // deregister a command: returns the pointer to a command and subsequently
    // removes it from the internal database of commands...returns a nullptr
    // if the command does not exist
    CommandPtr deregisterCommand(const string& name);

    // returns the number of commands currently registered
    size_t getNumberCommands() const { return factory_.size(); }

    // returns a pointer to a command without deregistering the command...returns
    // a nullptr if the command does not exist
    CommandPtr allocateCommand(const string& name) const;

    // returns true if the command is present, false otherwise
    bool hasKey(const string& s) const { return factory_.contains(s); }

    // returns a set of all the commands
    std::set<string> getAllCommandNames() const;

    // prints help for command
    string helpMessage(const string& command) const;

    // clears all commands; mainly needed for testing
    void clearAllCommands(){ factory_.clear(); }

private:
    CommandFactory() = default;
    ~CommandFactory() = default;

    CommandFactory(CommandFactory&) = delete;
    CommandFactory(CommandFactory&&) = delete;
    CommandFactory& operator=(CommandFactory&) = delete;
    CommandFactory& operator=(CommandFactory&&) = delete;

private:
    using Factory = unordered_map<string, CommandPtr>;
    Factory factory_;
};

set<string> CommandFactory::getAllCommandNames() const
{
    set<string> tmp;

    std::ranges::for_each(factory_ | std::views::keys, [&tmp](const auto& k){tmp.insert(k);});

    return tmp;
}

string CommandFactory::helpMessage(const string& command) const
{
    auto it = factory_.find(command);

    return std::format("{}: {}", command,
        it != factory_.end() ? it->second->helpMessage() : "no help entry found");
}

void CommandFactory::registerCommand(const string& name, CommandPtr c)
{
    if( hasKey(name) )
    {
        auto t = std::format("Command {} already registered", name);
        throw Exception{t};
    }
    else
        factory_.emplace( name, std::move(c) );

    return;
}

CommandPtr CommandFactory::deregisterCommand(const string& name)
{
    if( hasKey(name) )
    {
        auto i = factory_.find(name);
        auto tmp = MakeCommandPtr( i->second.release() );
        factory_.erase(i);
        return tmp;
    }
    else return MakeCommandPtr(nullptr);
}

CommandPtr CommandFactory::allocateCommand(const string &name) const
{
    if( hasKey(name) )
    {
        const auto& command = factory_.find(name)->second;
        return MakeCommandPtr( command->clone() );
    }
    else return MakeCommandPtr(nullptr);
}


CommandFactory& CommandFactory::Instance()
{
    static CommandFactory instance;
    return instance;
}

export void RegisterCoreCommands(UserInterface& ui)
{
    auto& cr = CommandFactory::Instance();
    try
    {
        cr.registerCommand( "swap", MakeCommandPtr<SwapTopOfStack>() );
        cr.registerCommand( "drop", MakeCommandPtr<DropTopOfStack>() );
        cr.registerCommand( "clear", MakeCommandPtr<ClearStack>() );
        cr.registerCommand( "+", MakeCommandPtr<Add>() );
        cr.registerCommand( "-", MakeCommandPtr<Subtract>() );
        cr.registerCommand("*",
            MakeCommandPtr<BinaryCommandAlternative>("Replace first two elements on the stack with their product",
            [](double d, double f){ return d * f; })
        );
        cr.registerCommand( "/", MakeCommandPtr<Divide>() );
        cr.registerCommand( "pow", MakeCommandPtr<Power>() );
        cr.registerCommand( "root", MakeCommandPtr<Root>() );
        cr.registerCommand( "sin", MakeCommandPtr<Sine>() );
        cr.registerCommand( "cos", MakeCommandPtr<Cosine>() );
        cr.registerCommand( "tan", MakeCommandPtr<Tangent>() );
        cr.registerCommand( "arcsin", MakeCommandPtr<Arcsine>() );
        cr.registerCommand( "arccos", MakeCommandPtr<Arccosine>() );
        cr.registerCommand( "arctan", MakeCommandPtr<Arctangent>() );
        cr.registerCommand( "neg", MakeCommandPtr<Negate>() );
        cr.registerCommand( "dup", MakeCommandPtr<Duplicate>() );
    }
    catch(Exception& e)
    {
        ui.postMessage( e.what() );
    }

    return;
}

}
