/* Affine_Space class declaration.
   Copyright (C) 2001-2010 Roberto Bagnara <bagnara@cs.unipr.it>
   Copyright (C) 2010-2020 BUGSENG srl (http://bugseng.com)

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

#ifndef PPL_Affine_Space_defs_hh
#define PPL_Affine_Space_defs_hh 1

#include "Affine_Space_types.hh"
#include "Variable_defs.hh"
#include "Variables_Set_types.hh"
#include "Linear_Expression_types.hh"
#include "Constraint_types.hh"
#include "Constraint_System_defs.hh"
#include "Constraint_System_inlines.hh"
#include "Congruence_System_defs.hh"
#include "Congruence_System_inlines.hh"
#include "Poly_Con_Relation_defs.hh"
#include "Poly_Gen_Relation_defs.hh"
#include "Box_types.hh"
#include "Polyhedron_types.hh"
#include "BD_Shape_types.hh"
#include "Octagonal_Shape_types.hh"
#include "Grid_defs.hh"
#include <iosfwd>

namespace Parma_Polyhedra_Library {

namespace IO_Operators {

//! Output operator.
/*!
  \relates Parma_Polyhedra_Library::Affine_Space
  Writes a textual representation of \p a on \p s: <CODE>false</CODE>
  is written if \p a is an empty affine space; <CODE>true</CODE> is written
  if \p a is a universe affine space; a minimized system of equalitiy
  constraints defining \p a is written otherwise, all equalities in one row
  separated by ", "s.
*/
std::ostream&
operator<<(std::ostream& s, const Affine_Space& gr);

} // namespace IO_Operators

//! Swaps \p x with \p y.
/*! \relates Affine_Space */
void swap(Affine_Space& x, Affine_Space& y);

/*! \brief
  Returns <CODE>true</CODE> if and only if \p x and \p y are the same
  affine space.

  \relates Affine_Space
  Note that \p x and \p y may be dimension-incompatible affine spaces: in
  those cases, the value <CODE>false</CODE> is returned.
*/
bool operator==(const Affine_Space& x, const Affine_Space& y);

/*! \brief
  Returns <CODE>true</CODE> if and only if \p x and \p y are different
  affine spaces.

  \relates Affine_Space
  Note that \p x and \p y may be dimension-incompatible affine spaces: in
  those cases, the value <CODE>true</CODE> is returned.
*/
bool operator!=(const Affine_Space& x, const Affine_Space& y);

} // namespace Parma_Polyhedra_Library


