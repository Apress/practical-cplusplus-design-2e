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
#include <QApplication>
#include "src/ui/MainWindow.h"

import pdCalc_utilities;
import pdCalc_commandDispatcher;
import pdCalc_stack;
import pdCalc_userInterface;

// I usually don't make blanket using namespace statements, but this
// example is meant to be simple.
using namespace pdCalc;

using std::cout;
using std::endl;
using std::make_unique;

int main(int argc, char* argv[])
{
    QApplication app{argc, argv};
    pdCalc::MainWindow gui{argc, argv};

    CommandInterpreter ci{gui};

    RegisterCoreCommands(gui);

    gui.attach(UserInterface::CommandEntered(), make_unique<CommandIssuedObserver>(ci) );

    Stack::Instance().attach(Stack::StackChanged(), make_unique<StackUpdatedObserver>( gui ) );

    gui.execute();

    return app.exec();
}
