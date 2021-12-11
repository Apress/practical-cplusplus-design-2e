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
#include <stack>
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>

export module pdCalc_commandDispatcher:CoreCommands;

import pdCalc_stack;
import pdCalc_command;
import pdCalc_utilities;

using std::vector;
using std::string;

#define CLONE(X) X* cloneImpl() const override { return new X{*this}; }
#define HELP(X) const char* helpMessageImpl() const noexcept override { return X; }

namespace {

using namespace pdCalc;

double eps = 1e-12; // arbitrary floating closeness

bool topOfStackisBetween(double lb, double ub)
{
    assert(lb <= ub);

    auto v = Stack::Instance().getElements(1);
    auto d = v.back();

    return d >= lb && d <= ub;
}

// for y^x, we must have
// 1) If y == 0, x must be >= 0
// 2) If y < 0, x must be integral
// also works for nth rootOf(y) for x = 1/n
bool passesPowerTest(double y, double x)
{
    // check against true 0; otherwise it is invertible
    // although not well conditioned
    if(y == 0 && x < 0)
        return false;

    if(double intPart; y < 0 && std::modf(x, &intPart) != 0.0 )
        return false;

    return true;
}

}

namespace pdCalc {

// accepts a number from input and adds it to the stack
// no preconditions are necessary for this command
export class EnterNumber : public Command
{
public:
    explicit EnterNumber(double d)
    : Command{}
    , number_{d}
    { }

    explicit EnterNumber(const EnterNumber& rhs)
    : Command{rhs}
    , number_{rhs.number_}
    { }

    ~EnterNumber() = default;

private:
    EnterNumber(EnterNumber&&) = delete;
    EnterNumber& operator=(const EnterNumber&) = delete;
    EnterNumber& operator=(EnterNumber&&) = delete;

    // adds the number to the stack
    void executeImpl() noexcept override
    {
        Stack::Instance().push(number_);
    }

    // removes the number from the stack
    void undoImpl() noexcept override
    {
        Stack::Instance().pop();
    }

    CLONE(EnterNumber)
    HELP("Adds a number to the stack")

    double number_;
};

// swaps two numbers on the stack
// precondition: must have two elements on the stack
export class SwapTopOfStack : public Command
{
public:
    SwapTopOfStack() = default;

    explicit SwapTopOfStack(const SwapTopOfStack& rhs)
    : Command{rhs}
    { }

    ~SwapTopOfStack() = default;

private:
    SwapTopOfStack(SwapTopOfStack&&) = delete;
    SwapTopOfStack& operator=(const SwapTopOfStack&) = delete;
    SwapTopOfStack& operator=(SwapTopOfStack&&) = delete;

    // throws an exception if the stack size is less than two
    void checkPreconditionsImpl() const override
    {
        if( Stack::Instance().size() < 2 )
            throw Exception{"Stack must have 2 elements"};
    }

    // swaps the top two stack positions
    void executeImpl() noexcept override
    {
        Stack::Instance().swapTop();
    }

    // reverses the swap operation
    void undoImpl() noexcept override
    {
        Stack::Instance().swapTop();
    }

    CLONE(SwapTopOfStack)
    HELP("Swap the top two elements of the stack")
};

// drop the top of the stack
// precondition: must have at least one element on the stack
export class DropTopOfStack : public Command
{
public:
    DropTopOfStack() = default;

    explicit DropTopOfStack(const DropTopOfStack& rhs)
    : Command{rhs}
    , droppedNumber_{rhs.droppedNumber_}
    { }

    ~DropTopOfStack() = default;

private:
    DropTopOfStack(DropTopOfStack&&) = delete;
    DropTopOfStack& operator=(const DropTopOfStack&) = delete;
    DropTopOfStack& operator=(DropTopOfStack&&) = delete;

    // throws an execption if the stack size is less than one
    void checkPreconditionsImpl() const override
    {
        if( Stack::Instance().size() < 1 )
            throw Exception{"Stack must have 1 element"};
    }

    // drops the top element on the stack
    void executeImpl() noexcept override
    {
        droppedNumber_ = Stack::Instance().pop();
    }

