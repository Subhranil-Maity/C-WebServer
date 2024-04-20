#include "web_log.h"
#include <time.h>

void web_log(logger *log, int log_lvl, const char *msg, va_list args) {
  // Print log level
  switch (log_lvl) {
  case DEBUG:
    if (log->debug) {
      printf("%s%s[ DEBUG ] %s%s", MAGENTA, BOLD, RESET, MAGENTA);
    }
    break;

  case INFO:
    if (log->info) {
      printf("%s%s[  INFO ] %s%s", GREEN, BOLD, RESET, GREEN);
    }
    break;

  case WARN:
    if (log->warn) {
      printf("%s%s[WARNING] %s%s", YELLOW, BOLD, RESET, YELLOW);
    }
    break;

  case ERROR:
    if (log->error) {
      printf("%s%s[ ERROR ] %s%s", RED, BOLD, RESET, RED);
    }
    break;

  default:
    printf("[UNKNOWN] ");
    break;
  }
  // Print time
  if (log->time) {
    time_t now = time(NULL);
    struct tm *timeinfo = localtime(&now);
    printf("[%04d-%02d-%02d %02d:%02d:%02d] ", timeinfo->tm_year + 1900,
           timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour,
           timeinfo->tm_min, timeinfo->tm_sec);
  }

  // Print message
  vfprintf(stdout, msg, args);
  printf("\n%s", RESET);
}

void debug(logger *log, const char *msg, ...) {
  va_list args;
  va_start(args, msg);
  web_log(log, DEBUG, msg, args);
  va_end(args);
}

void info(logger *log, const char *msg, ...) {
  va_list args;
  va_start(args, msg);
  web_log(log, INFO, msg, args);
  va_end(args);
}

void warn(logger *log, const char *msg, ...) {
  va_list args;
  va_start(args, msg);
  web_log(log, WARN, msg, args);
  va_end(args);
}

void error(logger *log, const char *msg, ...) {
  va_list args;
  va_start(args, msg);
  web_log(log, ERROR, msg, args);
  va_end(args);
}

