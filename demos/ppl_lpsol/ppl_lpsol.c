/* Solve linear programming problems by either vertex/point enumeration
   or the primal simplex algorithm.
   Copyright (C) 2001-2010 Roberto Bagnara <bagnara@cs.unipr.it>
   Copyright (C) 2010-2019 BUGSENG srl (http://bugseng.com)

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
site: http://bugseng.com/products/ppl/ . */

#include "ppl-config.h"
#include "ppl_c.h"
#include <gmp.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <time.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>

#ifdef PPL_THREAD_SAFE
# define PPL_LPSOL_MULTI_THREADED
#endif

#ifdef PPL_LPSOL_MULTI_THREADED
#include <pthread.h>
/*
  Note: check option is unsupported when using multiple threads,
  because it uses glpk (which is not thread safe).
*/
#else  /* !defined(PPL_LPSOL_MULTI_THREADED) */
# define PPL_LPSOL_SUPPORTS_CHECK_OPTION
#endif /* !defined(PPL_LPSOL_MULTI_THREADED) */

#if defined(PPL_HAVE_GLPK_GLPK_H)
#include <glpk/glpk.h>
#elif defined(PPL_HAVE_GLPK_H)
#include <glpk.h>
#endif

#ifdef PPL_HAVE_GETOPT_H
# include <getopt.h>

/* Try to accommodate non-GNU implementations of `getopt()'. */
#if !defined(no_argument) && defined(NO_ARG)
#define no_argument NO_ARG
#endif

#if !defined(required_argument) && defined(REQUIRED_ARG)
#define required_argument REQUIRED_ARG
#endif

#if !defined(optional_argument) && defined(OPTIONAL_ARG)
#define optional_argument OPTIONAL_ARG
#endif

#endif /* defined(PPL_HAVE_GETOPT_H) */

#ifdef PPL_HAVE_UNISTD_H
/* Include this for `getopt()': especially important if we do not have
   <getopt.h>. */
# include <unistd.h>
#endif

#ifdef PPL_HAVE_SIGNAL_H
# include <signal.h>
#endif

#ifdef PPL_HAVE_SYS_TIME_H
# include <sys/time.h>
#endif

#ifdef PPL_HAVE_SYS_RESOURCE_H
/* This should be included after <time.h> and <sys/time.h> so as to make
   sure we have the definitions for, e.g., `ru_utime'. */
# include <sys/resource.h>
#endif

#if PPL_VERSION_MAJOR == 0 && PPL_VERSION_MINOR < 10
# error "PPL version 0.10 or following is required"
#endif

static const char* ppl_source_version = PPL_VERSION;

#ifdef __GNUC__
# define ATTRIBUTE_UNUSED __attribute__ ((__unused__))
#else
# define ATTRIBUTE_UNUSED
#endif

#if PPL_HAVE_DECL_GETRUSAGE
# define PPL_LPSOL_SUPPORTS_TIMINGS
#endif

#if defined(PPL_HAVE_SYS_RESOURCE_H) \
  && PPL_CXX_SUPPORTS_LIMITING_MEMORY \
  && (defined(SA_ONESHOT) || defined(SA_RESETHAND))
# define PPL_LPSOL_SUPPORTS_LIMIT_ON_CPU_TIME
#endif

#ifdef PPL_HAVE_GETOPT_H
static struct option long_options[] = {
#ifdef PPL_LPSOL_SUPPORTS_CHECK_OPTION
  {"check",           optional_argument, 0, 'c'},
#endif /* defined(PPL_LPSOL_SUPPORTS_CHECK_OPTION) */
  {"help",            no_argument,       0, 'h'},
  {"incremental",     no_argument,       0, 'i'},
  {"min",             no_argument,       0, 'm'},
  {"max",             no_argument,       0, 'M'},
  {"no-optimization", no_argument,       0, 'n'},
  {"no-mip",          no_argument,       0, 'r'},
  {"max-cpu",         required_argument, 0, 'C'},
  {"max-memory",      required_argument, 0, 'R'},
  {"output",          required_argument, 0, 'o'},
  {"pricing",         required_argument, 0, 'p'},
  {"enumerate",       no_argument,       0, 'e'},
  {"simplex",         no_argument,       0, 's'},
#ifdef PPL_LPSOL_SUPPORTS_TIMINGS
  {"timings",         no_argument,       0, 't'},
#endif /* defined(PPL_LPSOL_SUPPORTS_TIMINGS) */
  {"verbosity",       required_argument, 0, 'v'},
  {"version",         no_argument,       0, 'V'},
  {0, 0, 0, 0}
};
#endif

#ifndef PPL_LPSOL_MULTI_THREADED
/* Single-thread mode: at most one input file. */
#define USAGE_STRING0                                                   \
  "Usage: %s [OPTION]... [FILE]\n"                                      \
  "Reads a file in MPS format and attempts their solution using the optimization\n" \
  "algorithms provided by the PPL.\n\n"                                 \
  "Options:\n"
#else /* defined(PPL_LPSOL_MULTI_THREADED) */
/* Multi-thread mode: at most MAX_THREADS input files. */
#define str(s) # s
#define xstr(s) str(s)
#define USAGE_STRING0                                                     \
  "Usage: %s [OPTION]... [FILE]...\n"                                     \
  "Reads at most " xstr(MAX_THREADS) " files in MPS format and attempts " \
  "solution using the\n"                                                  \
  "optimization algorithms provided by the PPL (each input file spawns\n" \
  "an execution thread).\n\n"                                             \
  "Options:\n"
#endif /* defined(PPL_LPSOL_MULTI_THREADED) */

#ifdef PPL_LPSOL_SUPPORTS_CHECK_OPTION
#define USAGE_STRING1                                                   \
  "  -c, --check[=THRESHOLD] checks the obtained results using GLPK;\n" \
  "                          optima are checked with a tolerance of\n"  \
  "                          THRESHOLD (default %.10g);  input data\n"  \
  "                          are also perturbed the same way as GLPK does\n"
#endif /* defined(PPL_LPSOL_SUPPORTS_CHECK_OPTION) */

#define USAGE_STRING2                                                   \
  "  -i, --incremental       solves the problem incrementally\n"        \
  "  -m, --min               minimizes the objective function\n"        \
  "  -M, --max               maximizes the objective function (default)\n" \
  "  -n, --no-optimization   checks for satisfiability only\n"          \
  "  -r, --no-mip            consider integer variables as real variables\n" \
  "  -CSECS, --max-cpu=SECS  limits CPU usage to SECS seconds\n"        \
  "  -RMB, --max-memory=MB   limits memory usage to MB megabytes\n"     \
  "  -h, --help              prints this help text to stdout\n"

#ifdef PPL_LPSOL_MULTI_THREADED
#define USAGE_STRING3                                                   \
  "  -oPATH, --output=PATH   writes output files in directory PATH\n"
#else
#define USAGE_STRING3                                                   \
  "  -oPATH, --output=PATH   appends output to PATH\n"
#endif /* !defined(PPL_LPSOL_MULTI_THREADED) */

#define USAGE_STRING4                                                   \
  "  -e, --enumerate         use the (expensive!) enumeration method\n" \
  "  -pM, --pricing=M        use pricing method M for simplex (assumes -s);\n" \
  "                          M is an int from 0 to 2, default 0:\n"     \
  "                          0 --> steepest-edge using floating point\n" \
  "                          1 --> steepest-edge using exact arithmetic\n" \
  "                          2 --> textbook\n"                          \
  "  -s, --simplex           use the simplex method\n"

#ifdef PPL_LPSOL_SUPPORTS_TIMINGS
#define USAGE_STRING5                                                   \
  "  -t, --timings           prints timings to stderr\n"
#else /* defined(PPL_LPSOL_SUPPORTS_TIMINGS) */
#define USAGE_STRING5                                                   \
  ""
#endif /* !defined(PPL_LPSOL_SUPPORTS_TIMINGS) */

