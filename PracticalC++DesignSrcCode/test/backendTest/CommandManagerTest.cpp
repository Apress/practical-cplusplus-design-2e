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

#include "CommandManagerTest.h"

#include <memory>
#include <string>
#include <iostream>

import pdCalc_commandDispatcher;
import pdCalc_command;

using namespace pdCalc;
using std::cout;
using std::endl;
using std::string;

namespace {

class TestCommand : public Command
{
public:
    TestCommand();

    unsigned int getExecuteCount() const { return executeCount_; }
    unsigned int getUndoCount() const { return undoCount_; }

private:
    void executeImpl() noexcept override;
    void undoImpl() noexcept override;
    Command* cloneImpl() const noexcept override { return nullptr; }
    const char* helpMessageImpl() const noexcept override { return ""; }

private:
    unsigned int executeCount_;
    unsigned int undoCount_;
};

TestCommand::TestCommand()
: executeCount_{0}
, undoCount_{0}
{

}

void TestCommand::executeImpl() noexcept
{
    ++executeCount_;
}

void TestCommand::undoImpl() noexcept
{
    ++undoCount_;
}

class TestDeleteCommand : public Command
{
public:
    TestDeleteCommand(bool& b);
    ~TestDeleteCommand();

private:
    void executeImpl() noexcept override { }
    void undoImpl() noexcept override { }
    Command* cloneImpl() const noexcept { return nullptr; }
    const char* helpMessageImpl() const noexcept override { return ""; }

private:
    bool& deleted_;
};

TestDeleteCommand::TestDeleteCommand(bool& b)
: deleted_{b}
{
    deleted_ = false;
}

TestDeleteCommand::~TestDeleteCommand()
{
    deleted_ = true;
}

void testExecute(CommandManager::UndoRedoStrategy st)
{
    TestCommand* raw1 = new TestCommand;
    TestCommand* raw2 = new TestCommand;
    TestCommand* raw3 = new TestCommand;

    CommandManager cm(st);
    QVERIFY( cm.getUndoSize() == 0 );
    QVERIFY( cm.getRedoSize() == 0 );

    cm.executeCommand( MakeCommandPtr(raw1) );
    QVERIFY( cm.getUndoSize() == 1 );

    cm.executeCommand( MakeCommandPtr(raw2) );
    QVERIFY( cm.getUndoSize() == 2 );

    cm.executeCommand( MakeCommandPtr(raw3) );
    QVERIFY( cm.getUndoSize() == 3 );
    QVERIFY( cm.getRedoSize() == 0 );

    QCOMPARE( raw1->getExecuteCount(), 1u );
    QCOMPARE( raw2->getExecuteCount(), 1u );
    QCOMPARE( raw3->getExecuteCount(), 1u );
    QCOMPARE( raw1->getUndoCount(), 0u );
    QCOMPARE( raw2->getUndoCount(), 0u );
    QCOMPARE( raw3->getUndoCount(), 0u );

    return;
}

void testUndo(CommandManager::UndoRedoStrategy st)
{
    TestCommand* raw1 = new TestCommand;
    TestCommand* raw2 = new TestCommand;
    TestCommand* raw3 = new TestCommand;

    CommandManager cm(st);

    cm.executeCommand( MakeCommandPtr(raw1) );
    cm.executeCommand( MakeCommandPtr(raw2) );
    cm.executeCommand( MakeCommandPtr(raw3) );
    QVERIFY( cm.getUndoSize() == 3 );

    cm.undo();

    QVERIFY( cm.getUndoSize() == 2 );
    QVERIFY( cm.getRedoSize() == 1 );
    QCOMPARE( raw1->getExecuteCount(), 1u );
    QCOMPARE( raw2->getExecuteCount(), 1u );
    QCOMPARE( raw3->getExecuteCount(), 1u );
    QCOMPARE( raw1->getUndoCount(), 0u );
    QCOMPARE( raw2->getUndoCount(), 0u );
    QCOMPARE( raw3->getUndoCount(), 1u );

    cm.undo();

    QVERIFY( cm.getUndoSize() == 1 );
    QVERIFY( cm.getRedoSize() == 2 );
    QCOMPARE( raw1->getExecuteCount(), 1u );
    QCOMPARE( raw2->getExecuteCount(), 1u );
    QCOMPARE( raw3->getExecuteCount(), 1u );
    QCOMPARE( raw1->getUndoCount(), 0u );
    QCOMPARE( raw2->getUndoCount(), 1u );
    QCOMPARE( raw3->getUndoCount(), 1u );

    cm.undo();

    QVERIFY( cm.getUndoSize() == 0 );
    QVERIFY( cm.getRedoSize() == 3 );
    QCOMPARE( raw1->getExecuteCount(), 1u );
    QCOMPARE( raw2->getExecuteCount(), 1u );
    QCOMPARE( raw3->getExecuteCount(), 1u );
    QCOMPARE( raw1->getUndoCount(), 1u );
    QCOMPARE( raw2->getUndoCount(), 1u );
    QCOMPARE( raw3->getUndoCount(), 1u );

    return;
}

void testRedo(CommandManager::UndoRedoStrategy st)
{
    TestCommand* raw1 = new TestCommand;
    TestCommand* raw2 = new TestCommand;
    TestCommand* raw3 = new TestCommand;

    CommandManager cm(st);

    cm.executeCommand( MakeCommandPtr(raw1) );
    cm.executeCommand( MakeCommandPtr(raw2) );
    cm.executeCommand( MakeCommandPtr(raw3) );
    QVERIFY( cm.getUndoSize() == 3 );

    cm.undo();
    cm.undo();
    cm.undo();

    cm.redo();
    QVERIFY( cm.getUndoSize() == 1 );
    QVERIFY( cm.getRedoSize() == 2 );
    QCOMPARE( raw1->getExecuteCount(), 2u );
    QCOMPARE( raw2->getExecuteCount(), 1u );
    QCOMPARE( raw3->getExecuteCount(), 1u );
    QCOMPARE( raw1->getUndoCount(), 1u );
    QCOMPARE( raw2->getUndoCount(), 1u );
    QCOMPARE( raw3->getUndoCount(), 1u );

    cm.redo();
    QVERIFY( cm.getUndoSize() == 2 );
    QVERIFY( cm.getRedoSize() == 1 );
    QCOMPARE( raw1->getExecuteCount(), 2u );
    QCOMPARE( raw2->getExecuteCount(), 2u );
    QCOMPARE( raw3->getExecuteCount(), 1u );
    QCOMPARE( raw1->getUndoCount(), 1u );
    QCOMPARE( raw2->getUndoCount(), 1u );
    QCOMPARE( raw3->getUndoCount(), 1u );

    cm.redo();
    QVERIFY( cm.getUndoSize() == 3 );
    QVERIFY( cm.getRedoSize() == 0 );
    QCOMPARE( raw1->getExecuteCount(), 2u );
    QCOMPARE( raw2->getExecuteCount(), 2u );
    QCOMPARE( raw3->getExecuteCount(), 2u );
    QCOMPARE( raw1->getUndoCount(), 1u );
    QCOMPARE( raw2->getUndoCount(), 1u );
    QCOMPARE( raw3->getUndoCount(), 1u );

    return;
}

void testRedoStackFlush(CommandManager::UndoRedoStrategy st)
{
    TestCommand* raw1 = new TestCommand;
    TestCommand* raw2 = new TestCommand;

    CommandManager cm(st);

    cm.executeCommand( MakeCommandPtr(raw1) );
    cm.executeCommand( MakeCommandPtr(raw2) );

    bool deleted = false;

    cm.executeCommand( MakeCommandPtr<TestDeleteCommand>(deleted) );

    // push TestDeleteCommand and raw2 to redo stack
    cm.undo();
    cm.undo();
    QVERIFY( cm.getUndoSize() == 1 );
    QVERIFY( cm.getRedoSize() == 2 );

    // add a new command, which should destroy the redo stack
    TestCommand* raw3 = new TestCommand;
    cm.executeCommand( MakeCommandPtr(raw3) );

    QCOMPARE( deleted, true );
    QVERIFY( cm.getUndoSize() == 2 );
    QVERIFY( cm.getRedoSize() == 0 );

    QCOMPARE( raw1->getExecuteCount(), 1u );
    QCOMPARE( raw1->getUndoCount(), 0u );
    QCOMPARE( raw3->getExecuteCount(), 1u );
    QCOMPARE( raw3->getUndoCount(), 0u );

    cm.undo();
    cm.redo();

    QVERIFY( cm.getUndoSize() == 2 );
    QVERIFY( cm.getRedoSize() == 0 );

    QCOMPARE( raw1->getExecuteCount(), 1u );
    QCOMPARE( raw1->getUndoCount(), 0u );
    QCOMPARE( raw3->getExecuteCount(), 2u );
    QCOMPARE( raw3->getUndoCount(), 1u );

    cm.undo();
    cm.undo();
    cm.redo();
    cm.redo();
    cm.undo();

    QVERIFY( cm.getUndoSize() == 1 );
    QVERIFY( cm.getRedoSize() == 1 );

    QCOMPARE( raw1->getExecuteCount(), 2u );
    QCOMPARE( raw1->getUndoCount(), 1u );
    QCOMPARE( raw3->getExecuteCount(), 3u );
    QCOMPARE( raw3->getUndoCount(), 3u );

    return;
}

void testResourceCleanup(CommandManager::UndoRedoStrategy st)
{
    bool deleted1 = false;
    bool deleted2 = false;
    bool deleted3 = false;

    CommandManager* cm = new CommandManager(st);
    cm->executeCommand( MakeCommandPtr<TestDeleteCommand>(deleted1) );
    cm->executeCommand( MakeCommandPtr<TestDeleteCommand>(deleted2) );
    cm->executeCommand( MakeCommandPtr<TestDeleteCommand>(deleted3) );

    QCOMPARE( deleted1, false );
    QCOMPARE( deleted2, false );
    QCOMPARE( deleted3, false );

    delete cm;

    QCOMPARE( deleted1, true );
    QCOMPARE( deleted2, true );
    QCOMPARE( deleted3, true );

    return;
}

void ignoreError(CommandManager::UndoRedoStrategy st)
{
    // This test passes by simply not crashing
    CommandManager cm(st);
    cm.undo();
    cm.redo();

    return;
}

}

