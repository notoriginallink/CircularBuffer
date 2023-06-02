#include "lib\CCircularBuffer\CCircularBuffer.h"
#include "lib\CCircularBuffer\CCircularBufferExt.h"

#include <gtest/gtest.h>

TEST(BufferTestSuite, CreationTest1) {
    CCircularBuffer<uint32_t> buf(6);
    ASSERT_EQ(buf.capacity(), 6);
    ASSERT_EQ(buf.size(), 0);
}

TEST(BufferTestSuite, CreationTest2) {
    CCircularBuffer<char> buf({'1', '2', '3', '4', '5'});
    ASSERT_EQ(buf.capacity(), 5);
    ASSERT_EQ(buf.size(), 5);
}


TEST(BufferTestSuite, PushTest) {
    CCircularBuffer<char> buf1(5);
    CCircularBuffer<char> buf2({'1', '2', '3', '4', '5'});
    buf1.push_back('3');
    buf1.push_front('2');
    buf1.push_front('1');
    buf1.push_back('4');
    buf1.push_back('5');
    ASSERT_EQ(buf1, buf2);
}

TEST(BufferTestSuite, PopTest) {
    CCircularBuffer<uint32_t> buf1({3, 4});
    CCircularBuffer<uint32_t> buf2({1, 2, 3, 4, 5});
    buf2.pop_front();
    buf2.pop_back();
    buf2.pop_front();
    EXPECT_EQ(buf2.size(), 2);
    ASSERT_EQ(buf1, buf2);
}

TEST(BufferTestSuite, PushOverTest) {
    CCircularBuffer<uint32_t> buf1({1, 2, 3, 4, 5});
    CCircularBuffer<uint32_t> buf2({5, 6, 7, 8 ,9});
    for (uint32_t i = 6; i  < 10; ++i)
        buf1.push_back(i);
    ASSERT_EQ(buf1, buf2);
}

TEST(BufferTestSuite, ClearTest) {
    CCircularBuffer<uint32_t> buf({1, 2, 3, 4, 5});
    buf.clear();
    ASSERT_EQ(buf.capacity(), 5);
    ASSERT_EQ(buf.size(), 0);
}

TEST (BufferTestSuite, ResizeTest) {
    CCircularBuffer<int32_t> buf({1, 2, 3, 4, 5});
    EXPECT_EQ(buf.size(), 5);
    EXPECT_EQ(buf.capacity(), 5);
    buf.resize(9);
    ASSERT_EQ(buf.size(), 5);
    ASSERT_EQ(buf.capacity(), 9);
}

TEST(BufferTestSuite, InsertTest1) {
    CCircularBuffer<char> buf({'A', 'B', 'C', 'D'});
    CCircularBuffer<char> buf1({'A', 'W', 'C', 'D'});
    buf.insert(buf.begin() + 2, 'W');
    ASSERT_EQ(buf, buf1);
    buf1 = {'Z', 'Z', 'Z', 'D'};
    buf.insert(buf.begin() + 3, {'Z', 'Z', 'Z'});
    ASSERT_EQ(buf, buf1);
}

TEST (BufferTestSuite, InsertTest2) {
    CCircularBuffer<char> buf({'A', 'B', 'C', 'D'});
    CCircularBuffer<char> buf1({'A', 'W', 'C', 'D'});
    buf.pop_back();
    buf.pop_back();
    buf.insert(buf.begin() + 1, buf1.begin() + 1, buf1.end());
    ASSERT_EQ(buf, CCircularBuffer<char>({'W', 'C', 'D', 'B'}));
    buf.insert(buf.end(), 3, 'Z');
    ASSERT_EQ(buf, CCircularBuffer<char>({'W', 'Z', 'Z', 'Z'}));
}

