
#include <stdio.h>
#include <ctype.h>  /* for isupper, tolower */

#include "api.h"
#include "stem.h"

static repetitions = 1;
static stem_count;

void stemfile(struct SN_env * z, FILE * f_in, FILE * f_out)
{
#define INC 10
    int lim = INC;
    char * b = (char *) malloc(lim);

    while(1)
    {   int ch = getc(f_in);
        if (ch == EOF) { free(b); return; }
        {   int i = 0;
            while(1)
            {
                if (ch == '\n' || ch == EOF) break;
                if (i == lim)
                {   char * q = (char *) malloc(lim + INC);
                    memmove(q, b, lim);
                    free(b); b = q;
                    lim = lim + INC;
                }
                /* force lower case: */
                if isupper(ch) ch = tolower(ch);

                b[i] = ch; i++;
                ch = getc(f_in);
            }

            {   int j; for (j = 1; j <= repetitions; j++)
                {
                    SN_set_current(z, i, b);
                    stem(z); stem_count++;
                }
            }
            {
                z->p[z->l] = 0;
                fprintf(f_out, "%s%c", z->p, '\n');
            }
        }
    }
}

static int intof(char * s)
{   int n = 0;
    int i; for (i = 0; i < strlen(s); i++)
    {   int d = s[i] - '0';
        if (d < 0 || d > 9) { fprintf(stderr, "%s not a number\n", s); exit(1); }
        n = 10*n + d;
    }
    return n;
}

static int eq(char * s1, char * s2)
{   int s1_len = strlen(s1);
    int s2_len = strlen(s2);
    return s1_len == s2_len && memcmp(s1, s2, s1_len) == 0;
}

int main(int argc, char * argv[])
{   char * in;
    char * out = 0;
    if (argc == 1)
    {    printf("options are: file [-o[utput] file] [-r[epetitions] number]\n");
         exit(1);
    }
    if (argc % 2 == 1)
    {    printf("number of options must be odd\n");
         exit(1);
    }
    {   char * s;
        int i = 1;
        while(1)
        {   if (i >= argc) break;
            s = argv[i++];
            if (s[0] == '-')
            {   if (eq(s, "-output") || eq(s, "-o")) out = argv[i++]; else
                if (eq(s, "-repetitions") || eq(s, "-r")) repetitions = intof(argv[i++]); else
                {   fprintf(stderr, "%s unknown\n", s); exit(1);
                }
            }
            else in = s;
        }
    }

    /* initialise the stemming process: */

    {   struct SN_env * z = create_env();
        FILE * f_in;
        FILE * f_out;
        f_in = fopen(in, "r");
        if (f_in == 0) { fprintf(stderr, "file %s not found\n", in); exit(1); }
        f_out = out == 0 ? stdout : fopen(out, "w");
        if (f_out == 0) { fprintf(stderr, "file %s cannot be opened\n", out); exit(1); }
        stemfile(z, f_in, f_out);
        close_env(z);
    }

    printf("%d calls to stem\n", stem_count);

    return 0;
}