void CommandManagerTest::testExecuteStackStrategy()
{
    testExecute(CommandManager::UndoRedoStrategy::StackStrategy);
}

void CommandManagerTest::testUndoStackStrategy()
{
    testUndo(CommandManager::UndoRedoStrategy::StackStrategy);
}

void CommandManagerTest::testRedoStackStrategy()
{
    testRedo(CommandManager::UndoRedoStrategy::StackStrategy);
}

void CommandManagerTest::testRedoStackFlushStackStrategy()
{
    testRedoStackFlush(CommandManager::UndoRedoStrategy::StackStrategy);
}

void CommandManagerTest::testResourceCleanupStackStrategy()
{
    testResourceCleanup(CommandManager::UndoRedoStrategy::StackStrategy);
}

void CommandManagerTest::ignoreErrorStackStrategy()
{
    ignoreError(CommandManager::UndoRedoStrategy::StackStrategy);
}

void CommandManagerTest::testExecuteListStrategy()
{
    testExecute(CommandManager::UndoRedoStrategy::ListStrategy);
}

void CommandManagerTest::testUndoListStrategy()
{
    testUndo(CommandManager::UndoRedoStrategy::ListStrategy);
}

void CommandManagerTest::testRedoListStrategy()
{
    testRedo(CommandManager::UndoRedoStrategy::ListStrategy);
}

