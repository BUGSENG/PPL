/* Any_Pointset class declaration.
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

#ifndef PPL_Any_Pointset_defs_hh
#define PPL_Any_Pointset_defs_hh 1

#include "Any_Pointset_types.hh"
#include "C_Polyhedron_defs.hh"
#include "NNC_Polyhedron_defs.hh"
#include "Grid_defs.hh"
#include "Box_defs.hh"
#include "BD_Shape_defs.hh"
#include "Octagonal_Shape_defs.hh"

namespace Parma_Polyhedra_Library {

namespace IO_Operators {

//! Output operator.
/*!
  \relates Parma_Polyhedra_Library::Any_Pointset
  Writes a textual representation of \p ph on \p s:
  <CODE>false</CODE> is written if \p ph is an empty pointset;
  <CODE>true</CODE> is written if \p ph is a universe pointset;
  a minimized system of constraints and congruences \p ph is written otherwise,
  all constraints in one row separated by ", ".
*/
std::ostream&
operator<<(std::ostream& s, const Any_Pointset& ph);

} // namespace IO_Operators

//! Swaps \p x with \p y.
/*! \relates Any_Pointset */
void swap(Any_Pointset& x, Any_Pointset& y);

/*! \brief
  Returns <CODE>true</CODE> if and only if
  \p x and \p y are the same pointset.

  \relates Any_Pointset
  Note that \p x and \p y may be topology- and/or dimension-incompatible
  pointsets: in those cases, the value <CODE>false</CODE> is returned.
*/
bool operator==(const Any_Pointset& x, const Any_Pointset& y);

/*! \brief
  Returns <CODE>true</CODE> if and only if
  \p x and \p y are different pointsets.

  \relates Any_Pointset
  Note that \p x and \p y may be topology- and/or dimension-incompatible
  pointsets: in those cases, the value <CODE>true</CODE> is returned.
*/
bool operator!=(const Any_Pointset& x, const Any_Pointset& y);

} // namespace Parma_Polyhedra_Library


//! Any PPL pointset.
/*! \ingroup PPL_CXX_interface */
class Parma_Polyhedra_Library::Any_Pointset {
public:
#if 0
  //! Returns the maximum space dimension all pointsets can handle.
  static dimension_type max_space_dimension();
#endif

  //! Default constructor.
  Any_Pointset();

  //! Ordinary copy constructor.
  Any_Pointset(const Any_Pointset& y);

  //! \name Member Functions that Do Not Modify the Pointset
  //@{

  //! Returns the dimension of the vector space enclosing \p *this.
  virtual dimension_type space_dimension() const = 0;

  /*! \brief
    Returns \f$0\f$, if \p *this is empty; otherwise, returns the
    \ref Affine_Independence_and_Affine_Dimension "affine dimension"
    of \p *this.
  */
  virtual dimension_type affine_dimension() const = 0;

  //! Returns a system of constraints that \p *this satisfies.
  virtual Constraint_System constraints() const = 0;

#if 0
  //! Returns a system of congruences that \p *this satisfies.
  virtual Congruence_System congruences() const = 0;
#endif

  /*! \brief
    Returns the relations holding between the pointset \p *this
    and the constraint \p c.

    \exception std::invalid_argument
    Thrown if \p *this and constraint \p c are dimension-incompatible.
  */
  Poly_Con_Relation relation_with(const Constraint& c) const;

  /*! \brief
    Returns the relations holding between the pointset \p *this
    and the generator \p g.

    \exception std::invalid_argument
    Thrown if \p *this and generator \p g are dimension-incompatible.
  */
  Poly_Gen_Relation relation_with(const Generator& g) const;

  /*! \brief
    Returns <CODE>true</CODE> if and only if \p *this is
    an empty pointset.
  */
  virtual bool is_empty() const = 0;

  /*! \brief
    Returns <CODE>true</CODE> if and only if \p *this
    is a universe pointset.
  */
  virtual bool is_universe() const = 0;

  /*! \brief
    Returns <CODE>true</CODE> if and only if \p *this
    is a topologically closed subset of the vector space.
  */
  virtual bool is_topologically_closed() const = 0;

  //! Returns <CODE>true</CODE> if and only if \p *this is discrete.
  virtual bool is_discrete() const = 0;

  /*! \brief
    Returns <CODE>true</CODE> if and only if \p *this
    is a bounded pointset.
  */
  virtual bool is_bounded() const = 0;

  /*! \brief
    Returns <CODE>true</CODE> if and only if \p *this
    contains at least one integer point.
  */
  virtual bool contains_integer_point() const = 0;

  /*! \brief
    Returns <CODE>true</CODE> if and only if \p expr is
    bounded from above in \p *this.

    \exception std::invalid_argument
    Thrown if \p expr and \p *this are dimension-incompatible.
  */
  virtual bool bounds_from_above(const Linear_Expression& expr) const = 0;

  /*! \brief
    Returns <CODE>true</CODE> if and only if \p expr is
    bounded from below in \p *this.

    \exception std::invalid_argument
    Thrown if \p expr and \p *this are dimension-incompatible.
  */
  virtual bool bounds_from_below(const Linear_Expression& expr) const = 0;

  /*! \brief
    Returns <CODE>true</CODE> if and only if \p *this is not empty
    and \p expr is bounded from above in \p *this, in which case
    the supremum value is computed.

    \param expr
    The linear expression to be maximized subject to \p *this;

    \param sup_n
    The numerator of the supremum value;

    \param sup_d
    The denominator of the supremum value;

    \param maximum
    <CODE>true</CODE> if and only if the supremum is also the maximum value.

    \exception std::invalid_argument
    Thrown if \p expr and \p *this are dimension-incompatible.

    If \p *this is empty or \p expr is not bounded from above,
    <CODE>false</CODE> is returned and \p sup_n, \p sup_d
    and \p maximum are left untouched.
  */
  virtual bool
  maximize(const Linear_Expression& expr,
           Coefficient& sup_n, Coefficient& sup_d, bool& maximum) const = 0;

