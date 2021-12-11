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

// This is the base class to abstract OS specific dynamic library loading.
// Loading should be the same in any POSIX compliant OS (e.g., Linux, Mac OS X,
// BSD). Windows loads dynamic libraries differently.

// Note, this class specifies that Plugins must have a function named AllocPlugin with
// signature
// pdCalc::Plugin* AllocPlugin()
// that allocates a pdCalc::Plugin with the global new function. The pdCalc::Plugin base
// class is defined in a separate header.
module;
#include <string>
export module pdCalc_pluginManagement:DynamicLoader;

using std::string;

namespace pdCalc {

export class DynamicLoader
{
public:
    virtual ~DynamicLoader() = default;

    // derived class must provide an OS specific way of loading a shared library,
    // dynamically binding to the AllocPlugin function in the plugin, allocating the
    // plugin, and closing the plugin
    virtual class Plugin* allocatePlugin(const string& pluginName) = 0;
    virtual void deallocatePlugin(class Plugin*) = 0;

    // Get the plugin allocation name for derived classes
    static string GetPluginAllocationName() { return "AllocPlugin"; }
    static string GetPluginDeallocationName() { return "DeallocPlugin"; }
};

}

// function pointer for the interface to allocate a plugin
export extern "C" { typedef void* (*PluginAllocator)(void); }

// function pointer for the interface to deallocate a plugin
export extern "C" { typedef void (*PluginDeallocator)(void*); }
