#include <stdio.h>
#include "code_gen.c"
// #include "gen_8086.c"

FILE *fp, *fp2;
main ()
{
	fp = fopen("3addr.txt", "w+");
	statements ();
	fclose(fp);
}
