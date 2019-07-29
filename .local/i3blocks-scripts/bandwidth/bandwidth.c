// From https://github.com/vivien/i3blocks-contrib/tree/master/bandwidth2

#include <getopt.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define RED "#FF7373"
#define ORANGE "#FFA500"

typedef unsigned long long ulli;

enum {
	STATE_OK,
	STATE_WARNING,
	STATE_CRITICAL,
	STATE_UNKNOWN,
};

void usage(char *argv[])
{
	printf("Usage: %s [-b|B] [-t seconds] [-i interface] [-w Bytes:Bytes] [-c Bytes:Bytes] [-h]\n", argv[0]);
	printf("\n");
	printf("-b \t\tuse bits/s\n");
	printf("-B \t\tuse Bytes/s  (default)\n");
	printf("-t seconds\trefresh time (default is 1)\n");
	printf("-i interface\tnetwork interface to monitor. If not specified, check all interfaces.\n");
	printf("-w Bytes:Bytes\tSet warning (color orange) for Rx:Tx bandwidth. (default: none)\n");
	printf("-c Bytes:Bytes\tSet critical (color red) for Rx:Tx bandwidth. (default: none)\n");
	printf("-h \t\tthis help\n");
	printf("\n");
}

void get_values(char *const iface, time_t * const s, ulli * const received, ulli * const sent)
{
	FILE *f;

	f = fopen("/proc/net/dev", "r");
		if (!f) {
		fprintf(stderr, "Can't open /proc/net/dev\n");
		exit(STATE_UNKNOWN);
	}

	ulli temp_r, temp_s;
	char line[BUFSIZ];
	char ifname[BUFSIZ];

	*received = 0;
	*sent = 0;
	while (fgets(line, BUFSIZ - 1, f) != NULL) {
		if (sscanf(line, "%s %llu %*u %*u %*u %*u %*u %*u %*u %llu", ifname, &temp_r, &temp_s) == 3) {
			if (iface && strcmp(iface, ifname) != 0) continue;

			if (strcmp(ifname, "lo:") == 0) continue;

			*received = *received + temp_r;
			*sent = *sent + temp_s;
		}
	}

	fclose(f);

	*s = time(NULL);
	if (!s) {
		fprintf(stderr, "Can't get Epoch time\n");
		exit(STATE_UNKNOWN);
	}
}

void display(int const unit, double b, int const warning, char *const warning_color, int const critical, char *const critical_color, char *const label)
{
	if (critical != 0 && b > critical) printf("<span color='%s'>", critical_color);
	else if (warning != 0 && b > warning) printf("<span color='%s'>", warning_color);
	else printf("<span>");
	
	printf("%s", label);

	if (unit == 'b') b = b * 8;

	// if (b < 1024) printf("%5.1lf  %c/s", b, unit);
	// else if (b < 1024 * 1024) printf("<span font_weight='bold'>%05.1lf</span>K%c/s", b / 1024, unit);
	if (b < 1024 * 1024 * 1024) printf("%5.1lfM%c/s", b / (1024 * 1024), unit);
    else printf("%5.1lfG%c/s", b / (1024 * 1024 * 1024), unit);
	printf("</span>");
}

int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "");
	int c, unit = 'B', t = 1;
	char iface[BUFSIZ] = {0};
	int warningrx = 0, warningtx = 0, criticalrx = 0, criticaltx = 0;
	char *warning_color = ORANGE, *critical_color = RED;
	char *label = "", *labelrx = "", *labeltx = "";
	char *first = "rx";
	char *envvar = NULL;

	envvar = getenv("USE_BITS");
	if (envvar && *envvar == '1') unit = 'b';
	envvar = getenv("USE_BYTES");
	if (envvar && *envvar == '1') unit = 'B';
	envvar = getenv("REFRESH_TIME");
	if (envvar) t = atoi(envvar);
	envvar = getenv("BLOCK_INSTANCE");
	if (envvar) snprintf(iface, BUFSIZ, "%s:", envvar);
	envvar = getenv("WARN_RX");
	if (envvar) warningrx = atoi(envvar);
	envvar = getenv("WARN_TX");
	if (envvar) warningtx = atoi(envvar);
	envvar = getenv("WARN_COLOR");
	if (envvar) warning_color = envvar;
	envvar = getenv("CRIT_RX");
	if (envvar) criticalrx = atoi(envvar);
	envvar = getenv("CRIT_TX");
	if (envvar) criticaltx = atoi(envvar);
	envvar = getenv("CRIT_COLOR");
	if (envvar) critical_color = envvar;
	envvar = getenv("LABEL");
	if (envvar) label = envvar;
	envvar = getenv("LABEL_RX");
	if (envvar) labelrx = envvar;
	envvar = getenv("LABEL_TX");
	if (envvar) labeltx = envvar;
	envvar = getenv("FIRST");
	if (envvar) first = envvar;

	while (c = getopt(argc, argv, "bBht:i:w:c:"), c != -1) {
    	switch (c) {
    	case 'b':
    	case 'B':
    		unit = c;
    		break;
    	case 't':
    		t = atoi(optarg);
    		break;
    	case 'i':
    		snprintf(iface, BUFSIZ, "%s:", optarg);
    		break;
    	case 'w':
    		sscanf(optarg, "%d:%d", &warningrx, &warningtx);
    		break;
    	case 'c':
    		sscanf(optarg, "%d:%d", &criticalrx, &criticaltx);
    		break;
    	case 'h':
    		usage(argv);
    		return STATE_UNKNOWN;
    	}
	}

	time_t s, s_old;
	ulli received, sent, received_old = 0, sent_old = 0;

	get_values(iface, &s_old, &received_old, &sent_old);

	while (1) {
		sleep(t);
		get_values(iface[0] ? iface : NULL, &s, &received, &sent);

		double rx = (received - received_old) / (float)(s - s_old);
		double tx = (sent - sent_old) / (float)(s - s_old);
		printf("%s", label);
		
		// TODO make this prettier
		if (*first == 't') {
			display(unit, tx, warningtx, warning_color, criticaltx, critical_color, labeltx);
			printf(" ");
			display(unit, rx, warningrx, warning_color, criticalrx, critical_color, labelrx);
		} else {
			display(unit, rx, warningrx, warning_color, criticalrx, critical_color, labelrx);
			printf(" ");
			display(unit, tx, warningtx, warning_color, criticaltx, critical_color, labeltx);
		}
		printf("\n");
		fflush(stdout);
		s_old = s;
		received_old = received;
		sent_old = sent;
	}

	return STATE_OK;
}