/*
	ProtoMapper - Map creation and pathfinding software for game development.
	Copyright (C) 2023  Samuel Bridgham - moosethree473@gmail.com

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "Logger.hpp"

namespace proto
{
    Logger Logger::_internal{};

    void Logger::Init()
    {
        
    }

    void Logger::Reset()
    {
        
    }

    void Logger::Acquire() { _internal._sync.acquire(); }

    void Logger::Release() { _internal._sync.release(); }

    void Logger::LogMessage(const std::string &src, const std::string &msg)
    {
        //Acquire();
        
        //Release();
    }

    void Logger::Update()
    {
        _internal.ClockSemaphore();
    }

    void Logger::ClockSemaphore()
    {
        _sync.release();
        _sync.acquire();
    }

} // namespace proto