    // returns the dropped element back to the top of the stack
    void undoImpl() noexcept override
    {
        Stack::Instance().push(droppedNumber_);
    }

    CLONE(DropTopOfStack)
    HELP("Drop the top element from the stack")

    double droppedNumber_;
};

// clear the stack
// precondition: must have at least one element on the stack
export class ClearStack : public Command
{
public:
    ClearStack() = default;

    explicit ClearStack(const ClearStack& rhs)
    : Command{rhs}
    , stack_{rhs.stack_}
    { }

    ~ClearStack() = default;

private:
    ClearStack(ClearStack&&) = delete;
    ClearStack& operator=(const ClearStack&) = delete;
    ClearStack& operator=(ClearStack&&) = delete;

    // clears the stack
    void executeImpl() noexcept override
    {
        const auto n = Stack::Instance().size();
        if(n == 0) return;
        for(auto i = 0u; i < n - 1; ++i)
        {
            stack_.push( Stack::Instance().pop(true) );
        }

        stack_.push( Stack::Instance().pop(false) );
    }

    // restores the stack
    void undoImpl() noexcept override
    {
        const auto n = stack_.size();
        if(n == 0) return;
        for(auto i = 0u; i < n - 1; ++i)
        {
            Stack::Instance().push(stack_.top(), true);
            stack_.pop();
        }

        Stack::Instance().push(stack_.top(), false);
        stack_.pop();
    }

    CLONE(ClearStack)
    HELP("Clear the stack")

    std::stack<double> stack_;
};

// adds two elements on the stack
export class Add : public BinaryCommand
{
public:
    Add() = default;

    explicit Add(const Add& rhs)
    : BinaryCommand{rhs}
    { }

    ~Add() = default;

private:
    Add(Add&&) = delete;
    Add& operator=(const Add&) = delete;
    Add& operator=(Add&&) = delete;

    double binaryOperation(double next, double top) const noexcept override
    {
        return next + top;
    }

    CLONE(Add)
    HELP("Replace first two elements on the stack with their sum")
};

// subtracts two elements on the stack
export class Subtract : public BinaryCommand
{
public:
    Subtract() = default;

    explicit Subtract(const Subtract& rhs)
    : BinaryCommand{rhs}
    { }

    ~Subtract() = default;

private:
    Subtract(Subtract&&) = delete;
    Subtract& operator=(const Subtract&) = delete;
    Subtract& operator=(Subtract&&) = delete;

    double binaryOperation(double next, double top) const noexcept override
    {
        return next - top;
    }

    CLONE(Subtract)
    HELP("Replace first two elements on the stack with their difference")
};

// class Multiply is missing. It is achieved through BinaryCommandAlterative.
// See its creation in RegisterCoreCommands()

// divides two elements on the stack
// precondition: divisor cannot be zero
export class Divide : public BinaryCommand
{
public:
    Divide() = default;

    explicit Divide(const Divide& rhs)
    : BinaryCommand{rhs}
    { }

    ~Divide() = default;

private:
    Divide(Divide&&) = delete;
    Divide& operator=(const Divide&) = delete;
    Divide& operator=(Divide&&) = delete;

    // checks binary preconditions
    // and throws if division by 0
    // checks for true 0, not epsilon close
    void checkPreconditionsImpl() const override
    {
        BinaryCommand::checkPreconditionsImpl();
        if(auto v = Stack::Instance().getElements(1); v.front() == 0. || v.front() == -0.)
            throw Exception{"Division by zero"};

        return;
    }

    double binaryOperation(double next, double top) const noexcept override
    {
        return next / top;
    }

    CLONE(Divide)
    HELP("Replace first two elements on the stack with their quotient")
};

// takes two numbers from the stack (y == next, x == top) and implements y^x
// preconditions: 1) Two elements on the stack
//                2) If y == 0, x must be >= 0
//                3) If y < 0, x must be integral
export class Power : public BinaryCommand
{
public:
    Power() = default;

    explicit Power(const Power& rhs)
    : BinaryCommand{rhs}
    { }

    ~Power() = default;

private:
    Power(Power&&) = delete;
    Power& operator=(const Power&) = delete;
    Power& operator=(Power&&) = delete;

