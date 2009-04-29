/* Scalar_Products class definition.
   Copyright (C) 2001-2009 Roberto Bagnara <bagnara@cs.unipr.it>

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

#ifndef PPL_Scalar_Products_defs_hh
#define PPL_Scalar_Products_defs_hh 1

#include "Scalar_Products.types.hh"
#include "Linear_Row.types.hh"
#include "Coefficient.types.hh"
#include "Linear_Expression.types.hh"
#include "Constraint.types.hh"
#include "Generator.types.hh"
#include "Congruence.types.hh"
#include "Grid_Generator.types.hh"

#ifdef PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
//! A class implementing various scalar product functions.
/*! \ingroup PPL_CXX_interface
  When computing the scalar product of (Linear_Row or Constraint or
  Generator) objects <CODE>x</CODE> and <CODE>y</CODE>, it is assumed
  that the space dimension of the first object <CODE>x</CODE> is less
  than or equal to the space dimension of the second object <CODE>y</CODE>.
*/
#endif // defined(PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS)
class Parma_Polyhedra_Library::Scalar_Products {
public:
  //! Computes the scalar product of \p x and \p y and assigns it to \p z.
  static void assign(Coefficient& z, const Linear_Row& x, const Linear_Row& y);
  //! Computes the scalar product of \p c and \p g and assigns it to \p z.
  static void assign(Coefficient& z, const Constraint& c, const Generator& g);
  //! Computes the scalar product of \p g and \p c and assigns it to \p z.
  static void assign(Coefficient& z, const Generator& g, const Constraint& c);
  //! Computes the scalar product of \p c and \p g and assigns it to \p z.
  static void assign(Coefficient& z,
		     const Constraint& c, const Grid_Generator& g);
  //! Computes the scalar product of \p g and \p cg and assigns it to \p z.
  static void assign(Coefficient& z,
		     const Grid_Generator& g, const Congruence& cg);
  //! Computes the scalar product of \p cg and \p g and assigns it to \p z.
  static void assign(Coefficient& z,
		     const Congruence& cg, const Grid_Generator& g);

  //! Returns the sign of the scalar product between \p x and \p y.
  static int sign(const Linear_Row& x, const Linear_Row& y);
  //! Returns the sign of the scalar product between \p c and \p g.
  static int sign(const Constraint& c, const Generator& g);
  //! Returns the sign of the scalar product between \p g and \p c.
  static int sign(const Generator& g, const Constraint& c);
  //! Returns the sign of the scalar product between \p c and \p g.
  static int sign(const Constraint& c, const Grid_Generator& g);

  /*! \brief
    Computes the \e reduced scalar product of \p x and \p y,
    where the \f$\epsilon\f$ coefficient of \p x is ignored,
    and assigns the result to \p z.
  */
  static void reduced_assign(Coefficient& z,
			     const Linear_Row& x, const Linear_Row& y);
  /*! \brief
    Computes the \e reduced scalar product of \p c and \p g,
    where the \f$\epsilon\f$ coefficient of \p c is ignored,
    and assigns the result to \p z.
  */
  static void reduced_assign(Coefficient& z,
			     const Constraint& c, const Generator& g);
  /*! \brief
    Computes the \e reduced scalar product of \p g and \p c,
    where the \f$\epsilon\f$ coefficient of \p g is ignored,
    and assigns the result to \p z.
  */
  static void reduced_assign(Coefficient& z,
			     const Generator& g, const Constraint& c);
  //! \brief
  //! Computes the \e reduced scalar product of \p g and \p cg,
  //! where the \f$\epsilon\f$ coefficient of \p g is ignored,
  //! and assigns the result to \p z.
  static void reduced_assign(Coefficient& z,
			     const Grid_Generator& g, const Congruence& cg);