//! An affine space.
/*! \ingroup PPL_CXX_interface
  An object of the class Affine_Space represents an affine space
  in the vector space \f$\Rset^n\f$.

  The domain of affine spaces linear equality constraints.
  Depending on the method, using a constraint that is not optimally
  supported by the domain will either raise an exception or
  result in a (possibly non-optimal) upward approximation.

  The domain of affine spaces support a concept of double description similar
  to the one developed for polyhedra: hence, an affine space can be specified
  as either a finite system of equalities or a finite system of
  generators  (see Section \ref representation)
  and it is always possible to obtain either representation.
  That is, if we know the system of equalities, we can obtain
  from this a system of generators that define the same affine space
  and vice versa.
  These systems can contain redundant members, or they can be in the
  minimal form.

  A key attribute of any affine space is its space dimension (the dimension
  \f$n \in \Nset\f$ of the enclosing vector space):

  - all affine spaces, the empty ones included, are endowed with a space
    dimension;
  - most operations working on an affine space and another object (another
    affine space, a congruence, a generator, a set of variables, etc.) will
    throw an exception if the affine space and the object are not
    dimension-compatible (see Section \ref representation);
  - the only ways in which the space dimension of an affine space can be
    changed are with <EM>explicit</EM> calls to operators provided for
    that purpose, and with standard copy, assignment and swap
    operators.

  Note that two different affine spaces can be defined on the zero-dimension
  space: the empty affine space and the universe affine space \f$R^0\f$.

  \par
  In all the examples it is assumed that variables
  <CODE>x</CODE> and <CODE>y</CODE> are defined (where they are
  used) as follows:
  \code
  Variable x(0);
  Variable y(1);
  \endcode

  FIXME: revise the contents below this point of this comment block.

  \par Example 1
  The following code builds an affine space corresponding to the even integer
  pairs in \f$\Rset^2\f$, given as a system of congruences:
  \code
  Congruence_System cgs;
  cgs.insert((x %= 0) / 2);
  cgs.insert((y %= 0) / 2);
  Affine_Space gr(cgs);
  \endcode
  The following code builds the same affine space as above, but starting
  from a system of generators specifying three of the points:
  \code
  Affine_Space_Generator_System gs;
  gs.insert(affine space_point(0*x + 0*y));
  gs.insert(affine space_point(0*x + 2*y));
  gs.insert(affine space_point(2*x + 0*y));
  Affine_Space gr(gs);
  \endcode

  \par Example 2
  The following code builds an affine space corresponding to a line in
  \f$\Rset^2\f$ by adding a single congruence to the universe affine space:
  \code
  Congruence_System cgs;
  cgs.insert(x - y == 0);
  Affine_Space gr(cgs);
  \endcode
  The following code builds the same affine space as above, but starting
  from a system of generators specifying a point and a line:
  \code
  Affine_Space_Generator_System gs;
  gs.insert(affine space_point(0*x + 0*y));
  gs.insert(affine space_line(x + y));
  Affine_Space gr(gs);
  \endcode

  \par Example 3
  The following code builds an affine space corresponding to the integral
  points on the line \f$x = y\f$ in \f$\Rset^2\f$ constructed
  by adding an equality and congruence to the universe affine space:
  \code
  Congruence_System cgs;
  cgs.insert(x - y == 0);
  cgs.insert(x %= 0);
  Affine_Space gr(cgs);
  \endcode
  The following code builds the same affine space as above, but starting
  from a system of generators specifying a point and a parameter:
  \code
  Affine_Space_Generator_System gs;
  gs.insert(affine space_point(0*x + 0*y));
  gs.insert(parameter(x + y));
  Affine_Space gr(gs);
  \endcode

  \par Example 4
  The following code builds the affine space corresponding to a plane by
  creating the universe affine space in \f$\Rset^2\f$:
  \code
  Affine_Space gr(2);
  \endcode
  The following code builds the same affine space as above, but starting
  from the empty affine space in \f$\Rset^2\f$ and inserting the appropriate
  generators (a point, and two lines).
  \code
  Affine_Space gr(2, EMPTY);
  gr.add_grid_generator(affine space_point(0*x + 0*y));
  gr.add_grid_generator(affine space_line(x));
  gr.add_grid_generator(affine space_line(y));
  \endcode
  Note that a generator system must contain a point when describing
  an affine space.  To ensure that this is always the case it is required
  that the first generator inserted in an empty affine space is a point
  (otherwise, an exception is thrown).

  \par Example 5
  The following code shows the use of the function
  <CODE>add_space_dimensions_and_embed</CODE>:
  \code
  Affine_Space gr(1);
  gr.add_congruence(x == 2);
  gr.add_space_dimensions_and_embed(1);
  \endcode
  We build the universe affine space in the 1-dimension space \f$\Rset\f$.
  Then we add a single equality congruence,
  thus obtaining the affine space corresponding to the singleton set
  \f$\{ 2 \} \sseq \Rset\f$.
  After the last line of code, the resulting affine space is
  \f[
  \bigl\{\,
  (2, y)^\transpose \in \Rset^2
  \bigm|
  y \in \Rset
  \,\bigr\}.
  \f]

  \par Example 6
  The following code shows the use of the function
  <CODE>add_space_dimensions_and_project</CODE>:
  \code
  Affine_Space gr(1);
  gr.add_congruence(x == 2);
  gr.add_space_dimensions_and_project(1);
  \endcode
  The first two lines of code are the same as in Example 4 for
  <CODE>add_space_dimensions_and_embed</CODE>.
  After the last line of code, the resulting affine space is
  the singleton set
  \f$\bigl\{ (2, 0)^\transpose \bigr\} \sseq \Rset^2\f$.

  \par Example 7
  The following code shows the use of the function
  <CODE>affine_image</CODE>:
  \code
  Affine_Space gr(2, EMPTY);
  gr.add_grid_generator(affine space_point(0*x + 0*y));
  gr.add_grid_generator(affine space_point(4*x + 0*y));
  gr.add_grid_generator(affine space_point(0*x + 2*y));
  Linear_Expression expr = x + 3;
  gr.affine_image(x, expr);
  \endcode
  In this example the starting affine space is all the pairs of \f$x\f$ and
  \f$y\f$ in \f$\Rset^2\f$ where \f$x\f$ is an integer multiple of 4
  and \f$y\f$ is an integer multiple of 2.  The considered variable
  is \f$x\f$ and the affine expression is \f$x+3\f$.  The resulting
  affine space is the given affine space translated 3 integers to the right (all the
  pairs \f$(x, y)\f$ where \f$x\f$ is -1 plus an integer multiple of 4
  and \f$y\f$ is an integer multiple of 2).
  Moreover, if the affine transformation for the same variable \p x
  is instead \f$x+y\f$:
  \code
  Linear_Expression expr = x + y;
  \endcode
  the resulting affine space is every second integral point along the \f$x=y\f$
  line, with this line of points repeated at every fourth integral value
  along the \f$x\f$ axis.
  Instead, if we do not use an invertible transformation for the
  same variable; for example, the affine expression \f$y\f$:
  \code
  Linear_Expression expr = y;
  \endcode
  the resulting affine space is every second point along the \f$x=y\f$ line.

  \par Example 8
  The following code shows the use of the function
  <CODE>affine_preimage</CODE>:
  \code
  Affine_Space gr(2, EMPTY);
  gr.add_grid_generator(affine space_point(0*x + 0*y));
  gr.add_grid_generator(affine space_point(4*x + 0*y));
  gr.add_grid_generator(affine space_point(0*x + 2*y));
  Linear_Expression expr = x + 3;
  gr.affine_preimage(x, expr);
  \endcode
  In this example the starting affine space, \p var and the affine
  expression and the denominator are the same as in Example 6, while
  the resulting affine space is similar but translated 3 integers to the
  left (all the pairs \f$(x, y)\f$
  where \f$x\f$ is -3 plus an integer multiple of 4 and
  \f$y\f$ is an integer multiple of 2)..
  Moreover, if the affine transformation for \p x is \f$x+y\f$
  \code
  Linear_Expression expr = x + y;
  \endcode
  the resulting affine space is a similar affine space to the result in Example 6,
  only the affine space is slanted along \f$x=-y\f$.
  Instead, if we do not use an invertible transformation for the same
  variable \p x, for example, the affine expression \f$y\f$:
  \code
  Linear_Expression expr = y;
  \endcode
  the resulting affine space is every fourth line parallel to the \f$x\f$
  axis.

  \par Example 9
  For this example we also use the variables:
  \code
  Variable z(2);
  Variable w(3);
  \endcode
  The following code shows the use of the function
  <CODE>remove_space_dimensions</CODE>:
  \code
  Affine_Space_Generator_System gs;
  gs.insert(affine space_point(3*x + y +0*z + 2*w));
  Affine_Space gr(gs);
  Variables_Set vars;
  vars.insert(y);
  vars.insert(z);
  gr.remove_space_dimensions(vars);
  \endcode
  The starting affine space is the singleton set
  \f$\bigl\{ (3, 1, 0, 2)^\transpose \bigr\} \sseq \Rset^4\f$, while
  the resulting affine space is
  \f$\bigl\{ (3, 2)^\transpose \bigr\} \sseq \Rset^2\f$.
  Be careful when removing space dimensions <EM>incrementally</EM>:
  since dimensions are automatically renamed after each application
  of the <CODE>remove_space_dimensions</CODE> operator, unexpected
  results can be obtained.
  For instance, by using the following code we would obtain
  a different result:
  \code
  set<Variable> vars1;
  vars1.insert(y);
  gr.remove_space_dimensions(vars1);
  set<Variable> vars2;
  vars2.insert(z);
  gr.remove_space_dimensions(vars2);
  \endcode
  In this case, the result is the affine space
  \f$\bigl\{(3, 0)^\transpose \bigr\} \sseq \Rset^2\f$:
  when removing the set of dimensions \p vars2
  we are actually removing variable \f$w\f$ of the original affine space.
  For the same reason, the operator \p remove_space_dimensions
  is not idempotent: removing twice the same non-empty set of dimensions
  is never the same as removing them just once.
*/

