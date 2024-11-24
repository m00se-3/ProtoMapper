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
#include <catch2/catch_test_macros.hpp>
#include <dyn_array.hpp>
#include <array>
#include <algorithm>

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)

TEST_CASE("dyn_array with trivial data", "[dyn_array]")
{
    constexpr int size_of_first = 6uz;
    auto container = proto::dyn_array<int>{size_t(size_of_first)};

    REQUIRE(container.size() == 0uz);
    REQUIRE(container.capacity() == 6uz);

    for(int i = 0; i < size_of_first; ++i) 
    {
        container.push_back(i);
    }

    REQUIRE(container.size() == 6uz);
    REQUIRE(container.front() == 0);
    REQUIRE(container.back() == 5);
    REQUIRE_THROWS(container.emplace_back(65));
}

TEST_CASE("test copying and for loops", "[dyn_array]")
{
    auto con1 = proto::dyn_array<int>{12uz, 0}; 
    auto con2 = con1;

    REQUIRE(con1.size() == 12uz);
    REQUIRE(con2.size() == 12uz);

    for(auto& item : con1)
    {
        ++item;
    }

    REQUIRE_FALSE(con1.front() == con2.front());
    REQUIRE(con1[3uz] == 1);
    REQUIRE_THROWS(con2.at(17) == 0);

    auto con3 = std::move(con2);
    REQUIRE(con2.data() == nullptr);
}

TEST_CASE("test dyn_array with STL algorithms", "[dyn_array]")
{
    const auto sample = std::array<int, 12uz> { 3, 4, 7, 1, 8, 9, 2, 3, 1, 5, 7, 8 };

    auto generator = [i = 0uz, &sample] () mutable {
                                                                    auto temp = i;
                                                                    ++i;    
                                                                    return sample.at(temp); 
                                                                };

    auto rng = proto::dyn_array<int>{sample.size(), 0};

    std::ranges::generate(rng, generator);
    
    std::ranges::stable_partition(rng, [](int i) -> bool { return i % 2 == 0; });

    REQUIRE(rng.at(3uz) % 2 == 0uz);

    auto it = std::ranges::remove_if(rng, [](int num) -> bool { return num < 3; });

    rng.erase(it.begin(), it.end());

    REQUIRE(rng.size() == 9uz);
}

TEST_CASE("dyn_array::iterators with arithmitic", "[dyn_array], [iterators]")
{
    auto container = proto::dyn_array<int>{15uz, 0};
    auto it = container.begin();

    auto some_element = 5uz + it; 

    *some_element = 45;
    it[3uz] = 42;

    some_element -= 4uz;
    *some_element = 35;

    REQUIRE(*(it + 5uz) == 45);
    REQUIRE(*(it + 3uz) == 42);
    REQUIRE(*(it + 1uz) == 35);
}

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)