#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>

#include <resource.hpp>
#include <cstdio>
#include <fstream>
#include <string>

TEST_CASE("shared_res with a File*", "[shared_res]")
{
    const std::string testDir = TEST_DIR;
    
    auto destructor = [](std::FILE* ptr) { REQUIRE(std::fclose(gsl::owner<std::FILE*>{ptr}) == 0); };
    auto file = proto::make_shared_res<std::FILE*>(destructor);

    REQUIRE(fopen_s(file.get_ptr(), (testDir + "/resource_test_file.txt").c_str(), "r") == 0);

    auto file2 = file;

    REQUIRE(file2.get_owners() == 2z);
}

TEST_CASE("shared_res with a file stream", "[shared_res]")
{
    const std::string testDir = TEST_DIR;
    
    auto file = proto::make_shared_res<std::fstream>(proto::shared_res_default_destructor, (testDir + "/resource_test_file.txt").c_str(), std::ios_base::in);

    REQUIRE(file.get().is_open());

    {
        
        auto file2 = file;
        auto file3 = proto::make_shared_res<std::fstream>(file);

        REQUIRE(file.get().rdbuf() == file2.get().rdbuf());
        REQUIRE(file3.get_owners() == 3z);

    }

    REQUIRE(file.get_owners() == 1z);
}

TEST_CASE("use shared_res with weak_res", "[shared_res][weak_res]")
{
    const std::string testDir = TEST_DIR;
    
    auto file = proto::make_shared_res<std::fstream>(proto::shared_res_default_destructor, (testDir + "/resource_test_file.txt").c_str(), std::ios_base::in);
    auto refer = proto::weak_res<std::fstream>(file);

    file.reset();

    REQUIRE(refer.expired());
    REQUIRE(refer.get_refs() == 1z);
}