void CommandManagerTest::testRedoStackFlushListStrategy()
{
    testRedoStackFlush(CommandManager::UndoRedoStrategy::ListStrategy);
}

void CommandManagerTest::testResourceCleanupListStrategy()
{
    testResourceCleanup(CommandManager::UndoRedoStrategy::ListStrategy);
}

void CommandManagerTest::ignoreErrorListStrategy()
{
    ignoreError(CommandManager::UndoRedoStrategy::ListStrategy);
}

void CommandManagerTest::testExecuteListStrategyVector()
{
    testExecute(CommandManager::UndoRedoStrategy::ListStrategyVector);
}

void CommandManagerTest::testUndoListStrategyVector()
{
    testUndo(CommandManager::UndoRedoStrategy::ListStrategyVector);
}

void CommandManagerTest::testRedoListStrategyVector()
{
    testRedo(CommandManager::UndoRedoStrategy::ListStrategyVector);
}

void CommandManagerTest::testRedoStackFlushListStrategyVector()
{
    testRedoStackFlush(CommandManager::UndoRedoStrategy::ListStrategyVector);
}

void CommandManagerTest::testResourceCleanupListStrategyVector()
{
    testResourceCleanup(CommandManager::UndoRedoStrategy::ListStrategyVector);
}

void CommandManagerTest::ignoreErrorListStrategyVector()
{
    ignoreError(CommandManager::UndoRedoStrategy::ListStrategyVector);
}