#define USAGE_STRING6                                                   \
  "  -v, --verbosity=LEVEL   sets verbosity level (from 0 to 4, default 3):\n" \
  "                          0 --> quiet: no output except for errors and\n" \
  "                                explicitly required notifications\n" \
  "                          1 --> solver state only\n"                 \
  "                          2 --> state + optimal value\n"             \
  "                          3 --> state + optimal value + optimum location\n" \
  "                          4 --> lots of output\n"                    \
  "  -V, --version           prints version information to stdout\n"

#ifndef PPL_HAVE_GETOPT_H
#define USAGE_STRING7                                                   \
  "\n"                                                                  \
  "NOTE: this version does not support long options.\n"
#else /* defined(PPL_HAVE_GETOPT_H) */
#define USAGE_STRING7                                                   \
  ""
#endif /* !defined(PPL_HAVE_GETOPT_H) */

#define USAGE_STRING8                                                   \
  "\n"                                                                  \
  "Report bugs to <ppl-devel@cs.unipr.it>.\n"


#define OPTION_LETTERS "bc::eimnMC:R:ho:p:rstVv:"

static const char* program_name = 0;
static unsigned long max_bytes_of_virtual_memory = 0;
static const char* output_argument = 0;
static int use_simplex = 0;
static int pricing_method = 0;
static int verbosity = 3;
static int maximize = 1;
static int incremental = 0;
static int no_optimization = 0;
static int no_mip = 0;

#ifdef PPL_LPSOL_SUPPORTS_CHECK_OPTION
static int check_results = 0;
static int check_results_failed = 0;
static double check_threshold = 0.0;
static const double default_check_threshold = 0.000000001;
#endif /* defined(PPL_LPSOL_SUPPORTS_CHECK_OPTION) */

#ifdef PPL_LPSOL_SUPPORTS_LIMIT_ON_CPU_TIME
static unsigned long max_seconds_of_cpu_time = 0;
#endif /* defined (PPL_LPSOL_SUPPORTS_LIMIT_ON_CPU_TIME) */

#ifdef PPL_LPSOL_SUPPORTS_TIMINGS
static int print_timings = 0;
#endif /* defined(PPL_LPSOL_SUPPORTS_TIMINGS) */

#ifdef PPL_LPSOL_MULTI_THREADED

#define MAX_THREADS 8
static pthread_t threads[MAX_THREADS];

/* Note: each thread has its own output_file */
static __thread FILE* output_file = NULL;
/* Note: each thread has its own variable_names */
static __thread const char** variable_names = NULL;

#else /* !defined(PPL_LPSOL_MULTI_THREADED) */

static FILE* output_file = NULL;
static const char** variable_names = NULL;

#endif /* !defined(PPL_LPSOL_MULTI_THREADED) */

typedef struct {
  const char* input_file_name;
  FILE* output_file;
  ppl_Constraint_System_t ppl_cs;
  ppl_Linear_Expression_t ppl_objective_le;
  int dimension;
  const char** variable_names;
  int num_integer_variables;
  ppl_dimension_type* integer_variables;
  mpz_t den_lcm;
#ifdef PPL_LPSOL_SUPPORTS_TIMINGS
  struct timeval saved_ru_utime;
#endif /* defined(PPL_LPSOL_SUPPORTS_TIMINGS) */
} args_t;

static void
my_exit(int status) {
  (void) ppl_finalize();
  exit(status);
}

void
fatal(const char* format, ...) {
  va_list ap;
  fprintf(stderr, "%s: ", program_name);
  va_start(ap, format);
  vfprintf(stderr, format, ap);
  va_end(ap);
  fprintf(stderr, "\n");
  my_exit(1);
}

#if 0
static void
warning(const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  fprintf(stderr, "%s: warning: ", program_name);
  vfprintf(stderr, format, ap);
  fprintf(stderr, "\n");
  va_end(ap);
}
#endif

/* Avoid unused function warning in multi-threaded mode. */
#ifdef PPL_LPSOL_SUPPORTS_CHECK_OPTION
static void
error(const char* format, ...) {
  va_list ap;
  fprintf(stderr, "%s: ", program_name);
  va_start(ap, format);
  vfprintf(stderr, format, ap);
  va_end(ap);
  fprintf(stderr, "\n");
  if (output_argument) {
    va_start(ap, format);
    vfprintf(output_file, format, ap);
    va_end(ap);
    fprintf(output_file, "\n");
  }
}
#endif /* defined(PPL_LPSOL_SUPPORTS_CHECK_OPTION) */


static const char*
get_ppl_version() {
  const char* p;
  (void) ppl_version(&p);
  return p;
}

static const char*
get_ppl_banner() {
  const char* p;
  (void) ppl_banner(&p);
  return p;
}

static void
process_options(int argc, char* argv[]) {
#ifdef PPL_HAVE_GETOPT_H
  int option_index;
#endif
  int enumerate_required = 0;
  int simplex_required = 0;
  int incremental_required = 0;
  int no_optimization_required = 0;
  int no_mip_required = 0;
  int c;
  char* endptr;
  long l;
#ifdef PPL_LPSOL_SUPPORTS_CHECK_OPTION
  double d;
#endif

  while (1) {
#ifdef PPL_HAVE_GETOPT_H
    option_index = 0;
    c = getopt_long(argc, argv, OPTION_LETTERS, long_options, &option_index);
#else
    c = getopt(argc, argv, OPTION_LETTERS);
#endif
    if (c == EOF)
      break;

    switch (c) {
    case 0:
      break;

#ifdef PPL_LPSOL_SUPPORTS_CHECK_OPTION
    case 'c':
      check_results = 1;
      if (optarg) {
        d = strtod(optarg, &endptr);
        if (*endptr || errno == ERANGE || d < 0.0)
          fatal("only a non-negative floating point number can `-c'");
        else
          check_threshold = d;
      }
      else
        check_threshold = default_check_threshold;
      break;
#endif /* defined(PPL_LPSOL_SUPPORTS_CHECK_OPTION) */

    case 'm':
      maximize = 0;
      break;

    case 'M':
      maximize = 1;
      break;

    case '?':
    case 'h':
      fprintf(stdout, USAGE_STRING0, argv[0]);
#ifdef PPL_LPSOL_SUPPORTS_CHECK_OPTION
      fprintf(stdout, USAGE_STRING1, default_check_threshold);
#endif
      fputs(USAGE_STRING2, stdout);
      fputs(USAGE_STRING3, stdout);
      fputs(USAGE_STRING4, stdout);
      fputs(USAGE_STRING5, stdout);
      fputs(USAGE_STRING6, stdout);
      fputs(USAGE_STRING7, stdout);
      fputs(USAGE_STRING8, stdout);
      my_exit(0);
      break;

#ifdef PPL_LPSOL_SUPPORTS_LIMIT_ON_CPU_TIME

    case 'C':
      l = strtol(optarg, &endptr, 10);
      if (*endptr || l < 0)
        fatal("a non-negative integer must follow `-C'");
      else
        max_seconds_of_cpu_time = l;
      break;

#endif /* defined (PPL_LPSOL_SUPPORTS_LIMIT_ON_CPU_TIME) */

    case 'R':
      l = strtol(optarg, &endptr, 10);
      if (*endptr || l < 0)
        fatal("a non-negative integer must follow `-R'");
      else if (((unsigned long) l) > ULONG_MAX/(1024*1024))
        max_bytes_of_virtual_memory = ULONG_MAX;
      else
        max_bytes_of_virtual_memory = l*1024*1024;
      break;

    case 'o':
      output_argument = optarg;
      break;

    case 'p':
      l = strtol(optarg, &endptr, 10);
      if (*endptr || l < 0 || l > 2)
        fatal("0 or 1 or 2 must follow `-p'");
      else
        pricing_method = l;
      break;

    case 'e':
      enumerate_required = 1;
      break;

    case 's':
      simplex_required = 1;
      break;

#ifdef PPL_LPSOL_SUPPORTS_TIMINGS

    case 't':
      print_timings = 1;
      break;

#endif /* defined(PPL_LPSOL_SUPPORTS_TIMINGS) */

    case 'v':
      l = strtol(optarg, &endptr, 10);
      if (*endptr || l < 0 || l > 4)
        fatal("verbosity must be an integer between 0 and 4");
      else
        verbosity = l;
      break;

    case 'V':
      fprintf(stdout, "%s\n", PPL_VERSION);
      my_exit(0);
      break;

    case 'i':
      incremental_required = 1;
      break;

    case 'n':
      no_optimization_required = 1;
      break;

    case 'r':
      no_mip_required = 1;
      break;

    default:
      abort();
    }
  }

  if (enumerate_required
      && (simplex_required
          || incremental_required))
      fatal("-e option is incompatible with -i and -s");

  if (enumerate_required)
    use_simplex = 0;
  else if (simplex_required)
    use_simplex = 1;

  if (incremental_required)
    incremental = 1;

  if (no_optimization_required)
    no_optimization = 1;

  if (no_mip_required)
    no_mip = 1;

  if (optind >= argc) {
    if (verbosity >= 4)
      fprintf(stderr,
              "Parma Polyhedra Library version:\n%s\n\n"
              "Parma Polyhedra Library banner:\n%s\n",
              get_ppl_version(),
              get_ppl_banner());
    else
      fatal("no input files");
  }

#ifndef PPL_LPSOL_MULTI_THREADED
  if (argc - optind > 1)
    /* We have multiple input files. */
    fatal("only one input file is accepted");
#else /* defined(PPL_LPSOL_MULTI_THREADED) */
  if (argc - optind > MAX_THREADS)
    fatal("at most " xstr(MAX_THREADS) " input files are accepted" );
  if (output_argument == NULL)
    fatal("option -oPATH is mandatory");
#endif /* defined(PPL_LPSOL_MULTI_THREADED) */

}

