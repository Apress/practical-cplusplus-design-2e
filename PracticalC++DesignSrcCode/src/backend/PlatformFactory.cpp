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
#include <memory>
module pdCalc_pluginManagement:PlatformFactory;

#ifdef POSIX
    import :PosixFactory;
#elif WIN32
    import :WindowsFactory;
#endif

import :DynamicLoader;

namespace pdCalc {

PlatformFactory::PlatformFactory()
{
}

PlatformFactory& PlatformFactory::Instance()
{
#ifdef POSIX
    static PosixFactory instance;
#elif WIN32
    static WindowsFactory instance;
#endif

    return instance;
}

PlatformFactory::~PlatformFactory()
{
}

}
