#include <ArduinoUnitTests.h>
#include "aether.h"

unittest(Libbcrypt)
{
  int unitTestResult = 0;
  
  unitTestResult = ArduinoLibbcryptUnitTestsRunner();
  assertEqual(unitTestResult, 0);
  unitTestResult = ArduinoLibhydrogenUnitTestsRunner();
  assertEqual(unitTestResult, 0);
  unitTestResult = ArduinoLibsodiumUnitTestsRunner();
  assertEqual(unitTestResult, 0);
}

unittest_main()