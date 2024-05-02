#include <stdio.h>
#include <stdbool.h>

#define DEBUG 1
#define INFO 2
#define WARN 3
#define ERROR 4




#include "stdarg.h"

#define YELLOW "\033[33m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define BOLD "\033[1m"
#define UNDERLINE "\033[4m"
#define BLINK "\033[5m"

#define RESET "\033[0m"

// #define bool uint8_t
// #define true 1
// #define false 0

typedef struct logger {
	bool debug;
	bool info;
	bool warn;
	bool error;

	bool time;
	bool date;
} logger;


void web_log(logger *log, int log_lvl, const char *msg, va_list args);

void debug(logger *log, const char *msg, ...);

void info(logger *log, const char *msg, ...);

void warn(logger *log, const char *msg, ...);

void error(logger *log, const char *msg, ...);
