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

#include <iostream>
#include <memory>
import pdCalc_userInterface;
import pdCalc_utilities;
import pdCalc_stack;
import pdCalc_commandDispatcher;

// I usually don't make blanket using namespace statements, but this
// example is meant to be simple.
using namespace pdCalc;

using std::cout;
using std::endl;
using std::make_unique;

int main()
{
    Cli cli{std::cin, std::cout};

    CommandInterpreter ci{cli};

    RegisterCoreCommands(cli);

    cli.attach(UserInterface::CommandEntered(), make_unique<CommandIssuedObserver>(ci) );

    Stack::Instance().attach(Stack::StackChanged(), make_unique<StackUpdatedObserver>(cli) );

    cli.execute();

  return 0;
}
