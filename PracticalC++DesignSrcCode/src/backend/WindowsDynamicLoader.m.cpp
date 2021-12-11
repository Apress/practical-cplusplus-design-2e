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
#include <cstdlib>
#include <vector>
#include <windows.h>
#include <string>
export module pdCalc_pluginManagement:WindowsDynamicLoader;

import :DynamicLoader;
import pdCalc_utilities;

using std::string;
using std::vector;

namespace pdCalc {

export class WindowsDynamicLoader : public DynamicLoader
{
public:
    WindowsDynamicLoader();
    ~WindowsDynamicLoader();

    Plugin* allocatePlugin(const string& pluginName) override;
    void deallocatePlugin(Plugin* p) override;

private:
    HINSTANCE handle_;
};

WindowsDynamicLoader::WindowsDynamicLoader()
: DynamicLoader{}
, handle_{nullptr}
{

}

WindowsDynamicLoader::~WindowsDynamicLoader()
{
    if(handle_) FreeLibrary(handle_);
}

Plugin* WindowsDynamicLoader::allocatePlugin(const string& pluginName)
{
    handle_ = LoadLibrary(pluginName.c_str());

    if(!handle_) return nullptr;
    else
    {
        auto alloc = GetProcAddress(handle_, GetPluginAllocationName().c_str());
        if( PluginAllocator allocator{ reinterpret_cast<PluginAllocator>(alloc) } )
        {
            auto p = static_cast<Plugin*>((*allocator)());
            return p;
        }
        else return nullptr;
    }
}

void WindowsDynamicLoader::deallocatePlugin(Plugin* p)
{
    if(!handle_)
        throw Exception("Trying to deallocate a plugin, but shared library is not open.");
    else
    {
        auto dealloc = GetProcAddress(handle_, GetPluginDeallocationName().c_str());
        if( auto deallocator = reinterpret_cast<PluginDeallocator>(dealloc) )
        {
            (*deallocator)(p);
        }
        else throw Exception("Could not load the deallocator function in the plugin");
    }

    return;
}

}
