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

#include <vector>
#include <string>
#include <deque>

export module pdCalc_stack;

using std::string;
using std::vector;
using std::deque;

import pdCalc_utilities;

namespace pdCalc {

export class StackErrorData
{
public:
    enum class ErrorConditions { Empty, TooFewArguments };
    explicit StackErrorData(ErrorConditions e) : err_(e) { }

    static const char* Message(ErrorConditions ec);
    const char* message() const;
    ErrorConditions error() const { return err_; }

private:
    ErrorConditions err_;
};

export class Stack : private Publisher
{
public:
    static Stack& Instance();
    void push(double, bool suppressChangeEvent = false);
    double pop(bool suppressChangeEvent = false);
    void swapTop();

    // returns first min(n, stackSize) elements of the stack with the top of stack at position 0
    vector<double> getElements(size_t n) const;
    void getElements(size_t n, std::vector<double>&) const;

    using Publisher::attach;
    using Publisher::detach;

    // these are just needed for testing
    size_t size() const { return stack_.size(); }
    void clear();

    static string StackChanged();
    static string StackError();

private:
    Stack();
    ~Stack() = default;
    Stack(const Stack&) = delete;
    Stack(Stack&&) = delete;
    Stack& operator=(const Stack&) = delete;
    Stack& operator=(const Stack&&) = delete;

    deque<double> stack_;
};

string Stack::StackChanged()
{
    return "stackChanged";
}

string Stack::StackError()
{
    return "error";
}

const char* StackErrorData::Message(StackErrorData::ErrorConditions ec)
{
    switch(ec)
    {
    case ErrorConditions::Empty: return "Attempting to pop empty stack";
    case ErrorConditions::TooFewArguments: return "Need at least two stack elements to swap top";
    default: return "Unknown error";
    };
}

const char* StackErrorData::message() const
{
    return Message(err_);
}


void Stack::push(double d, bool suppressChangeEvent)
{
    stack_.push_back(d);
    if(!suppressChangeEvent) raise(Stack::StackChanged(), nullptr);

    return;
}

double Stack::pop(bool suppressChangeEvent)
{
    if( stack_.empty() )
    {
        raise(Stack::StackError(), StackErrorData{StackErrorData::ErrorConditions::Empty});

        throw Exception{StackErrorData::Message(StackErrorData::ErrorConditions::Empty)};
    }
    else
    {
        auto val = stack_.back();
        stack_.pop_back();
        if(!suppressChangeEvent) raise(Stack::StackChanged(), nullptr);
        return val;
    }
}

void Stack::swapTop()
{
    if( stack_.size() < 2 )
    {
        raise(Stack::StackError(), StackErrorData{StackErrorData::ErrorConditions::TooFewArguments});

        throw Exception{StackErrorData::Message(StackErrorData::ErrorConditions::TooFewArguments)};
    }
    else
    {
        auto first = stack_.back();
        stack_.pop_back();
        auto second = stack_.back();
        stack_.pop_back();
        stack_.push_back(first);
        stack_.push_back(second);

        raise(Stack::StackChanged(), nullptr);
    }

    return;
}

vector<double> Stack::getElements(size_t n) const
{
    vector<double> v;
    getElements(n, v);
    return v;
}

void Stack::getElements(size_t n, vector<double>& v) const
{
    // if n is > stack's size, just return size of stack
    if(n > stack_.size()) n = stack_.size();

    v.insert(v.end(), stack_.rbegin(), stack_.rbegin() + n);

    return;
}

void Stack::clear()
{
    stack_.clear();

    raise(Stack::StackChanged(), nullptr);

    return;
}

Stack& Stack::Instance()
{
    static Stack instance;
    return instance;
}

Stack::Stack()
{
    registerEvent( StackChanged() );
    registerEvent( StackError() );
}

}