class Parma_Polyhedra_Library::Affine_Space {
public:
  //! The numeric type of coefficients.
  typedef Coefficient coefficient_type;

  //! Returns the maximum space dimension all kinds of Affine_Space can handle.
  static dimension_type max_space_dimension();

  /*! \brief
    Returns true indicating that this domain has methods that
    can recycle constraints.
  */
  static bool can_recycle_constraint_systems();

  /*! \brief
    Returns true indicating that this domain has methods that
    can recycle congruences
  */
  static bool can_recycle_congruence_systems();

  //! Builds an affine space having the specified properties.
  /*!
    \param num_dimensions
    The number of dimensions of the vector space enclosing the affine space;

    \param kind
    Specifies whether the universe or the empty affine space has to be built.

    \exception std::length_error
    Thrown if \p num_dimensions exceeds the maximum allowed space
    dimension.
  */
  explicit Affine_Space(dimension_type num_dimensions = 0,
                        Degenerate_Element kind = UNIVERSE);

  //! Builds an affine space, copying a system of constraints.
  /*!
    The affine space inherits the space dimension of the constraint system.

    \param cs
    The system of constraints defining the affine space.

    \exception std::invalid_argument
    Thrown if the constraint system \p cs contains inequality constraints.

    \exception std::length_error
    Thrown if \p num_dimensions exceeds the maximum allowed space
    dimension.
  */
  explicit Affine_Space(const Constraint_System& cs);

  //! Builds an affine space, recycling a system of constraints.
  /*!
    The affine space inherits the space dimension of the constraint system.

    \param cs
    The system of constraints defining the affine space.  Its data-structures
    may be recycled to build the affine space.

    \param dummy
    A dummy tag to syntactically differentiate this one
    from the other constructors.

    \exception std::invalid_argument
    Thrown if the constraint system \p cs contains inequality constraints.

    \exception std::length_error
    Thrown if \p num_dimensions exceeds the maximum allowed space
    dimension.
  */
  Affine_Space(Constraint_System& cs, Recycle_Input dummy);

  //! Builds an affine space, copying a system of affine space generators.
  /*!
    The affine space inherits the space dimension of the generator system.

    \param gs
    The system of generators defining the affine space.

    \exception std::invalid_argument
    Thrown if the system of generators contains rays or is not empty but
    has no points.

    \exception std::length_error
    Thrown if \p num_dimensions exceeds the maximum allowed space
    dimension.
  */
  explicit Affine_Space(const Generator_System& gs);

  //! Builds an affine space, recycling a system of generators.
  /*!
    The affine space inherits the space dimension of the generator system.

    \param gs
    The system of generators defining the affine space.  Its data-structures
    may be recycled to build the affine space.

    \param dummy
    A dummy tag to syntactically differentiate this one
    from the other constructors.

    \exception std::invalid_argument
    Thrown if the system of generators contains rays or is not empty but
    has no points.

    \exception std::length_error
    Thrown if \p num_dimensions exceeds the maximum allowed space dimension.
  */
  Affine_Space(Generator_System& gs, Recycle_Input dummy);

  //! Builds an affine space out of a box.
  /*!
    The affine space inherits the space dimension of the box.
    The built affine space is the most precise affine space that
    includes the box.

    \param box
    The box representing the affine space to be built.

    \param complexity
    This argument is ignored as the algorithm used has
    polynomial complexity.

    \exception std::length_error
    Thrown if the space dimension of \p box exceeds the maximum
    allowed space dimension.
  */
  template <typename Interval>
  explicit Affine_Space(const Box<Interval>& box,
                        Complexity_Class complexity = ANY_COMPLEXITY);

  //! Builds an affine space out of a bounded-difference shape.
  /*!
    The affine space inherits the space dimension of the BDS.
    The built affine space is the most precise affine space that
    includes the BDS.

    \param bd
    The BDS representing the affine space to be built.

    \param complexity
    This argument is ignored as the algorithm used has
    polynomial complexity.

    \exception std::length_error
    Thrown if the space dimension of \p bd exceeds the maximum
    allowed space dimension.
  */
  template <typename U>
  explicit Affine_Space(const BD_Shape<U>& bd,
                        Complexity_Class complexity = ANY_COMPLEXITY);

  //! Builds an affine space out of an octagonal shape.
  /*!
    The affine space inherits the space dimension of the octagonal shape.
    The built affine space is the most precise affine space that
    includes the octagonal shape.

    \param os
    The octagonal shape representing the affine space to be built.

    \param complexity
    This argument is ignored as the algorithm used has
    polynomial complexity.

    \exception std::length_error
    Thrown if the space dimension of \p os exceeds the maximum
    allowed space dimension.
  */
  template <typename U>
  explicit Affine_Space(const Octagonal_Shape<U>& os,
                        Complexity_Class complexity = ANY_COMPLEXITY);

  /*! \brief
    Builds an affine space from a polyhedron using algorithms whose complexity
    does not exceed the one specified by \p complexity.
    If \p complexity is \p ANY_COMPLEXITY, then the affine space built is the
    smallest one containing \p ph.

    The affine space inherits the space dimension of \p ph.

    \param ph
    The polyhedron.

    \param complexity
    The complexity class.

    \exception std::length_error
    Thrown if \p num_dimensions exceeds the maximum allowed space
    dimension.
  */
  explicit Affine_Space(const Polyhedron& ph,
                        Complexity_Class complexity = ANY_COMPLEXITY);

  //! Ordinary copy constructor.
  /*!
    The complexity argument is ignored.
  */
  Affine_Space(const Affine_Space& y,
               Complexity_Class complexity = ANY_COMPLEXITY);

  /*! \brief
    The assignment operator.  (\p *this and \p y can be
    dimension-incompatible.)
  */
  Affine_Space& operator=(const Affine_Space& y);

  //! \name Member Functions that Do Not Modify the Affine_Space
  //@{

  //! Returns the dimension of the vector space enclosing \p *this.
  dimension_type space_dimension() const;

  /*! \brief
    Returns \f$0\f$, if \p *this is empty; otherwise, returns
    \ref Affine_Independence_and_Affine_Dimension "affine dimension"
    of \p *this.
  */
  dimension_type affine_dimension() const;

