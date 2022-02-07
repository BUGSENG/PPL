/* Declarations for thread safety.
   Copyright (C) 2014-2016 Maxim Gaina <maxim.gaina@studenti.unipr.it>
   Copyright (C) 2016      BUGSENG srl (http://bugseng.com)

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

#ifndef PPL_thread_safe_hh
#define PPL_thread_safe_hh 1

#ifdef PPL_THREAD_SAFE
#    define PPL_TLS thread_local
#else
#    define PPL_TLS
#endif

#endif // !defined(PPL_thread_safe_hh)

