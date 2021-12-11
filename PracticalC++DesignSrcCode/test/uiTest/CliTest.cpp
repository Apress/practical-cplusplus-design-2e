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

#include "CliTest.h"

#include <algorithm>
#include <fstream>
#include <format>
#include <cstdlib>
#include <cstdio>
#include <iterator>
#include <string>

using std::ifstream;
using std::string;
using std::ostream;
using std::vector;
using std::string_view;

namespace {

void checkFile(ifstream& ifs)
{
    bool valid{ifs};
    QVERIFY(valid);
}

void fixupPath(string& path)
{
#ifdef WIN32
    for(auto& c : path)
    {
        if(c == '/') c = '\\';
    }
#endif
    return;
}

// Windows and linux don't print exponents the same way
// windows prints e-00x while linux prints e-0x. To simplify
// comparisons, as lines are read, this function replaces
// e-00x with e-0x
void rationalizeExponent(string& s)
{
    if(string::size_type pos = s.find("e-00"); pos != string::npos)
    {
        s.erase(pos + 2, 1);
    }
}

}

string CliTest::path()
{
    string t = CLI_TEST_DIR;
    t += "/testCases/";
    return t;
}

void CliTest::runCliOnFile(const string& in, const string& out)
{
    string exe{("./pdCalc")};
    fixupPath(exe);

    auto cmd = std::format("{} --batch {} {}", exe, in, out);
    int result = system( cmd.c_str() );

    QCOMPARE(result, 0);

    return;
}

void CliTest::verifyOutput(const string& result, const string& baseline)
{
    vector<string> res{ vectorizeFile(result) };
    vector<string> base{ vectorizeFile(baseline) };

    QCOMPARE(res, base);

    return;
}

std::vector<string> CliTest::vectorizeFile(const string& fname)
{
    ifstream ifs{ fname.c_str() };
    checkFile(ifs);

    vector<string> tmp;
    string line;
    while( std::getline(ifs, line, '\n') )
    {
        rationalizeExponent(line);
        tmp.emplace_back( line );
    }

    return tmp;
}

void CliTest::removeFile(const string& f)
{
    int rem = remove( f.c_str() );
    QCOMPARE(rem, 0);

    return;
}

void CliTest::runTest(string_view name)
{

    string input = path() + "input" + name.data() + ".txt";
    string output = path() + "output" + name.data() + ".txt";
    string baseline = path() + "baseline" + name.data() + ".txt";

    runCliOnFile(input, output);
    verifyOutput(output, baseline);
    removeFile(output);
}

void CliTest::testCli1()
{
    runTest("Cli1");

    return;
}

void CliTest::testCli2()
{
    runTest("Cli2");

    return;
}