  /*! \brief
    Returns <CODE>true</CODE> if and only if \p *this is not empty
    and \p expr is bounded from above in \p *this, in which case
    the supremum value and a point where \p expr reaches it are computed.

    \param expr
    The linear expression to be maximized subject to \p *this;

    \param sup_n
    The numerator of the supremum value;

    \param sup_d
    The denominator of the supremum value;

    \param maximum
    <CODE>true</CODE> if and only if the supremum is also the maximum value;

    \param point
    When maximization succeeds, will be assigned the point or
    closure point where \p expr reaches its supremum value.

    \exception std::invalid_argument
    Thrown if \p expr and \p *this are dimension-incompatible.

    If \p *this is empty or \p expr is not bounded from above,
    <CODE>false</CODE> is returned and \p sup_n, \p sup_d, \p maximum
    and \p point are left untouched.
  */
  virtual bool maximize(const Linear_Expression& expr,
                        Coefficient& sup_n, Coefficient& sup_d, bool& maximum,
                        Generator& point) const = 0;

  /*! \brief
    Returns <CODE>true</CODE> if and only if \p *this is not empty
    and \p expr is bounded from below in \p *this, in which case
    the infimum value is computed.

    \param expr
    The linear expression to be minimized subject to \p *this;

    \param inf_n
    The numerator of the infimum value;

    \param inf_d
    The denominator of the infimum value;

    \param minimum
    <CODE>true</CODE> if and only if the infimum is also the minimum value.

    \exception std::invalid_argument
    Thrown if \p expr and \p *this are dimension-incompatible.

    If \p *this is empty or \p expr is not bounded from below,
    <CODE>false</CODE> is returned and \p inf_n, \p inf_d
    and \p minimum are left untouched.
  */
  virtual bool
  minimize(const Linear_Expression& expr,
           Coefficient& inf_n, Coefficient& inf_d, bool& minimum) const = 0;

  /*! \brief
    Returns <CODE>true</CODE> if and only if \p *this is not empty
    and \p expr is bounded from below in \p *this, in which case
    the infimum value and a point where \p expr reaches it are computed.

    \param expr
    The linear expression to be minimized subject to \p *this;

    \param inf_n
    The numerator of the infimum value;

    \param inf_d
    The denominator of the infimum value;

    \param minimum
    <CODE>true</CODE> if and only if the infimum is also the minimum value;

    \param point
    When minimization succeeds, will be assigned a point or
    closure point where \p expr reaches its infimum value.

    \exception std::invalid_argument
    Thrown if \p expr and \p *this are dimension-incompatible.

    If \p *this is empty or \p expr is not bounded from below,
    <CODE>false</CODE> is returned and \p inf_n, \p inf_d, \p minimum
    and \p point are left untouched.
  */
  virtual bool minimize(const Linear_Expression& expr,
                        Coefficient& inf_n, Coefficient& inf_d, bool& minimum,
                        Generator& point) const = 0;

  //! Returns <CODE>true</CODE> if and only if \p *this contains \p y.
  /*!
    \exception std::invalid_argument
    Thrown if \p *this and \p y are topology-incompatible or
    dimension-incompatible.
  */
  virtual bool contains(const Any_Pointset& y) const = 0;

  //! Returns <CODE>true</CODE> if and only if \p *this strictly contains \p y.
  /*!
    \exception std::invalid_argument
    Thrown if \p *this and \p y are topology-incompatible or
    dimension-incompatible.
  */
  virtual bool strictly_contains(const Any_Pointset& y) const = 0;

  //! Returns <CODE>true</CODE> if and only if \p *this and \p y are disjoint.
  /*!
    \exception std::invalid_argument
    Thrown if \p x and \p y are topology-incompatible or
    dimension-incompatible.
  */
  virtual bool is_disjoint_from(const Any_Pointset& y) const = 0;

  //! Checks if all the invariants are satisfied.
  virtual bool OK() const = 0;

  //@} // Member Functions that Do Not Modify the Any_Pointset

  //! \name Space Dimension Preserving Member Functions that May Modify the Any_Pointset
  //@{

  /*! \brief
    Adds a copy of constraint \p c to the system of constraints
    of \p *this (without minimizing the result).

    \exception std::invalid_argument
    Thrown if \p *this and constraint \p c are topology-incompatible
    or dimension-incompatible.
  */
  virtual void add_constraint(const Constraint& c) = 0;

#if 0
  /*! \brief
    Adds a copy of generator \p g to the system of generators
    of \p *this (without minimizing the result).

    \exception std::invalid_argument
    Thrown if \p *this and generator \p g are topology-incompatible or
    dimension-incompatible, or if \p *this is an empty pointset and
    \p g is not a point.
  */
  virtual void add_generator(const Generator& g) = 0;

  //! Domain compatibility method.
  virtual void add_grid_generator(const Grid_Generator& g) const = 0;

  /*! \brief
    Adds a copy of congruence \p cg to the system of congruences of \p
    *this (without minimizing the result).

    \exception std::invalid_argument
    Thrown if \p *this and congruence \p cg are topology-incompatible
    or dimension-incompatible.
  */
  virtual void add_congruence(const Congruence& cg) = 0;
#endif

  /*! \brief
    Adds a copy of the constraints in \p cs to the system
    of constraints of \p *this (without minimizing the result).

    \param cs
    Contains the constraints that will be added to the system of
    constraints of \p *this.

    \exception std::invalid_argument
    Thrown if \p *this and \p cs are topology-incompatible or
    dimension-incompatible.
  */
  virtual void add_constraints(const Constraint_System& cs) = 0;

#if 0
  /*! \brief
    Adds a copy of the generators in \p gs to the system
    of generators of \p *this (without minimizing the result).

    \param gs
    Contains the generators that will be added to the system of
    generators of \p *this.

    \exception std::invalid_argument
    Thrown if \p *this and \p gs are topology-incompatible or
    dimension-incompatible, or if \p *this is empty and the system of
    generators \p gs is not empty, but has no points.
  */
  virtual void add_generators(const Generator_System& gs) = 0;

