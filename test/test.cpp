#include <gtest/gtest.h>
class test : public ::testing::Test{

};

TEST_F(test, testFoo){
  ASSERT_TRUE(false);
}