/* Documentation for used namespaces.
   Copyright (C) 2001-2006 Roberto Bagnara <bagnara@cs.unipr.it>

This file is part of the Parma Polyhedra Library (PPL).

The PPL is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2 of the License, or (at your
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

#ifndef PPL_namespaces_hh
#define PPL_namespaces_hh 1

//! The entire library is confined to this namespace.
/*! \ingroup PPL_CXX_interface */
namespace Parma_Polyhedra_Library {

//! All input/output operators are confined to this namespace.
/*! \ingroup PPL_CXX_interface
  This is done so that the library's input/output operators
  do not interfere with those the user might want to define.
  In fact, it is highly unlikely that any predefined I/O
  operator will suit the needs of a client application.
  On the other hand, those applications for which the PPL
  I/O operator are enough can easily obtain access to them.
  For example, a directive like
  \code
    using namespace Parma_Polyhedra_Library::IO_Operators;
  \endcode
  would suffice for most uses.
  In more complex situations, such as
  \code
    const Constraint_System& cs = ...;
    copy(cs.begin(), cs.end(),
	 ostream_iterator<Constraint>(cout, "\n"));
  \endcode
  the Parma_Polyhedra_Library namespace must be suitably extended.
  This can be done as follows:
  \code
    namespace Parma_Polyhedra_Library {
      // Import all the output operators into the main PPL namespace.
      using IO_Operators::operator<<;
    }
  \endcode
*/
namespace IO_Operators {
} // namespace IO_Operators

#ifdef PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
//! Types and functions implementing checked numbers.
/*! \ingroup PPL_CXX_interface */
#endif // PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
namespace Checked {
} // namespace Checked

#ifdef PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
//! %Implementation related data and functions.
/*! \ingroup PPL_CXX_interface */
#endif // PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
namespace Implementation {
} // namespace Implementation


} // namespace Parma_Polyhedra_Library


//! The standard C++ namespace.
/*! \ingroup PPL_CXX_interface
  The Parma Polyhedra Library conforms to the C++ standard and,
  in particular, as far as reserved names are concerned (17.4.3.1,
  [lib.reserved.names]).  The PPL, however, defines several
  template specializations for the standard library function templates
  <CODE>swap()</CODE> and <CODE>iter_swap()</CODE> (25.2.2, [lib.alg.swap]),
  and for the class template <CODE>numeric_limits</CODE>
  (18.2.1, [lib.limits]).

  \note
  The PPL provides the specializations of the class template
  <CODE>numeric_limits</CODE> not only for PPL-specific numeric types,
  but also for the GMP types <CODE>mpz_class</CODE> and
  <CODE>mpq_class</CODE>. These specializations will be removed
  as soon as they will be provided by the C++ interface of GMP.
*/
namespace std {
} // namespace std


#endif // !defined(PPL_namespaces_hh)