  /*! \brief
    Returns a system of equality constraints satisfied by \p *this
    with the same affine dimension as \p *this.
  */
  Constraint_System constraints() const;

  /*! \brief
    Returns a minimal system of equality constraints satisfied by
    \p *this with the same affine dimension as \p *this.
  */
  Constraint_System minimized_constraints() const;

  //! Returns the system of equality congruences satisfied by \p *this.
  const Congruence_System& congruences() const;

  /*! \brief
    Returns the system of equality congruences satisfied by \p *this,
    with no redundant congruences and having the same affine dimension
    as \p *this.
  */
  const Congruence_System& minimized_congruences() const;

  //! Returns a system of generators defining \p *this.
  Generator_System generators() const;

  //! Returns a minimized system of generators defining \p *this.
  Generator_System minimized_generators() const;

  //! Returns the relations holding between \p *this and \p cg.
  /*
    \exception std::invalid_argument
    Thrown if \p *this and congruence \p cg are dimension-incompatible.
  */
  Poly_Con_Relation relation_with(const Congruence& cg) const;

  //! Returns the relations holding between \p *this and \p g.
  /*
    \exception std::invalid_argument
    Thrown if \p *this and generator \p g are dimension-incompatible.
  */
  Poly_Gen_Relation
  relation_with(const Generator& g) const;

  //! Returns the relations holding between \p *this and \p c.
  /*
    \exception std::invalid_argument
    Thrown if \p *this and constraint \p c are dimension-incompatible.
  */
  Poly_Con_Relation relation_with(const Constraint& c) const;

  //! Returns \c true if and only if \p *this is an empty affine space.
  bool is_empty() const;

  //! Returns \c true if and only if \p *this is a universe affine space.
  bool is_universe() const;

  /*! \brief
    Returns <CODE>true</CODE> if and only if \p *this is a
    topologically closed subset of the vector space.

    An affine space is always topologically closed.
  */
  bool is_topologically_closed() const;

  /*! \brief
    Returns <CODE>true</CODE> if and only if \p *this and \p y are
    disjoint.

    \exception std::invalid_argument
    Thrown if \p x and \p y are dimension-incompatible.
  */
  bool is_disjoint_from(const Affine_Space& y) const;

  //! Returns <CODE>true</CODE> if and only if \p *this is discrete.
  /*!
    An affine space is discrete if it can be defined by a generator system
    that contains at most one point.  This includes the empty affine space
    and any affine space in dimension zero.
  */
  bool is_discrete() const;

  //! Returns <CODE>true</CODE> if and only if \p *this is bounded.
  bool is_bounded() const;

  /*! \brief
    Returns <CODE>true</CODE> if and only if \p *this
    contains at least one integer point.
  */
  bool contains_integer_point() const;

  /*! \brief
    Returns <CODE>true</CODE> if and only if \p var is constrained in
    \p *this.

    \exception std::invalid_argument
    Thrown if \p var is not a space dimension of \p *this.
  */
  bool constrains(Variable var) const;

  //! Returns <CODE>true</CODE> if and only if \p expr is bounded in \p *this.
  /*!
    This method is the same as bounds_from_below.

    \exception std::invalid_argument
    Thrown if \p expr and \p *this are dimension-incompatible.
  */
  bool bounds_from_above(const Linear_Expression& expr) const;

  //! Returns <CODE>true</CODE> if and only if \p expr is bounded in \p *this.
  /*!
    This method is the same as bounds_from_above.

    \exception std::invalid_argument
    Thrown if \p expr and \p *this are dimension-incompatible.
  */
  bool bounds_from_below(const Linear_Expression& expr) const;

  // FIXME: revise all the comment blocks below this one.

  /*! \brief
    Returns <CODE>true</CODE> if and only if \p *this is not empty and
    \p expr is bounded from above in \p *this, in which case the
    supremum value is computed.

    \param expr
    The linear expression to be maximized subject to \p *this;

    \param sup_n
    The numerator of the supremum value;

    \param sup_d
    The denominator of the supremum value;

    \param maximum
    <CODE>true</CODE> if the supremum value can be reached in \p this.
    Always <CODE>true</CODE> when \p this bounds \p expr.  Present for
    interface compatibility with class Polyhedron, where closure
    points can result in a value of false.

    \exception std::invalid_argument
    Thrown if \p expr and \p *this are dimension-incompatible.

    If \p *this is empty or \p expr is not bounded by \p *this,
    <CODE>false</CODE> is returned and \p sup_n, \p sup_d and \p
    maximum are left untouched.
  */
  bool maximize(const Linear_Expression& expr,
                Coefficient& sup_n, Coefficient& sup_d, bool& maximum) const;

  /*! \brief
    Returns <CODE>true</CODE> if and only if \p *this is not empty and
    \p expr is bounded from above in \p *this, in which case the
    supremum value and a point where \p expr reaches it are computed.

    \param expr
    The linear expression to be maximized subject to \p *this;

    \param sup_n
    The numerator of the supremum value;

    \param sup_d
    The denominator of the supremum value;

    \param maximum
    <CODE>true</CODE> if the supremum value can be reached in \p this.
    Always <CODE>true</CODE> when \p this bounds \p expr.  Present for
    interface compatibility with class Polyhedron, where closure
    points can result in a value of false;

    \param point
    When maximization succeeds, will be assigned a point where \p expr
    reaches its supremum value.

    \exception std::invalid_argument
    Thrown if \p expr and \p *this are dimension-incompatible.

    If \p *this is empty or \p expr is not bounded by \p *this,
    <CODE>false</CODE> is returned and \p sup_n, \p sup_d, \p maximum
    and \p point are left untouched.
  */
  bool maximize(const Linear_Expression& expr,
                Coefficient& sup_n, Coefficient& sup_d, bool& maximum,
                Generator& point) const;

  /*! \brief
    Returns <CODE>true</CODE> if and only if \p *this is not empty and
    \p expr is bounded from below in \p *this, in which case the
    infimum value is computed.

    \param expr
    The linear expression to be minimized subject to \p *this;

    \param inf_n
    The numerator of the infimum value;

    \param inf_d
    The denominator of the infimum value;

    \param minimum
    <CODE>true</CODE> if the is the infimum value can be reached in \p
    this.  Always <CODE>true</CODE> when \p this bounds \p expr.
    Present for interface compatibility with class Polyhedron, where
    closure points can result in a value of false.

    \exception std::invalid_argument
    Thrown if \p expr and \p *this are dimension-incompatible.

    If \p *this is empty or \p expr is not bounded from below,
    <CODE>false</CODE> is returned and \p inf_n, \p inf_d
    and \p minimum are left untouched.
  */
  bool minimize(const Linear_Expression& expr,
                Coefficient& inf_n, Coefficient& inf_d, bool& minimum) const;