    // throws if preconditions are not met
    void checkPreconditionsImpl() const override
    {
        BinaryCommand::checkPreconditionsImpl();

        if( auto v = Stack::Instance().getElements(2); !passesPowerTest(v[1], v[0]) )
            throw Exception{"Invalid result"};
    }

    double binaryOperation(double next, double top) const noexcept override
    {
        return std::pow(next, top);
    }

    CLONE(Power)
    HELP("Replace first two elements on the stack, y, x, with y^x. Note, x is top of stack")
};

// takes two numbers from the stack (y == next, x == top) and implements xth root of y
// preconditions: 1) Two elements on the stack
//                2) If y == 0, x must be >= 0
//                3) If y < 0, 1/x must be integral
//                4) x != 0
export class Root : public BinaryCommand
{
public:
    Root() = default;

    explicit Root(const Root& rhs)
    : BinaryCommand{rhs}
    { }

    ~Root() = default;

private:
    Root(Root&&) = delete;
    Root& operator=(const Root&) = delete;
    Root& operator=(Root&&) = delete;

    // throws if preconditions are not met
    void checkPreconditionsImpl() const override
    {
        BinaryCommand::checkPreconditionsImpl();

        if( auto v = Stack::Instance().getElements(2); !passesPowerTest(v[1], 1. / v[0]) || v[0] == 0.0 )
            throw Exception{"Invalid result"};
    }

    double binaryOperation(double next, double top) const noexcept override
    {
        return std::pow(next, 1. / top);
    }

    CLONE(Root)
    HELP("Replace first two elements on teh stack, y, x, with xth root of y. Note, x is top of stack")
};

// takes the sine of a number on the stack in radians
// precondition: at least one number on the stack
export class Sine : public UnaryCommand
{
public:
    Sine() = default;

    explicit Sine(const Sine& rhs)
    : UnaryCommand{rhs}
    { }

    ~Sine() = default;

private:
    Sine(Sine&&) = delete;
    Sine& operator=(const Sine&) = delete;
    Sine& operator=(Sine&&) = delete;

    double unaryOperation(double top) const noexcept override
    {
        return std::sin(top);
    }

    CLONE(Sine)
    HELP("Replace the first element, x, on the stack with sin(x). x must be in radians")
};

// takes the cosine of a number on the stack in radians
// precondition: at least one number on the stack
export class Cosine : public UnaryCommand
{
public:
    Cosine() = default;

    explicit Cosine(const Cosine& rhs)
    : UnaryCommand{rhs}
    { }

    ~Cosine() = default;

private:
    Cosine(Cosine&&) = delete;
    Cosine& operator=(const Cosine&) = delete;
    Cosine& operator=(Cosine&&) = delete;

    double unaryOperation(double top) const noexcept override
    {
        return std::cos(top);
    }

    CLONE(Cosine)
    HELP("Replace the first element, x, on the stack with cos(x). x must be in radians")
};

// takes the tangent of a number on the stack in radians
// preconditions: 1) at least one number on the stack
//                2) number cannot be a multiple of pi/2 +/- pi
export class Tangent : public UnaryCommand
{
public:
    Tangent() = default;

    explicit Tangent(const Tangent& rhs)
    : UnaryCommand{rhs}
    { }

    ~Tangent() = default;

private:
    Tangent(Tangent&&) = delete;
    Tangent& operator=(const Tangent&) = delete;
    Tangent& operator=(Tangent&&) = delete;

    // checks unary condition
    // throws if a multiple of pi/2 +/- pi
    void checkPreconditionsImpl() const override
    {
        UnaryCommand::checkPreconditionsImpl();

        auto v = Stack::Instance().getElements(1);
        double d{ v.back() + M_PI / 2. };
        double r{ std::fabs(d) / std::fabs(M_PI) };
        int w{ static_cast<int>(std::floor(r + eps)) };
        r = r - w;
        if(r < eps && r > -eps)
            throw Exception{"Infinite result"};
    }

    double unaryOperation(double top) const noexcept override
    {
        return std::tan(top);
    }

    CLONE(Tangent)
    HELP("Replace the first element, x, on the stack with tan(x). x must be in radians")
};

// takes the arcsine of a number on the stack and returns radians
// preconditions: 1) at least one number on the stack
//                2) number, x, must satisfy -1 <= x <= 1
export class Arcsine : public UnaryCommand
{
public:
    Arcsine() = default;

