/* Pointset_Ask_Tell class declaration.
   Copyright (C) 2001-2007 Roberto Bagnara <bagnara@cs.unipr.it>

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

#ifndef PPL_Pointset_Ask_Tell_defs_hh
#define PPL_Pointset_Ask_Tell_defs_hh

#include "Pointset_Ask_Tell.types.hh"
#include "globals.defs.hh"
#include "BHRZ03_Certificate.types.hh"
#include "Constraint.types.hh"
#include "Constraint_System.types.hh"
#include "Congruence.types.hh"
#include "Congruence_System.types.hh"
#include "C_Polyhedron.types.hh"
#include "NNC_Polyhedron.types.hh"
#include "Polyhedron.defs.hh"
#include "Variables_Set.types.hh"
#include "Determinate.defs.hh"
#include "Ask_Tell.defs.hh"
#include <iosfwd>
#include <list>
#include <map>

//! The powerset construction instantiated on PPL polyhedra.
/*! \ingroup PPL_CXX_interface */
template <typename PS>
class Parma_Polyhedra_Library::Pointset_Ask_Tell
  : public Parma_Polyhedra_Library::Ask_Tell
<Parma_Polyhedra_Library::Determinate<PS> > {
public:
  typedef PS element_type;

private:
  typedef Determinate<PS> CS;
  typedef Ask_Tell<CS> Base;

public:
  //! Returns the maximum space dimension a Pointset_Ask_Tell<PS> can handle.
  static dimension_type max_space_dimension();

  //! \name Constructors
  //@{

  //! Builds a universe (top) or empty (bottom) Pointset_Ask_Tell.
  /*!
    \param num_dimensions
    The number of dimensions of the vector space enclosing the powerset;

    \param kind
    Specifies whether the universe or the empty powerset has to be built.
  */
  explicit
  Pointset_Ask_Tell(dimension_type num_dimensions = 0,
		     Degenerate_Element kind = UNIVERSE);

  //! Ordinary copy-constructor.
  Pointset_Ask_Tell(const Pointset_Ask_Tell& y);

  /*! \brief
    If \p ph is nonempty, builds a powerset containing only \p ph.
    Builds the empty powerset otherwise.
  */
  explicit Pointset_Ask_Tell(const PS& ph);

  /*! \brief
    Copy-constructor allowing a source powerset with elements of a
    different polyhedron kind.
  */
  template <typename QH>
  explicit Pointset_Ask_Tell(const Pointset_Ask_Tell<QH>& y);

  /*! \brief
    Creates a Pointset_Ask_Tell with a single polyhedron
    with the same information contents as \p cs.
  */
  explicit Pointset_Ask_Tell(const Constraint_System& cs);

  //! Creates a Pointset_Ask_Tell with a single polyhedron
  //! with the same information contents as \p cgs.
  explicit Pointset_Ask_Tell(const Congruence_System& cgs);

  //@} // Constructors and Destructor

  //! \name Member Functions that Do Not Modify the Ask_Tell of Polyhedra
  //@{

  //! Returns the dimension of the vector space enclosing \p *this.
  dimension_type space_dimension() const;

  /*! \brief
    Returns <CODE>true</CODE> if and only if \p *this geometrically
    covers \p y, i.e., if any point (in some element) of \p y is also
    a point (of some element) of \p *this.

    \exception std::invalid_argument
    Thrown if \p *this and \p y are topology-incompatible or
    dimension-incompatible.

    \warning
    This may be <EM>really</EM> expensive!
  */
  bool geometrically_covers(const Pointset_Ask_Tell& y) const;

  /*! \brief
    Returns <CODE>true</CODE> if and only if \p *this is geometrically
    equal to \p y, i.e., if (the elements of) \p *this and \p y
    contain the same set of points.

    \exception std::invalid_argument
    Thrown if \p *this and \p y are topology-incompatible or
    dimension-incompatible.

    \warning
    This may be <EM>really</EM> expensive!
  */
  bool geometrically_equals(const Pointset_Ask_Tell& y) const;

  /*! \brief
    Returns a lower bound to the total size in bytes of the memory
    occupied by \p *this.
  */
  memory_size_type total_memory_in_bytes() const;

  /*! \brief
    Returns a lower bound to the size in bytes of the memory
    managed by \p *this.
  */
  memory_size_type external_memory_in_bytes() const;

  //! Checks if all the invariants are satisfied.
  bool OK() const;

  //@} // Member Functions that Do Not Modify the Ask_Tell

  //! \name Space Dimension Preserving Member Functions that May Modify the Ask_Tell of Polyhedra
  //@{

  //! Adds to \p *this the disjunct \p ph.
  /*!
    \exception std::invalid_argument
    Thrown if \p *this and \p ph are dimension-incompatible.
  */
  void add_disjunct(const PS& ph);

  //! Intersects \p *this with constraint \p c.
  /*!
    \exception std::invalid_argument
    Thrown if \p *this and constraint \p c are topology-incompatible
    or dimension-incompatible.
  */
  void add_constraint(const Constraint& c);

  //! Intersects \p *this with the constraint \p c, minimizing the result.
  /*!
    \return
    <CODE>false</CODE> if and only if the result is empty.

    \exception std::invalid_argument
    Thrown if \p *this and \p c are topology-incompatible or
    dimension-incompatible.
  */
  bool add_constraint_and_minimize(const Constraint& c);

  //! Intersects \p *this with the constraints in \p cs.
  /*!
    \param cs
    The constraints to intersect with.

    \exception std::invalid_argument
    Thrown if \p *this and \p cs are topology-incompatible or
    dimension-incompatible.
  */
  void add_constraints(const Constraint_System& cs);

  /*! \brief
    Intersects \p *this with the constraints in \p cs,
    minimizing the result.

    \return
    <CODE>false</CODE> if and only if the result is empty.

    \param cs
    The constraints to intersect with.

    \exception std::invalid_argument
    Thrown if \p *this and \p cs are topology-incompatible or
    dimension-incompatible.
  */
  bool add_constraints_and_minimize(const Constraint_System& cs);

  /*! \brief
    Assign to \p *this the result of (recursively) merging together
    the pairs of polyhedra whose poly-hull is the same as their
    set-theoretical union.

    On exit, for all the pairs \f$\cP\f$, \f$\cQ\f$ of different polyhedra
    in \p *this, we have \f$\cP \uplus \cQ \neq \cP \union \cQ\f$.
  */
  void pairwise_reduce();

  /*! \brief
    Assigns to \p *this the result of applying the
    \ref pps_bgp99_extrapolation "BGP99 extrapolation operator"
    to \p *this and \p y, using the widening function \p wf
    and the cardinality threshold \p max_disjuncts.

    \param y
    A finite powerset of polyhedra.
    It <EM>must</EM> definitely entail \p *this;

    \param wf
    The widening function to be used on polyhedra objects. It is obtained
    from the corresponding widening method by using the helper function
    Parma_Polyhedra_Library::widen_fun_ref. Legal values are, e.g.,
    <CODE>widen_fun_ref(&Polyhedron::H79_widening_assign)</CODE> and
    <CODE>widen_fun_ref(&Polyhedron::limited_H79_extrapolation_assign, cs)</CODE>;

    \param max_disjuncts
    The maximum number of disjuncts occurring in the powerset \p *this
    <EM>before</EM> starting the computation. If this number is exceeded,
    some of the disjuncts in \p *this are collapsed (i.e., joined together).

    \exception std::invalid_argument
    Thrown if \p *this and \p y are topology-incompatible or
    dimension-incompatible.

    For a description of the extrapolation operator,
    see \ref BGP99 "[BGP99]" and \ref BHZ03b "[BHZ03b]".
  */
  template <typename Widening>
  void BGP99_extrapolation_assign(const Pointset_Ask_Tell& y,
				  Widening wf,
				  unsigned max_disjuncts);

  /*! \brief
    Assigns to \p *this the result of computing the
    \ref pps_certificate_widening "BHZ03-widening"
    between \p *this and \p y, using the widening function \p wf
    certified by the convergence certificate \p Cert.

    \param y
    The finite powerset of polyhedra computed in the previous iteration step.
    It <EM>must</EM> definitely entail \p *this;

    \param wf
    The widening function to be used on polyhedra objects.
    It is obtained from the corresponding widening method by using
    the helper function widen_fun_ref. Legal values are, e.g.,
    <CODE>widen_fun_ref(&Polyhedron::H79_widening_assign)</CODE> and
    <CODE>widen_fun_ref(&Polyhedron::limited_H79_extrapolation_assign, cs)</CODE>.

    \exception std::invalid_argument
    Thrown if \p *this and \p y are topology-incompatible or
    dimension-incompatible.

    \warning
    In order to obtain a proper widening operator, the template parameter
    \p Cert should be a finite convergence certificate for the base-level
    widening function \p wf; otherwise, an extrapolation operator is
    obtained.
    For a description of the methods that should be provided
    by \p Cert, see BHRZ03_Certificate or H79_Certificate.
  */
  template <typename Cert, typename Widening>
  void BHZ03_widening_assign(const Pointset_Ask_Tell& y, Widening wf);

  //@} // Space Dimension Preserving Member Functions that May Modify [...]

  //! \name Member Functions that May Modify the Dimension of the Vector Space
  //@{

  /*! \brief
    The assignment operator
    (\p *this and \p y can be dimension-incompatible).
  */
  Pointset_Ask_Tell& operator=(const Pointset_Ask_Tell& y);

  /*! \brief
    Assignment operator allowing a source powerset with elements of a
    different polyhedron kind
    (\p *this and \p y can be dimension-incompatible).
  */
  template <typename QH>
  Pointset_Ask_Tell& operator=(const Pointset_Ask_Tell<QH>& y);

  //! Swaps \p *this with \p y.
  void swap(Pointset_Ask_Tell& y);

  /*! \brief
    Adds \p m new dimensions to the vector space containing \p *this
    and embeds each polyhedron in \p *this in the new space.
  */
  void add_space_dimensions_and_embed(dimension_type m);

  /*! \brief
    Adds \p m new dimensions to the vector space containing \p *this
    without embedding the polyhedra in \p *this in the new space.
  */
  void add_space_dimensions_and_project(dimension_type m);

  //! Assigns to \p *this the intersection of \p *this and \p y.
  /*!
    The result is obtained by intersecting each polyhedron in \p *this
    with each polyhedron in \p y and collecting all these intersections.
  */
  void intersection_assign(const Pointset_Ask_Tell& y);

  //! Assigns to \p *this the difference of \p *this and \p y.
  /*!
    The result is obtained by computing the
    \ref Convex_Polyhedral_Difference "poly-difference" of each polyhedron
    in \p *this with each polyhedron in \p y and collecting all these
    differences.
  */
  void poly_difference_assign(const Pointset_Ask_Tell& y);

  //! Assigns to \p *this the concatenation of \p *this and \p y.
  /*!
    The result is obtained by computing the pairwise
    \ref Concatenating_Polyhedra "concatenation" of each polyhedron
    in \p *this with each polyhedron in \p y.
  */
  void concatenate_assign(const Pointset_Ask_Tell& y);

  /*! \brief
    Assigns to \p *this the result of computing the
    \ref Time_Elapse_Operator "time-elapse" between \p *this and \p y.

    The result is obtained by computing the pairwise
    \ref Time_Elapse_Operator "time elapse" of each polyhedron
    in \p *this with each polyhedron in \p y.
  */
  void time_elapse_assign(const Pointset_Ask_Tell& y);

  //! Removes all the specified space dimensions.
  /*!
    \param to_be_removed
    The set of Variable objects corresponding to the space dimensions
    to be removed.

    \exception std::invalid_argument
    Thrown if \p *this is dimension-incompatible with one of the
    Variable objects contained in \p to_be_removed.
  */
  void remove_space_dimensions(const Variables_Set& to_be_removed);

  /*! \brief
    Removes the higher space dimensions so that the resulting space
    will have dimension \p new_dimension.

    \exception std::invalid_argument
    Thrown if \p new_dimensions is greater than the space dimension
    of \p *this.
  */
  void remove_higher_space_dimensions(dimension_type new_dimension);

  /*! \brief
    Remaps the dimensions of the vector space according to
    a partial function.

    See also Polyhedron::map_space_dimensions.
  */
  template <typename Partial_Function>
  void map_space_dimensions(const Partial_Function& pfunc);

  //@} // Member Functions that May Modify the Dimension of the Vector Space

public:
  typedef typename Base::size_type size_type;
  typedef typename Base::value_type value_type;
  typedef typename Base::iterator iterator;
  typedef typename Base::const_iterator const_iterator;
  typedef typename Base::reverse_iterator reverse_iterator;
  typedef typename Base::const_reverse_iterator const_reverse_iterator;

  PPL_OUTPUT_DECLARATIONS

#ifdef PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
  /*! \brief
    Loads from \p s an ASCII representation (as produced by
    ascii_dump(std::ostream&) const) and sets \p *this accordingly.
    Returns <CODE>true</CODE> if successful, <CODE>false</CODE> otherwise.
  */
#endif // defined(PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS)
  bool ascii_load(std::istream& s);

private:
  typedef typename Base::Sequence Sequence;
  typedef typename Base::Sequence_iterator Sequence_iterator;
  typedef typename Base::Sequence_const_iterator Sequence_const_iterator;

  //! The number of dimensions of the enclosing vector space.
  dimension_type space_dim;

  /*! \brief
    Assigns to \p *this the result of applying the BGP99 heuristics
    to \p *this and \p y, using the widening function \p wf.
  */
  template <typename Widening>
  void BGP99_heuristics_assign(const Pointset_Ask_Tell& y, Widening wf);

  //! Records in \p cert_ms the certificates for this set of polyhedra.
  template <typename Cert>
  void collect_certificates(std::map<Cert, size_type,
		                     typename Cert::Compare>& cert_ms) const;

  /*! \brief
    Returns <CODE>true</CODE> if and only if the current set of polyhedra
    is stabilizing with respect to the multiset of certificates \p y_cert_ms.
  */
  template <typename Cert>
  bool is_cert_multiset_stabilizing(const std::map<Cert, size_type,
                                                   typename Cert::Compare>&
				    y_cert_ms) const;

  // FIXME: here it should be enough to befriend the template constructor
  //   template <typename QH>
  //   Pointset_Ask_Tell(const Pointset_Ask_Tell<QH>&)
  // but, apparently, this cannot be done.
  // As a workaround, we could use
  //   friend class Pointset_Ask_Tell<NNC_Polyhedron>
  // but GCC 3.3.3 has a bug that causes its rejection.
  // So, temporarily, we make all Pointset_Ask_Tell's friends of each other.
  template <typename QH> friend class Pointset_Ask_Tell;
};