  /*! \brief
    Returns <CODE>true</CODE> if and only if \p *this is not empty and
    \p expr is bounded from below in \p *this, in which case the
    infimum value and a point where \p expr reaches it are computed.

    \param expr
    The linear expression to be minimized subject to \p *this;

    \param inf_n
    The numerator of the infimum value;

    \param inf_d
    The denominator of the infimum value;

    \param minimum
    <CODE>true</CODE> if the is the infimum value can be reached in \p
    this.  Always <CODE>true</CODE> when \p this bounds \p expr.
    Present for interface compatibility with class Polyhedron, where
    closure points can result in a value of false;

    \param point
    When minimization succeeds, will be assigned a point where \p expr
    reaches its infimum value.

    \exception std::invalid_argument
    Thrown if \p expr and \p *this are dimension-incompatible.

    If \p *this is empty or \p expr is not bounded from below,
    <CODE>false</CODE> is returned and \p inf_n, \p inf_d, \p minimum
    and \p point are left untouched.
  */
  bool minimize(const Linear_Expression& expr,
                Coefficient& inf_n, Coefficient& inf_d, bool& minimum,
                Generator& point) const;

  /*! \brief
    Returns <CODE>true</CODE> if and only if \p *this is not empty and
    \p expr is discrete in \p *this, in which case the maximum frequency
    and the value for \p expr that is closest to zero are computed.

    \param expr
    The linear expression for which the frequency is needed;

    \param freq_n
    The numerator of the maximum frequency of \p expr;

    \param freq_d
    The denominator of the maximum frequency of \p expr;

    \param val_n
    The numerator of a value of \p expr at a point in the grid
    that is closest to zero;

    \param val_d
    The denominator of a value of \p expr at a point in the grid
    that is closest to zero;

    \exception std::invalid_argument
    Thrown if \p expr and \p *this are dimension-incompatible.

    If \p *this is empty or \p expr can take any real number in \p *this,
    <CODE>false</CODE> is returned and \p freq_n, \p freq_d,
    \p val_n and \p val_d are left untouched.
  */
  bool frequency(const Linear_Expression& expr,
                 Coefficient& freq_n, Coefficient& freq_d,
                 Coefficient& val_n, Coefficient& val_d) const;

  //! Returns <CODE>true</CODE> if and only if \p *this contains \p y.
  /*!
    \exception std::invalid_argument
    Thrown if \p *this and \p y are dimension-incompatible.
  */
  bool contains(const Affine_Space& y) const;

  /*! \brief
    Returns <CODE>true</CODE> if and only if \p *this strictly
    contains \p y.

    \exception std::invalid_argument
    Thrown if \p *this and \p y are dimension-incompatible.
  */
  bool strictly_contains(const Affine_Space& y) const;

  //! Checks if all the invariants are satisfied.
  /*!
    \return
    <CODE>true</CODE> if and only if \p *this satisfies all the
    invariants and either \p check_not_empty is <CODE>false</CODE> or
    \p *this is not empty.

    \param check_not_empty
    <CODE>true</CODE> if and only if, in addition to checking the
    invariants, \p *this must be checked to be not empty.

    The check is performed so as to intrude as little as possible.  If
    the library has been compiled with run-time assertions enabled,
    error messages are written on <CODE>std::cerr</CODE> in case
    invariants are violated. This is useful for the purpose of
    debugging the library.
  */
  bool OK(bool check_not_empty = false) const;

  //@} // Member Functions that Do Not Modify the Affine_Space

  //! \name Space Dimension Preserving Member Functions that May Modify the Affine_Space
  //@{

  //! Adds a copy of congruence \p cg to \p *this.
  /*!
    \exception std::invalid_argument
    Thrown if \p *this and congruence \p cg are
    dimension-incompatible.
  */
  void add_congruence(const Congruence& cg);

  /*! \brief
    Adds a copy of affine space generator \p g to the system of generators of
    \p *this.

    \exception std::invalid_argument
    Thrown if \p *this and generator \p g are dimension-incompatible,
    or if \p *this is an empty affine space and \p g is not a point.
  */
  void add_generator(const Generator& g);

  //! Adds a copy of each congruence in \p cgs to \p *this.
  /*!
    \param cgs
    Contains the congruences that will be added to the system of
    congruences of \p *this.

    \exception std::invalid_argument
    Thrown if \p *this and \p cgs are dimension-incompatible.
  */
  void add_congruences(const Congruence_System& cgs);

  //! Adds the congruences in \p cgs to *this.
  /*!
    \param cgs
    The congruence system to be added to \p *this.  The congruences in
    \p cgs may be recycled.

    \exception std::invalid_argument
    Thrown if \p *this and \p cs are dimension-incompatible.

    \warning
    The only assumption that can be made about \p cgs upon successful
    or exceptional return is that it can be safely destroyed.
  */
  void add_recycled_congruences(Congruence_System& cgs);

  /*! \brief
    Adds to \p *this a congruence equivalent to constraint \p c.

    \param c
    The constraint to be added.

    \exception std::invalid_argument
    Thrown if \p *this and \p c are dimension-incompatible
    or if constraint \p c is not optimally supported by the affine space domain.
  */
  void add_constraint(const Constraint& c);

  /*! \brief
    Adds to \p *this congruences equivalent to the constraints in \p cs.

    \param cs
    The constraints to be added.

    \exception std::invalid_argument
    Thrown if \p *this and \p cs are dimension-incompatible
    or if \p cs contains a constraint whcih is not optimally supported
    by the affine space domain.
  */
  void add_constraints(const Constraint_System& cs);

  /*! \brief
    Adds to \p *this congruences equivalent to the constraints in \p cs.

    \param cs
    The constraints to be added. They may be recycled.

    \exception std::invalid_argument
    Thrown if \p *this and \p cs are dimension-incompatible
    or if \p cs contains a constraint whcih is not optimally supported
    by the affine space domain.

    \warning
    The only assumption that can be made about \p cs upon successful
    or exceptional return is that it can be safely destroyed.
  */
  void add_recycled_constraints(Constraint_System& cs);

