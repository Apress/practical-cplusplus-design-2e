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

#include "MainWindow.h"
#include "Display.h"
#include "InputWidget.h"
#include <QVBoxLayout>
#include <string>
#include <string_view>
#include <memory>
#include <QGridLayout>
#include <iostream>
#include "LookAndFeel.h"
#include "StoredProcedureDialog.h"
#include <format>
#include "GuiModel.h"
#include <any>

import pdCalc_stack;

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::unique_ptr;
using std::string_view;

namespace pdCalc {

class MainWindow::MainWindowImpl : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindowImpl(MainWindow* parent);
    void showMessage(string_view m);
    void stackChanged();
    void setupFinalButtons();
    void addCommandButton(const string& dispPrimaryCmd, const string& primaryCmd, const string& dispShftCmd, const string& shftCmd);

public slots:
    void onCommandEntered(std::string cmd);
    void onShowMessage(std::string m);

private slots:
    void onProcedure();

private:
    void connectInputToModel();
    void doLayout();

    MainWindow& parent_;
    int nLinesStack_;
    Display* display_;
    InputWidget* inputWidget_;
    GuiModel* guiModel_;
};

MainWindow::MainWindowImpl::MainWindowImpl(MainWindow* parent)
: QWidget{parent}
, parent_(*parent)
, nLinesStack_{6}
{
    guiModel_ = new GuiModel{this};
    display_ = new Display{*guiModel_, this, nLinesStack_};

    connect(guiModel_, SIGNAL(modelChanged()), display_, SLOT(onModelChanged()));

    inputWidget_ = new InputWidget{this};

    doLayout();

    connectInputToModel();
}

void MainWindow::MainWindowImpl::doLayout()
{
    auto vblayout = new QVBoxLayout{this};
    vblayout->addWidget(display_);
    vblayout->addWidget(inputWidget_);
    vblayout->addStretch();
}

void MainWindow::MainWindowImpl::showMessage(string_view m)
{
    display_->showMessage(m);

    return;
}

void MainWindow::MainWindowImpl::stackChanged()
{
    auto v = Stack::Instance().getElements(nLinesStack_);
    guiModel_->stackChanged(v);

    return;
}

void MainWindow::MainWindowImpl::onProcedure()
{
    if(StoredProcedureDialog dialog; dialog.exec() == QDialog::Accepted)
    {
        auto t = std::format( "proc:{}", dialog.getProcedure() );
        guiModel_->onCommandEntered(t, "");
    }

    return;
}

void MainWindow::MainWindowImpl::connectInputToModel()
{
    connect(inputWidget_, SIGNAL(backspacePressed()), guiModel_, SLOT(onBackspace()));
    connect(inputWidget_, SIGNAL(enterPressed()), guiModel_, SLOT(onEnter()));
    connect(inputWidget_, SIGNAL(characterEntered(char)), guiModel_, SLOT(onCharacterEntered(char)));
    connect(inputWidget_, SIGNAL(plusMinusPressed()), guiModel_, SLOT(onPlusMinus()));
    connect(inputWidget_, SIGNAL(commandEntered(std::string, std::string)), guiModel_, SLOT(onCommandEntered(std::string, std::string)));
    connect(inputWidget_, SIGNAL(procedurePressed()), this, SLOT(onProcedure()));
    connect(inputWidget_, SIGNAL(shiftPressed()), guiModel_, SLOT(onShift()));
    connect(guiModel_, SIGNAL(commandEntered(std::string)), this, SLOT(onCommandEntered(std::string)));
    connect(guiModel_, SIGNAL(errorDetected(std::string)), this, SLOT(onShowMessage(std::string)));

    return;
}

void MainWindow::MainWindowImpl::setupFinalButtons()
{
    inputWidget_->setupFinalButtons();
}

void MainWindow::MainWindowImpl::addCommandButton(const string& dispPrimaryCmd, const string& primaryCmd,
const string& dispShftCmd, const string& shftCmd)
{
    inputWidget_->addCommandButton(dispPrimaryCmd, primaryCmd, dispShftCmd, shftCmd);

    return;
}

void MainWindow::MainWindowImpl::onCommandEntered(string cmd)
{
    parent_.UserInterface::raise(UserInterface::CommandEntered(), cmd);

    return;
}

void MainWindow::MainWindowImpl::onShowMessage(string m)
{
    showMessage(m);
}

MainWindow::MainWindow(int, char*[], QWidget* parent)
: QMainWindow{parent}
{
    pimpl_ = new MainWindowImpl{this};
    setCentralWidget(pimpl_);
}

void MainWindow::postMessage(string_view m)
{
    pimpl_->showMessage(m);

    return;
}

void MainWindow::stackChanged()
{
    pimpl_->stackChanged();
}

void MainWindow::addCommandButton(const string& dispPrimaryCmd, const string& primaryCmd,
const string& dispShftCmd, const string& shftCmd)
{
    pimpl_->addCommandButton(dispPrimaryCmd, primaryCmd, dispShftCmd, shftCmd);
}

void MainWindow::execute()
{
    // setup the undo, redo, proc buttons after inserting plugin buttons
    pimpl_->setupFinalButtons();

    show();

    // force the window to be fixed size...should be called as a final step once the GUI has fixed its own size
    setFixedSize( size() );
}

}

#include "MainWindow.moc"