  /*! \brief
    Adds to \p *this constraints equivalent to the congruences in \p
    cgs (without minimizing the result).

    \param cgs
    Contains the congruences that will be added to the system of
    constraints of \p *this.

    \exception std::invalid_argument
    Thrown if \p *this and \p cgs are topology-incompatible or
    dimension-incompatible.
  */
  virtual void add_congruences(const Congruence_System& cgs) = 0;
#endif

  /*! \brief
    Computes the \ref Cylindrification "cylindrification" of \p *this with
    respect to space dimension \p var, assigning the result to \p *this.

    \param var
    The space dimension that will be unconstrained.

    \exception std::invalid_argument
    Thrown if \p var is not a space dimension of \p *this.
  */
  virtual void unconstrain(Variable var) = 0;

  /*! \brief
    Computes the \ref Cylindrification "cylindrification" of \p *this with
    respect to the set of space dimensions \p vars,
    assigning the result to \p *this.

    \param vars
    The set of space dimension that will be unconstrained.

    \exception std::invalid_argument
    Thrown if \p *this is dimension-incompatible with one of the
    Variable objects contained in \p vars.
  */
  virtual void unconstrain(const Variables_Set& vars) = 0;

  /*! \brief
    Assigns to \p *this the intersection of \p *this and \p y.
    The result is not guaranteed to be minimized.

    \exception std::invalid_argument
    Thrown if \p *this and \p y are topology-incompatible or
    dimension-incompatible.
  */
  virtual void intersection_assign(const Any_Pointset& y) = 0;


  /*! \brief
    Assigns to \p *this the smallest pointset, in the class of
    \p *this and \p y, that contains both \p *this and \p y.

    \exception std::invalid_argument
    Thrown if \p *this and \p y are topology-incompatible or
    dimension-incompatible.
  */
  virtual void upper_bound_assign(const Any_Pointset& y) = 0;

  /*! \brief
    Assigns to \p *this the smallest pointset, in the class of \p
    *this and \p y, that contains the set-theoretic difference of \p
    *this and \p y.

    \exception std::invalid_argument
    Thrown if \p *this and \p y are topology-incompatible or
    dimension-incompatible.
  */
  virtual void difference_assign(const Any_Pointset& y) = 0;

  /*! \brief
    Assigns to \p *this the
    \ref Single_Update_Affine_Functions "affine image"
    of \p *this under the function mapping variable \p var to the
    affine expression specified by \p expr and \p denominator.

    \param var
    The variable to which the affine expression is assigned;

    \param expr
    The numerator of the affine expression;

    \param denominator
    The denominator of the affine expression (optional argument with
    default value 1).

    \exception std::invalid_argument
    Thrown if \p denominator is zero or if \p expr and \p *this are
    dimension-incompatible or if \p var is not a space dimension of
    \p *this.

    \if Include_Implementation_Details

    When considering the generators of a pointset, the
    affine transformation
    \f[
      \frac{\sum_{i=0}^{n-1} a_i x_i + b}{\mathrm{denominator}}
    \f]
    is assigned to \p var where \p expr is
    \f$\sum_{i=0}^{n-1} a_i x_i + b\f$
    (\f$b\f$ is the inhomogeneous term).

    If constraints are up-to-date, it uses the specialized function
    affine_preimage() (for the system of constraints)
    and inverse transformation to reach the same result.
    To obtain the inverse transformation we use the following observation.

    Observation:
    -# The affine transformation is invertible if the coefficient
       of \p var in this transformation (i.e., \f$a_\mathrm{var}\f$)
       is different from zero.
    -# If the transformation is invertible, then we can write
       \f[
         \mathrm{denominator} * {x'}_\mathrm{var}
           = \sum_{i = 0}^{n - 1} a_i x_i + b
           = a_\mathrm{var} x_\mathrm{var}
             + \sum_{i \neq var} a_i x_i + b,
       \f]
       so that the inverse transformation is
       \f[
         a_\mathrm{var} x_\mathrm{var}
           = \mathrm{denominator} * {x'}_\mathrm{var}
             - \sum_{i \neq j} a_i x_i - b.
       \f]

    Then, if the transformation is invertible, all the entities that
    were up-to-date remain up-to-date. Otherwise only generators remain
    up-to-date.

    In other words, if \f$R\f$ is a \f$m_1 \times n\f$ matrix representing
    the rays of the pointset, \f$V\f$ is a \f$m_2 \times n\f$
    matrix representing the points of the pointset and
    \f[
      P = \bigl\{\,
            \vect{x} = (x_0, \ldots, x_{n-1})^\mathrm{T}
          \bigm|
            \vect{x} = \vect{\lambda} R + \vect{\mu} V,
            \vect{\lambda} \in \Rset^{m_1}_+,
            \vect{\mu} \in \Rset^{m_2}_+,
            \sum_{i = 0}^{m_2 - 1} \mu_i = 1
          \,\bigr\}
    \f]
    and \f$T\f$ is the affine transformation to apply to \f$P\f$, then
    the resulting pointset is
    \f[
      P' = \bigl\{\,
             (x_0, \ldots, T(x_0, \ldots, x_{n-1}),
                     \ldots, x_{n-1})^\mathrm{T}
           \bigm|
             (x_0, \ldots, x_{n-1})^\mathrm{T} \in P
           \,\bigr\}.
    \f]

    Affine transformations are, for example:
    - translations
    - rotations
    - symmetries.
    \endif
  */
  virtual void affine_image(Variable var,
                            const Linear_Expression& expr,
                            Coefficient_traits::const_reference denominator
                            = Coefficient_one()) = 0;