#ifdef PPL_LPSOL_SUPPORTS_TIMINGS

static void
start_clock(struct timeval* saved_ru_utime) {
  struct rusage rsg;
  if (getrusage(RUSAGE_SELF, &rsg) != 0)
    fatal("getrusage failed: %s", strerror(errno));
  else
    *saved_ru_utime = rsg.ru_utime;
}

static void
print_clock(FILE* f, struct timeval* saved_ru_utime) {
  struct rusage rsg;
  if (getrusage(RUSAGE_SELF, &rsg) != 0)
    fatal("getrusage failed: %s", strerror(errno));
  else {
    time_t current_secs = rsg.ru_utime.tv_sec;
    time_t current_usecs = rsg.ru_utime.tv_usec;
    time_t saved_secs = saved_ru_utime->tv_sec;
    time_t saved_usecs = saved_ru_utime->tv_usec;
    int secs;
    int csecs;
    if (current_usecs < saved_usecs) {
      csecs = (((1000000 + current_usecs) - saved_usecs) + 5000) / 10000;
      secs = (current_secs - saved_secs) -1;
    }
    else {
      csecs = ((current_usecs - saved_usecs) + 5000) / 10000;
      secs = current_secs - saved_secs;
    }
    assert(csecs >= 0 && csecs < 100 && secs >= 0);
    fprintf(f, "%d.%.2d", secs, csecs);
  }
}

#endif /* defined(PPL_LPSOL_SUPPORTS_TIMINGS) */

#ifdef PPL_LPSOL_SUPPORTS_LIMIT_ON_CPU_TIME

void
set_alarm_on_cpu_time(unsigned seconds, void (*handler)(int)) {
  sigset_t mask;
  struct sigaction s;
  struct rlimit t;

  sigemptyset(&mask);

  s.sa_handler = handler;
  s.sa_mask = mask;
#if defined(SA_ONESHOT)
  s.sa_flags = SA_ONESHOT;
#elif defined(SA_RESETHAND)
  s.sa_flags = SA_RESETHAND;
#else
# error "Either SA_ONESHOT or SA_RESETHAND must be defined."
#endif

  if (sigaction(SIGXCPU, &s, 0) != 0)
    fatal("sigaction failed: %s", strerror(errno));

  if (getrlimit(RLIMIT_CPU, &t) != 0)
    fatal("getrlimit failed: %s", strerror(errno));

  if (seconds < t.rlim_cur) {
    t.rlim_cur = seconds;
    if (setrlimit(RLIMIT_CPU, &t) != 0)
      fatal("setrlimit failed: %s", strerror(errno));
  }
}

#endif /* defined(PPL_LPSOL_SUPPORTS_LIMIT_ON_CPU_TIME) */

#if PPL_CXX_SUPPORTS_LIMITING_MEMORY && PPL_HAVE_DECL_RLIMIT_AS

void
limit_virtual_memory(unsigned long bytes) {
  struct rlimit t;

  if (getrlimit(RLIMIT_AS, &t) != 0)
    fatal("getrlimit failed: %s", strerror(errno));

  if (bytes < t.rlim_cur) {
    t.rlim_cur = bytes;
    if (setrlimit(RLIMIT_AS, &t) != 0)
      fatal("setrlimit failed: %s", strerror(errno));
  }
}

#else

void
limit_virtual_memory(unsigned long bytes ATTRIBUTE_UNUSED) {
}

#endif /* !PPL_HAVE_DECL_RLIMIT_AS */

#ifdef PPL_LPSOL_SUPPORTS_LIMIT_ON_CPU_TIME

static void
my_timeout(int dummy ATTRIBUTE_UNUSED) {
  fprintf(stderr, "TIMEOUT\n");
  if (output_argument)
    fprintf(output_file, "TIMEOUT\n");
  my_exit(0);
}

#endif /* defined(PPL_LPSOL_SUPPORTS_LIMIT_ON_CPU_TIME) */

static mpz_t tmp_z;
static ppl_Coefficient_t ppl_coeff;
static glp_prob* glpk_lp;

