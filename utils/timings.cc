/* Definitions of simple functions for printing timings.
   Copyright (C) 2001-2010 Roberto Bagnara <bagnara@cs.unipr.it>
   Copyright (C) 2010-2011 BUGSENG srl (http://bugseng.com)

This file is part of the Parma Polyhedra Library (PPL).

The PPL is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

The PPL is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1307, USA.

For the most up-to-date information see the Parma Polyhedra Library
site: http://www.cs.unipr.it/ppl/ . */

#include <ppl-config.h>

#include "timings.hh"
#include <cassert>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cerrno>
#include <cstdlib>

#ifdef PPL_HAVE_SYS_TIME_H
# include <sys/time.h>
#endif

#ifdef PPL_HAVE_SYS_RESOURCE_H
// This should be included after <time.h> and <sys/time.h> so as to make
// sure we have the definitions for, e.g., `ru_utime'.
# include <sys/resource.h>
#endif

using namespace std;

#ifdef PPL_HAVE_TIMEVAL
// To save the time when start_clock is called.
static struct timeval saved_ru_utime;
#endif

void
start_clock() {
#if defined(PPL_HAVE_DECL_GETRUSAGE) && defined(PPL_HAVE_TIMEVAL)
  struct rusage rsg;
  if (getrusage(RUSAGE_SELF, &rsg) != 0) {
    cerr << "getrusage failed: " << strerror(errno) << endl;
    exit(1);
  }
  else
    saved_ru_utime = rsg.ru_utime;
#endif
}

void
print_clock(ostream& s) {
#if defined(PPL_HAVE_DECL_GETRUSAGE) && defined(PPL_HAVE_TIMEVAL)
  struct rusage rsg;
  if (getrusage(RUSAGE_SELF, &rsg) != 0) {
    cerr << "getrusage failed: " << strerror(errno) << endl;
    exit(1);
  }
  else {
    time_t current_secs = rsg.ru_utime.tv_sec;
    time_t current_usecs = rsg.ru_utime.tv_usec;
    time_t saved_secs = saved_ru_utime.tv_sec;
    time_t saved_usecs = saved_ru_utime.tv_usec;
    time_t secs;
    time_t hsecs;
    secs = current_secs - saved_secs;
    if (current_usecs < saved_usecs) {
      hsecs = (((1000000 + current_usecs) - saved_usecs) + 5000) / 10000;
      if (hsecs < 100)
	--secs;
      else
	hsecs = 0;
    }
    else {
      hsecs = ((current_usecs - saved_usecs) + 5000) / 10000;
      if (hsecs == 100) {
	++secs;
	hsecs = 0;
      }
    }
    assert(hsecs >= 0 && hsecs < 100 && secs >= 0);
    int fill_char = s.fill();
    s << secs << "." << setfill('0') << setw(2) << hsecs;
    s.fill(fill_char);
  }
#else
  s << "(no clock available)";
#endif
}
