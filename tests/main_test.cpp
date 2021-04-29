#include <gtest/gtest.h>


int add(int a, int b) {
    return a + b;
}

TEST(MainTest, add) {
    EXPECT_EQ(add(1,2), 2);
}



