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

#include "DisplayTest.h"
#include "src/ui/Display.h"
#include "src/ui/GuiModel.h"
#include <QApplication>
#include <string>
#include <vector>
#include <iostream>
#include <format>
#include <iterator>

using std::string_view;
using std::cout;
using std::endl;
using std::vector;
using std::string;

DisplayTest::DisplayTest()
: nCharWide_(25)
, nLinesStack_(6)
{ }

bool DisplayTest::displayMatches(string_view input, const std::vector<double>& v) const
{
    string text = display_->getText();

    bool hasInput = !input.empty();

    string ans;
    auto bi = std::back_inserter(ans);
    int start = nLinesStack_ - 1 - (hasInput ? 1 : 0);
    for(int i = start; i > -1; --i)
    {
        string t = i < static_cast<int>(v.size()) ? std::format("{}", v[i]) : "";
        std::format_to(bi, "{0}:{1:>{2}}{3}", i + 1, t, nCharWide_ - 2,
            (i == 0 && !hasInput ? "" : "\n"));
    }

    if(hasInput)
        std::format_to(bi, "{}", input);

    bool result = text == ans;

#ifdef VERBOSE
    if(!result)
    {
        cout << "\n"
             << "Display:\n"
             << text << "\n\n"
             << "Expected:\n"
             << oss.str() << endl;
    }
#endif

    return result;
}

void DisplayTest::init()
{
    int argc = 1;
    char* argv[1];
    argv[0] = new char[2]{'t', '\0'};
    app_ = new QApplication(argc, argv);

    guiModel_ = new pdCalc::GuiModel(nullptr);

    display_ = new pdCalc::Display{*guiModel_, nullptr, nLinesStack_, nCharWide_};

    QObject::connect(guiModel_, SIGNAL(modelChanged()), display_, SLOT(onModelChanged()));

    QVERIFY( displayMatches("", {}) );

    delete [] argv[0];
}

void DisplayTest::cleanup()
{
    delete display_;
    delete guiModel_;
    delete app_;
}

void DisplayTest::testUpdateStack()
{
    vector<double> v{ 1.3, 4.5 };
    guiModel_->stackChanged(v);
    QVERIFY( displayMatches("", v) );

    v = { 1.8, 15.3, 18.2, -21.4, 1e-3, -34, 62 };
    guiModel_->stackChanged(v);
    QVERIFY( displayMatches("", v) );

    v = { 1.7 };
    guiModel_->stackChanged(v);
    QVERIFY( displayMatches("", v) );

    v = {  };
    guiModel_->stackChanged(v);
    QVERIFY( displayMatches("", v) );

    v = { -3.4 };
    guiModel_->stackChanged(v);
    QVERIFY( displayMatches("", v) );

    return;
}

void DisplayTest::testInput(const vector<char>& input, const vector<double>& values)
{
    string t;
    auto bi = back_inserter(t);

    QVERIFY( displayMatches(t, values) );

    for(auto c : input)
    {
        guiModel_->onCharacterEntered(c);
        format_to(bi, "{}", c);
        QVERIFY( displayMatches(t, values) );
    }

    // do loop longer than input to see how it responds to backspace on empty
    for(auto i = 0u; i < input.size() + 2; ++i)
    {
        guiModel_->onBackspace();
        if(i < input.size()) t.erase(t.size() - 1, 1);
        QVERIFY( displayMatches(t, values) );
    }

    return;
}

void DisplayTest::testInput()
{
    vector<double> v;
    vector<char> input = { '1', '2', '.', '3', 'e', '-', '2' };

    testInput(input, v);

    guiModel_->clearInput();

    testInput({}, {});

    v = { 1.3, 4.5 };
    guiModel_->stackChanged(v);
    testInput(input, v);

    guiModel_->clearInput();
    testInput({}, v);

    v = { 1.8, 15.3, 18.2, -21.4, 1e-3, -34, 62 };
    guiModel_->stackChanged(v);
    testInput(input, v);

    guiModel_->clearInput();
    testInput({}, v);

    return;
}

void DisplayTest::testState()
{
    QCOMPARE( guiModel_->inputEmpty(), true );

    guiModel_->onCharacterEntered('-');
    QCOMPARE( guiModel_->getState().curInputValidity, QValidator::Intermediate );

    guiModel_->onCharacterEntered('3');
    QCOMPARE( guiModel_->getState().curInputValidity, QValidator::Acceptable );

    guiModel_->onCharacterEntered('0');
    QCOMPARE( guiModel_->getState().curInputValidity, QValidator::Acceptable );

    guiModel_->onCharacterEntered('.');
    QCOMPARE( guiModel_->getState().curInputValidity, QValidator::Acceptable );

    guiModel_->onCharacterEntered('1');
    QCOMPARE( guiModel_->getState().curInputValidity, QValidator::Acceptable );

    guiModel_->onCharacterEntered('e');
    QCOMPARE( guiModel_->getState().curInputValidity, QValidator::Intermediate );

    guiModel_->onCharacterEntered('+');
    QCOMPARE( guiModel_->getState().curInputValidity, QValidator::Intermediate );

    QCOMPARE( guiModel_->inputEmpty(), false );

    guiModel_->onCharacterEntered('2');
    QCOMPARE( guiModel_->getState().curInputValidity, QValidator::Acceptable );

    // This is still acceptable because display simply rejects it
    guiModel_->onCharacterEntered('b');
    QCOMPARE( guiModel_->getState().curInputValidity, QValidator::Acceptable );

    guiModel_->onBackspace();
    QCOMPARE( guiModel_->getState().curInputValidity, QValidator::Intermediate );

    guiModel_->clearInput();
    QCOMPARE( guiModel_->getState().curInputValidity, QValidator::Intermediate );

    QCOMPARE( guiModel_->inputEmpty(), true );

    guiModel_->onCharacterEntered('-');
    QCOMPARE( guiModel_->getState().curInputValidity, QValidator::Intermediate );

    guiModel_->onCharacterEntered('3');
    QCOMPARE( guiModel_->getState().curInputValidity, QValidator::Acceptable );

    return;
}

void DisplayTest::testPlusMinus()
{
    QCOMPARE( guiModel_->inputEmpty(), true );

    guiModel_->onCharacterEntered('1');
    guiModel_->onCharacterEntered('.');
    guiModel_->onCharacterEntered('1');
    guiModel_->onPlusMinus();
    QVERIFY(displayMatches("-1.1", {}));
    guiModel_->onPlusMinus();
    QVERIFY(displayMatches("1.1", {}));
    guiModel_->onPlusMinus();
    guiModel_->onCharacterEntered('e');
    guiModel_->onPlusMinus();
    QVERIFY(displayMatches("-1.1e-", {}));
    guiModel_->onPlusMinus();
    QVERIFY(displayMatches("-1.1e", {}));
    guiModel_->onCharacterEntered('5');
    QVERIFY(displayMatches("-1.1e5", {}));
    guiModel_->onPlusMinus();
    QVERIFY(displayMatches("-1.1e-5", {}));
    guiModel_->onPlusMinus();
    QVERIFY(displayMatches("-1.1e5", {}));
    guiModel_->onPlusMinus();
    QVERIFY(displayMatches("-1.1e-5", {}));
    guiModel_->onBackspace();
    guiModel_->onBackspace();
    QVERIFY(displayMatches("-1.1e", {}));

    return;
}