TEST (BufferTestSuite, EraseTest) {
    CCircularBuffer<char> buf({'A', 'B', 'C', 'D'});
    CCircularBuffer<char> buf1({'A', 'D'});
    buf.erase(buf.begin() + 1, buf.begin() + 3);
    ASSERT_EQ(buf, buf1);
}

TEST (BufferTestSuite, IndexTest) {
    CCircularBuffer<char> buf({'A', 'B', 'C', 'D'});
    ASSERT_EQ(buf[2], 'C');
    ASSERT_EQ(buf[4], 'A');
    ASSERT_EQ(buf[9], 'B');
    ASSERT_EQ(buf.at(7), 'D');
}

TEST (BufferTestSuite, AssignTest) {
    CCircularBuffer<int32_t> buf1({3, 4, 5, 6, 7});
    CCircularBuffer<int32_t> buf2({4, 4, 4, 4});
    buf1.assign({4, 4, 4, 4});
    EXPECT_EQ(buf1.size(), 4);
    EXPECT_EQ(buf1.capacity(), 5);
    ASSERT_EQ(buf1, buf2);
    buf1.assign({10, 11, 12, 15, 16, 17});
    ASSERT_EQ(buf1.capacity(), 6);
    ASSERT_EQ(buf1.size(), 6);
}

TEST (BufferTestSuite, FrontBackTest) {
    CCircularBuffer<int32_t> buf1({3, 4, 5, 6, 7});
    ASSERT_EQ(buf1.back(), 7);
    ASSERT_EQ(buf1.front(), 3);
}

TEST (BufferTestSuite, SwapTest) {
    CCircularBuffer<int32_t> buf1({3, 4, 5, 6, 7});
    CCircularBuffer<int32_t> buf2;
    swap(buf1, buf2);
    ASSERT_EQ(buf2, CCircularBuffer<int32_t>({3, 4, 5, 6, 7}));
}

// ExtendedBufferTests
TEST(BufferExtTestSuite, CreationTest1) {
    CCircularBufferExt<uint32_t> buf(6);
    ASSERT_EQ(buf.capacity(), 6);
    ASSERT_EQ(buf.size(), 0);
}

TEST(BufferExtTestSuite, CreationTest2) {
    CCircularBufferExt<char> buf({'1', '2', '3', '4', '5'});
    ASSERT_EQ(buf.capacity(), 5);
    ASSERT_EQ(buf.size(), 5);
}

TEST(BufferExtTestSuite, PushTest) {
    CCircularBufferExt<char> buf1(5);
    CCircularBufferExt<char> buf2({'1', '2', '3', '4', '5'});
    buf1.push_back('3');
    buf1.push_front('2');
    buf1.push_front('1');
    buf1.push_back('4');
    buf1.push_back('5');
    ASSERT_EQ(buf1, buf2);
    buf1.push_back('6');
    buf1.push_back('7');
    ASSERT_EQ(buf1.capacity(), 10);
    ASSERT_EQ(buf1.size(), 7);
    ASSERT_EQ(buf1, CCircularBufferExt<char>({'1', '2', '3', '4', '5', '6', '7'}));
}

TEST(BufferExtTestSuite, PopTest) {
    CCircularBufferExt<uint32_t> buf1({3, 4});
    CCircularBufferExt<uint32_t> buf2({1, 2, 3, 4, 5});
    buf2.pop_front();
    buf2.pop_back();
    buf2.pop_front();
    EXPECT_EQ(buf2.size(), 2);
    ASSERT_EQ(buf1, buf2);
    buf2.pop_front();
    buf2.pop_back();
    ASSERT_EQ(buf2.size(), 0);
}

TEST (BufferExtTestSuite, PushOverTest) {
    CCircularBufferExt<uint32_t> buf1({1, 2, 3, 4, 5});
    CCircularBufferExt<uint32_t> buf2({1, 2, 3, 4, 5, 6, 7, 8, 9});
    for (uint32_t i = 6; i  < 10; ++i)
        buf1.push_back(i);
    ASSERT_EQ(buf1.size(), 9);
    ASSERT_EQ(buf1.capacity(), 10);
    ASSERT_EQ(buf1, buf2);
}

