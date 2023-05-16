/*=========================================================================

  ws2txt

  main.c

  Copyright (c)2022-3 Kevin Boone, GPLv3.0

=========================================================================*/
#include <stdio.h>
#ifdef z80
#include <unixio.h>
#include <conio.h>
#endif
#ifndef z80
#include <fcntl.h> 
#include <unistd.h> 
#endif
#include <string.h>

#include "getopt.h"
#include "defs.h"
#include "compat.h"

/* CP/M pads files with ctrl-z */
#define CTRL_Z ('Z' - 64)

/* Input buffer 
 * This buffer could be an automatic variable in modern C. But it's
 * too big for the stack in CP/M. We could malloc() it in main() or
 * something, but that would mean including all the memory allocator
 * code in the binary. */
char buff[CPMBLK];

/* Number of lines output so far, in pager mode */
int lines = 0;

/* Maximum number of lines to display, before prompting the user */
int max_lines = 23;

/*=========================================================================

  show_help 

=========================================================================*/
void show_help (void)
  {
  fputs ("Usage: ws2txt [/p][/o [text_file}] {wordstar_file}", stdout);
  puteol();
  fputs ("   /h            Show this help message", stdout);
  puteol();
  fputs ("   /o text_file  Send output to a text file, not the console", 
    stdout);
  puteol();
  fputs ("   /p            Enable screen paging", stdout);
  puteol();
  fputs ("   /v            Show version information", stdout);
  puteol();
  }

/*=========================================================================

  show_version

=========================================================================*/
void show_version (void)
  {
  fputs ("ws2txt version 0.1", stdout);
  puteol();
  fputs ("Copyright (c)2023 Kevin Boone", stdout);
  puteol();
  fputs ("Distributed under the terms of the GNU Public Licence, v3.0", stdout);
  puteol();
  }

/*=========================================================================
  
  get_char_from_console

  Note that we use the ordinary getchar() in the Linux build, because
    we've already set the console to non-echo mode. For CP/M, we use
    getche(), which does not echo, and does not need additional 
    set-up

=========================================================================*/
char get_char_from_console (void) 
  {
#ifdef z80
  return getche();
#else
  return getchar();
#endif
  }

/*=========================================================================
  
  handle_screen_full 

  When the screen is full in paging mode, prompt the user, and reset
    the line count.

=========================================================================*/
void handle_screen_full (BOOL *quit)
  {
  char c;
  fputs ("--- space/enter/q --- ", stdout);
  fflush (stdout);
  c = get_char_from_console ();
  if (c == 'q' || c == 'Q')
    *quit = TRUE;
  else if (c == ' ')
    lines = 0;
  else
    lines--;
  fputc ('\r', stdout);
  fputs ("                      ", stdout);
  fputc ('\r', stdout);
  }

/*=========================================================================

  do_char

  Process the next char from the input file, and write it, with 
    transformation if necessary

=========================================================================*/
void do_char (int c, FILE *f_out, BOOL *quit, BOOL page)
  {
  c = c & 0x7F;

/* Let's ignore CR completely if we're building for Linux. */
#ifndef z80 
  if (c == 13)
    return;
#endif

  fwrite (&c, 1, 1, f_out); 
  if (c == 10)
    {
    if (page && (f_out == stdout))
      {
      lines++;
      if (lines >= max_lines) 
        {
        handle_screen_full (quit); 
        }
      }
    }
  }

/*=========================================================================

  do_file

  Process the WordStar input file, writing either to the console or
    to the specified output file. If outfile is NULL, write to
    stdout. 

=========================================================================*/
void do_file (char* infile, char *outfile, BOOL page)
  {
  BOOL out_ok = FALSE;
  FILE *f_out = stdout;
  
  if (outfile)
    {
    /* HI-TECH open() does not create files -- must creat() first. */
    f_out = fopen (outfile, "w");
    if (f_out)
      {
      out_ok = TRUE;
      }
    else
      {
      fputs ("Can't open '", stdout);
      fputs (outfile, stdout);
      fputs ("' for writing", stdout);
      puteol();
      }
    }
  else
    out_ok = TRUE;

  if (out_ok)
    {
    int f_in;
    f_in = open (infile, O_RDONLY);
    if (f_in >= 0)
      {
      BOOL eof = FALSE;
      BOOL quit = FALSE;
      while (!eof && !quit)
	{
	int i, n = read (f_in, buff, CPMBLK);  
	if (n > 0)
	  {
	  for (i = 0; i < n && !quit; i++)
	    {
	    char c = buff[i];
	    if (c == CTRL_Z)
	      eof = TRUE;
	    else
	      do_char (c, f_out, &quit, page);
	    }
	  }
	else
	  eof = TRUE;
	}
      close (f_in);
      }
    else
      {
      fputs ("Can't open '", stdout);
      fputs (infile, stdout);
      fputs ("' for reading", stdout);
      puteol ();
      }
    }

  if (f_out != stdout && f_out != NULL) fclose (f_out);
  }

/*=========================================================================

  main

=========================================================================*/
int main (int argc, char **argv)
  {
  int opt;
  BOOL opt_page = FALSE;
  BOOL opt_version = FALSE;
  char *outfile = NULL;

  /* argv[0] is not set by the HI-TECH C compiler */
  argv[0] = "ws2txt";

  while ((opt = getopt (argc, argv, "pP?hHo:O:vV")) != -1)
    {
    switch (opt)
      {
      case '?':
      case ':':
      case 'h':
      case 'H':
        show_help ();
        return 0; 
      case 'p': case 'P':
        opt_page = TRUE;
        break;
      case 'v': case 'V':
        opt_version = TRUE;
        break;
      case 'o': case 'O':
        outfile = optarg;
        break;
      default:
        return 1;
      }
    }

  if (opt_version)
    {
    show_version();
    }
  else if (argc == optind)
    {
    show_help();
    }
  else
    {
#ifndef z80
    raw_mode_on (STDIN_FILENO);
#endif
    do_file (argv[optind], outfile, opt_page);
#ifndef z80
    raw_mode_off (STDIN_FILENO);
#endif
    }

  return 0;
  }

