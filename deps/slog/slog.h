// Enjoy your day
// Author: George Potoshin <george.potoshin@gmail.com>
// slog.h: simple c logging

#ifndef __SLOG_H__
#define __SLOG_H__

#include <stdio.h>

#ifndef SLOG_OUT
#define SLOG_OUT stdout
#endif

#define SLOG_BLACK		"\033[30m"
#define SLOG_RED		"\033[31m"
#define SLOG_GREEN		"\033[32m"
#define SLOG_YELLOW		"\033[33m"
#define SLOG_BLUE		"\033[34m"
#define SLOG_MAGENTA	"\033[35m"
#define SLOG_CYAN		"\033[36m"
#define SLOG_WHITE		"\033[37m"

#define SLOG_BR_BLACK	"\033[30;1m"
#define SLOG_BR_RED		"\033[31;1m"
#define SLOG_BR_GREEN	"\033[32;1m"
#define SLOG_BR_YELLOW	"\033[33;1m"
#define SLOG_BR_BLUE	"\033[34;1m"
#define SLOG_BR_MAGENTA	"\033[35;1m"
#define SLOG_BR_CYAN	"\033[36;1m"
#define SLOG_BR_WHITE	"\033[37;1m"

#define SLOG_RESET		"\033[0m"

#ifndef MY_SLOG_MESSAGE_COLOR
#define SLOG_MESSAGE_COLOR SLOG_YELLOW
#else
#define SLOG_MESSAGE_COLOR MY_SLOG_MESSAGE_COLOR
#endif

#ifndef MY_SLOG_ERROR_COLOR
#define SLOG_ERROR_COLOR SLOG_RED
#else
#define SLOG_ERROR_COLOR MY_SLOG_ERROR_COLOR
#endif

#ifndef MY_SLOG_WARNING_COLOR
#define SLOG_WARNING_COLOR SLOG_MAGENTA
#else
#define SLOG_WARNING_COLOR MY_SLOG_WARNING_COLOR
#endif

#ifndef MY_SLOG_INFO_COLOR
#define SLOG_INFO_COLOR SLOG_BLUE
#else
#define SLOG_INFO_COLOR MY_SLOG_INFO_COLOR
#endif

#ifndef NERROR
#define SLOG_ERROR(source, error)						fputs \
	(SLOG_ERROR_COLOR "error" SLOG_RESET "[" source "]: " error "!\n", SLOG_OUT)

#define SLOG_FAILED_FILE(source, file_name)				fputs \
	(SLOG_ERROR_COLOR "error" SLOG_RESET "[" source "]: can't open " SLOG_MESSAGE_COLOR file_name SLOG_RESET "!\n", SLOG_OUT)
#else
#define SLOG_ERROR(source, error)
#define SLOG_FAILED_FILE(source, filename)
#endif

#if !defined(NLOG) && !defined(NERROR)
#define SLOG_INFO(source, info)							fputs \
	(SLOG_INFO_COLOR "info" SLOG_RESET "[" source "]: " info ".\n", SLOG_OUT)

#define SLOG_FN_CALL(source, fn_name)					fputs \
	(SLOG_INFO_COLOR "function call" SLOG_RESET "[" source "]: " fn_name ".\n", SLOG_OUT)

#define SLOG_SYNTAX_ERROR(source, line, message, word)	fputs \
	(SLOG_ERROR_COLOR "syntax error" SLOG_RESET "[" source ":" #line "]: " message  " " SLOG_MESSAGE_COLOR word SLOG_RESET "!\n", SLOG_OUT)

#define SLOG_WARNING(source, warning)					fputs \
	(SLOG_WARNING_COLOR "warning" SLOG_RESET "[" source "]: " warning "!\n", SLOG_OUT)
#else
#define SLOG_INFO(source, info)
#define SLOG_FN_CALL(source, fn_name)
#define SLOG_SYNTAX_ERROR(source, line, message, word)
#define SLOG_WARNING(source, warning)
#endif

#endif /* __SLOG_H__ */
