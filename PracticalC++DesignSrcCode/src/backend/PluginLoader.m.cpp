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
#include <fstream>
#include <iterator>
#include <algorithm>
#include <memory>
#include <ranges>
#include <vector>
#include <string>
#include <format>
export module pdCalc_pluginManagement;

import pdCalc_userInterface;
import pdCalc_utilities;
import pdCalc_plugin;
import :DynamicLoader;
import :PlatformFactory;

using std::vector;
using std::string;
using std::ifstream;
using std::unique_ptr;
using std::format;
namespace ranges = std::ranges;

namespace pdCalc {

export class PluginDeleter
{
public:
    explicit PluginDeleter(DynamicLoader& d) : loader_{d} {}
    void operator()(pdCalc::Plugin* p) { loader_.deallocatePlugin(p); }

private:
    DynamicLoader& loader_;
};

export class PluginLoader
{
public:
    PluginLoader() = default;
    ~PluginLoader() = default;

    void loadPlugins(UserInterface& ui, const string& pluginFileName);
    const vector<const Plugin*> getPlugins() const;

private:
    PluginLoader(const PluginLoader&) = delete;
    PluginLoader(PluginLoader&&) = delete;
    PluginLoader& operator=(const PluginLoader&) = delete;
    PluginLoader& operator=(PluginLoader&&) = delete;

    void load(UserInterface& ui, const string&);

    vector<unique_ptr<DynamicLoader>> loaders_;
    vector<unique_ptr<Plugin, PluginDeleter>> plugins_;
};

void PluginLoader::loadPlugins(UserInterface& ui, const string& pluginFileName)
{

    if( ifstream ifs{ pluginFileName.c_str() } )
    {
        ranges::for_each( vector<string>{ std::istream_iterator<string>(ifs), std::istream_iterator<string>() },
            [&ui, this](auto n){load(ui, n);} );
    }
    else
    {
        ui.postMessage("Could not open plugin file");
    }

    return;
}

const vector<const Plugin*> PluginLoader::getPlugins() const
{
    vector<const Plugin*> v;
    for(auto& i : plugins_)
        v.push_back( i.get() );

    return v;
}

void PluginLoader::load(UserInterface& ui, const string& name)
{
    loaders_.emplace_back( PlatformFactory::Instance().createDynamicLoader() );

    // may be null
    if( auto p = loaders_.back()->allocatePlugin(name) )
        plugins_.emplace_back( p, PluginDeleter( *loaders_.back() ) );
    else
        ui.postMessage( format("Error opening plugin: {}", name) );

    return;
}

}