static void
maybe_check_results(args_t* args,
                    const int ppl_status,
                    const double ppl_optimum_value) {
#ifndef PPL_LPSOL_SUPPORTS_CHECK_OPTION

  /* Do nothing at all. */
  (void) args;
  (void) ppl_status;
  (void) ppl_optimum_value;

#else /* defined(PPL_LPSOL_SUPPORTS_CHECK_OPTION) */

  const char* ppl_status_string;
  const char* glpk_status_string;
  int glpk_status;
  int treat_as_lp = 0;
  glp_smcp glpk_smcp;

  if (!check_results)
    return;

  if (no_mip || args->num_integer_variables == 0)
    treat_as_lp = 1;

  glp_set_obj_dir(glpk_lp, (maximize ? GLP_MAX : GLP_MIN));

  glp_init_smcp(&glpk_smcp);
  /* Disable GLPK output. */
  glpk_smcp.msg_lev = GLP_MSG_OFF;

  if (treat_as_lp) {
    /* Set the problem class to LP: MIP problems are thus treated as
       LP ones. */
    glp_exact(glpk_lp, &glpk_smcp);
    glpk_status = glp_get_status(glpk_lp);
  }
  else {
    /* MIP case. */
    glp_simplex(glpk_lp, &glpk_smcp);
    glpk_status = glp_get_status(glpk_lp);
    if (glpk_status != GLP_NOFEAS && glpk_status != GLP_UNBND) {
      glp_iocp glpk_iocp;
      glp_init_iocp(&glpk_iocp);
      /* Disable GLPK output. */
      glpk_iocp.msg_lev = GLP_MSG_OFF;
      glp_intopt(glpk_lp, &glpk_iocp);
      glpk_status = glp_mip_status(glpk_lp);
    }
  }
  /* If no_optimization is enabled, the second case is not possibile. */
  if (!((ppl_status == PPL_MIP_PROBLEM_STATUS_UNFEASIBLE
         && glpk_status == GLP_NOFEAS)
        || (ppl_status == PPL_MIP_PROBLEM_STATUS_UNBOUNDED
            && glpk_status == GLP_UNBND)
        || (ppl_status == PPL_MIP_PROBLEM_STATUS_OPTIMIZED
            && (glpk_status == GLP_OPT
                /* If no_optimization is enabled, check if the problem is
                   unbounded for GLPK.  */
                || (no_optimization && (glpk_status == GLP_UNBND
                                        || glpk_status == GLP_UNDEF))))))  {

    if (ppl_status == PPL_MIP_PROBLEM_STATUS_UNFEASIBLE)
      ppl_status_string = "unfeasible";
    else if (ppl_status == PPL_MIP_PROBLEM_STATUS_UNBOUNDED)
      ppl_status_string = "unbounded";
    else if (ppl_status == PPL_MIP_PROBLEM_STATUS_OPTIMIZED)
      ppl_status_string = "optimizable";
    else
      ppl_status_string = "<?>";

    switch (glpk_status) {
    case GLP_NOFEAS:
      glpk_status_string = "unfeasible";
      break;
    case GLP_UNBND:
      glpk_status_string = "unbounded";
      break;
    case GLP_OPT:
      glpk_status_string = "optimizable";
      break;
    case GLP_UNDEF:
      glpk_status_string = "undefined";
      break;
    default:
      glpk_status_string = "<?>";
      break;
    }

    error("check failed: for GLPK the problem is %s, not %s",
          glpk_status_string, ppl_status_string);

    check_results_failed = 1;
  }
  else if (!no_optimization
           && ppl_status == PPL_MIP_PROBLEM_STATUS_OPTIMIZED) {

    double glpk_optimum_value
      = (treat_as_lp ? glp_get_obj_val(glpk_lp) : glp_mip_obj_val(glpk_lp));

    if (fabs(ppl_optimum_value - glpk_optimum_value) > check_threshold) {
      error("check failed: for GLPK the problem's optimum is %.20g,"
            " not %.20g", glpk_optimum_value, ppl_optimum_value);
      check_results_failed = 1;
    }
  }

#endif /* defined(PPL_LPSOL_SUPPORTS_CHECK_OPTION) */

}


static const char**
copy_variable_names(int dimension) {
  char** names;
  int i;
  const char* name;
  size_t size;

  names = (char**) malloc(dimension * sizeof(char*));
  for (i = 0; i < dimension; ++i) {
    name = glp_get_col_name(glpk_lp, i+1);
    if (name == NULL)
      names[i] = NULL;
    else {
      size = 1 + strlen(name);
      names[i] = (char*) malloc(size);
      strncpy(names[i], name, size);
    }
  }
  return (const char**) names;
}

static const char*
variable_output_function(ppl_dimension_type var) {
  return variable_names[var];
}

static void
add_constraints(ppl_Linear_Expression_t ppl_le,
                int type, mpq_t rational_lb, mpq_t rational_ub, mpz_t den_lcm,
                ppl_Constraint_System_t ppl_cs) {
  ppl_Constraint_t ppl_c;
  ppl_Linear_Expression_t ppl_le2;
  switch (type) {
  case GLP_FR:
    break;

  case GLP_LO:
    mpz_mul(tmp_z, den_lcm, mpq_numref(rational_lb));
    mpz_divexact(tmp_z, tmp_z, mpq_denref(rational_lb));
    mpz_neg(tmp_z, tmp_z);
    ppl_assign_Coefficient_from_mpz_t(ppl_coeff, tmp_z);
    ppl_Linear_Expression_add_to_inhomogeneous(ppl_le, ppl_coeff);
    ppl_new_Constraint(&ppl_c, ppl_le, PPL_CONSTRAINT_TYPE_GREATER_OR_EQUAL);
    if (verbosity >= 4) {
      ppl_io_fprint_Constraint(output_file, ppl_c);
      fprintf(output_file, "\n");
    }
    ppl_Constraint_System_insert_Constraint(ppl_cs, ppl_c);
    ppl_delete_Constraint(ppl_c);
    break;

  case GLP_UP:
    mpz_mul(tmp_z, den_lcm, mpq_numref(rational_ub));
    mpz_divexact(tmp_z, tmp_z, mpq_denref(rational_ub));
    mpz_neg(tmp_z, tmp_z);
    ppl_assign_Coefficient_from_mpz_t(ppl_coeff, tmp_z);
    ppl_Linear_Expression_add_to_inhomogeneous(ppl_le, ppl_coeff);
    ppl_new_Constraint(&ppl_c, ppl_le,
                       PPL_CONSTRAINT_TYPE_LESS_OR_EQUAL);
    if (verbosity >= 4) {
      ppl_io_fprint_Constraint(output_file, ppl_c);
      fprintf(output_file, "\n");
    }
    ppl_Constraint_System_insert_Constraint(ppl_cs, ppl_c);
    ppl_delete_Constraint(ppl_c);
    break;

  case GLP_DB:
    ppl_new_Linear_Expression_from_Linear_Expression(&ppl_le2, ppl_le);

    mpz_mul(tmp_z, den_lcm, mpq_numref(rational_lb));
    mpz_divexact(tmp_z, tmp_z, mpq_denref(rational_lb));
    mpz_neg(tmp_z, tmp_z);
    ppl_assign_Coefficient_from_mpz_t(ppl_coeff, tmp_z);
    ppl_Linear_Expression_add_to_inhomogeneous(ppl_le, ppl_coeff);
    ppl_new_Constraint(&ppl_c, ppl_le, PPL_CONSTRAINT_TYPE_GREATER_OR_EQUAL);
    if (verbosity >= 4) {
      ppl_io_fprint_Constraint(output_file, ppl_c);
      fprintf(output_file, "\n");
    }
    ppl_Constraint_System_insert_Constraint(ppl_cs, ppl_c);
    ppl_delete_Constraint(ppl_c);

    mpz_mul(tmp_z, den_lcm, mpq_numref(rational_ub));
    mpz_divexact(tmp_z, tmp_z, mpq_denref(rational_ub));
    mpz_neg(tmp_z, tmp_z);
    ppl_assign_Coefficient_from_mpz_t(ppl_coeff, tmp_z);
    ppl_Linear_Expression_add_to_inhomogeneous(ppl_le2, ppl_coeff);
    ppl_new_Constraint(&ppl_c, ppl_le2, PPL_CONSTRAINT_TYPE_LESS_OR_EQUAL);
    ppl_delete_Linear_Expression(ppl_le2);
    if (verbosity >= 4) {
      ppl_io_fprint_Constraint(output_file, ppl_c);
      fprintf(output_file, "\n");
    }
    ppl_Constraint_System_insert_Constraint(ppl_cs, ppl_c);
    ppl_delete_Constraint(ppl_c);
    break;

  case GLP_FX:
    mpz_mul(tmp_z, den_lcm, mpq_numref(rational_lb));
    mpz_divexact(tmp_z, tmp_z, mpq_denref(rational_lb));
    mpz_neg(tmp_z, tmp_z);
    ppl_assign_Coefficient_from_mpz_t(ppl_coeff, tmp_z);
    ppl_Linear_Expression_add_to_inhomogeneous(ppl_le, ppl_coeff);
    ppl_new_Constraint(&ppl_c, ppl_le,
                       PPL_CONSTRAINT_TYPE_EQUAL);
    if (verbosity >= 4) {
      ppl_io_fprint_Constraint(output_file, ppl_c);
      fprintf(output_file, "\n");
    }
    ppl_Constraint_System_insert_Constraint(ppl_cs, ppl_c);
    ppl_delete_Constraint(ppl_c);
    break;

  default:
    fatal("internal error");
    break;
  }
}

