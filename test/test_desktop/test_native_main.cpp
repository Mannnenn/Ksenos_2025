#include <unity.h>
#include "add.h"

void test_addition()
{
    TEST_ASSERT_EQUAL(5, add(2, 3));
    TEST_ASSERT_EQUAL(-1, add(2, -3));
}

void setUp() {}

void tearDown() {}

int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_addition);
    return UNITY_END();
}