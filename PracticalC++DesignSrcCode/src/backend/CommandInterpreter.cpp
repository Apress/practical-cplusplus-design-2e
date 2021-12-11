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
#include <format>
#include <regex>
#include <cassert>
#include <algorithm>
#include <fstream>
#include <stack>
#include <string_view>
#include <string>
#include <iterator>
module pdCalc_commandDispatcher:CommandInterpreter;

import pdCalc_command;
import pdCalc_utilities;
import :CommandManager;
import :CoreCommands;
import :StoredProcedure;
import :CommandFactory;

using std::string;
using std::unique_ptr;
using std::set;
using std::string_view;

namespace pdCalc {

class CommandInterpreter::CommandInterpreterImpl
{
public:
    explicit CommandInterpreterImpl(UserInterface& ui);

    void executeCommand(const string& command);


private:
    bool isNum(const string&, double& d);
    void handleCommand(CommandPtr command);
    void printHelp() const;

    CommandManager manager_;
    UserInterface& ui_;
};

CommandInterpreter::CommandInterpreterImpl::CommandInterpreterImpl(UserInterface& ui)
: ui_(ui)
{ }

void CommandInterpreter::CommandInterpreterImpl::executeCommand(const string& command)
{
    string_view sv{command};

    // entry of a number simply goes onto the the stack
    if(double d; isNum(command, d) )
        manager_.executeCommand(MakeCommandPtr<EnterNumber>(d));
    else if(command == "undo")
        manager_.undo();
    else if(command == "redo")
        manager_.redo();
    else if(command == "help")
        printHelp();
    else if( command.size() > 6 && sv.starts_with("proc:") )
    {
        string filename{sv.substr(5, command.size() - 5)};
        handleCommand( MakeCommandPtr<StoredProcedure>(ui_, filename) );
    }
    else
    {
        if( auto c = CommandFactory::Instance().allocateCommand(command) )
        {
            handleCommand( std::move(c) );
        }
        else
        {
            auto t = std::format("Command {} is not a known command", command);
            ui_.postMessage(t);
        }
    }

    return;
}

void CommandInterpreter::CommandInterpreterImpl::handleCommand(CommandPtr c)
{
    try
    {
        manager_.executeCommand( std::move(c) );
    }
    catch(Exception& e)
    {
        ui_.postMessage( e.what() );
    }

    return;
}

void CommandInterpreter::CommandInterpreterImpl::printHelp() const
{
    string help = "\n"
                  "undo: undo last operation\n"
                  "redo: redo last operation\n";

    set<string> allCommands = CommandFactory::Instance().getAllCommandNames();
    for(auto i : allCommands)
    {
        help += CommandFactory::Instance().helpMessage(i) + "\n";
    }

    ui_.postMessage( help );

    return;
}

// uses a C++11 regular expression to check if this is a valid double number
// if so, converts it into one and returns it
bool CommandInterpreter::CommandInterpreterImpl::isNum(const string& s, double& d)
{
     if(s == "+" || s == "-") return false;

     std::regex dpRegex("((\\+|-)?[[:digit:]]*)(\\.(([[:digit:]]+)?))?((e|E)((\\+|-)?)[[:digit:]]+)?");
     bool isNumber{ std::regex_match(s, dpRegex) };

     if(isNumber)
     {
         d = std::stod(s);
     }

     return isNumber;
}

void CommandInterpreter::commandEntered(const string& command)
{
    pimpl_->executeCommand(command);

    return;
}

CommandInterpreter::CommandInterpreter(UserInterface& ui)
: pimpl_{ std::make_unique<CommandInterpreterImpl>(ui) }
{

}

CommandInterpreter::~CommandInterpreter()
{ }

}
