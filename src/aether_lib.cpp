#include "third_party/libbcrypt_unit_tests/libbcrypt_unit_tests.h"
#include "third_party/libhydrogen_unit_tests/libhydrogen_unit_tests.h"
#include "third_party/libsodium_unit_tests/libsodium_unit_tests.h"

extern void AetherCloudExample();
extern void AetherKeyLedExample();
extern void AetherRegisteredExample();

void ArduinoAetherCloudExample()
{
  AetherCloudExample();
}

int ArduinoLibbcryptUnitTestsRunner()
{
    return libbcrypt_unit_tests_runner();
}

int ArduinoLibhydrogenUnitTestsRunner()
{
    return libhydrogen_unit_tests_runner();
}

int ArduinoLibsodiumUnitTestsRunner()
{
    return libsodium_unit_tests_runner();
}