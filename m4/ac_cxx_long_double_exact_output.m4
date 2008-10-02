dnl A function to detect whether C++ provides exact output for long doubles.
dnl Copyright (C) 2001-2008 Roberto Bagnara <bagnara@cs.unipr.it>
dnl
dnl This file is part of the Parma Polyhedra Library (PPL).
dnl
dnl The PPL is free software; you can redistribute it and/or modify it
dnl under the terms of the GNU General Public License as published by the
dnl Free Software Foundation; either version 3 of the License, or (at your
dnl option) any later version.
dnl
dnl The PPL is distributed in the hope that it will be useful, but WITHOUT
dnl ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
dnl FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
dnl for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program; if not, write to the Free Software Foundation,
dnl Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1307, USA.
dnl
dnl For the most up-to-date information see the Parma Polyhedra Library
dnl site: http://www.cs.unipr.it/ppl/ .

AC_DEFUN([AC_CXX_LONG_DOUBLE_EXACT_OUTPUT],
[
AC_REQUIRE([AC_C_BIGENDIAN])
AC_REQUIRE([AC_CXX_LONG_DOUBLE_BINARY_FORMAT])
ac_save_CPPFLAGS="$CPPFLAGS"
ac_save_LIBS="$LIBS"
AC_LANG_PUSH(C++)

AC_MSG_CHECKING([whether C++ provide exact output for long doubles])
ac_cxx_long_double_exact_output=unknown

AC_RUN_IFELSE([AC_LANG_SOURCE([[
#include <limits>
#ifdef HAVE_STDINT_H
#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS 1
#endif
#include <stdint.h>
#endif
#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif
#include <cstdlib>
#include <sstream>
#include <iostream>

bool
check(long double value, const char* text) {
  std::ostringstream ss;
  ss.precision(10000);
  ss << value;
  std::cout << ss.str() << " ?==? " << text << std::endl;
  return ss.str() == text;
}

#if SIZEOF_LONG_DOUBLE == 12

long double
convert(uint32_t msp, uint64_t lsp) {
  union {
    long double value;
    struct {
#ifdef WORDS_BIGENDIAN
      uint32_t msp;
      uint64_t lsp;
#else
      uint64_t lsp;
      uint32_t msp;
#endif
    } parts;
  } u;

  u.parts.msp = msp;
  u.parts.lsp = lsp;
  return u.value;
}

#if CXX_LONG_DOUBLE_BINARY_FORMAT == FLOAT_INTEL_DOUBLE_EXTENDED

int
main() {
  if (check(convert(0xaaacccaaUL, 0xacccaaacccaaacccULL),
            "-23475151196235330448360987288488448023604990597437922665537894499317141030270831473500040521309097782521743811281100731620612303910141158923283064807755815684643856544564541670181998003713114595098650350075763370916908460284838902787487703020001649220669684278577319085319204471093693972621987808779314674903648668236688723332999785552894845325917244897920804830290985291535343986477508649100409893486651507134071296739623686984014433965803259868867855465109457220731656260670349710137932272802677796915669809481885196101414678015638835049035133835457854893845093496950772367562664955830097293678125205138879788670490610650322134182370990336443894886171520732434535881085387893610114822547025980969387956199048733969351859470324032549906964255557877860343871346618951696178837035563054101786829980983909935265617095747543882856776297861197958453847978446679149969948882161264279705948735019353220550905117946051015070744207453853343171175921378515361160726195198161165083475968")
      && check(convert(0xcccaaaccUL, 0xcaaacccaaacccaaaULL),
               "-3.234349908433673569788362433758236701401379200386310478070230491201716094576208088733092209352711081763530973139439402754721006217889031902309566905344106766697050593355851405189694125966100801537593811320493850579152938679705465870788684523518541806158791408378718596936132854683425581754205242594614192465876640102628432246897674583649745467641139234207139584810101868772915312454075582265240687184279243861217351667059920878236164506473261797703393981510799228030778152399657445848839855361831637014754038570644257393307922665529947406582062857348101442206039278033447012163028384634753160589791458287895963629514043345016273218385584012204321293761500625038828699451659598873739820939586314044735551522599884065690704853850058509917597610804664086074437219778400030370474948166770867639885264893441594112815147445129222900623635467542926999246959939000796968650036727515644358583656635086409945418378371210848857160579105176395869221692977516017583375976424496977231821652434877830409470264872174e-1634"))
    exit(0);
  else
    exit(1);
}

#else // CXX_LONG_DOUBLE_BINARY_FORMAT != FLOAT_INTEL_DOUBLE_EXTENDED

int
main() {
  exit(1);
}

#endif // CXX_LONG_DOUBLE_BINARY_FORMAT != FLOAT_INTEL_DOUBLE_EXTENDED

#elif SIZEOF_LONG_DOUBLE == 16

long double
convert(uint64_t msp, uint64_t lsp) {
  union {
    long double value;
    struct {
#ifdef WORDS_BIGENDIAN
      uint64_t msp;
      uint64_t lsp;
#else
      uint64_t lsp;
      uint64_t msp;
#endif
    } parts;
  } u;

  u.parts.msp = msp;
  u.parts.lsp = lsp;
  return u.value;
}

#if CXX_LONG_DOUBLE_BINARY_FORMAT == FLOAT_IEEE754_QUAD

int
main() {
  if (check(convert(0xaaacccaaacccaaacULL, 0xccaaacccaaacccaaULL),
            "-8.5585565388510043474134185399390263e-1644")
      &&  check(convert(0xcccaaacccaaacccaULL, 0xaacccaaacccaaaccULL),
                "-1.245145811075115522107964569349668e+986"))
    exit(0);
  else
    exit(1);
}

#elif CXX_LONG_DOUBLE_BINARY_FORMAT == FLOAT_INTEL_DOUBLE_EXTENDED

int
main() {
  if (check(convert(0xaaacccaaacccaaacULL, 0xccaaacccaaacccaaULL),
            "-7.6048540943660203383375998875878002021755137452856942966991187375790618225432711954682137529456970686263412309600617913197560145993931792374946076061175594550915924955313391528566777352996816141138550473090399783817652516896553304678788796532976847530386107750759127004897950996937530751872097675156395937218234460013748219954603465088115161828048215763754331575034447236689765045674584188927116128770082933362914567237187017530082528540058002631800274192146485961758366625476271676375489358478966148719270989233284454992652229565352739884432045036085427546783826745250197623257802129011015273728848161395367551745780868250488191368846207890422268873532651627591649389757751752362072212699309947970918940313250863861141479770240104635553035870698789854752554391365757900620463269938427975381635241159694500569550818398323639621243086116154792038064941523448921284117826015673798399008555604372098051095571652170081985843614541956756469168604624857938653843172027861680499952062356914208628014745759010068414302636374017506453133466034362025545080555878082849488386388633197121003201192243150535143329731394874806398663589117246866205872431804204733617792317769570293754056793574570217834482046448788177551912250351735294891953434051815332631157674735614138641122106756749236824504094008852401427746414294902929999820878756040223246586814590339767277305131757114819123212897500810087015748205562614251410818122967399030151040544000563841334553718469935435435467196184665715776274355094471974036803615388313095637066824428688301387559074204015990782977635088047810891072724763113879034313256582868462323549141603475107584159170279311985555035822254439699354641467841523895459190280971487264907972306090449968125859834702627544039374770757804202780369083049631377117943503836158566134919890165392965694050010089785271180956974707841066789578714463804030217977213138932711313311673101336981407798868338574571646697479192174043005729926344062133712267722538847635563454980776602355539986984320762864338177037919835721981686778834558429264194379257521818498431326991615024245632036376472844033831041082865649867453234086495085785897620758602105639344460383483879837994732204331335249564434458633345708439547881275060781028856140511029543559841701048277393119691261302356763314623124427357421753631218361407626116820986377721964654181839884670481278855478057996766639496909094607181503284084821580817995489740980323820218685313435967428474842973086612053963343516426226771094376179468881265734324847319127862733279299033951360912038720313525324094079916937921290391173035983474894312808532257620563284126400481460163180987618432784215807933623038747129658746767737999870125917269554155887740155308886259497202672935871853706835670467770080598813171256855182510726902033818328368569915805303784312301890212694874227119220544172084552511408717615136830401766455214293141216171050374325461714383991698910564587610624980490526840861990674615172112493813749497898151186927777122955666180439323595468816129418014664347852590958362752922841159054568358354108159485566264221008508127941168192513144760720303715640604755054290633421601734103622748053919536319416025380484868142967373186019970714662893713233834399238357795019603619284595839414945178963942707310299674873908104634979966232181071013259653467885733418936194081202149071958107062125873498848807502257018093517304220495114497635240876547262237411350327960679115197531609627900227913193653254580253539005743563270152329126178570329413401577715075363838562221558204219798925982825556469998988615897432425107152113543617151738802778259560442930074253918797315241708532198010528564246933829811859278710445896556638416265987381678070434371355795025956293319362293063591248665429733880825207421011852020394068840244110140622151494488609575671090387230241296949998536132398625008053543954239573998167556201366323875845409929752349853515625e-1644")
      && check(convert(0xcccaaacccaaacccaULL, 0xaacccaaacccaaaccULL),
               "-99658331877181425640389193712445288804009112642407197633229907048864350192381814628233384153539524368748305269642704450459572458913058829202094408933558533552137589226430537671503754737153845553845646099179512540696038707395491223325946106007770844660381340028079827237033670900446083793353682761885084154898636897779677124010119288945740273072415898996441722571487815052387317025675191665761918119006431828756780493604546658949166486641354783002536071366287780290680620995991797712341457334946893188786269086688063732222194404683551757689083590842400866213237312413463207537587813396338061744078437770542720749055069473347142994267706326342325536219464867910547533482061181116137767384001927599515332824741827726661184966512254203502805790565338206862173475388342339711722457620964017690492860707751327158273522191943184085888284707357024653025991470473697475045491586713324994056478341556198451786713470909185879382607340766256394396819602885198511409676789226542867632933493115191296"))
    exit(0);
  else
    exit(1);
}

#else // CXX_LONG_DOUBLE_BINARY_FORMAT != FLOAT_INTEL_DOUBLE_EXTENDED

int
main() {
  exit(1);
}

#endif // CXX_LONG_DOUBLE_BINARY_FORMAT != FLOAT_INTEL_DOUBLE_EXTENDED

#elif SIZEOF_LONG_DOUBLE == 8

double
convert(uint32_t msp, uint32_t lsp) {
  union {
    long double value;
    struct {
#ifdef WORDS_BIGENDIAN
      uint32_t msp;
      uint32_t lsp;
#else
      uint32_t lsp;
      uint32_t msp;
#endif
    } parts;
  } u;

  u.parts.msp = msp;
  u.parts.lsp = lsp;
  return u.value;
}

#if CXX_LONG_DOUBLE_BINARY_FORMAT == FLOAT_IEEE754_DOUBLE

int
main() {
  if (check(convert(0xaaacccaaUL, 0xacccaaacUL),
            "-4.018242396032647e-103")
      && check(convert(0xcccaaaccUL, 0xcaaacccaUL),
               "-85705035845709846787631445265530356117787053916987832397725696"))
    exit(0);
  else
    exit(1);
}

#else // CXX_LONG_DOUBLE_BINARY_FORMAT != FLOAT_IEEE754_DOUBLE

int
main() {
  exit(1);
}

#endif // CXX_LONG_DOUBLE_BINARY_FORMAT != FLOAT_IEEE754_DOUBLE

#else // SIZEOF_LONG_DOUBLE != 8

int
main() {
  exit(1);
}

#endif // SIZEOF_LONG_DOUBLE != 8
]])],
  AC_MSG_RESULT(yes)
  ac_cxx_long_double_exact_output=1,
  AC_MSG_RESULT(no)
  ac_cxx_long_double_exact_output=0)

AC_DEFINE(CXX_LONG_DOUBLE_EXACT_OUTPUT, $ac_cxx_long_double_output,
  [Not zero if C++ supports exact output for long doubles.])

AC_LANG_POP(C++)
CPPFLAGS="$ac_save_CPPFLAGS"
LIBS="$ac_save_LIBS"
])
