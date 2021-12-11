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

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <string>
#include <string_view>

import pdCalc_utilities;
import pdCalc_userInterface;

namespace pdCalc {

class CommandButton;

class MainWindow : public QMainWindow, public UserInterface
{
    class MainWindowImpl;
public:
    MainWindow(int argc, char* argv[], QWidget* parent = nullptr);

    void postMessage(std::string_view m) override;
    void stackChanged() override;

    void execute();

    // Add a command button, for example, from a plugin
    // Buttons are added in order from left to right just below the
    // line undo, redo, proc from the bottom up
    // Buttons are reparented to MainWindow
    void addCommandButton(const std::string& dispPrimaryCmd, const std::string& primaryCmd,
        const std::string& dispShftCmd, const std::string& shftCmd);

private:

    MainWindowImpl* pimpl_;
};

}

#endif