  /*! \brief
    Assigns to \p *this the
    \ref Single_Update_Affine_Functions "affine preimage"
    of \p *this under the function mapping variable \p var to the
    affine expression specified by \p expr and \p denominator.

    \param var
    The variable to which the affine expression is substituted;

    \param expr
    The numerator of the affine expression;

    \param denominator
    The denominator of the affine expression (optional argument with
    default value 1).

    \exception std::invalid_argument
    Thrown if \p denominator is zero or if \p expr and \p *this are
    dimension-incompatible or if \p var is not a space dimension of \p *this.

    \if Include_Implementation_Details

    When considering constraints of a pointset, the affine transformation
    \f[
      \frac{\sum_{i=0}^{n-1} a_i x_i + b}{denominator},
    \f]
    is assigned to \p var where \p expr is
    \f$\sum_{i=0}^{n-1} a_i x_i + b\f$
    (\f$b\f$ is the inhomogeneous term).

    If generators are up-to-date, then the specialized function
    affine_image() is used (for the system of generators)
    and inverse transformation to reach the same result.
    To obtain the inverse transformation, we use the following observation.

    Observation:
    -# The affine transformation is invertible if the coefficient
       of \p var in this transformation (i.e. \f$a_\mathrm{var}\f$)
       is different from zero.
    -# If the transformation is invertible, then we can write
       \f[
         \mathrm{denominator} * {x'}_\mathrm{var}
           = \sum_{i = 0}^{n - 1} a_i x_i + b
           = a_\mathrm{var} x_\mathrm{var}
               + \sum_{i \neq \mathrm{var}} a_i x_i + b,
       \f],
       the inverse transformation is
       \f[
         a_\mathrm{var} x_\mathrm{var}
           = \mathrm{denominator} * {x'}_\mathrm{var}
               - \sum_{i \neq j} a_i x_i - b.
       \f].

    Then, if the transformation is invertible, all the entities that
    were up-to-date remain up-to-date. Otherwise only constraints remain
    up-to-date.

    In other words, if \f$A\f$ is a \f$m \times n\f$ matrix representing
    the constraints of the pointset, \f$T\f$ is the affine transformation
    to apply to \f$P\f$ and
    \f[
      P = \bigl\{\,
            \vect{x} = (x_0, \ldots, x_{n-1})^\mathrm{T}
          \bigm|
            A\vect{x} \geq \vect{0}
          \,\bigr\}.
    \f]
    The resulting pointset is
    \f[
      P' = \bigl\{\,
             \vect{x} = (x_0, \ldots, x_{n-1}))^\mathrm{T}
           \bigm|
             A'\vect{x} \geq \vect{0}
           \,\bigr\},
    \f]
    where \f$A'\f$ is defined as follows:
    \f[
      {a'}_{ij}
        = \begin{cases}
            a_{ij} * \mathrm{denominator} + a_{i\mathrm{var}}*\mathrm{expr}[j]
              \quad \mathrm{for } j \neq \mathrm{var}; \\
            \mathrm{expr}[\mathrm{var}] * a_{i\mathrm{var}},
              \quad \text{for } j = \mathrm{var}.
          \end{cases}
    \f]
    \endif
  */
  virtual void affine_preimage(Variable var,
                               const Linear_Expression& expr,
                               Coefficient_traits::const_reference denominator
                               = Coefficient_one()) = 0;

  /*! \brief
    Assigns to \p *this the image of \p *this with respect to the
    \ref Generalized_Affine_Relations "generalized affine relation"
    \f$\mathrm{var}' \relsym \frac{\mathrm{expr}}{\mathrm{denominator}}\f$,
    where \f$\mathord{\relsym}\f$ is the relation symbol encoded
    by \p relsym.

    \param var
    The left hand side variable of the generalized affine relation;

    \param relsym
    The relation symbol;

    \param expr
    The numerator of the right hand side affine expression;

    \param denominator
    The denominator of the right hand side affine expression (optional
    argument with default value 1).

    \exception std::invalid_argument
    Thrown if \p denominator is zero or if \p expr and \p *this are
    dimension-incompatible or if \p var is not a space dimension of \p *this
    or if \p *this is a Any_Pointset and \p relsym is a strict
    relation symbol.
  */
  virtual
  void generalized_affine_image(Variable var,
                                Relation_Symbol relsym,
                                const Linear_Expression& expr,
                                Coefficient_traits::const_reference denominator
                                = Coefficient_one()) = 0;

  /*! \brief
    Assigns to \p *this the preimage of \p *this with respect to the
    \ref Generalized_Affine_Relations "generalized affine relation"
    \f$\mathrm{var}' \relsym \frac{\mathrm{expr}}{\mathrm{denominator}}\f$,
    where \f$\mathord{\relsym}\f$ is the relation symbol encoded
    by \p relsym.

    \param var
    The left hand side variable of the generalized affine relation;

    \param relsym
    The relation symbol;

    \param expr
    The numerator of the right hand side affine expression;

    \param denominator
    The denominator of the right hand side affine expression (optional
    argument with default value 1).

    \exception std::invalid_argument
    Thrown if \p denominator is zero or if \p expr and \p *this are
    dimension-incompatible or if \p var is not a space dimension of \p *this
    or if \p *this is a C_Any_Pointset and \p relsym is a strict
    relation symbol.
  */
  virtual void
  generalized_affine_preimage(Variable var,
                              Relation_Symbol relsym,
                              const Linear_Expression& expr,
                              Coefficient_traits::const_reference denominator
                              = Coefficient_one()) = 0;

  /*! \brief
    Assigns to \p *this the image of \p *this with respect to the
    \ref Generalized_Affine_Relations "generalized affine relation"
    \f$\mathrm{lhs}' \relsym \mathrm{rhs}\f$, where
    \f$\mathord{\relsym}\f$ is the relation symbol encoded by \p relsym.

    \param lhs
    The left hand side affine expression;

    \param relsym
    The relation symbol;

    \param rhs
    The right hand side affine expression.

    \exception std::invalid_argument
    Thrown if \p *this is dimension-incompatible with \p lhs or \p rhs
    or if \p *this is a C_Any_Pointset and \p relsym is a strict
    relation symbol.
  */
  virtual void generalized_affine_image(const Linear_Expression& lhs,
                                        Relation_Symbol relsym,
                                        const Linear_Expression& rhs) = 0;

