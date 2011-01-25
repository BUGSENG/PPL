/* Watchdog and associated classes' implementation (non-inline functions).
   Copyright (C) 2001-2010 Roberto Bagnara <bagnara@cs.unipr.it>
   Copyright (C) 2010-2011 BUGSENG srl (http://bugseng.com)

This file is part of the Parma Watchdog Library (PWL).

The PWL is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

The PWL is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1307, USA.

For the most up-to-date information see the Parma Polyhedra Library
site: http://www.cs.unipr.it/ppl/ . */

#include <pwl-config.h>

#include "Watchdog.defs.hh"

namespace PWL = Parma_Watchdog_Library;

#if PWL_HAVE_DECL_SETITIMER && PWL_HAVE_DECL_SIGACTION

#include <csignal>
#include <iostream>
#include <stdexcept>
#include <cerrno>
#include <string>
#include <string.h>

#ifdef PWL_TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <ctime>
#else
# ifdef PWL_HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <ctime>
# endif
#endif

// Cygwin only supports ITIMER_REAL.
// Apparently GNU Hurd also only supports ITIMER_REAL
// (see http://www.cs.unipr.it/pipermail/ppl-devel/2010-March/016072.html).
// Profiling does not work on programs that use the ITIMER_PROF timer.
#if defined(__CYGWIN__) || defined(__gnu_hurd__) || defined(PWL_PROFILING)
#define THE_TIMER  ITIMER_REAL
#define THE_SIGNAL SIGALRM
#else
#define THE_TIMER  ITIMER_PROF
#define THE_SIGNAL SIGPROF
#endif

using std::cerr;
using std::endl;

// Pass this to getitimer().
itimerval PWL::Watchdog::current_timer_status;

// Pass this to setitimer().
itimerval PWL::Watchdog::signal_once;

// Last time value we set the timer to.
PWL::Time PWL::Watchdog::last_time_requested;

// Records the time elapsed since last fresh start.
PWL::Time PWL::Watchdog::time_so_far;

// The ordered queue of pending watchdog events.
PWL::Watchdog::WD_Pending_List PWL::Watchdog::pending;

// Whether the alarm clock is running.
volatile bool PWL::Watchdog::alarm_clock_running = false;

// Whether we are changing data which are also changed by the signal handler.
volatile bool PWL::Watchdog::in_critical_section = false;

namespace {

void
throw_syscall_error(const char* syscall_name) {
  throw std::runtime_error(std::string(syscall_name) + ": " + strerror(errno));
}

void
my_getitimer(int which, struct itimerval* value) {
  if (getitimer(which, value) != 0)
    throw_syscall_error("getitimer");
}

void
my_setitimer(int which,
	     const struct itimerval* value, struct itimerval* ovalue) {
  if (setitimer(which, value, ovalue) != 0)
    throw_syscall_error("setitimer");
}

void
my_sigaction(int signum,
	     const struct sigaction* act, struct sigaction* oldact) {
  if (sigaction(signum, act, oldact) != 0)
    throw_syscall_error("sigaction");
}

} // namespace

void
PWL::Watchdog::get_timer(Time& time) {
  my_getitimer(THE_TIMER, &current_timer_status);
  time = Time(current_timer_status.it_value.tv_sec,
	      current_timer_status.it_value.tv_usec);
}

void
PWL::Watchdog::set_timer(const Time& time) {
  if (time.seconds() == 0 && time.microseconds() == 0)
    throw std::runtime_error("PWL internal error");
  last_time_requested = time;
  signal_once.it_value.tv_sec = time.seconds();
  signal_once.it_value.tv_usec = time.microseconds();
  my_setitimer(THE_TIMER, &signal_once, 0);
}

void
PWL::Watchdog::stop_timer() {
  signal_once.it_value.tv_sec = 0;
  signal_once.it_value.tv_usec = 0;
  my_setitimer(THE_TIMER, &signal_once, 0);
}

void
PWL::Watchdog::handle_timeout(int) {
  if (in_critical_section)
    reschedule();
  else {
    time_so_far += last_time_requested;
    if (!pending.empty()) {
      WD_Pending_List::Iterator i = pending.begin();
      do {
	i->handler().act();
	i->expired_flag() = true;
	i = pending.erase(i);
      } while (i != pending.end() && i->deadline() <= time_so_far);
      if (pending.empty())
	alarm_clock_running = false;
      else
	set_timer((*pending.begin()).deadline() - time_so_far);
    }
    else
      alarm_clock_running = false;
  }
}

void
PWL::PWL_handle_timeout(int signum) {
  PWL::Watchdog::handle_timeout(signum);
}

PWL::Watchdog::WD_Pending_List::Iterator
PWL::Watchdog::new_watchdog_event(unsigned int units,
				  const Handler& handler,
				  bool& expired_flag) {
  assert(units > 0);
  WD_Pending_List::Iterator position;
  Time deadline(units);
  if (!alarm_clock_running) {
    position = pending.insert(deadline, handler, expired_flag);
    time_so_far = Time(0);
    set_timer(deadline);
    alarm_clock_running = true;
  }
  else {
    Time time_to_shoot;
    get_timer(time_to_shoot);
    Time elapsed_time(last_time_requested);
    elapsed_time -= time_to_shoot;
    Time current_time(time_so_far);
    current_time += elapsed_time;
    Time real_deadline(deadline);
    real_deadline += current_time;
    position = pending.insert(real_deadline, handler, expired_flag);
    if (deadline < time_to_shoot) {
      time_so_far = current_time;
      set_timer(deadline);
    }
  }
  return position;
}

void
PWL::Watchdog::remove_watchdog_event(WD_Pending_List::Iterator position) {
  assert(!pending.empty());
  if (position == pending.begin()) {
    WD_Pending_List::Iterator next = position;
    ++next;
    if (next != pending.end()) {
      Time first_deadline(position->deadline());
      Time next_deadline(next->deadline());
      if (first_deadline != next_deadline) {
	Time time_to_shoot;
	get_timer(time_to_shoot);
	Time elapsed_time(last_time_requested);
	elapsed_time -= time_to_shoot;
	time_so_far += elapsed_time;
	next_deadline -= first_deadline;
	time_to_shoot += next_deadline;
	set_timer(time_to_shoot);
      }
    }
    else {
      stop_timer();
      alarm_clock_running = false;
    }
  }
  pending.erase(position);
}

PWL::Time PWL::Watchdog::reschedule_time(1);

void
PWL::Watchdog::initialize() {
  signal_once.it_interval.tv_sec = 0;
  signal_once.it_interval.tv_usec = 0;

  sigset_t mask;
  sigemptyset(&mask);

  struct sigaction s;
  s.sa_handler = PWL_handle_timeout;
  s.sa_mask = mask;
  s.sa_flags = 0;  // Was SA_ONESHOT: why?

  my_sigaction(THE_SIGNAL, &s, 0);
}

void
PWL::Watchdog::finalize() {
}

unsigned int PWL::Init::count = 0;

#endif // PWL_HAVE_DECL_SETITIMER && PWL_HAVE_DECL_SIGACTION