namespace Parma_Polyhedra_Library {

// CHECKME: according to the Intel compiler, the declaration of the
// following specialization (of the class template parameter) should come
// before the declaration of the corresponding full specialization
// (where the member template parameter is specialized too).
template <>
template <typename QH>
Pointset_Ask_Tell<NNC_Polyhedron>
::Pointset_Ask_Tell(const Pointset_Ask_Tell<QH>& y);

// CHECKME: according to the Intel compiler, the declaration of the
// following specialization (of the class template parameter) should come
// before the declaration of the corresponding full specialization
// (where the member template parameter is specialized too).
template <>
template <typename QH>
Pointset_Ask_Tell<C_Polyhedron>
::Pointset_Ask_Tell(const Pointset_Ask_Tell<QH>& y);

// Non-inline full specializations should be declared here
// so as to inhibit multiple instantiations of the generic template.
template <>
template <>
Pointset_Ask_Tell<NNC_Polyhedron>
::Pointset_Ask_Tell(const Pointset_Ask_Tell<C_Polyhedron>& y);

template <>
template <>
Pointset_Ask_Tell<C_Polyhedron>
::Pointset_Ask_Tell(const Pointset_Ask_Tell<NNC_Polyhedron>& y);

template <>
void
Pointset_Ask_Tell<NNC_Polyhedron>
::poly_difference_assign(const Pointset_Ask_Tell& y);

template <>
bool
Pointset_Ask_Tell<NNC_Polyhedron>
::geometrically_covers(const Pointset_Ask_Tell& y) const;

} // namespace Parma_Polyhedra_Library


namespace std {

//! Specializes <CODE>std::swap</CODE>.
/*! \relates Parma_Polyhedra_Library::Pointset_Ask_Tell */
template <typename PS>
void swap(Parma_Polyhedra_Library::Pointset_Ask_Tell<PS>& x,
	  Parma_Polyhedra_Library::Pointset_Ask_Tell<PS>& y);

} // namespace std

#include "Pointset_Ask_Tell.inlines.hh"
#include "Pointset_Ask_Tell.templates.hh"

#endif // !defined(PPL_Pointset_Ask_Tell_defs_hh)
