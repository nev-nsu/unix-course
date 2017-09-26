#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <string.h>

const char* timezone_name = "America/Los_Angeles";

void on_error()
{
    printf("%d %s", errno, strerror(errno));
    exit(1);
}

int main()
{
    char backup[80];
    char* current_tz = getenv("TZ");
    if (current_tz)
        strncpy(backup, current_tz, 79);

    if (setenv("TZ", timezone_name, 1) == -1)
        on_error();
    tzset();

    time_t seconds = time(NULL);
    struct tm* time_object = localtime(&seconds);
    if (!time_object)
        on_error();

    char buffer[80];
    if (!strftime (buffer, 80, "%Z %H:%M", time_object))
        on_error();
    printf("%s\n", buffer);

    if (setenv("TZ", backup, 1) == -1)
        on_error();
}

