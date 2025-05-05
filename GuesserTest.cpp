/**
 * Unit Tests for the class
**/

#include <gtest/gtest.h>
#include "Guesser.h"

class GuesserTest : public ::testing::Test
{
	protected:
		GuesserTest(){} //constructor runs before each test
		virtual ~GuesserTest(){} //destructor cleans up after tests
		virtual void SetUp(){} //sets up before each test (after constructor)
		virtual void TearDown(){} //clean up after each test, (before destructor)
};

TEST(GuesserTest, SpecialCharactersMatch) {
    Guesser g("@#$% secret!");
    EXPECT_TRUE(g.match("@#$% secret!"));
    EXPECT_FALSE(g.match("@#$% SECRET!")); // mismatch in case
}

// Long secret is properly truncated to 32 characters
TEST(GuesserTest, TruncateLongSecret) {
	std::string long_secret = "Please close the fridge next time, dude.";
	std::string truncated = "Please close the fridge next time, du"; // 32 chars
	Guesser g(long_secret);
	EXPECT_TRUE(g.match(truncated));
	EXPECT_FALSE(g.match(long_secret)); // locks
	EXPECT_FALSE(g.match(truncated));
}

// correct palindrome guess, slight typo, brute force guess
TEST(GuesserTest, PalindromeMatchAndLock) {
    Guesser g("racecar");
    EXPECT_TRUE(g.match("racecar"));
    EXPECT_FALSE(g.match("racecat")); // 1 char off
    EXPECT_EQ(g.remaining(), 2);
    EXPECT_FALSE(g.match("tacocat")); // too far off so locks
    EXPECT_FALSE(g.match("racecar")); // lcoked
}

// matching math equation secret with wrong guess
TEST(GuesserTest, MathEquationMatchAndLock) {
    Guesser g("2 + 2 = 4");
    EXPECT_TRUE(g.match("2 + 2 = 4"));
    EXPECT_FALSE(g.match("2 + 2 = 5")); // dist 1
    EXPECT_EQ(g.remaining(), 2);
    EXPECT_FALSE(g.match("3 * 3 = 9")); // dist > 2 -> lock
    EXPECT_FALSE(g.match("2 + 2 = 4")); // 
}

// 3 near misses reduce guess count and lead to lock
TEST(GuesserTest, ThreeNearMissesLockOut) {
    Guesser g("i definitely didn’t eat your leftovers");
    EXPECT_FALSE(g.match("i definitely didn’t eat your leftlvers")); // l for o
    EXPECT_EQ(g.remaining(), 2);
    EXPECT_FALSE(g.match("i definitely didn’t eat your leftowers")); // o for f
    EXPECT_EQ(g.remaining(), 1);
    EXPECT_FALSE(g.match("i definitely didn’t eat your lestovers")); // s for f
    EXPECT_EQ(g.remaining(), 0);
    EXPECT_FALSE(g.match("i definitely didn’t eat your leftovers")); // locks
}


// emoji guess
TEST(GuesserTest, EmojiTriggersLock){
    Guesser g("hello world");
    EXPECT_FALSE(g.match("hello 🤓")); // dist > 2 due to emoji
    EXPECT_EQ(g.remaining(), 3); // no decrement when locked
    EXPECT_FALSE(g.match("hello world")); // locked still
}

// riddle, locks on wrong answer
TEST(GuesserTest, WrongRiddleLocksOut){
    Guesser g("What has 4 legs?");
    EXPECT_FALSE(g.match("a table"));
    EXPECT_EQ(g.remaining(), 3); // locked, still reads 3
    EXPECT_FALSE(g.match("What has 4 legs?")); // Locked
}

// empty secret, space off by 1
TEST(GuesserTest, EmptySecretEdgeCase) {
    Guesser g("");
    EXPECT_TRUE(g.match(""));
    EXPECT_FALSE(g.match(" "));
    EXPECT_EQ(g.remaining(), 2);
}

// scrambled guess far from secret → lock
TEST(GuesserTest, PatternVsScrambleLock) {
    Guesser g("abcabcabc");
    EXPECT_FALSE(g.match("cababccba")); // way off
    EXPECT_EQ(g.remaining(), 3);
    EXPECT_FALSE(g.match("abcabcabc")); // locked
}

// binary strings flipped → lock
TEST(GuesserTest, BinaryFlipTriggersLock) {
    Guesser g("101010");
    EXPECT_FALSE(g.match("010101")); //flipped bits
    EXPECT_EQ(g.remaining(), 3);
    EXPECT_FALSE(g.match("101010"));
}
