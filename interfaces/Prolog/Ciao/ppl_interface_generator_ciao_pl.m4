dnl This file generates ppl_ciao.pl.
/* Ciao Prolog interface: Ciao Prolog part.
include(`ppl_interface_generator_copyright')
*/

:- module(ppl_ciao,
[
divert(1)
],
[
        assertions,
        basicmodes,
        regtypes,
        foreign_interface
]).

divert(2)dnl
:- extra_linker_opts('-L.libs').
:- use_foreign_library(ppl_ciao).

:- impl_defined(
[
divert(3)
]).

:- comment(version_maintenance,off).

divert`'dnl
include(`ppl_interface_generator_prolog_systems.m4')dnl
define(`m4_extension', `ifelse($4, 0, , `COMMA
')	  $1/$2')dnl
patsubst(ppl_prolog_sys_code, COMMA, `,')`'dnl
undivert(1)`'dnl
divert`'dnl
define(`check_fail', `ifelse(index($1, nofail), -1, failok, nofail)')
define(`term', `Term$1')dnl
define(`anyterm', `any_term')dnl
define(`interm', `in(Term$1)')dnl
define(`m4_extension', `ifelse(check_fail($3), nofail,
  `:- true pred $1`'ifelse(`$2', 0, ,(`'m4_term_sequence($2, `interm'))
          :: `'m4_term_sequence($2, `anyterm', ` *')) +  foreign.

',
  `:- true pred $1_2(`'m4_term_sequence($2, `interm')`'ifelse(`$2', 0, `go(Success)', `, go(Success)'))
          :: `'m4_term_sequence($2, `anyterm', ` *')`'ifelse(`$2', 0, ` int', ` * int')
  + (returns(Success), foreign($1)).

$1`'ifelse(`$2', 0, ,`(m4_term_sequence($2, `term'))') :-
   $1_2(`'m4_term_sequence($2, `term')`'ifelse(`$2', 0, `1', `, 1')).

')')dnl
ppl_prolog_sys_code`'dnl
undivert(2)dnl
divert`'dnl
define(`m4_extension',
  `ifelse(check_fail($3), nofail, ,ifelse($4, 0, , `COMMA
')	`'$1_2/incr($2))')dnl
patsubst(ppl_prolog_sys_code, COMMA, `,')`'dnl