  /*! \brief
    Returns the sign of the \e reduced scalar product of \p x and \p y,
    where the \f$\epsilon\f$ coefficient of \p x is ignored.
  */
  static int reduced_sign(const Linear_Row& x, const Linear_Row& y);
  /*! \brief
    Returns the sign of the \e reduced scalar product of \p c and \p g,
    where the \f$\epsilon\f$ coefficient of \p c is ignored.
  */
  static int reduced_sign(const Constraint& c, const Generator& g);
  /*! \brief
    Returns the sign of the \e reduced scalar product of \p g and \p c,
    where the \f$\epsilon\f$ coefficient of \p g is ignored.
  */
  static int reduced_sign(const Generator& g, const Constraint& c);

  /*! \brief
    Computes the \e homogeneous scalar product of \p x and \p y,
    where the inhomogeneous terms are ignored,
    and assigns the result to \p z.
  */
  static void homogeneous_assign(Coefficient& z,
				 const Linear_Row& x, const Linear_Row& y);
  /*! \brief
    Computes the \e homogeneous scalar product of \p e and \p g,
    where the inhomogeneous terms are ignored,
    and assigns the result to \p z.
  */
  static void homogeneous_assign(Coefficient& z,
				 const Linear_Expression& e,
				 const Generator& g);
  //! \brief
  //! Computes the \e homogeneous scalar product of \p g and \p c,
  //! where the inhomogeneous terms are ignored,
  //! and assigns the result to \p z.
  static void homogeneous_assign(Coefficient& z,
				 const Grid_Generator& g, const Constraint& c);
  //! \brief
  //! Computes the \e homogeneous scalar product of \p g and \p cg,
  //! where the inhomogeneous terms are ignored,
  //! and assigns the result to \p z.
  static void homogeneous_assign(Coefficient& z,
				 const Grid_Generator& g, const Congruence& cg);
  //! \brief
  //! Computes the \e homogeneous scalar product of \p e and \p g,
  //! where the inhomogeneous terms are ignored,
  //! and assigns the result to \p z.
  static void homogeneous_assign(Coefficient& z,
				 const Linear_Expression& e,
				 const Grid_Generator& g);

  /*! \brief
    Returns the sign of the \e homogeneous scalar product of \p x and \p y,
    where the inhomogeneous terms are ignored.
  */
  static int homogeneous_sign(const Linear_Row& x, const Linear_Row& y);
  /*! \brief
    Returns the sign of the \e homogeneous scalar product of \p e and \p g,
    where the inhomogeneous terms are ignored.
  */
  static int homogeneous_sign(const Linear_Expression& e, const Generator& g);
  //! \brief
  //! Returns the sign of the \e homogeneous scalar product of \p e and \p g,
  //! where the inhomogeneous terms are ignored,
  static int homogeneous_sign(const Linear_Expression& e,
			      const Grid_Generator& g);
  //! \brief
  //! Returns the sign of the \e homogeneous scalar product of \p g and \p c,
  //! where the inhomogeneous terms are ignored,
  static int homogeneous_sign(const Grid_Generator& g, const Constraint& c);
};

#ifdef PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
//! Scalar product sign function object depending on topology.
/*! \ingroup PPL_CXX_interface */
#endif // defined(PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS)
class Parma_Polyhedra_Library::Topology_Adjusted_Scalar_Product_Sign {
public:
  //! Constructs the function object according to the topology of \p c.
  Topology_Adjusted_Scalar_Product_Sign(const Constraint& c);
  //! Constructs the function object according to the topology of \p g.
  Topology_Adjusted_Scalar_Product_Sign(const Generator& g);

  //! Computes the (topology adjusted) scalar product sign of \p c and \p g.
  int operator()(const Constraint&, const Generator&) const;
  //! Computes the (topology adjusted) scalar product sign of \p g and \p c.
  int operator()(const Generator&, const Constraint&) const;

private:
  //! The type of the scalar product sign function pointer.
  typedef int (*SPS_type)(const Linear_Row&, const Linear_Row&);

  //! The scalar product sign function pointer.
  SPS_type sps_fp;
};

#include "Scalar_Products.inlines.hh"

#endif // !defined(PPL_Scalar_Products_defs_hh)
