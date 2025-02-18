#ifndef CONFIG_USER_CONFIG_OPTIMIZED_H_
#define CONFIG_USER_CONFIG_OPTIMIZED_H_

#include "aether/config_consts.h"


#define AE_CRYPTO_ASYNC AE_HYDRO_CRYPTO_PK
#define AE_CRYPTO_SYNC AE_HYDRO_CRYPTO_SK
#define AE_SIGNATURE AE_HYDRO_SIGNATURE
#define AE_KDF AE_HYDRO_KDF

// telemetry
#define AE_TELE_ENABLED 0
#define AE_TELE_LOG_CONSOLE 0

// all except kLog
#define AE_TELE_METRICS_MODULES 0
#define AE_TELE_METRICS_DURATION 0

#define AE_TELE_LOG_TIME_POINT 0
// location only for kLog module
#define AE_TELE_LOG_LOCATION 0
// tag name for all except kLog
#define AE_TELE_LOG_NAME 0
#define AE_TELE_LOG_LEVEL_MODULE 0
#define AE_TELE_LOG_BLOB 0

#define AE_SUPPORT_REGISTRATION 1
#define AE_SUPPORT_CLOUD_DNS 1


#endif /* CONFIG_USER_CONFIG_OPTIMIZED_H_ */