  //! Uses a copy of the congruence \p cg to refine \p *this.
  /*!
    \param cg
    The congruence used.

    \exception std::invalid_argument
    Thrown if \p *this and congruence \p cg are dimension-incompatible.
  */
  void refine_with_congruence(const Congruence& cg);

 //! Uses a copy of the congruences in \p cgs to refine \p *this.
  /*!
    \param cgs
    The congruences used.

    \exception std::invalid_argument
    Thrown if \p *this and \p cgs are dimension-incompatible.
  */
  void refine_with_congruences(const Congruence_System& cgs);

  //! Uses a copy of the constraint \p c to refine \p *this.
  /*!

    \param c
    The constraint used. If it is not an equality, it will be ignored

    \exception std::invalid_argument
    Thrown if \p *this and \p c are dimension-incompatible.
  */
  void refine_with_constraint(const Constraint& c);

  //! Uses a copy of the constraints in \p cs to refine \p *this.
  /*!
    \param cs
    The constraints used. Constraints that are not equalities are ignored.

    \exception std::invalid_argument
    Thrown if \p *this and \p cs are dimension-incompatible.
  */
  void refine_with_constraints(const Constraint_System& cs);

  /*! \brief
    Adds a copy of the generators in \p gs to the system of generators
    of \p *this.

    \param gs
    Contains the generators that will be added to the system of
    generators of \p *this.

    \exception std::invalid_argument
    Thrown if \p *this and \p gs are dimension-incompatible, or if
    \p *this is empty and the system of generators \p gs is not empty,
    but has no points.
  */
  void add_generators(const Generator_System& gs);

  /*! \brief
    Adds the generators in \p gs to the system of generators of \p
    *this.

    \param gs
    The generator system to be added to \p *this.  The generators in
    \p gs may be recycled.

    \exception std::invalid_argument
    Thrown if \p *this and \p gs are dimension-incompatible.

    \warning
    The only assumption that can be made about \p gs upon successful
    or exceptional return is that it can be safely destroyed.
  */
  void add_recycled_generators(Generator_System& gs);

  /*! \brief
    Computes the \ref Cylindrification "cylindrification" of \p *this with
    respect to space dimension \p var, assigning the result to \p *this.

    \param var
    The space dimension that will be unconstrained.

    \exception std::invalid_argument
    Thrown if \p var is not a space dimension of \p *this.
  */
  void unconstrain(Variable var);

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
  void unconstrain(const Variables_Set& vars);

  /*! \brief
    Assigns to \p *this the intersection of \p *this and \p y.

    \exception std::invalid_argument
    Thrown if \p *this and \p y are dimension-incompatible.
  */
  void intersection_assign(const Affine_Space& y);

  /*! \brief
    Assigns to \p *this the least upper bound of \p *this and \p y.

    \exception std::invalid_argument
    Thrown if \p *this and \p y are dimension-incompatible.
  */
  void upper_bound_assign(const Affine_Space& y);

  /*! \brief
    If the upper bound of \p *this and \p y is exact it is assigned to \p
    *this and <CODE>true</CODE> is returned, otherwise
    <CODE>false</CODE> is returned.

    \exception std::invalid_argument
    Thrown if \p *this and \p y are dimension-incompatible.
  */
  bool upper_bound_assign_if_exact(const Affine_Space& y);

  /*! \brief
    Assigns to \p *this the \ref Convex_Polyhedral_Difference
    "poly-difference" of \p *this and \p y.

    \exception std::invalid_argument
    Thrown if \p *this and \p y are dimension-incompatible.
  */
  void difference_assign(const Affine_Space& y);

  /*! \brief
    Assigns to \p *this a \ref Meet_Preserving_Simplification
    "meet-preserving simplification" of \p *this with respect to \p y.
    If \c false is returned, then the intersection is empty.

    \exception std::invalid_argument
    Thrown if \p *this and \p y are topology-incompatible or
    dimension-incompatible.
  */
  bool simplify_using_context_assign(const Affine_Space& y);

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

    When considering the generators of an affine space, the
    affine transformation
    \f[
      \frac{\sum_{i=0}^{n-1} a_i x_i + b}{\mathrm{denominator}}
    \f]
    is assigned to \p var where \p expr is
    \f$\sum_{i=0}^{n-1} a_i x_i + b\f$
    (\f$b\f$ is the inhomogeneous term).

    If congruences are up-to-date, it uses the specialized function
    affine_preimage() (for the system of congruences)
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

    \endif
  */
  void affine_image(Variable var,
                    const Linear_Expression& expr,
                    Coefficient_traits::const_reference denominator
                    = Coefficient_one());

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
  */
  void affine_preimage(Variable var,
                       const Linear_Expression& expr,
                       Coefficient_traits::const_reference denominator
                         = Coefficient_one());

  /*! \brief
    Assigns to \p *this the image of \p *this with respect to
    the \ref Generalized_Affine_Relations "generalized affine relation"
    \f$\mathrm{var}' = \frac{\mathrm{expr}}{\mathrm{denominator}}
    \pmod{\mathrm{modulus}}\f$.

    \param var
    The left hand side variable of the generalized affine relation;

    \param relsym
    The relation symbol where EQUAL is the symbol for a congruence
    relation;

    \param expr
    The numerator of the right hand side affine expression;

    \param denominator
    The denominator of the right hand side affine expression.
    Optional argument with an automatic value of one;

    \exception std::invalid_argument
    Thrown if \p denominator is zero or if \p expr and \p *this are
    dimension-incompatible or if \p var is not a space dimension of \p
    *this.
  */
  void
  generalized_affine_image(Variable var,
                           Relation_Symbol relsym,
                           const Linear_Expression& expr,
                           Coefficient_traits::const_reference denominator
                           = Coefficient_one());

