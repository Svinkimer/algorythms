#include <gtest/gtest.h>
#include "DynamicArray.h"

using namespace svinkimer;

// ------------------- TESTS -------------------

TEST(ArrayTest, DefaultConstructor) {
    Array<int> arr;
    EXPECT_EQ(arr.size(), 0);
}

TEST(ArrayTest, ConstructorWithCapacity) {
    Array<int> arr(10);
    EXPECT_EQ(arr.size(), 0);
    
    for (int i = 0; i < 10; ++i) arr.insert(i);
    EXPECT_EQ(arr.size(), 10);
}

TEST(ArrayTest, CopyConstructor) {
    Array<int> arr1;
    arr1.insert(1);
    arr1.insert(2);

    Array<int> arr2 = arr1;
    EXPECT_EQ(arr2.size(), 2);
    EXPECT_EQ(arr2[0], 1);
    EXPECT_EQ(arr2[1], 2);

    arr2[0] = 99;
    EXPECT_EQ(arr1[0], 1);
}

TEST(ArrayTest, CopyAssignment) {
    Array<int> arr1;
    arr1.insert(1);
    arr1.insert(2);

    Array<int> arr2;
    arr2 = arr1; 
    EXPECT_EQ(arr2.size(), 2);
    EXPECT_EQ(arr2[0], 1);
    EXPECT_EQ(arr2[1], 2);

    arr2[1] = 99;
    EXPECT_EQ(arr1[1], 2);
}

TEST(ArrayTest, MoveConstructor) {
    Array<int> arr1;
    arr1.insert(10);
    arr1.insert(20);

    Array<int> arr2 = std::move(arr1); 
    EXPECT_EQ(arr2.size(), 2);
    EXPECT_EQ(arr2[0], 10);
    EXPECT_EQ(arr2[1], 20);

    EXPECT_EQ(arr1.size(), 0);
}


TEST(ArrayTest, MoveAssignment) {
    Array<int> arr1;
    arr1.insert(100);
    arr1.insert(200);

    Array<int> arr2;
    arr2 = std::move(arr1); 
    EXPECT_EQ(arr2.size(), 2);
    EXPECT_EQ(arr2[0], 100);
    EXPECT_EQ(arr2[1], 200);

    EXPECT_EQ(arr1.size(), 0);
}


TEST(ArrayTest, ResourceBasedType) {
    Array<std::string> arr;
    arr.insert("Hello");
    arr.insert("World");

    EXPECT_EQ(arr.size(), 2);
    EXPECT_EQ(arr[0], "Hello");
    EXPECT_EQ(arr[1], "World");

    Array<std::string> copyArr = arr;
    EXPECT_EQ(copyArr.size(), 2);
    EXPECT_EQ(copyArr[0], "Hello");
    EXPECT_EQ(copyArr[1], "World");

    copyArr[0] = "Hi";
    EXPECT_EQ(arr[0], "Hello");
}



TEST(ArrayTest, InsertAtVariousPositions) {
    Array<int> arr;
    arr.insert(1);           // [1]
    arr.insert(0, 42);       // [42, 1] start
    arr.insert(1, 99);       // [42, 99, 1] - mid
    arr.insert(arr.size(), 77); // [42, 99, 1, 77] - end

    EXPECT_EQ(arr.size(), 4);
    EXPECT_EQ(arr[0], 42);
    EXPECT_EQ(arr[1], 99);
    EXPECT_EQ(arr[2], 1);
    EXPECT_EQ(arr[3], 77);
}


TEST(ArrayTest, RemoveElements) {
    Array<int> arr;
    arr.insert(10);
    arr.insert(20);
    arr.insert(30);
    arr.insert(40); // [10, 20, 30, 40]

    arr.remove(0); //  [20, 30, 40] start
    EXPECT_EQ(arr.size(), 3);
    EXPECT_EQ(arr[0], 20);

    arr.remove(1); //  [20, 40] mid
    EXPECT_EQ(arr.size(), 2);
    EXPECT_EQ(arr[1], 40);

    arr.remove(arr.size() - 1); // [20] end
    EXPECT_EQ(arr.size(), 1);
    EXPECT_EQ(arr[0], 20);
}



TEST(ArrayTest, IteratorForward) {
    Array<int> arr;
    arr.insert(1);
    arr.insert(2);
    arr.insert(3);

    int sum = 0;
    for (auto it = arr.iterator(); it.hasNext(); ++it) {
        sum += *it;
    }
    EXPECT_EQ(sum, 6);
}



TEST(ArrayTest, IteratorReverse) {
    Array<int> arr;
    arr.insert(1);
    arr.insert(2);
    arr.insert(3);

    int sum = 0;
    for (auto it = arr.reverseIterator(); it.hasNext(); ++it) {
        sum += *it;
    }
    EXPECT_EQ(sum, 6);
}

TEST(ArrayTest, ConstIteratorForward) {
    Array<int> arr;
    arr.insert(10);
    arr.insert(20);

    const Array<int>& constArr = arr;
    int product = 1;
    for (auto it = constArr.iterator(); it.hasNext(); ++it) {
        product *= *it;
    }
    EXPECT_EQ(product, 200);
}

TEST(ArrayTest, ConstIteratorReverse) {
    Array<int> arr;
    arr.insert(2);
    arr.insert(3);
    arr.insert(4);

    const Array<int>& constArr = arr;
    int sum = 0;
    for (auto it = constArr.reverseIterator(); it.hasNext(); ++it) {
        sum += *it;
    }
    EXPECT_EQ(sum, 9);
}


TEST(ArrayTest, ResizeOnInsert) {
    Array<int> arr(2); 
    arr.insert(1);
    arr.insert(2);
    arr.insert(3);
    arr.insert(4);

    EXPECT_EQ(arr.size(), 4);
    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[1], 2);
    EXPECT_EQ(arr[2], 3);
    EXPECT_EQ(arr[3], 4);
}

TEST(ArrayTest, RValueInsert) {
    Array<std::string> arr;
    std::string s = "Hello";

    arr.insert(std::move(s));
    EXPECT_EQ(arr.size(), 1);
    EXPECT_EQ(arr[0], "Hello");
    EXPECT_TRUE(s.empty());
}

TEST(ArrayTest, LargeNumberOfElements) {
    Array<int> arr;
    const int N = 1000;

    for (int i = 0; i < N; ++i) {
        arr.insert(i);
    }

    EXPECT_EQ(arr.size(), N);
    EXPECT_EQ(arr[0], 0);
    EXPECT_EQ(arr[N - 1], N - 1);
}

TEST(ArrayTest, IteratorsAfterResize) {
    Array<int> arr(2);
    arr.insert(1);
    arr.insert(2);
    arr.insert(3);

    int sum = 0;
    for (auto it = arr.iterator(); it.hasNext(); ++it) {
        sum += *it;
    }
    EXPECT_EQ(sum, 6); // 1 + 2 + 3
}


TEST(ArrayTest, MoveResourceLargeObject) {
    Array<std::string> arr;
    std::string longStr(1000, 'x');
    arr.insert(std::move(longStr));

    EXPECT_EQ(arr.size(), 1);
    EXPECT_EQ(arr[0].size(), 1000);
    EXPECT_TRUE(longStr.empty());
}


#pragma endregion

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}