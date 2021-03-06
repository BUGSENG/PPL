/* Declarations for the Division_Floating_Point_Expression class and its
   constituents.
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

#ifndef PPL_Division_Floating_Point_Expression_defs_hh
#define PPL_Division_Floating_Point_Expression_defs_hh 1

#include "Floating_Point_Expression_defs.hh"
#include "globals_defs.hh"
#include "Division_Floating_Point_Expression_types.hh"
#include <map>

namespace Parma_Polyhedra_Library {

//! Swaps \p x with \p y.
/*! \relates Division_Floating_Point_Expression */
template <typename FP_Interval_Type, typename FP_Format>
void swap(Division_Floating_Point_Expression<FP_Interval_Type, FP_Format>& x,
          Division_Floating_Point_Expression<FP_Interval_Type, FP_Format>& y);

/*! \brief
  A generic Division Floating Point Expression.

  \ingroup PPL_CXX_interface

  \par Template type parameters

  - The class template type parameter \p FP_Interval_Type represents the type
  of the intervals used in the abstract domain.
  - The class template type parameter \p FP_Format represents the floating
  point format used in the concrete domain.

  \par Linearizationd of division floating-point expressions

  Let \f$i + \sum_{v \in \cV}i_{v}v \f$ and
  \f$i' + \sum_{v \in \cV}i'_{v}v \f$
  be two linear forms, \f$\aslf\f$ and \f$\adivlf\f$ two sound abstract
  operator on linear forms such that:
  \f[
  \left(i + \sum_{v \in \cV}i_{v}v\right)
  \aslf
  \left(i' + \sum_{v \in \cV}i'_{v}v\right)
  =
  \left(i \asifp i'\right)
  + \sum_{v \in \cV}\left(i_{v} \asifp i'_{v}\right)v,
  \f]
  \f[
  \left(i + \sum_{v \in \cV}i_{v}v\right)
  \adivlf
  i'
  =
  \left(i \adivifp i'\right)
  + \sum_{v \in \cV}\left(i_{v} \adivifp i'\right)v.
  \f]
  Given an expression \f$e_{1} \oslash [a, b]\f$ and a composite
  abstract store \f$\left \llbracket \rho^{\#}, \rho^{\#}_l \right
  \rrbracket\f$,
  we construct the interval linear form
  \f$
  \linexprenv{e_{1} \oslash [a, b]}{\rho^{\#}}{\rho^{\#}_l}
  \f$
  as follows:
  \f[
  \linexprenv{e_{1} \oslash [a, b]}{\rho^{\#}}{\rho^{\#}_l}
  =
  \left(\linexprenv{e_{1}}{\rho^{\#}}{\rho^{\#}_l}
  \adivlf
  [a, b]\right)
  \aslf
  \left(\varepsilon_{\mathbf{f}}\left(
  \linexprenv{e_{1}}{\rho^{\#}}{\rho^{\#}_l}
  \right)
  \adivlf
  [a, b]\right)
  \aslf
  mf_{\mathbf{f}}[-1, 1],
  \f]
  given an expression \f$e_{1} \oslash e_{2}\f$ and a composite
  abstract store \f$\left \llbracket \rho^{\#}, \rho^{\#}_l \right
  \rrbracket\f$, we construct the interval linear form
  \f$\linexprenv{e_{1} \oslash e_{2}}{\rho^{\#}}{\rho^{\#}_l}\f$
  as follows:
  \f[
  \linexprenv{e_{1} \oslash e_{2}}{\rho^{\#}}{\rho^{\#}_l}
  =
  \linexprenv{e_{1} \oslash \iota\left(
  \linexprenv{e_{2}}{\rho^{\#}}{\rho^{\#}_l}
  \right)\rho^{\#}}{\rho^{\#}}{\rho^{\#}_l},
  \f]
  where \f$\varepsilon_{\mathbf{f}}(l)\f$ is the linear form computed by
  calling method <CODE>Floating_Point_Expression::relative_error</CODE>
  on \f$l\f$, \f$\iota(l)\rho^{\#}\f$ is the linear form computed by calling
  method <CODE>Floating_Point_Expression::intervalize</CODE> on \f$l\f$
  and \f$\rho^{\#}\f$, and \f$mf_{\mathbf{f}}\f$ is a rounding error defined in
  <CODE>Floating_Point_Expression::absolute_error</CODE>.
*/
template <typename FP_Interval_Type, typename FP_Format>
class Division_Floating_Point_Expression
  : public Floating_Point_Expression<FP_Interval_Type, FP_Format> {

public:

  /*! \brief
     Alias for the Linear_Form<FP_Interval_Type> from
     Floating_Point_Expression
  */
  typedef typename
  Floating_Point_Expression<FP_Interval_Type, FP_Format>
  ::FP_Linear_Form FP_Linear_Form;

  /*! \brief
     Alias for the Box<FP_Interval_Type> from
     Floating_Point_Expression.
  */
  typedef typename
  Floating_Point_Expression<FP_Interval_Type, FP_Format>
  ::FP_Interval_Abstract_Store FP_Interval_Abstract_Store;

  /*! \brief
     Alias for the std::map<dimension_type, FP_Linear_Form> from
     Floating_Point_Expression.
  */
  typedef typename
  Floating_Point_Expression<FP_Interval_Type, FP_Format>::
  FP_Linear_Form_Abstract_Store FP_Linear_Form_Abstract_Store;

  /*! \brief
     Alias for the FP_Interval_Type::boundary_type from
     Floating_Point_Expression.
  */
  typedef typename
  Floating_Point_Expression<FP_Interval_Type, FP_Format>::boundary_type
  boundary_type;

  /*! \brief
     Alias for the FP_Interval_Type::info_type from Floating_Point_Expression.
  */
  typedef typename
  Floating_Point_Expression<FP_Interval_Type, FP_Format>::info_type info_type;

  //! \name Constructors and Destructor
  //@{
  /*! \brief
    Constructor with two parameters: builds the division floating point
    expression corresponding to \p num \f$\oslash\f$ \p den.
  */
  Division_Floating_Point_Expression(
           Floating_Point_Expression<FP_Interval_Type, FP_Format>* const num,
           Floating_Point_Expression<FP_Interval_Type, FP_Format>* const den);

  //! Destructor.
  ~Division_Floating_Point_Expression();

  //@} // Constructors and Destructor

  /*! \brief
    Linearizes the expression in a given astract store.

    Makes \p result become the linearization of \p *this in the given
    composite abstract store.

    \param int_store The interval abstract store.
    \param lf_store The linear form abstract store.
    \param result The modified linear form.

    \return <CODE>true</CODE> if the linearization succeeded,
    <CODE>false</CODE> otherwise.

    Note that all variables occuring in the expressions represented
    by \p first_operand and \p second_operand MUST have an associated value in
    \p int_store. If this precondition is not met, calling the method
    causes an undefined behavior.

    See the class description for a detailed explanation of how \p result
    is computed.
  */
  bool linearize(const FP_Interval_Abstract_Store& int_store,
                 const FP_Linear_Form_Abstract_Store& lf_store,
                 FP_Linear_Form& result) const;

  //! Swaps \p *this with \p y.
  void m_swap(Division_Floating_Point_Expression<FP_Interval_Type,
                                                 FP_Format>& y);

private:

  //! Pointer to the first operand.
  Floating_Point_Expression<FP_Interval_Type, FP_Format>* first_operand;
  //! Pointer to the second operand.
  Floating_Point_Expression<FP_Interval_Type, FP_Format>* second_operand;

  #ifdef PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
  /*! \brief
    Copy constructor: temporary inhibited.
  */
  #endif // PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
  Division_Floating_Point_Expression(
         const Division_Floating_Point_Expression<FP_Interval_Type,
                                                  FP_Format>& e);

  #ifdef PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
  /*! \brief
    Assignment operator: temporary inhibited.
  */
  #endif // PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
  Division_Floating_Point_Expression<FP_Interval_Type, FP_Format>&
  operator=(const Division_Floating_Point_Expression<FP_Interval_Type,
            FP_Format>& e);

}; // class Division_Floating_Point_Expression

} // namespace Parma_Polyhedra_Library

#include "Division_Floating_Point_Expression_inlines.hh"
#include "Division_Floating_Point_Expression_templates.hh"

#endif // !defined(PPL_Division_Floating_Point_Expression_defs_hh)