  /*! \brief
    Assigns to \p *this the preimage of \p *this with respect to the
    \ref Generalized_Affine_Relations "generalized affine relation"
    \f$\mathrm{var}' = \frac{\mathrm{expr}}{\mathrm{denominator}}
    \pmod{\mathrm{modulus}}\f$.

    \param var
    The left hand side variable of the generalized affine relation;

    \param relsym
    The relation symbol where EQUAL is the symbol for a congruence
    relation;

    \param expr
    The numerator of the right hand side affine expression;

    \param denominator
    The denominator of the right hand side affine expression.
    Optional argument with an automatic value of one;

    \exception std::invalid_argument
    Thrown if \p denominator is zero or if \p expr and \p *this are
    dimension-incompatible or if \p var is not a space dimension of \p
    *this.
  */
  void
  generalized_affine_preimage(Variable var,
                              Relation_Symbol relsym,
                              const Linear_Expression& expr,
                              Coefficient_traits::const_reference denominator
                              = Coefficient_one());

  /*! \brief
    Assigns to \p *this the image of \p *this with respect to
    the \ref Generalized_Affine_Relations "generalized affine relation"
    \f$\mathrm{lhs}' = \mathrm{rhs} \pmod{\mathrm{modulus}}\f$.

    \param lhs
    The left hand side affine expression.

    \param relsym
    The relation symbol where EQUAL is the symbol for a congruence
    relation;

    \param rhs
    The right hand side affine expression.

    \exception std::invalid_argument
    Thrown if \p *this is dimension-incompatible with \p lhs or \p
    rhs.
  */
  void
  generalized_affine_image(const Linear_Expression& lhs,
                           Relation_Symbol relsym,
                           const Linear_Expression& rhs);

  /*! \brief
    Assigns to \p *this the preimage of \p *this with respect to the
    \ref Generalized_Affine_Relations "generalized affine relation"
    \f$\mathrm{lhs}' = \mathrm{rhs} \pmod{\mathrm{modulus}}\f$.

    \param lhs
    The left hand side affine expression;

    \param relsym
    The relation symbol where EQUAL is the symbol for a congruence
    relation;

    \param rhs
    The right hand side affine expression;

    \exception std::invalid_argument
    Thrown if \p *this is dimension-incompatible with \p lhs or \p
    rhs.
  */
  void
  generalized_affine_preimage(const Linear_Expression& lhs,
                              Relation_Symbol relsym,
                              const Linear_Expression& rhs);

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
  void bounded_affine_image(Variable var,
                            const Linear_Expression& lb_expr,
                            const Linear_Expression& ub_expr,
                            Coefficient_traits::const_reference denominator
                            = Coefficient_one());

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
  void bounded_affine_preimage(Variable var,
                               const Linear_Expression& lb_expr,
                               const Linear_Expression& ub_expr,
                               Coefficient_traits::const_reference denominator
                               = Coefficient_one());

  /*! \brief
    Assigns to \p *this the result of computing the
    \ref Time_Elapse_Operator "time-elapse" between \p *this and \p y.

    \exception std::invalid_argument
    Thrown if \p *this and \p y are dimension-incompatible.
  */
  void time_elapse_assign(const Affine_Space& y);

  //! Assigns to \p *this its topological closure.
  void topological_closure_assign();

  /*! \brief
    Does nothing, as the domain of affine spaces has finite height.

    \param y
    An affine space that <EM>must</EM> be contained in \p *this;

    \param tp
    An optional pointer, which is ignored.

    \exception std::invalid_argument
    Thrown if \p *this and \p y are dimension-incompatible.
  */
  void widening_assign(const Affine_Space& y, unsigned* tp = nullptr);

  /*! \brief
    Does nothing, as the domain of affine spaces has finite height.

    \param y
    An affine space that <EM>must</EM> be contained in \p *this;

    \param cs
    A system of constraints, which is ignored.

    \param tp
    An optional pointer, which is ignored.

    \exception std::invalid_argument
    Thrown if \p *this, \p y and \p cs are dimension-incompatible.
  */
  void limited_extrapolation_assign(const Affine_Space& y,
                                    const Constraint_System& cs,
                                    unsigned* tp = nullptr);

  //@} // Space Dimension Preserving Member Functions that May Modify [...]

  //! \name Member Functions that May Modify the Dimension of the Vector Space
  //@{

  /*! \brief
    Adds \p m new space dimensions and embeds the old affine space in the new
    vector space.

    \param m
    The number of dimensions to add.

    \exception std::length_error
    Thrown if adding \p m new space dimensions would cause the vector
    space to exceed dimension <CODE>max_space_dimension()</CODE>.

    The new space dimensions will be those having the highest indexes
    in the new affine space, which is characterized by a system of congruences
    in which the variables which are the new dimensions can have any
    value.  For instance, when starting from the affine space \f$\cL \sseq
    \Rset^2\f$ and adding a third space dimension, the result will be
    the affine space
    \f[
      \bigl\{\,
        (x, y, z)^\transpose \in \Rset^3
      \bigm|
        (x, y)^\transpose \in \cL
      \,\bigr\}.
    \f]
  */
  void add_space_dimensions_and_embed(dimension_type m);

  /*! \brief
    Adds \p m new space dimensions to the affine space and does not embed it
    in the new vector space.

    \param m
    The number of space dimensions to add.

    \exception std::length_error
    Thrown if adding \p m new space dimensions would cause the
    vector space to exceed dimension <CODE>max_space_dimension()</CODE>.

    The new space dimensions will be those having the highest indexes
    in the new affine space, which is characterized by a system of congruences
    in which the variables running through the new dimensions are all
    constrained to be equal to 0.  For instance, when starting from
    the affine space \f$\cL \sseq \Rset^2\f$ and adding a third space
    dimension, the result will be the affine space
    \f[
      \bigl\{\,
        (x, y, 0)^\transpose \in \Rset^3
      \bigm|
        (x, y)^\transpose \in \cL
      \,\bigr\}.
    \f]
  */
  void add_space_dimensions_and_project(dimension_type m);

  /*! \brief
    Assigns to \p *this the \ref Concatenating_Polyhedra "concatenation"
    of \p *this and \p y, taken in this order.

    \exception std::length_error
    Thrown if the concatenation would cause the vector space
    to exceed dimension <CODE>max_space_dimension()</CODE>.
  */
  void concatenate_assign(const Affine_Space& y);

  //! Removes all the specified dimensions from the vector space.
  /*!
    \param vars
    The set of Variable objects corresponding to the space dimensions
    to be removed.

    \exception std::invalid_argument
    Thrown if \p *this is dimension-incompatible with one of the
    Variable objects contained in \p vars.
  */
  void remove_space_dimensions(const Variables_Set& vars);

