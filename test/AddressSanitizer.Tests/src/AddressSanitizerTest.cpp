#include <gtest/gtest.h>
#include "AddressSanitizerTest.h"

namespace AddressSanitizer::Tests
{
	TEST(AddressSanitizerTest, MemoryLeak_WhenFailed)
	{

		int* ptr = new int[100];
		//delete[] ptr;

		EXPECT_EQ(1.0f, 1.0f);
	}
}