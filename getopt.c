/*===========================================================================

  getopt.c

  A simple implementation of getopt(), specifically for CP/M, and intended
  to be compiled using Aztec C. It works with HI_TECH C as well, but Aztech
  is really fussy about parameter syntax. This code is somewhat based on the
  version in the MUSL C library. I've modified it to accept / as the 
  switch character, to be more CP/M-like (and CP/M does not support
  directories).

  Copyright (c)2022-3  Kevin Boone, GPL v3.0.

===========================================================================*/

#include <stdio.h>
#include "defs.h"
#include "compat.h"
#include "getopt.h"

char *optarg;
int optind=1, opterr=1, optopt, __optpos, optreset=0;

#define optpos __optpos

/* define GO_ERRMSG  to enable error report for command-line switches.
   This adds a few hundred bytes to the executable. If not enabled,
   you can just catch the '?' or ':' return and print a usage message. */
#define GO_ERRMSG

#ifdef GO_ERRMSG
static void __getopt_msg (a, b, c, l)
char *a;
char *b;
char *c;
int l;
  {
  fputs (a, stdout);
  fputs (b, stdout);
  fputs (c, stdout);
  puteol();
  }
#endif

static BOOL is_switch (c)
char c;
  {
  if (c == '-' || c == '/') return TRUE;
  return FALSE;
  }

int getopt (argc, argv, optstring)
int argc;
char **argv;
char *optstring;
  {
  int i, c, d;
  int k, l;
#ifdef GO_ERRMSG
  char *optchar;
#endif

  if (!optind || optreset) 
    {
    optreset = 0;
    __optpos = 0;
    optind = 1;
    }

  if (optind >= argc || !argv[optind])
    return -1;

  if (!is_switch (argv[optind][0]))
  /*if (argv[optind][0] != '-' && argv[optind][0] != '/') */
    {
    if (is_switch (optstring[0])) 
      {
      optarg = argv[optind++];
      return 1;
      }
    return -1;
    }

  if (!argv[optind][1])
    return -1;

  if (is_switch (argv[optind][1]) && !argv[optind][2])
    return optind++, -1;

  if (!optpos) optpos++;
    c = argv[optind][optpos], k = 1;
#ifdef GO_ERRMSG
  optchar = argv[optind] + optpos;
#endif
  optopt = c;
  optpos += k;

  if (!argv[optind][optpos]) 
    {
    optind++;
    optpos = 0;
    }

  if (optstring[0] == '-' || optstring[0] == '/')
    optstring++;

  i = 0;
  d = 0;
  do 
    {
    d = optstring[i], l = 1;
    if (l > 0) i+=l; else i++;
    } while (l && d != c && d != 0);

  if (d != c) 
    {
    if (optstring[0] != ':' && opterr)
#ifdef GO_ERRMSG
      __getopt_msg(argv[0], ": unrecognized option: ", optchar, k);
#endif
     return '?';
    }

  if (optstring[i] == ':') 
    {
    if (optstring[i+1] == ':') optarg = 0;
    else if (optind >= argc) 
      {
      if (optstring[0] == ':') return ':';
#ifdef GO_ERRMSG
      if (opterr) __getopt_msg (argv[0],
                                ": option requires an argument: ",
        optchar, k);
#endif
      return '?';
      }

    if (optstring[i+1] != ':' || optpos) 
      {
      optarg = argv[optind++] + optpos;
      optpos = 0;
      }
    }
  return c;
  }

