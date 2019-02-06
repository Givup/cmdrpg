#ifndef _LOG_H_
#define _LOG_H_

#define OPEN_LOGF(file, format, ...) { FILE* log_file = fopen(file, "w"); \
    fprintf(log_file, "Log opened by: %s:%d\n", __FILE__, __LINE__);	\
    fprintf(log_file, format, __VA_ARGS__);				\
    fclose(log_file); }							\

#define OPEN_LOG(format, ...) OPEN_LOGF("log.txt", format, __VA_ARGS__)

#define PRINT_LOGF(file, format, ...) { FILE* log_file = fopen(file, "a"); \
    fprintf(log_file, format, __VA_ARGS__);				 \
    fclose(log_file); }							 \

#define PRINT_LOG(format, ...) PRINT_LOGF("log.txt", format, __VA_ARGS__)

#endif
