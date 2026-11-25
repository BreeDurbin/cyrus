#include "TestHelpers.h"

#include "AttackRate.h"

TEST_CASE("AttackRate formats whole and half attack rates", "[model][AttackRate]") {
    CHECK(AttackRate{2}.toString() == QString("1"));
    CHECK(AttackRate{4}.toString() == QString("2"));
    CHECK(AttackRate{3}.toString() == QString("3/2"));
    CHECK(AttackRate{5}.toString() == QString("5/2"));
}

TEST_CASE("AttackRate defaults to one attack per round", "[model][AttackRate]") {
    AttackRate rate;
    CHECK(rate.halfAttacks_ == 2);
    CHECK(rate.attack() == 1);
}

TEST_CASE("AttackRate::attack gives a constant count for whole rates", "[model][AttackRate]") {
    AttackRate rate{4};
    CHECK(rate.attack() == 2);
    CHECK(rate.attack() == 2);
    CHECK(rate.attack() == 2);
}

TEST_CASE("AttackRate::attack alternates for half rates", "[model][AttackRate]") {
    SECTION("3/2 attacks: 1, then 2, then 1") {
        AttackRate rate{3};
        CHECK(rate.attack() == 1);
        CHECK(rate.attack() == 2);
        CHECK(rate.attack() == 1);
    }
    SECTION("5/2 attacks: 2, then 3, then 2") {
        AttackRate rate{5};
        CHECK(rate.attack() == 2);
        CHECK(rate.attack() == 3);
        CHECK(rate.attack() == 2);
    }
}

TEST_CASE("AttackRate increments and decrements in half attacks within [2, 20]", "[model][AttackRate]") {
    SECTION("prefix and postfix increment") {
        AttackRate rate{2};
        ++rate;
        CHECK(rate.halfAttacks_ == 3);
        AttackRate before = rate++;
        CHECK(before.halfAttacks_ == 3);
        CHECK(rate.halfAttacks_ == 4);
    }
    SECTION("prefix and postfix decrement") {
        AttackRate rate{5};
        --rate;
        CHECK(rate.halfAttacks_ == 4);
        AttackRate before = rate--;
        CHECK(before.halfAttacks_ == 4);
        CHECK(rate.halfAttacks_ == 3);
    }
    SECTION("never drops below one attack") {
        AttackRate rate{2};
        --rate;
        rate--;
        CHECK(rate.halfAttacks_ == 2);
    }
    SECTION("never exceeds ten attacks") {
        AttackRate rate{20};
        ++rate;
        rate++;
        CHECK(rate.halfAttacks_ == 20);
    }
}

TEST_CASE("AttackRate::add clamps to [2, 20]", "[model][AttackRate]") {
    AttackRate rate{4};
    rate.add(3);
    CHECK(rate.halfAttacks_ == 7);

    rate += 100;
    CHECK(rate.halfAttacks_ == 20);

    rate.add(-100);
    CHECK(rate.halfAttacks_ == 2);
}

TEST_CASE("AttackRate::undoAttack reverts the half attack toggle", "[model][AttackRate]") {
    SECTION("half rates") {
        AttackRate rate{5};
        CHECK(rate.attack() == 2);
        rate.undoAttack();
        CHECK(rate.attack() == 2);
        CHECK(rate.attack() == 3);
        rate.undoAttack();
        CHECK(rate.attack() == 3);
    }
    SECTION("whole rates are unaffected") {
        AttackRate rate{4};
        rate.attack();
        rate.undoAttack();
        CHECK(rate.attack() == 2);
    }
}