static int
solve_with_generators(args_t* args,
                      ppl_Coefficient_t optimum_n,
                      ppl_Coefficient_t optimum_d,
                      ppl_Generator_t point) {
  ppl_Polyhedron_t ppl_ph;
  int optimum_found = 0;
  int empty;
  int unbounded;
  int included;

  /* Create the polyhedron (recycling the data structures of ppl_cs). */
  ppl_new_C_Polyhedron_recycle_Constraint_System(&ppl_ph, args->ppl_cs);

#ifdef PPL_LPSOL_SUPPORTS_TIMINGS

  if (print_timings) {
    fprintf(stderr, "Time to create a PPL polyhedron for %s: ",
            args->input_file_name);
    print_clock(stderr, &args->saved_ru_utime);
    fprintf(stderr, " s\n");
    start_clock(&args->saved_ru_utime);
  }

#endif /* defined(PPL_LPSOL_SUPPORTS_TIMINGS) */

  empty = ppl_Polyhedron_is_empty(ppl_ph);

#ifdef PPL_LPSOL_SUPPORTS_TIMINGS

  if (print_timings) {
    fprintf(stderr, "Time to check for emptiness for %s: ",
            args->input_file_name);
    print_clock(stderr, &args->saved_ru_utime);
    fprintf(stderr, " s\n");
    start_clock(&args->saved_ru_utime);
  }

#endif /* defined(PPL_LPSOL_SUPPORTS_TIMINGS) */

  if (empty) {
    if (verbosity >= 1)
      fprintf(output_file, "Unfeasible problem %s.\n",
              args->input_file_name);
    maybe_check_results(args, PPL_MIP_PROBLEM_STATUS_UNFEASIBLE, 0.0);
    goto exit;
  }

  if (!empty && no_optimization) {
    if (verbosity >= 1)
      fprintf(output_file, "Feasible problem %s.\n",
              args->input_file_name);
    /* Kludge: let's pass PPL_MIP_PROBLEM_STATUS_OPTIMIZED,
       to let work `maybe_check_results'. */
    maybe_check_results(args, PPL_MIP_PROBLEM_STATUS_OPTIMIZED, 0.0);
    goto exit;
  }

  /* Check whether the problem is unbounded. */
  unbounded = maximize
    ? !ppl_Polyhedron_bounds_from_above(ppl_ph, args->ppl_objective_le)
    : !ppl_Polyhedron_bounds_from_below(ppl_ph, args->ppl_objective_le);

#ifdef PPL_LPSOL_SUPPORTS_TIMINGS

  if (print_timings) {
    fprintf(stderr, "Time to check for unboundedness for %s: ",
            args->input_file_name);
    print_clock(stderr, &args->saved_ru_utime);
    fprintf(stderr, " s\n");
    start_clock(&args->saved_ru_utime);
  }

#endif /* defined(PPL_LPSOL_SUPPORTS_TIMINGS) */

  if (unbounded) {
    if (verbosity >= 1)
      fprintf(output_file, "Unbounded problem %s.\n",
              args->input_file_name);
    maybe_check_results(args, PPL_MIP_PROBLEM_STATUS_UNBOUNDED, 0.0);
    goto exit;
  }

  optimum_found = maximize
    ? ppl_Polyhedron_maximize_with_point(ppl_ph, args->ppl_objective_le,
                                         optimum_n, optimum_d, &included,
                                         point)
    : ppl_Polyhedron_minimize_with_point(ppl_ph, args->ppl_objective_le,
                                         optimum_n, optimum_d, &included,
                                         point);

#ifdef PPL_LPSOL_SUPPORTS_TIMINGS

  if (print_timings) {
    fprintf(stderr, "Time to find the optimum for %s: ",
            args->input_file_name);
    print_clock(stderr, &args->saved_ru_utime);
    fprintf(stderr, " s\n");
    start_clock(&args->saved_ru_utime);
  }

#endif /* defined(PPL_LPSOL_SUPPORTS_TIMINGS) */

  if (!optimum_found)
    fatal("internal error");

  if (!included)
    fatal("internal error");

 exit:
  ppl_delete_Polyhedron(ppl_ph);
  return optimum_found;
}

static int
solve_with_simplex(args_t* args,
                   ppl_Coefficient_t optimum_n,
                   ppl_Coefficient_t optimum_d,
                   ppl_Generator_t point) {
  ppl_MIP_Problem_t ppl_mip;
  int optimum_found = 0;
  int pricing = 0;
  int status = 0;
  int satisfiable = 0;
  ppl_dimension_type space_dim;
  ppl_const_Constraint_t c;
  ppl_const_Generator_t g;
  ppl_Constraint_System_const_iterator_t i;
  ppl_Constraint_System_const_iterator_t iend;
  int counter;
  int mode = maximize
    ? PPL_OPTIMIZATION_MODE_MAXIMIZATION
    : PPL_OPTIMIZATION_MODE_MINIMIZATION;

  ppl_Constraint_System_space_dimension(args->ppl_cs, &space_dim);
  ppl_new_MIP_Problem_from_space_dimension(&ppl_mip, space_dim);
  switch (pricing_method) {
  case 0:
    pricing = PPL_MIP_PROBLEM_CONTROL_PARAMETER_PRICING_STEEPEST_EDGE_FLOAT;
    break;
  case 1:
    pricing = PPL_MIP_PROBLEM_CONTROL_PARAMETER_PRICING_STEEPEST_EDGE_EXACT;
    break;
  case 2:
    pricing = PPL_MIP_PROBLEM_CONTROL_PARAMETER_PRICING_TEXTBOOK;
    break;
  default:
    fatal("ppl_lpsol internal error");
  }
  ppl_MIP_Problem_set_control_parameter(ppl_mip, pricing);
  ppl_MIP_Problem_set_objective_function(ppl_mip, args->ppl_objective_le);
  ppl_MIP_Problem_set_optimization_mode(ppl_mip, mode);
  if (!no_mip) {
    ppl_MIP_Problem_add_to_integer_space_dimensions
      (ppl_mip, args->integer_variables, args->num_integer_variables);
  }
  if (incremental) {
    /* Add the constraints of `cs' one at a time. */
    ppl_new_Constraint_System_const_iterator(&i);
    ppl_new_Constraint_System_const_iterator(&iend);
    ppl_Constraint_System_begin(args->ppl_cs, i);
    ppl_Constraint_System_end(args->ppl_cs, iend);

    counter = 0;
    while (!ppl_Constraint_System_const_iterator_equal_test(i, iend)) {
      ++counter;
      if (verbosity >= 4)
        fprintf(output_file, "\nSolving constraint %d\n", counter);
      ppl_Constraint_System_const_iterator_dereference(i, &c);
      ppl_MIP_Problem_add_constraint(ppl_mip, c);

      if (no_optimization) {
        satisfiable = ppl_MIP_Problem_is_satisfiable(ppl_mip);
        if (!satisfiable)
          break;
      }
      else
        status = ppl_MIP_Problem_solve(ppl_mip);
      ppl_Constraint_System_const_iterator_increment(i);
    }
    ppl_delete_Constraint_System_const_iterator(i);
    ppl_delete_Constraint_System_const_iterator(iend);
  }

  else {
    ppl_MIP_Problem_add_constraints(ppl_mip, args->ppl_cs);
    if (no_optimization)
      satisfiable = ppl_MIP_Problem_is_satisfiable(ppl_mip);
    else
      status = ppl_MIP_Problem_solve(ppl_mip);
  }

#ifdef PPL_LPSOL_SUPPORTS_TIMINGS

  if (print_timings) {
    fprintf(stderr, "Time to solve the problem %s: ", args->input_file_name);
    print_clock(stderr, &args->saved_ru_utime);
    fprintf(stderr, " s\n");
    start_clock(&args->saved_ru_utime);
  }

#endif /* defined(PPL_LPSOL_SUPPORTS_TIMINGS) */

  if ((no_optimization && !satisfiable)
      || (!no_optimization && status == PPL_MIP_PROBLEM_STATUS_UNFEASIBLE)) {
    if (verbosity >= 1)
      fprintf(output_file, "Unfeasible problem %s.\n",
              args->input_file_name);
    maybe_check_results(args, status, 0.0);
    goto exit;
  }
  else if (no_optimization && satisfiable) {
    if (verbosity >= 1)
      fprintf(output_file, "Feasible problem %s\n",
              args->input_file_name);
    /* Kludge: let's pass PPL_MIP_PROBLEM_STATUS_OPTIMIZED,
       to let work `maybe_check_results'. */
    maybe_check_results(args, PPL_MIP_PROBLEM_STATUS_OPTIMIZED, 0.0);
    goto exit;
  }
  else if (status == PPL_MIP_PROBLEM_STATUS_UNBOUNDED) {
    if (verbosity >= 1)
      fprintf(output_file, "Unbounded problem %s\n",
              args->input_file_name);
    maybe_check_results(args, status, 0.0);
    goto exit;
  }
  else if (status == PPL_MIP_PROBLEM_STATUS_OPTIMIZED) {
    ppl_MIP_Problem_optimal_value(ppl_mip, optimum_n, optimum_d);
    ppl_MIP_Problem_optimizing_point(ppl_mip, &g);
    ppl_assign_Generator_from_Generator(point, g);
    optimum_found = 1;
    goto exit;
  }
  else
    fatal("internal error");

 exit:
  ppl_delete_MIP_Problem(ppl_mip);
  return optimum_found;
}

