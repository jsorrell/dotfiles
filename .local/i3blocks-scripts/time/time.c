#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

const char *const SEPS[] = {
	":",
	"."
};

int main ()
{
	setlocale(LC_ALL, "");
	char *label = getenv("LABEL");
	if (!label) label = "";
	
	while (1) {
		time_t rawtime;
		time(&rawtime);
		struct tm *timeinfo = localtime(&rawtime);
		int err = printf("<span>%s%02d%s%02d</span>\n", label, timeinfo->tm_hour, SEPS[timeinfo->tm_sec % 2], timeinfo->tm_min);
		if (err < 0) {
			fprintf(stderr, "Printf errored. Is the locale correct?");
			return 1;
		}
		fflush(stdout);
		sleep(1);
	}
	// Should never get here
	return 1;
}
