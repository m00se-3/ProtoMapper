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
module;

#include <string>
#include <string_view>
#include <semaphore>
#include <memory>

export module proto.Logger;

namespace proto
{
    export class Logger
    {
    public:
        Logger(const Logger&) = delete;
        Logger(Logger&&) noexcept = delete;
        Logger& operator=(const Logger&) = delete;
        Logger& operator=(Logger&&) = delete;

        static void Init();
        static void Reset();

        static void Acquire();
        static void Release();

        // TODO: Use the multithreaded stuff when the time comes.
        static void LogMessage(const std::string& src, const std::string& msg);

        static void Update();

    private:
        Logger() = default;

        static Logger _internal;

        void ClockSemaphore();

        std::counting_semaphore<1> _sync{0};

    };
}