extern void set_d_eps(mpq_t x, double val);

static void
set_mpq_t_from_double(mpq_t q, double d) {
#ifdef PPL_LPSOL_SUPPORTS_CHECK_OPTION
  if (check_results)
    set_d_eps(q, d);
  else
    mpq_set_d(q, d);
#else
  mpq_set_d(q, d);
#endif
}

static void
process_args(void* vp) {
  args_t* args;
#ifndef NDEBUG
  ppl_Constraint_System_t ppl_cs_copy;
#endif
  ppl_Coefficient_t optimum_n;
  ppl_Coefficient_t optimum_d;
  ppl_Generator_t optimum_location;
  int optimum_found;
  mpq_t optimum;
  mpz_t z;
  mpq_t q;
  ppl_Coefficient_t coeff;
  int i;

  args = (args_t*) vp;

  /* Set the TLS variables output_file and variable_names. */
  output_file = args->output_file;
  variable_names = args->variable_names;

  /* Install the (thread specific) variable output function. */
  if (ppl_io_set_variable_output_function(variable_output_function) < 0)
    fatal("cannot install the custom variable output function");

#ifndef NDEBUG
  ppl_new_Constraint_System_from_Constraint_System(&ppl_cs_copy, args->ppl_cs);
#endif

  ppl_new_Coefficient(&optimum_n);
  ppl_new_Coefficient(&optimum_d);
  ppl_new_Generator_zero_dim_point(&optimum_location);

  optimum_found = use_simplex
    ? solve_with_simplex(args, optimum_n, optimum_d, optimum_location)
    : solve_with_generators(args, optimum_n, optimum_d, optimum_location);

  if (optimum_found) {
    mpz_init(z);
    mpq_init(optimum);
    mpq_init(q);
    ppl_new_Coefficient(&coeff);

    ppl_Coefficient_to_mpz_t(optimum_n, z);
    mpq_set_num(optimum, z);
    ppl_Coefficient_to_mpz_t(optimum_d, z);
    mpz_mul(z, z, args->den_lcm);
    mpq_set_den(optimum, z);
    if (verbosity == 1)
      fprintf(output_file, "Optimized problem.\n");
    if (verbosity >= 2)
      fprintf(output_file, "Optimum value: %.10g\n", mpq_get_d(optimum));
    if (verbosity >= 3) {
      fprintf(output_file, "Optimum location:\n");
      ppl_Generator_divisor(optimum_location, coeff);
      ppl_Coefficient_to_mpz_t(coeff, z);
      for (i = 0; i < args->dimension; ++i) {
        mpz_set(mpq_denref(q), z);
        ppl_Generator_coefficient(optimum_location, i, coeff);
        ppl_Coefficient_to_mpz_t(coeff, mpq_numref(q));
        ppl_io_fprint_variable(output_file, i);
        fprintf(output_file, " = %.10g\n", mpq_get_d(q));
      }
    }
#ifndef NDEBUG
    {
      ppl_Polyhedron_t ph;
      unsigned int relation;
      ppl_new_C_Polyhedron_recycle_Constraint_System(&ph, ppl_cs_copy);
      ppl_delete_Constraint_System(ppl_cs_copy);
      relation = ppl_Polyhedron_relation_with_Generator(ph, optimum_location);
      ppl_delete_Polyhedron(ph);
      assert(relation == PPL_POLY_GEN_RELATION_SUBSUMES);
    }
#endif
    maybe_check_results(args, PPL_MIP_PROBLEM_STATUS_OPTIMIZED,
                        mpq_get_d(optimum));

    ppl_delete_Coefficient(coeff);
    mpq_clear(q);
    mpq_clear(optimum);
    mpz_clear(z);
  }

  /* Release resources for local variables. */
  ppl_delete_Coefficient(optimum_d);
  ppl_delete_Coefficient(optimum_n);
  ppl_delete_Generator(optimum_location);

  /* Release args_t resources. */
  ppl_delete_Constraint_System(args->ppl_cs);
  ppl_delete_Linear_Expression(args->ppl_objective_le);
  assert(args->variable_names == variable_names);
  for (i = 0; i < args->dimension; ++i)
    free((void*) variable_names[i]);
  free(variable_names);
  free(args->integer_variables);
  mpz_clear(args->den_lcm);
  assert(args->output_file == output_file);
  fclose(output_file);
  output_file = NULL;
  free(args);
}

#ifdef PPL_LPSOL_MULTI_THREADED
static void*
thread_process_args(void* vp) {
  ppl_thread_initialize();
  process_args(vp);
  ppl_thread_finalize();
  return NULL;
}
#endif /* defined(PPL_LPSOL_MULTI_THREADED) */

static void
error_handler(enum ppl_enum_error_code code,
              const char* description) {
  if (output_argument)
    fprintf(output_file, "PPL error code %d: %s\n", code, description);
  fatal("PPL error code %d: %s", code, description);
}

#if defined(NDEBUG)

#if !(defined(PPL_GLPK_HAS_GLP_TERM_OUT) && defined(GLP_OFF))

#if defined(PPL_GLPK_HAS_GLP_TERM_HOOK) \
  || defined(PPL_GLPK_HAS__GLP_LIB_PRINT_HOOK)

static int
glpk_message_interceptor(void* info, const char* msg) {
  (void) info;
  (void) msg;
  return 1;
}

#elif defined(PPL_GLPK_HAS_LIB_SET_PRINT_HOOK)

static int
glpk_message_interceptor(void* info, char* msg) {
  (void) info;
  (void) msg;
  return 1;
}

#endif /* !(defined(PPL_GLPK_HAS_GLP_TERM_HOOK)
            || defined(PPL_GLPK_HAS__GLP_LIB_PRINT_HOOK))
          && defined(PPL_GLPK_HAS_LIB_SET_PRINT_HOOK) */

#endif /* !(defined(PPL_GLPK_HAS_GLP_TERM_OUT) && defined(GLP_OFF)) */

#endif /* defined(NDEBUG) */