  /*! \brief
    Assigns to \p *this the preimage of \p *this with respect to the
    \ref Generalized_Affine_Relations "generalized affine relation"
    \f$\mathrm{lhs}' \relsym \mathrm{rhs}\f$, where
    \f$\mathord{\relsym}\f$ is the relation symbol encoded by \p relsym.

    \param lhs
    The left hand side affine expression;

    \param relsym
    The relation symbol;

    \param rhs
    The right hand side affine expression.

    \exception std::invalid_argument
    Thrown if \p *this is dimension-incompatible with \p lhs or \p rhs
    or if \p *this is a C_Any_Pointset and \p relsym is a strict
    relation symbol.
  */
  virtual void generalized_affine_preimage(const Linear_Expression& lhs,
                                           Relation_Symbol relsym,
                                           const Linear_Expression& rhs) = 0;

  /*!
    \brief
    Assigns to \p *this the image of \p *this with respect to the
    \ref Single_Update_Bounded_Affine_Relations "bounded affine relation"
    \f$\frac{\mathrm{lb\_expr}}{\mathrm{denominator}}
         \leq \mathrm{var}'
           \leq \frac{\mathrm{ub\_expr}}{\mathrm{denominator}}\f$.

    \param var
    The variable updated by the affine relation;

    \param lb_expr
    The numerator of the lower bounding affine expression;

    \param ub_expr
    The numerator of the upper bounding affine expression;

    \param denominator
    The (common) denominator for the lower and upper bounding
    affine expressions (optional argument with default value 1).

    \exception std::invalid_argument
    Thrown if \p denominator is zero or if \p lb_expr (resp., \p ub_expr)
    and \p *this are dimension-incompatible or if \p var is not a space
    dimension of \p *this.
  */
  virtual
  void bounded_affine_image(Variable var,
                            const Linear_Expression& lb_expr,
                            const Linear_Expression& ub_expr,
                            Coefficient_traits::const_reference denominator
                            = Coefficient_one()) = 0;

  /*!
    \brief
    Assigns to \p *this the preimage of \p *this with respect to the
    \ref Single_Update_Bounded_Affine_Relations "bounded affine relation"
    \f$\frac{\mathrm{lb\_expr}}{\mathrm{denominator}}
         \leq \mathrm{var}'
           \leq \frac{\mathrm{ub\_expr}}{\mathrm{denominator}}\f$.

    \param var
    The variable updated by the affine relation;

    \param lb_expr
    The numerator of the lower bounding affine expression;

    \param ub_expr
    The numerator of the upper bounding affine expression;

    \param denominator
    The (common) denominator for the lower and upper bounding
    affine expressions (optional argument with default value 1).

    \exception std::invalid_argument
    Thrown if \p denominator is zero or if \p lb_expr (resp., \p ub_expr)
    and \p *this are dimension-incompatible or if \p var is not a space
    dimension of \p *this.
  */
  virtual
  void bounded_affine_preimage(Variable var,
                               const Linear_Expression& lb_expr,
                               const Linear_Expression& ub_expr,
                               Coefficient_traits::const_reference denominator
                               = Coefficient_one()) = 0;

  /*! \brief
    Assigns to \p *this the result of computing the
    \ref Time_Elapse_Operator "time-elapse" between \p *this and \p y.

    \exception std::invalid_argument
    Thrown if \p *this and \p y are topology-incompatible or
    dimension-incompatible.
  */
  virtual void time_elapse_assign(const Any_Pointset& y) = 0;

  //! Assigns to \p *this its topological closure.
  void topological_closure_assign();

#if 0
  //! Same as H79_widening_assign(y, tp).
  void widening_assign(const Any_Pointset& y, unsigned* tp = nullptr);

  /*! \brief
    Improves the result of the \ref H79_widening "H79-widening"
    computation by also enforcing those constraints in \p cs that are
    satisfied by all the points of \p *this.

    \param y
    A pointset that <EM>must</EM> be contained in \p *this;

    \param cs
    The system of constraints used to improve the widened pointset;

    \param tp
    An optional pointer to an unsigned variable storing the number of
    available tokens (to be used when applying the
    \ref Widening_with_Tokens "widening with tokens" delay technique).

    \exception std::invalid_argument
    Thrown if \p *this, \p y and \p cs are topology-incompatible or
    dimension-incompatible.
  */
  void limited_H79_extrapolation_assign(const Any_Pointset& y,
                                        const Constraint_System& cs,
                                        unsigned* tp = nullptr);

  /*! \brief
    Improves the result of the \ref H79_widening "H79-widening"
    computation by also enforcing those constraints in \p cs that are
    satisfied by all the points of \p *this, plus all the constraints
    of the form \f$\pm x \leq r\f$ and \f$\pm x < r\f$, with
    \f$r \in \Qset\f$, that are satisfied by all the points of \p *this.

    \param y
    A pointset that <EM>must</EM> be contained in \p *this;

    \param cs
    The system of constraints used to improve the widened pointset;

    \param tp
    An optional pointer to an unsigned variable storing the number of
    available tokens (to be used when applying the
    \ref Widening_with_Tokens "widening with tokens" delay technique).

    \exception std::invalid_argument
    Thrown if \p *this, \p y and \p cs are topology-incompatible or
    dimension-incompatible.
  */
  void bounded_H79_extrapolation_assign(const Any_Pointset& y,
                                        const Constraint_System& cs,
                                        unsigned* tp = nullptr);
#endif

  //@} // Space Dimension Preserving Member Functions that May Modify [...]

  //! \name Member Functions that May Modify the Dimension of the Vector Space
  //@{