    explicit Arcsine(const Arcsine& rhs)
    : UnaryCommand{rhs}
    { }

    ~Arcsine() = default;

private:
    Arcsine(Arcsine&&) = delete;
    Arcsine& operator=(const Arcsine&) = delete;
    Arcsine& operator=(Arcsine&&) = delete;

    // checks unary condition
    // throws if value outside [-1, 1]
    void checkPreconditionsImpl() const override
    {
        UnaryCommand::checkPreconditionsImpl();

        if(!topOfStackisBetween(-1, 1))
            throw Exception{"Invalid argument"};
    }

    double unaryOperation(double top) const noexcept override
    {
        return std::asin(top);
    }

    CLONE(Arcsine)
    HELP("Replace the first element, x, on the stack with arcsin(x). Returns result in radians")
};

// takes the arccosine of a number on the stack and returns radians
// preconditions: 1) at lesat one number on the stack
//                2) number, x, must satisfy -1 <= x <= 1
export class Arccosine : public UnaryCommand
{
public:
    Arccosine() = default;

    explicit Arccosine(const Arccosine& rhs)
    : UnaryCommand{rhs}
    { }

    ~Arccosine() = default;

private:
    Arccosine(Arccosine&&) = delete;
    Arccosine& operator=(const Arccosine&) = delete;
    Arccosine& operator=(Arccosine&&) = delete;

    // check unary condition
    // throws if value outside [-1, 1]
    void checkPreconditionsImpl() const override
    {
        UnaryCommand::checkPreconditionsImpl();

        if(!topOfStackisBetween(-1, 1))
            throw Exception{"Invalid argument"};
    }

    double unaryOperation(double top) const noexcept override
    {
        return std::acos(top);
    }

    CLONE(Arccosine)
    HELP("Replace the first element, x, on the stack with arccos(x). Returns result in radians")
 };

// takes the arctangent of a number ron the stack and returns radians
// precondition: at least one number on the stack
export class Arctangent : public UnaryCommand
{
public:
    Arctangent() = default;
    explicit Arctangent(const Arctangent& rhs)
    : UnaryCommand{rhs}
    { }

    ~Arctangent() = default;

private:
    Arctangent(Arctangent&&) = delete;
    Arctangent& operator=(const Arctangent&) = delete;
    Arctangent& operator=(Arctangent&&) = delete;

    double unaryOperation(double top) const noexcept override
    {
        return std::atan(top);
    }

    CLONE(Arctangent)
    HELP("Replace the first element, x, on the stack with arctan(x). Returns result in radians")
};

// takes the top of the stack and negates it
// precondition: at least one number on the stack
export class Negate : public UnaryCommand
{
public:
    Negate() = default;

    explicit Negate(const Negate& rhs)
    : UnaryCommand{rhs}
    { }

    ~Negate() = default;

 private:
    Negate(Negate&&) = delete;
    Negate& operator=(const Negate&) = delete;
    Negate& operator=(Negate&&) = delete;

    double unaryOperation(double top) const noexcept override
    {
        return -top;
    }

    CLONE(Negate)
    HELP("Negates the top number on the stack")
};

// takes the top of the stack and duplicates it
// precondition: at least one number on the stack
export class Duplicate : public Command
{
public:
    Duplicate() = default;

    explicit Duplicate(const Duplicate& rhs)
    : Command{rhs}
    { }

    ~Duplicate() = default;

private:
    Duplicate(Duplicate&&) = delete;
    Duplicate& operator=(const Duplicate&) = delete;
    Duplicate& operator=(Duplicate&&) = delete;

    void checkPreconditionsImpl() const override
    {
        if( Stack::Instance().size() < 1 )
            throw Exception{"Stack must have 1 element"};
    }

    void executeImpl() noexcept override
    {
        auto v = Stack::Instance().getElements(1);
        Stack::Instance().push( v.back() );
    }

    void undoImpl() noexcept override
    {
        Stack::Instance().pop();
    }

    CLONE(Duplicate)
    HELP("Duplicates the top number on the stack")
};

}