#ifdef PPL_LPSOL_MULTI_THREADED
static const char*
get_output_name(const char* input_name) {
  /* FIXME: to be made platform independent. */
  const char* extension = ".out";
  int output_length = 0;
  char* output_name;

  if (strrchr(input_name, '/') != NULL) {
    input_name = strrchr(input_name, '/') + 1;
  }

  if (output_argument != 0) {
    output_length += strlen(output_argument);
    if (output_argument[strlen(output_argument) - 1] != '/')
      ++output_length;
  }
  output_length += strlen(input_name);
  output_length += strlen(extension);

  output_name = (char*) malloc(output_length + 1);
  *output_name = 0;

  if (output_argument != 0) {
    strcat(output_name, output_argument);
    if (output_argument[strlen(output_argument) - 1] != '/')
      strcat(output_name, "/");
  }
  strcat(output_name, input_name);
  strcat(output_name, extension);
  return output_name;
}
#endif /* defined(PPL_LPSOL_MULTI_THREADED) */

static void
set_output_file(const char* file_name) {
#ifdef PPL_LPSOL_MULTI_THREADED
  const char* output_name = NULL;
  output_name = get_output_name(file_name);
  output_file = fopen(output_name, "a");
  if (output_file == NULL)
    fatal("cannot open output file `%s'", output_name);
#else /* !defined(PPL_LPSOL_MULTI_THREADED) */
  /* In single threaded mode, output name does not depend on input name. */
  (void) file_name;
  if (output_argument) {
    output_file = fopen(output_argument, "w");
    if (output_file == NULL)
      fatal("cannot open output file `%s'", output_argument);
  }
  else
    output_file = stdout;
#endif /* !defined(PPL_LPSOL_MULTI_THREADED) */
}

static args_t*
prepare_args(const char* file_name) {
  ppl_Constraint_System_t ppl_cs;
  ppl_Linear_Expression_t ppl_le;
  int dimension, row, num_rows, column, nz, i, j, type;
  const char** variable_names;
  int* coefficient_index;
  double lb, ub;
  double* coefficient_value;
  mpq_t rational_lb, rational_ub;
  mpq_t* rational_coefficient;
  mpq_t* objective;
  ppl_Linear_Expression_t ppl_objective_le;
  mpz_t den_lcm;
  glp_mpscp glpk_mpscp;
  int num_integer_variables;
  ppl_dimension_type* integer_variables;
  struct timeval saved_ru_utime;
  args_t* args;

  set_output_file(file_name);

  glp_init_mpscp(&glpk_mpscp);

  if (verbosity == 0) {
    /* FIXME: find a way to suppress output from glp_read_mps. */
  }

#ifdef PPL_LPSOL_SUPPORTS_TIMINGS
  if (print_timings)
    start_clock(&saved_ru_utime);
#endif /* defined(PPL_LPSOL_SUPPORTS_TIMINGS) */

  if (glp_read_mps(glpk_lp, GLP_MPS_FILE, &glpk_mpscp, file_name) != 0)
    fatal("cannot read MPS file `%s'", file_name);

#ifdef PPL_LPSOL_SUPPORTS_TIMINGS
  if (print_timings) {
    fprintf(stderr, "Time to read the input file %s: ", file_name);
    print_clock(stderr, &saved_ru_utime);
    fprintf(stderr, " s\n");
    start_clock(&saved_ru_utime);
  }
#endif /* defined(PPL_LPSOL_SUPPORTS_TIMINGS) */

  num_integer_variables = glp_get_num_int(glpk_lp);

  if (num_integer_variables > 0 && !no_mip && !use_simplex)
    fatal("the enumeration solving method can not handle MIP problems");

  dimension = glp_get_num_cols(glpk_lp);
  variable_names = copy_variable_names(dimension);

  /* Read variables constrained to be integer. */
  integer_variables = 0;
  if (num_integer_variables > 0 && !no_mip && use_simplex) {
    if (verbosity >= 4)
      fprintf(output_file, "Integer variables:\n");
    integer_variables = (ppl_dimension_type*)
      malloc((num_integer_variables + 1)*sizeof(ppl_dimension_type));
    for (i = 0, j = 0; i < dimension; ++i) {
      int col_kind = glp_get_col_kind(glpk_lp, i+1);
      if (col_kind == GLP_IV || col_kind == GLP_BV) {
        integer_variables[j] = i;
        if (verbosity >= 4) {
          ppl_io_fprint_variable(output_file, i);
          fprintf(output_file, " ");
        }
        ++j;
      }
    }
  }
  coefficient_index = (int*) malloc((dimension+1)*sizeof(int));
  coefficient_value = (double*) malloc((dimension+1)*sizeof(double));
  rational_coefficient = (mpq_t*) malloc((dimension+1)*sizeof(mpq_t));

  ppl_new_Constraint_System(&ppl_cs);

  mpq_init(rational_lb);
  mpq_init(rational_ub);
  for (i = 1; i <= dimension; ++i)
    mpq_init(rational_coefficient[i]);

  mpz_init(den_lcm);

  if (verbosity >= 4)
    fprintf(output_file, "\nConstraints:\n");

  /* Set up the row (ordinary) constraints. */
  num_rows = glp_get_num_rows(glpk_lp);
  for (row = 1; row <= num_rows; ++row) {
    /* Initialize the least common multiple computation. */
    mpz_set_si(den_lcm, 1);
    /* Set `nz' to the number of non-zero coefficients. */
    nz = glp_get_mat_row(glpk_lp, row, coefficient_index, coefficient_value);
    for (i = 1; i <= nz; ++i) {
      set_mpq_t_from_double(rational_coefficient[i], coefficient_value[i]);
      /* Update den_lcm. */
      mpz_lcm(den_lcm, den_lcm, mpq_denref(rational_coefficient[i]));
    }

    lb = glp_get_row_lb(glpk_lp, row);
    ub = glp_get_row_ub(glpk_lp, row);

    set_mpq_t_from_double(rational_lb, lb);
    set_mpq_t_from_double(rational_ub, ub);

    mpz_lcm(den_lcm, den_lcm, mpq_denref(rational_lb));
    mpz_lcm(den_lcm, den_lcm, mpq_denref(rational_ub));

    ppl_new_Linear_Expression_with_dimension(&ppl_le, dimension);

    for (i = 1; i <= nz; ++i) {
      mpz_mul(tmp_z, den_lcm, mpq_numref(rational_coefficient[i]));
      mpz_divexact(tmp_z, tmp_z, mpq_denref(rational_coefficient[i]));
      ppl_assign_Coefficient_from_mpz_t(ppl_coeff, tmp_z);
      ppl_Linear_Expression_add_to_coefficient(ppl_le, coefficient_index[i]-1,
                                               ppl_coeff);
    }

    type = glp_get_row_type(glpk_lp, row);
    add_constraints(ppl_le, type, rational_lb, rational_ub, den_lcm, ppl_cs);

    ppl_delete_Linear_Expression(ppl_le);
  }

  free(coefficient_value);
  for (i = 1; i <= dimension; ++i)
    mpq_clear(rational_coefficient[i]);
  free(rational_coefficient);
  free(coefficient_index);

  /*
    FIXME: here we could build the polyhedron and minimize it before
    adding the variable bounds.
  */

  /* Set up the columns constraints, i.e., variable bounds. */
  for (column = 1; column <= dimension; ++column) {

    lb = glp_get_col_lb(glpk_lp, column);
    ub = glp_get_col_ub(glpk_lp, column);

    set_mpq_t_from_double(rational_lb, lb);
    set_mpq_t_from_double(rational_ub, ub);

    /* Initialize the least common multiple computation. */
    mpz_set_si(den_lcm, 1);
    mpz_lcm(den_lcm, den_lcm, mpq_denref(rational_lb));
    mpz_lcm(den_lcm, den_lcm, mpq_denref(rational_ub));

    ppl_new_Linear_Expression_with_dimension(&ppl_le, dimension);
    ppl_assign_Coefficient_from_mpz_t(ppl_coeff, den_lcm);
    ppl_Linear_Expression_add_to_coefficient(ppl_le, column-1, ppl_coeff);

    type = glp_get_col_type(glpk_lp, column);
    add_constraints(ppl_le, type, rational_lb, rational_ub, den_lcm, ppl_cs);

    ppl_delete_Linear_Expression(ppl_le);
  }

  mpq_clear(rational_ub);
  mpq_clear(rational_lb);

  /* Deal with the objective function. */
  objective = (mpq_t*) malloc((dimension+1)*sizeof(mpq_t));

  /* Initialize the least common multiple computation. */
  mpz_set_si(den_lcm, 1);

  mpq_init(objective[0]);
  set_mpq_t_from_double(objective[0], glp_get_obj_coef(glpk_lp, 0));
  for (i = 1; i <= dimension; ++i) {
    mpq_init(objective[i]);
    set_mpq_t_from_double(objective[i], glp_get_obj_coef(glpk_lp, i));
    /* Update den_lcm. */
    mpz_lcm(den_lcm, den_lcm, mpq_denref(objective[i]));
  }

  /* Set the ppl_objective_le to be the objective function. */
  ppl_new_Linear_Expression_with_dimension(&ppl_objective_le, dimension);
  /* Set value for objective function's inhomogeneous term. */
  mpz_mul(tmp_z, den_lcm, mpq_numref(objective[0]));
  mpz_divexact(tmp_z, tmp_z, mpq_denref(objective[0]));
  ppl_assign_Coefficient_from_mpz_t(ppl_coeff, tmp_z);
  ppl_Linear_Expression_add_to_inhomogeneous(ppl_objective_le, ppl_coeff);
  /* Set values for objective function's variable coefficients. */
  for (i = 1; i <= dimension; ++i) {
    mpz_mul(tmp_z, den_lcm, mpq_numref(objective[i]));
    mpz_divexact(tmp_z, tmp_z, mpq_denref(objective[i]));
    ppl_assign_Coefficient_from_mpz_t(ppl_coeff, tmp_z);
    ppl_Linear_Expression_add_to_coefficient(ppl_objective_le, i-1, ppl_coeff);
  }

  if (verbosity >= 4) {
    fprintf(output_file, "Objective function:\n");
    if (mpz_cmp_si(den_lcm, 1) != 0)
      fprintf(output_file, "(");
    ppl_io_fprint_Linear_Expression(output_file, ppl_objective_le);
  }

  for (i = 0; i <= dimension; ++i)
    mpq_clear(objective[i]);
  free(objective);

  if (verbosity >= 4) {
    if (mpz_cmp_si(den_lcm, 1) != 0) {
      fprintf(output_file, ")/");
      mpz_out_str(output_file, 10, den_lcm);
    }
    fprintf(output_file, "\n%s\n",
            (maximize ? "Maximizing." : "Minimizing."));
  }

  /* Allocate a new args_t struct */
  args = (args_t*) malloc(sizeof(args_t));
  /* Pack thread arguments */
  args->input_file_name = file_name;
  args->output_file = output_file;
  args->ppl_cs = ppl_cs;
  args->ppl_objective_le = ppl_objective_le;
  args->dimension = dimension;
  args->variable_names = variable_names;
  args->num_integer_variables = num_integer_variables;
  args->integer_variables = integer_variables;
  mpz_init_set(args->den_lcm, den_lcm);
  args->saved_ru_utime = saved_ru_utime;

  mpz_clear(den_lcm);

  return args;
}

