#include "NcaHeader.h"
#include "gtest/gtest.h"

TEST(VersionTests, IsVersionSupported_version3_returnsTrue)
{
    EXPECT_TRUE(nc::asset::IsVersionSupported(nc::asset::version3));
}

TEST(VersionTests, IsVersionSupported_currentVersion_returnsTrue)
{
    EXPECT_TRUE(nc::asset::IsVersionSupported(nc::asset::currentVersion));
}

TEST(VersionTests, IsVersionSupported_badVersion_returnsFalse)
{
    EXPECT_FALSE(nc::asset::IsVersionSupported(1ull));
}
