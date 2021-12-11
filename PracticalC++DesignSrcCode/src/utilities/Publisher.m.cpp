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

#include <ranges>
#include <algorithm>
#include <any>
#include <string>
#include <format>
#include <set>
#include <memory>
#include <vector>
#include <unordered_map>

export module pdCalc_utilities:Publisher;

import :Exception;
import :Observer;

using std::string;
using std::vector;
using std::set;
using std::unique_ptr;
using std::shared_ptr;
using std::any;
using std::unordered_map;

namespace views = std::views;
namespace ranges = std::ranges;

namespace pdCalc {

export class Publisher
{
    using ObserversList = unordered_map<string, unique_ptr<Observer>>;
    using Events = unordered_map<string, ObserversList>;

public:
    Publisher() = default;

    void attach(const string& eventName, unique_ptr<Observer> observer);
    unique_ptr<Observer> detach(const string& eventName, const string& observerName);

    set<string> listEvents() const;
    set<string> listEventObservers(const string& eventName) const;

protected:
    // first overload is a workaround for Visual Studio, which ICEs if data is given
    // a default parameter value of null (in a module)
    void raise(const string& eventName) const;
    void raise(const string& eventName, any data) const;

    void registerEvent(const string& eventName);
    void registerEvents(const vector<string>& eventNames);

private:
    Events::const_iterator findCheckedEvent(const string& eventName) const;
    Events::iterator findCheckedEvent(const string& eventName);

    Events events_;
};

Publisher::Events::const_iterator Publisher::findCheckedEvent(const string& eventName) const
{
    auto ev = events_.find(eventName);
    if( ev == events_.end() )
    {
        throw Exception{ std::format("Publisher does not support event '{}'", eventName) };
    }

    return ev;
}

Publisher::Events::iterator Publisher::findCheckedEvent(const string &eventName)
{
    auto ev = events_.find(eventName);
    if( ev == events_.end() )
    {
        throw Exception{ std::format("Publisher does not support event '{}'", eventName) };
    }

    return ev;
}

void Publisher::attach(const string& eventName, unique_ptr<Observer> observer)
{
    auto& obsList = findCheckedEvent(eventName)->second;
    
    if( obsList.contains( observer->name() ) )
        throw Exception("Observer already attached to publisher");

    obsList.insert( std::pair(observer->name(), std::move(observer)) );

    return;
}

unique_ptr<Observer> Publisher::detach(const string& eventName, const string& observer)
{
    auto& obsList = findCheckedEvent(eventName)->second;

    auto obs = obsList.find(observer);
    if( obs == obsList.end() )
        throw Exception("Cannot detach observer because observer not found");

    auto tmp = std::move(obs->second);
    obsList.erase(obs);

    return tmp;
}


void Publisher::raise(const string& eventName) const
{
    raise(eventName, any{});
}

void Publisher::raise(const string& eventName, const std::any d) const
{
    const auto& obsList = findCheckedEvent(eventName)->second;
    ranges::for_each( views::values(obsList), [d](const auto& v){ v->notify(d);} );

    return;
}

void Publisher::registerEvent(const string& eventName)
{
    if( events_.contains(eventName) )
        throw Exception{"Event already registered"};

    events_[eventName] = ObserversList{};

    return;
}

void Publisher::registerEvents(const vector<string>& eventNames)
{
    ranges::for_each(eventNames, [this](const auto& i){registerEvent(i);});

    return;
}

set<string> Publisher::listEvents() const
{
    set<string> tmp;
    ranges::for_each(events_, [&tmp](const auto& i){tmp.insert(i.first);});

    return tmp;
}

set<string> Publisher::listEventObservers(const string& eventName) const
{
    auto ev = findCheckedEvent(eventName);

    set<string> tmp;

    ranges::for_each( ev->second | views::keys, [&tmp](const auto& k){tmp.insert(k);} );

    return tmp;
}

}


