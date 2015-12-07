#include "gtest/gtest.h"
#include "hime/piece.h"

using hime::MasterPiece;

namespace {

class MyIntTest : public ::testing::Test{};

TEST_F(MyIntTest, isOdd) {
    MasterPiece mp(1, "姫", 60, 50, 80, 2);
    EXPECT_EQ(1, mp.id_);
}

}  // namespace

