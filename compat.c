/*===========================================================================

  compat.c

  Various CP/M / Linux compatibility constants and function
  declarations. See compat.h for information.

  Copyright (c)2022-3 Kevin Boone, GPL v3.0

===========================================================================*/
#include <stdio.h>
#include <string.h>
#include "compat.h"
#ifndef z80
#include <fcntl.h> 
#include <unistd.h> 
#include <termios.h>
static struct termios orig_termios;
#endif

/*===========================================================================

  puteol

===========================================================================*/
void puteol (void)
  {
  fputs ("\r\n", stdout); 
  fflush (stdout);
  }

/*===========================================================================

  raw_mode_on 

===========================================================================*/
#ifndef z80
void raw_mode_on (int fd_in)
  {
  struct termios raw;
  tcgetattr (fd_in, &orig_termios);
  raw = orig_termios;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  tcsetattr (fd_in, TCSAFLUSH, &raw);
  }
#endif

/*===========================================================================

  raw_mode_off

===========================================================================*/
#ifndef z80
void raw_mode_off (int fd_in)
  {
  tcsetattr (fd_in, TCSAFLUSH, &orig_termios);
  }
#endif




