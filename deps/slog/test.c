#include <stdio.h>
#include "slog.h"

int main () {
	SLOG_ERROR("testing", "it's an error message");
	SLOG_FAILED_FILE("testing", "that's what it logs for FILEs");
	SLOG_INFO("testing", "that's how info log looks like");
	SLOG_FN_CALL("testing", "some times it's useful to log function calls");
	SLOG_SYNTAX_ERROR("testing", 9, "in parsing you might want to log syntax errors", "word with error");
	SLOG_WARNING("testing", "c is unsafe language, but it's punk");
	return 0;
}
