#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

const char *const MONTHS[] = {
	"January",
	"February",
	"March",
	"April",
	"May",
	"June",
	"July",
	"August",
	"September",
	"October",
	"November",
	"December",
};

const char *const WEEKDAYS[] = {
	"Sunday",
	"Monday",
	"Tuesday",
	"Wednesday",
	"Thursday",
	"Friday",
	"Saturday"
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
		// Don't need to force length because the month rarely changes.
		int err = printf("%s%s, %s %02d, %4d\n", label, WEEKDAYS[timeinfo->tm_wday], MONTHS[timeinfo->tm_mon], timeinfo->tm_mday, 1900+timeinfo->tm_year);
		if (err < 0) {
			fprintf(stderr, "Printf errored. Is the locale correct?");
			return 1;
		}
		
		fflush(stdout);
		
		// Only sleep to the next hour for safety with daylight savings, timezone changes, etc
		int sleepsecs = 3600 - (timeinfo->tm_min*60) - (timeinfo->tm_sec); 
		sleep(sleepsecs);
	}
	// Should never get here
	return 1;
}