/* This is the header file switcher of the Parma Polyhedra Library.
   Copyright (C) 2001-2010 Roberto Bagnara <bagnara@cs.unipr.it>
   Copyright (C) 2010-2022 BUGSENG srl (http://bugseng.com)

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

#if defined(__x86_64__)
#    include "ppl-x86_64.hh"
#elif defined(__i386__)
#    include "ppl-i386.hh"
#elif defined(__powerpc64__)
#    include "ppl-ppc64.hh"
#elif defined(__powerpc__)
#    include "ppl-ppc.hh"
#elif defined(__ia64__)
#    include "ppl-ia64.hh"
#elif defined(__alpha__)
#    include "ppl-alpha.hh"
#elif defined(__arm__)
#    include "ppl-arm.hh"
#elif defined(__s390x__)
#    include "ppl-s390x.hh"
#elif defined(__s390__)
#    include "ppl-s390.hh"
#elif defined(__sh__)
#    include "ppl-sh.hh"
#elif defined(__sparc__) && defined(__arch64__)
#    include "ppl-sparc64.hh"
#elif defined(__sparc__)
#    include "ppl-sparc.hh"
#else
#    error \
    "This architecture is not supported by the currently installed ppl-devel packages"
#endif