  /*! \brief
    Adds \p m new space dimensions and embeds the old pointset
    in the new vector space.

    \param m
    The number of dimensions to add.

    \exception std::length_error
    Thrown if adding \p m new space dimensions would cause the
    vector space to exceed dimension <CODE>max_space_dimension()</CODE>.

    The new space dimensions will be those having the highest indexes
    in the new pointset, which is characterized by a system
    of constraints in which the variables running through
    the new dimensions are not constrained.
    For instance, when starting from the pointset \f$\cP \sseq \Rset^2\f$
    and adding a third space dimension, the result will be the pointset
    \f[
      \bigl\{\,
        (x, y, z)^\transpose \in \Rset^3
      \bigm|
        (x, y)^\transpose \in \cP
      \,\bigr\}.
    \f]
  */
  virtual void add_space_dimensions_and_embed(dimension_type m) = 0;

  /*! \brief
    Adds \p m new space dimensions to the pointset
    and does not embed it in the new vector space.

    \param m
    The number of space dimensions to add.

    \exception std::length_error
    Thrown if adding \p m new space dimensions would cause the
    vector space to exceed dimension <CODE>max_space_dimension()</CODE>.

    The new space dimensions will be those having the highest indexes
    in the new pointset, which is characterized by a system
    of constraints in which the variables running through
    the new dimensions are all constrained to be equal to 0.
    For instance, when starting from the pointset \f$\cP \sseq \Rset^2\f$
    and adding a third space dimension, the result will be the pointset
    \f[
      \bigl\{\,
        (x, y, 0)^\transpose \in \Rset^3
      \bigm|
        (x, y)^\transpose \in \cP
      \,\bigr\}.
    \f]
  */
  virtual void add_space_dimensions_and_project(dimension_type m) = 0;

  /*! \brief
    Assigns to \p *this the \ref Concatenating_Polyhedra "concatenation"
    of \p *this and \p y, taken in this order.

    \exception std::invalid_argument
    Thrown if \p *this and \p y are topology-incompatible.

    \exception std::length_error
    Thrown if the concatenation would cause the vector space
    to exceed dimension <CODE>max_space_dimension()</CODE>.
  */
  virtual void concatenate_assign(const Any_Pointset& y) = 0;

  //! Removes all the specified dimensions from the vector space.
  /*!
    \param vars
    The set of Variable objects corresponding to the space dimensions
    to be removed.

    \exception std::invalid_argument
    Thrown if \p *this is dimension-incompatible with one of the
    Variable objects contained in \p vars.
  */
  virtual void remove_space_dimensions(const Variables_Set& vars) = 0;

  /*! \brief
    Removes the higher dimensions of the vector space so that
    the resulting space will have dimension \p new_dimension.

    \exception std::invalid_argument
    Thrown if \p new_dimensions is greater than the space dimension of
    \p *this.
  */
  virtual
  void remove_higher_space_dimensions(dimension_type new_dimension) = 0;

#if 0
  /*! \brief
    Remaps the dimensions of the vector space according to
    a \ref Mapping_the_Dimensions_of_the_Vector_Space "partial function".

    \param pfunc
    The partial function specifying the destiny of each space dimension.

    The template type parameter Partial_Function must provide
    the following methods.
    \code
      bool has_empty_codomain() const
    \endcode
    returns <CODE>true</CODE> if and only if the represented partial
    function has an empty codomain (i.e., it is always undefined).
    The <CODE>has_empty_codomain()</CODE> method will always be called
    before the methods below.  However, if
    <CODE>has_empty_codomain()</CODE> returns <CODE>true</CODE>, none
    of the functions below will be called.
    \code
      dimension_type max_in_codomain() const
    \endcode
    returns the maximum value that belongs to the codomain
    of the partial function.
    The <CODE>max_in_codomain()</CODE> method is called at most once.
    \code
      bool maps(dimension_type i, dimension_type& j) const
    \endcode
    Let \f$f\f$ be the represented function and \f$k\f$ be the value
    of \p i.  If \f$f\f$ is defined in \f$k\f$, then \f$f(k)\f$ is
    assigned to \p j and <CODE>true</CODE> is returned.
    If \f$f\f$ is undefined in \f$k\f$, then <CODE>false</CODE> is
    returned.
    This method is called at most \f$n\f$ times, where \f$n\f$ is the
    dimension of the vector space enclosing the pointset.

    The result is undefined if \p pfunc does not encode a partial
    function with the properties described in the
    \ref Mapping_the_Dimensions_of_the_Vector_Space
    "specification of the mapping operator".
  */
  template <typename Partial_Function>
  void map_space_dimensions(const Partial_Function& pfunc);
#endif

  //! Creates \p m copies of the space dimension corresponding to \p var.
  /*!
    \param var
    The variable corresponding to the space dimension to be replicated;

    \param m
    The number of replicas to be created.

    \exception std::invalid_argument
    Thrown if \p var does not correspond to a dimension of the vector space.

    \exception std::length_error
    Thrown if adding \p m new space dimensions would cause the
    vector space to exceed dimension <CODE>max_space_dimension()</CODE>.

    If \p *this has space dimension \f$n\f$, with \f$n > 0\f$,
    and <CODE>var</CODE> has space dimension \f$k \leq n\f$,
    then the \f$k\f$-th space dimension is
    \ref expand_space_dimension "expanded" to \p m new space dimensions
    \f$n\f$, \f$n+1\f$, \f$\dots\f$, \f$n+m-1\f$.
  */
  void expand_space_dimension(Variable var, dimension_type m);

  //! Folds the space dimensions in \p vars into \p dest.
  /*!
    \param vars
    The set of Variable objects corresponding to the space dimensions
    to be folded;

    \param dest
    The variable corresponding to the space dimension that is the
    destination of the folding operation.

    \exception std::invalid_argument
    Thrown if \p *this is dimension-incompatible with \p dest or with
    one of the Variable objects contained in \p vars.
    Also thrown if \p dest is contained in \p vars.

    If \p *this has space dimension \f$n\f$, with \f$n > 0\f$,
    <CODE>dest</CODE> has space dimension \f$k \leq n\f$,
    \p vars is a set of variables whose maximum space dimension
    is also less than or equal to \f$n\f$, and \p dest is not a member
    of \p vars, then the space dimensions corresponding to
    variables in \p vars are \ref fold_space_dimensions "folded"
    into the \f$k\f$-th space dimension.
  */
  void fold_space_dimensions(const Variables_Set& vars, Variable dest);