  /*! \brief
    Removes the higher dimensions of the vector space so that the
    resulting space will have dimension \p new_dimension.

    \exception std::invalid_argument
    Thrown if \p new_dimensions is greater than the space dimension of
    \p *this.
  */
  void remove_higher_space_dimensions(dimension_type new_dimension);

  /*! \brief
    Remaps the dimensions of the vector space according to
    a \ref Mapping_the_Dimensions_of_the_Vector_Space "partial function".

    If \p pfunc maps only some of the dimensions of \p *this then the
    rest will be projected away.

    If the highest dimension mapped to by \p pfunc is higher than the
    highest dimension in \p *this then the number of dimensions in \p
    *this will be increased to the highest dimension mapped to by \p
    pfunc.

    \param pfunc
    The partial function specifying the destiny of each space
    dimension.

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
    returns the maximum value that belongs to the codomain of the
    partial function.
    The <CODE>max_in_codomain()</CODE> method is called at most once.
    \code
      bool maps(dimension_type i, dimension_type& j) const
    \endcode
    Let \f$f\f$ be the represented function and \f$k\f$ be the value
    of \p i.  If \f$f\f$ is defined in \f$k\f$, then \f$f(k)\f$ is
    assigned to \p j and <CODE>true</CODE> is returned.  If \f$f\f$ is
    undefined in \f$k\f$, then <CODE>false</CODE> is returned.
    This method is called at most \f$n\f$ times, where \f$n\f$ is the
    dimension of the vector space enclosing the affine space.

    The result is undefined if \p pfunc does not encode a partial
    function with the properties described in the
    \ref Mapping_the_Dimensions_of_the_Vector_Space
    "specification of the mapping operator".
  */
  template <typename Partial_Function>
  void map_space_dimensions(const Partial_Function& pfunc);

  //! Creates \p m copies of the space dimension corresponding to \p var.
  /*!
    \param var
    The variable corresponding to the space dimension to be replicated;

    \param m
    The number of replicas to be created.

    \exception std::invalid_argument
    Thrown if \p var does not correspond to a dimension of the vector
    space.

    \exception std::length_error
    Thrown if adding \p m new space dimensions would cause the vector
    space to exceed dimension <CODE>max_space_dimension()</CODE>.

    If \p *this has space dimension \f$n\f$, with \f$n > 0\f$,
    and <CODE>var</CODE> has space dimension \f$k \leq n\f$,
    then the \f$k\f$-th space dimension is
    \ref expand_space_dimension "expanded" to \p m new space dimensions
    \f$n\f$, \f$n+1\f$, \f$\dots\f$, \f$n+m-1\f$.
  */
  void expand_space_dimension(Variable var, dimension_type m);

  //! Folds the space dimensions in \p to_be_folded into \p var.
  /*!
    \param to_be_folded
    The set of Variable objects corresponding to the space dimensions
    to be folded;

    \param var
    The variable corresponding to the space dimension that is the
    destination of the folding operation.

    \exception std::invalid_argument
    Thrown if \p *this is dimension-incompatible with \p var or with
    one of the Variable objects contained in \p to_be_folded.  Also
    thrown if \p var is contained in \p to_be_folded.

    If \p *this has space dimension \f$n\f$, with \f$n > 0\f$,
    <CODE>var</CODE> has space dimension \f$k \leq n\f$,
    \p to_be_folded is a set of variables whose maximum space dimension
    is also less than or equal to \f$n\f$, and \p var is not a member
    of \p to_be_folded, then the space dimensions corresponding to
    variables in \p to_be_folded are
    \ref Folding_Multiple_Dimensions_of_the_Vector_Space_into_One_Dimension "folded"
    into the \f$k\f$-th space dimension.
  */
  void fold_space_dimensions(const Variables_Set& to_be_folded, Variable var);

  //@} // Member Functions that May Modify the Dimension of the Vector Space

  friend bool operator==(const Affine_Space& x, const Affine_Space& y);

  //! \name Miscellaneous Member Functions
  //@{

  //! Destructor.
  ~Affine_Space();

  /*! \brief
    Swaps \p *this with affine space \p y.  (\p *this and \p y can be
    dimension-incompatible.)
  */
  void m_swap(Affine_Space& y);

  PPL_OUTPUT_DECLARATIONS

  /*! \brief
    Loads from \p s an ASCII representation (as produced by
    ascii_dump(std::ostream&) const) and sets \p *this accordingly.
    Returns <CODE>true</CODE> if successful, <CODE>false</CODE> otherwise.
  */
  bool ascii_load(std::istream& s);

  //! Returns the total size in bytes of the memory occupied by \p *this.
  memory_size_type total_memory_in_bytes() const;

  //! Returns the size in bytes of the memory managed by \p *this.
  memory_size_type external_memory_in_bytes() const;

  /*! \brief
    Returns a 32-bit hash code for \p *this.

    If \p x and \p y are such that <CODE>x == y</CODE>,
    then <CODE>x.hash_code() == y.hash_code()</CODE>.
  */
  int32_t hash_code() const;

  //@} // Miscellaneous Member Functions

private:
  //! The rational grid implementing \p *this.
  Grid gr;

#ifdef PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
  //! \name Exception Throwers
  //@{
#endif // defined(PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS)
protected:
#if 0
  void throw_invalid_argument(const char* method, const char* reason) const;
#endif
  void throw_dimension_incompatible(const char* method,
                                    const char* other_name,
                                    dimension_type other_dim) const;
  void throw_dimension_incompatible(const char* method,
                                    const char* as_name,
                                    const Affine_Space& as) const;

  // Note: it has to be a static method, because it can be called inside
  // constructors (before actually constructing the grid object).
  static void throw_space_dimension_overflow(const char* method,
                                             const char* reason);

  void throw_invalid_constraint(const char* method,
                                const char* c_name) const;
  void throw_invalid_constraints(const char* method,
                                 const char* cs_name) const;
  void throw_invalid_generator(const char* method,
                               const char* g_name) const;
  void throw_invalid_generators(const char* method,
                                const char* gs_name) const;
#ifdef PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
  //@} // Exception Throwers
#endif // defined(PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS)

};

#include "Affine_Space_inlines.hh"

#endif // !defined(PPL_Affine_Space_defs_hh)