int
main(int argc, char* argv[]) {
  args_t* args;
  const char* file_name;
#ifdef PPL_LPSOL_MULTI_THREADED
  pthread_t* worker;
  int i, num_threads;
  struct timeval master_thread_time;
#endif

#if defined(PPL_GLPK_HAS__GLP_LIB_PRINT_HOOK)
  extern void _glp_lib_print_hook(int (*func)(void *info, const char *buf),
                                  void *info);
#endif

  program_name = argv[0];
  if (ppl_initialize() < 0)
    fatal("cannot initialize the Parma Polyhedra Library");

  /* The PPL solver does not use floating point numbers, except
     perhaps for the steepest edge heuristics.  In contrast, GLPK does
     use them, so it is best to restore the rounding mode as it was
     prior to the PPL initialization.  */
  if (ppl_restore_pre_PPL_rounding() < 0)
    fatal("cannot restore the rounding mode");

  if (ppl_set_error_handler(error_handler) < 0)
    fatal("cannot install the custom error handler");

  if (strcmp(ppl_source_version, get_ppl_version()) != 0)
    fatal("was compiled with PPL version %s, but linked with version %s",
          ppl_source_version, get_ppl_version());

#if defined(NDEBUG)
#if defined(PPL_GLPK_HAS_GLP_TERM_OUT) && defined(GLP_OFF)
  glp_term_out(GLP_OFF);
#elif defined(PPL_GLPK_HAS_GLP_TERM_HOOK)
  glp_term_hook(glpk_message_interceptor, 0);
#elif defined(PPL_GLPK_HAS__GLP_LIB_PRINT_HOOK)
  _glp_lib_print_hook(glpk_message_interceptor, 0);
#elif defined(PPL_GLPK_HAS_LIB_SET_PRINT_HOOK)
  lib_set_print_hook(0, glpk_message_interceptor);
#endif
#endif

  /* Process command line options. */
  process_options(argc, argv);

#if defined(PPL_LPSOL_MULTI_THREADED) && defined(PPL_LPSOL_SUPPORTS_TIMINGS)
  if (print_timings) {
    start_clock(&master_thread_time);
  }
#endif

  /* Initialize globals. */
  mpz_init(tmp_z);
  ppl_new_Coefficient(&ppl_coeff);

#ifdef PPL_LPSOL_SUPPORTS_LIMIT_ON_CPU_TIME

  if (max_seconds_of_cpu_time > 0)
    set_alarm_on_cpu_time(max_seconds_of_cpu_time, my_timeout);

#endif /* defined (PPL_LPSOL_SUPPORTS_LIMIT_ON_CPU_TIME) */

  if (max_bytes_of_virtual_memory > 0)
    limit_virtual_memory(max_bytes_of_virtual_memory);

#ifndef PPL_LPSOL_MULTI_THREADED

  /* Initialize glpk_lp (only once). */
  glpk_lp = glp_create_prob();
  assert(optind + 1 == argc);
  if (check_results)
    check_results_failed = 0;
  file_name = argv[optind];
  args = prepare_args(file_name);
  process_args(args);
  /* Finalize glpk_lp (only once). */
  glp_delete_prob(glpk_lp);

#else /* defined(PPL_LPSOL_MULTI_THEADED) */

  /* Span all threads (MAX_THREADS at most). */
  num_threads = 0;
  while (optind < argc && num_threads < MAX_THREADS) {
    /* Initialize glpk_lp (once per thread). */
    glpk_lp = glp_create_prob();
    file_name = argv[optind++];
    args = prepare_args(file_name);
    /* Finalize glpk_lp (once per thread). */
    glp_delete_prob(glpk_lp);
    /* Closing output_file is up to thread_process_args. */
    output_file = NULL;
    worker = &threads[num_threads++];
    pthread_create(worker, NULL, thread_process_args, args);
  }

  /* Join all threads. */
  for (i = 0; i < num_threads; ++i)
    pthread_join(threads[i], NULL);

#ifdef PPL_LPSOL_SUPPORTS_TIMINGS
  if (print_timings) {
    fprintf(stderr, "Master thread life time: ");
    print_clock(stderr, &master_thread_time);
    fprintf(stderr, " s\n");
  }
#endif

#endif /* defined(PPL_LPSOL_MULTI_THEADED) */

  /* Finalize globals. */
  ppl_delete_Coefficient(ppl_coeff);
  mpz_clear(tmp_z);

  /* Close output file, if any. */
  if (output_file) {
    fclose(output_file);
    output_file = NULL;
  }

#ifdef PPL_LPSOL_SUPPORTS_CHECK_OPTION
  my_exit((check_results && check_results_failed) ? 1 : 0);
#endif

  /* This is just to avoid a compiler warning. */
  return 0;
}