  //@} // Member Functions that May Modify the Dimension of the Vector Space

#if 0
  friend bool Parma_Polyhedra_Library::operator==(const Any_Pointset& x,
                                                  const Any_Pointset& y);
#endif

  //! \name Miscellaneous Member Functions
  //@{

  //! Destructor.
  virtual ~Any_Pointset();

  /*! \brief
    Swaps \p *this with pointset \p y.
    (\p *this and \p y can be dimension-incompatible.)

    \exception std::invalid_argument
    Thrown if \p x and \p y are topology-incompatible.
  */
  void m_swap(Any_Pointset& y);

  PPL_OUTPUT_DECLARATIONS

  /*! \brief
    Loads from \p s an ASCII representation (as produced by
    ascii_dump(std::ostream&) const) and sets \p *this accordingly.
    Returns <CODE>true</CODE> if successful, <CODE>false</CODE> otherwise.
  */
  bool ascii_load(std::istream& s);

  //! Returns the total size in bytes of the memory occupied by \p *this.
  virtual memory_size_type total_memory_in_bytes() const = 0;

  //! Returns the size in bytes of the memory managed by \p *this.
  virtual memory_size_type external_memory_in_bytes() const = 0;

  /*! \brief
    Returns a 32-bit hash code for \p *this.

    If \p x and \p y are such that <CODE>x == y</CODE>,
    then <CODE>x.hash_code() == y.hash_code()</CODE>.
  */
  virtual int32_t hash_code() const = 0;

  //@} // Miscellaneous Member Functions
};

