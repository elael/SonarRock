#include <boost/test/unit_test.hpp>
#include <osgSonartest/Dummy.hpp>

using namespace osgSonartest;

BOOST_AUTO_TEST_CASE(it_should_not_crash_when_welcome_is_called)
{
    osgSonartest::DummyClass dummy;
    dummy.welcome();
}
