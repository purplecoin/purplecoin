#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-PURP");
    BOOST_CHECK(GetBoolArg("-PURP"));
    BOOST_CHECK(GetBoolArg("-PURP", false));
    BOOST_CHECK(GetBoolArg("-PURP", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-PURPo"));
    BOOST_CHECK(!GetBoolArg("-PURPo", false));
    BOOST_CHECK(GetBoolArg("-PURPo", true));

    ResetArgs("-PURP=0");
    BOOST_CHECK(!GetBoolArg("-PURP"));
    BOOST_CHECK(!GetBoolArg("-PURP", false));
    BOOST_CHECK(!GetBoolArg("-PURP", true));

    ResetArgs("-PURP=1");
    BOOST_CHECK(GetBoolArg("-PURP"));
    BOOST_CHECK(GetBoolArg("-PURP", false));
    BOOST_CHECK(GetBoolArg("-PURP", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noPURP");
    BOOST_CHECK(!GetBoolArg("-PURP"));
    BOOST_CHECK(!GetBoolArg("-PURP", false));
    BOOST_CHECK(!GetBoolArg("-PURP", true));

    ResetArgs("-noPURP=1");
    BOOST_CHECK(!GetBoolArg("-PURP"));
    BOOST_CHECK(!GetBoolArg("-PURP", false));
    BOOST_CHECK(!GetBoolArg("-PURP", true));

    ResetArgs("-PURP -noPURP");  // -PURP should win
    BOOST_CHECK(GetBoolArg("-PURP"));
    BOOST_CHECK(GetBoolArg("-PURP", false));
    BOOST_CHECK(GetBoolArg("-PURP", true));

    ResetArgs("-PURP=1 -noPURP=1");  // -PURP should win
    BOOST_CHECK(GetBoolArg("-PURP"));
    BOOST_CHECK(GetBoolArg("-PURP", false));
    BOOST_CHECK(GetBoolArg("-PURP", true));

    ResetArgs("-PURP=0 -noPURP=0");  // -PURP should win
    BOOST_CHECK(!GetBoolArg("-PURP"));
    BOOST_CHECK(!GetBoolArg("-PURP", false));
    BOOST_CHECK(!GetBoolArg("-PURP", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--PURP=1");
    BOOST_CHECK(GetBoolArg("-PURP"));
    BOOST_CHECK(GetBoolArg("-PURP", false));
    BOOST_CHECK(GetBoolArg("-PURP", true));

    ResetArgs("--noPURP=1");
    BOOST_CHECK(!GetBoolArg("-PURP"));
    BOOST_CHECK(!GetBoolArg("-PURP", false));
    BOOST_CHECK(!GetBoolArg("-PURP", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-PURP", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-PURP", "eleven"), "eleven");

    ResetArgs("-PURP -bar");
    BOOST_CHECK_EQUAL(GetArg("-PURP", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-PURP", "eleven"), "");

    ResetArgs("-PURP=");
    BOOST_CHECK_EQUAL(GetArg("-PURP", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-PURP", "eleven"), "");

    ResetArgs("-PURP=11");
    BOOST_CHECK_EQUAL(GetArg("-PURP", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-PURP", "eleven"), "11");

    ResetArgs("-PURP=eleven");
    BOOST_CHECK_EQUAL(GetArg("-PURP", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-PURP", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-PURP", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-PURP", 0), 0);

    ResetArgs("-PURP -bar");
    BOOST_CHECK_EQUAL(GetArg("-PURP", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-PURP=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-PURP", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-PURP=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-PURP", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--PURP");
    BOOST_CHECK_EQUAL(GetBoolArg("-PURP"), true);

    ResetArgs("--PURP=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-PURP", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noPURP");
    BOOST_CHECK(!GetBoolArg("-PURP"));
    BOOST_CHECK(!GetBoolArg("-PURP", true));
    BOOST_CHECK(!GetBoolArg("-PURP", false));

    ResetArgs("-noPURP=1");
    BOOST_CHECK(!GetBoolArg("-PURP"));
    BOOST_CHECK(!GetBoolArg("-PURP", true));
    BOOST_CHECK(!GetBoolArg("-PURP", false));

    ResetArgs("-noPURP=0");
    BOOST_CHECK(GetBoolArg("-PURP"));
    BOOST_CHECK(GetBoolArg("-PURP", true));
    BOOST_CHECK(GetBoolArg("-PURP", false));

    ResetArgs("-PURP --noPURP");
    BOOST_CHECK(GetBoolArg("-PURP"));

    ResetArgs("-noPURP -PURP"); // PURP always wins:
    BOOST_CHECK(GetBoolArg("-PURP"));
}

BOOST_AUTO_TEST_SUITE_END()