#define PPL_ANY_POINTSET_WRAPPER_CLASS(TEMPLATE, WRAPPER_NAME, BASE_CLASS) \
TEMPLATE                                                                \
class WRAPPER_NAME : public Any_Pointset {                              \
 private:                                                               \
  BASE_CLASS x;                                                         \
 public:                                                                \
  explicit WRAPPER_NAME(const C_Polyhedron& y)                          \
    : x(y) {                                                            \
  }                                                                     \
  explicit WRAPPER_NAME(const NNC_Polyhedron& y)                        \
    : x(y) {                                                            \
  }                                                                     \
  template <typename U>                                                 \
  explicit WRAPPER_NAME(const Box<U>& y)                                \
    : x(y) {                                                            \
  }                                                                     \
  template <typename U>                                                 \
  explicit WRAPPER_NAME(const BD_Shape<U>& y)                           \
    : x(y) {                                                            \
  }                                                                     \
  template <typename U>                                                 \
  explicit WRAPPER_NAME(const Octagonal_Shape<U>& y)                    \
    : x(y) {                                                            \
  }                                                                     \
                                                                        \
  explicit WRAPPER_NAME(const Constraint_System& y)                     \
    : x(y) {                                                            \
  }                                                                     \
  explicit WRAPPER_NAME(const Congruence_System& y)                     \
    : x(y) {                                                            \
  }                                                                     \
                                                                        \
  ~WRAPPER_NAME() {                                                     \
  }                                                                     \
                                                                        \
  dimension_type space_dimension() const {                              \
    return x.space_dimension();                                         \
  }                                                                     \
  dimension_type affine_dimension() const {                             \
    return x.space_dimension();                                         \
  }                                                                     \
  Constraint_System constraints() const {                               \
    return x.constraints();                                             \
  }                                                                     \
  Congruence_System congruences() const {                               \
    return x.congruences();                                             \
  }                                                                     \
                                                                        \
  bool is_empty() const {                                               \
    return x.is_empty();                                                \
  }                                                                     \
  bool is_universe() const {                                            \
    return x.is_universe();                                             \
  }                                                                     \
  bool is_bounded() const {                                             \
    return x.is_bounded();                                              \
  }                                                                     \
  bool is_topologically_closed() const {                                \
    return x.is_topologically_closed();                                 \
  }                                                                     \
  bool is_discrete() const {                                            \
    return x.is_discrete();                                             \
  }                                                                     \
  bool contains_integer_point() const {                                 \
    return x.contains_integer_point();                                  \
  }                                                                     \
                                                                        \
  bool contains(const Any_Pointset& y) const {                          \
    return x.contains(dynamic_cast<const BASE_CLASS&>(y));              \
  }                                                                     \
  bool strictly_contains(const Any_Pointset& y) const {                 \
    return x.strictly_contains(dynamic_cast<const BASE_CLASS&>(y));     \
  }                                                                     \
  bool is_disjoint_from(const Any_Pointset& y) const {                  \
    return x.is_disjoint_from(dynamic_cast<const BASE_CLASS&>(y));      \
  }                                                                     \
                                                                        \
  void add_space_dimensions_and_embed(dimension_type m) {               \
    return x.add_space_dimensions_and_embed(m);                         \
  }                                                                     \
  void add_space_dimensions_and_project(dimension_type m) {             \
    return x.add_space_dimensions_and_project(m);                       \
  }                                                                     \
  void remove_space_dimensions(const Variables_Set& vars) {             \
    x.remove_space_dimensions(vars);                                    \
  }                                                                     \
  void remove_higher_space_dimensions(dimension_type new_dimension) {   \
    x.remove_higher_space_dimensions(new_dimension);                    \
  }                                                                     \
                                                                        \
  void add_constraint(const Constraint& c) {                            \
    x.add_constraint(c);                                                \
  }                                                                     \
                                                                        \
  void add_constraints(const Constraint_System& cs) {                   \
    x.add_constraints(cs);                                              \
  }                                                                     \
                                                                        \
  void intersection_assign(const Any_Pointset& y) {                     \
    x.intersection_assign(dynamic_cast<const BASE_CLASS&>(y));          \
  }                                                                     \
  void upper_bound_assign(const Any_Pointset& y) {                      \
    x.upper_bound_assign(dynamic_cast<const BASE_CLASS&>(y));           \
  }                                                                     \
  void difference_assign(const Any_Pointset& y) {                       \
    x.difference_assign(dynamic_cast<const BASE_CLASS&>(y));            \
  }                                                                     \
  void concatenate_assign(const Any_Pointset& y) {                      \
    x.concatenate_assign(dynamic_cast<const BASE_CLASS&>(y));           \
  }                                                                     \
  void time_elapse_assign(const Any_Pointset& y) {                      \
    x.time_elapse_assign(dynamic_cast<const BASE_CLASS&>(y));           \
  }                                                                     \
                                                                        \
  void affine_image(Variable var,                                       \
                    const Linear_Expression& expr,                      \
                    Coefficient_traits::const_reference denominator     \
                    = Coefficient_one()) {                              \
    x.affine_image(var, expr, denominator);                             \
  }                                                                     \
  void affine_preimage(Variable var,                                    \
                       const Linear_Expression& expr,                   \
                       Coefficient_traits::const_reference denominator  \
                       = Coefficient_one()) {                           \
    x.affine_image(var, expr, denominator);                             \
  }                                                                     \
  void generalized_affine_image(Variable var,                           \
                                Relation_Symbol relsym,                 \
                                const Linear_Expression& expr,          \
                                Coefficient_traits::const_reference     \
                                denominator = Coefficient_one()) {      \
    x.generalized_affine_image(var, relsym, expr, denominator);         \
  }                                                                     \
  void                                                                  \
  generalized_affine_preimage(Variable var,                             \
                              Relation_Symbol relsym,                   \
                              const Linear_Expression& expr,            \
                              Coefficient_traits::const_reference denominator \
                              = Coefficient_one()) {                    \
    x.generalized_affine_preimage(var, relsym, expr, denominator);      \
  }                                                                     \
  virtual void generalized_affine_image(const Linear_Expression& lhs,   \
                                        Relation_Symbol relsym,         \
                                        const Linear_Expression& rhs) { \
    x.generalized_affine_image(lhs, relsym, rhs);                       \
  }                                                                     \
  void generalized_affine_preimage(const Linear_Expression& lhs,        \
                                   Relation_Symbol relsym,              \
                                   const Linear_Expression& rhs) {      \
    x.generalized_affine_preimage(lhs, relsym, rhs);                    \
  }                                                                     \
                                                                        \
  void bounded_affine_image(Variable var,                               \
                            const Linear_Expression& lb_expr,           \
                            const Linear_Expression& ub_expr,           \
                            Coefficient_traits::const_reference         \
                            denominator = Coefficient_one()) {          \
    x.bounded_affine_image(var, lb_expr, ub_expr, denominator);         \
  }                                                                     \
  void bounded_affine_preimage(Variable var,                            \
                               const Linear_Expression& lb_expr,        \
                               const Linear_Expression& ub_expr,        \
                               Coefficient_traits::const_reference      \
                               denominator = Coefficient_one()) {       \
    x.bounded_affine_preimage(var, lb_expr, ub_expr, denominator);      \
  }                                                                     \
                                                                        \
  bool bounds_from_above(const Linear_Expression& expr) const {         \
    return x.bounds_from_above(expr);                                   \
  }                                                                     \
  bool bounds_from_below(const Linear_Expression& expr) const {         \
    return x.bounds_from_below(expr);                                   \
  }                                                                     \
  bool maximize(const Linear_Expression& expr,                          \
                Coefficient& sup_n, Coefficient& sup_d,                 \
                bool& maximum) const {                                  \
    return maximize(expr, sup_n, sup_d, maximum);                       \
  }                                                                     \
  bool maximize(const Linear_Expression& expr,                          \
                Coefficient& sup_n, Coefficient& sup_d,                 \
                bool& maximum, Generator& point) const {                \
    return maximize(expr, sup_n, sup_d, maximum, point);                \
  }                                                                     \
  bool minimize(const Linear_Expression& expr,                          \
                Coefficient& inf_n, Coefficient& inf_d,                 \
                bool& minimum) const {                                  \
    return minimize(expr, inf_n, inf_d, minimum);                       \
  }                                                                     \
  bool minimize(const Linear_Expression& expr,                          \
                Coefficient& inf_n, Coefficient& inf_d,                 \
                bool& minimum, Generator& point) const {                \
    return minimize(expr, inf_n, inf_d, minimum, point);                \
  }                                                                     \
                                                                        \
  memory_size_type total_memory_in_bytes() const {                      \
    return x.total_memory_in_bytes();                                   \
  }                                                                     \
  memory_size_type external_memory_in_bytes() const {                   \
    return x.total_memory_in_bytes();                                   \
  }                                                                     \
                                                                        \
  int32_t hash_code() const {                                           \
    return x.hash_code();                                               \
  }                                                                     \
                                                                        \
  bool OK() const {                                                     \
    return x.OK();                                                      \
  }                                                                     \
};

namespace Parma_Polyhedra_Library {

PPL_ANY_POINTSET_WRAPPER_CLASS(, C_Polyhedron_Pointset, C_Polyhedron)
PPL_ANY_POINTSET_WRAPPER_CLASS(, NNC_Polyhedron_Pointset, NNC_Polyhedron)
//PPL_ANY_POINTSET_WRAPPER_CLASS(, Grid_Pointset, Grid)

PPL_ANY_POINTSET_WRAPPER_CLASS(template <typename T>, Box_Pointset, Box<T>)
PPL_ANY_POINTSET_WRAPPER_CLASS(template <typename T>, BD_Shape_Pointset, BD_Shape<T>)
PPL_ANY_POINTSET_WRAPPER_CLASS(template <typename T>, Octagonal_Shape_Pointset, Octagonal_Shape<T>)

//C_Polyhedron_Pointset a(C_Polyhedron(3));
//Octagonal_Shape_Pointset<double> b(C_Polyhedron(3));
//BD_Shape_Pointset<double> c(C_Polyhedron(3));

} // namespace Parma_Polyhedra_Library

#include "Any_Pointset_inlines.hh"
//#include "Any_Pointset_templates.hh"

#endif // !defined(PPL_Any_Pointset_defs_hh)