TEST(BufferExtTestSuite, ClearTest) {
    CCircularBufferExt<uint32_t> buf({1, 2, 3, 4, 5});
    buf.clear();
    ASSERT_EQ(buf.capacity(), 5);
    ASSERT_EQ(buf.size(), 0);
}

TEST (BufferExtTestSuite, ResizeTest) {
    CCircularBufferExt<int32_t> buf({1, 2, 3, 4, 5});
    EXPECT_EQ(buf.size(), 5);
    EXPECT_EQ(buf.capacity(), 5);
    buf.resize(9);
    ASSERT_EQ(buf.size(), 5);
    ASSERT_EQ(buf.capacity(), 9);
}

TEST(BufferExtTestSuite, InsertTest1) {
    CCircularBufferExt<char> buf({'A', 'B', 'C', 'D'});
    CCircularBufferExt<char> buf1({'A', 'B', 'W', 'C', 'D'});
    buf.insert(buf.begin() + 2, 'W');
    ASSERT_EQ(buf.capacity(), 8);
    ASSERT_EQ(buf.size(), 5);
    ASSERT_EQ(buf, buf1);
    buf1 = {'A', 'B', 'W', 'Z', 'Z', 'Z', 'C', 'D'};
    buf.insert(buf.begin() + 3, {'Z', 'Z', 'Z'});
    ASSERT_EQ(buf.size(), 8);
    ASSERT_EQ(buf.capacity(), 8);
    ASSERT_EQ(buf, buf1);
}

TEST (BufferExtTestSuite, InsertTest2) {
    CCircularBufferExt<char> buf({'A', 'B', 'C', 'D'});
    CCircularBufferExt<char> buf1({'A', 'W', 'C', 'D'});
    buf.pop_back();
    buf.pop_back();
    buf.insert(buf.begin() + 1, buf1.begin() + 1, buf1.end());
    ASSERT_EQ(buf, CCircularBufferExt<char>({'A', 'W', 'C', 'D', 'B'}));
    buf.insert(buf.end(), 3, 'Z');
    ASSERT_EQ(buf, CCircularBufferExt<char>({'A', 'W', 'C', 'D', 'B', 'Z', 'Z', 'Z'}));
}

TEST (BufferExtTestSuite, FrontBackTest) {
    CCircularBufferExt<char> buf({'A', 'B', 'C', 'D'});
    ASSERT_EQ(buf.front(), 'A');
    ASSERT_EQ(buf.back(), 'D');
}

TEST (BufferExtTestSuite, EraseTest) {
    CCircularBufferExt<char> buf({'A', 'B', 'C', 'D'});
    CCircularBufferExt<char> buf1({'A', 'D'});
    buf.erase(buf.begin() + 1, buf.begin() + 3);
    ASSERT_EQ(buf, buf1);
}

TEST (BufferExtTestSuite, IndexTest) {
    CCircularBufferExt<char> buf({'A', 'B', 'C', 'D'});
    ASSERT_EQ(buf[2], 'C');
    ASSERT_EQ(buf[4], 'A');
    ASSERT_EQ(buf[9], 'B');
}

TEST (BufferExtTestSuite, AssignTest) {
    CCircularBufferExt<int32_t> buf1({3, 4, 5, 6, 7});
    CCircularBufferExt<int32_t> buf2({4, 4, 4, 4});
    buf1.assign({4, 4, 4, 4});
    EXPECT_EQ(buf1.size(), 4);
    EXPECT_EQ(buf1.capacity(), 5);
    ASSERT_EQ(buf1, buf2);
    buf1.assign({10, 11, 12, 15, 16, 17});
    ASSERT_EQ(buf1.capacity(), 6);
    ASSERT_EQ(buf1.size(), 6);
}

int main (int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}