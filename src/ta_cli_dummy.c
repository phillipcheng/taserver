
/* dummy
 */

#include <stdio.h>


int ta_cli_init(void)
{
    fprintf(stderr, "Never call me, I am dummy");
	return 0;
}

void ta_cli_fini(void)
{
    fprintf(stderr, "Never call me, I am dummy");
	return;
};
