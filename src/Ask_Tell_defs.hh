/* Ask_Tell class declaration.
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

#ifndef PPL_Ask_Tell_defs_hh
#define PPL_Ask_Tell_defs_hh

#include "Ask_Tell_types.hh"
#include "iterator_to_const_defs.hh"
#include "globals_types.hh"
#include <iosfwd>
#include <list>

namespace Parma_Polyhedra_Library {

//! Swaps \p x with \p y.
/*! \relates Ask_Tell */
template <typename D>
void swap(Ask_Tell<D>& x, Ask_Tell<D>& y);

//! Returns <CODE>true</CODE> if and only if \p x and \p y are equivalent.
/*! \relates Ask_Tell */
template <typename D>
bool
operator==(const Ask_Tell<D>& x, const Ask_Tell<D>& y);

//! Returns <CODE>true</CODE> if and only if \p x and \p y are not equivalent.
/*! \relates Ask_Tell */
template <typename D>
bool
operator!=(const Ask_Tell<D>& x, const Ask_Tell<D>& y);

namespace IO_Operators {

//! Output operator.
/*! \relates Parma_Polyhedra_Library::Ask_Tell */
template <typename D>
std::ostream&
operator<<(std::ostream&, const Ask_Tell<D>&);

} // namespace IO_Operators

} // namespace Parma_Polyhedra_Library

//! A pair of <EM>ask</EM> and <EM>tell</EM> descriptions.
/*! \ingroup PPL_CXX_interface */
template <typename D>
class Parma_Polyhedra_Library::Ask_Tell_Pair {
public:
  //! Pair constructor.
  Ask_Tell_Pair(const D& ask, const D& tell);

  //! Const accessor to the <EM>ask</EM> component.
  const D& ask() const;

  //! Non-const accessor to the <EM>ask</EM> component.
  D& ask();

  //! Const accessor to the <EM>ask</EM> component.
  const D& tell() const;

  //! Non-const accessor to the <EM>tell</EM> component.
  D& tell();

  /*! \brief
    Returns <CODE>true</CODE> if \p *this definitely entails \p y.
    Returns <CODE>false</CODE> if \p *this may not entail \p y
    (i.e., if \p *this does not entail \p y or if entailment could
    not be decided).
  */
  bool definitely_entails(const Ask_Tell_Pair& y) const;

private:
  //! The <EM>ask</EM> component.
  D a;

  //! The <EM>tell</EM> component.
  D t;
};

//! The ask and tell construction on a base-level domain.
/*!
  This class offers a generic implementation of <EM>ask-and-tell
  constraint systems</EM> as defined in \ref Bag98 "[Bag98]".
*/
template <typename D>
class Parma_Polyhedra_Library::Ask_Tell {
public:
  //! \name Constructors and Destructor
  //@{

  /*! \brief
    Default constructor: builds the top of the ask-and-tell constraint
    system (i.e., the empty system).
  */
  Ask_Tell();

  //! Copy constructor.
  Ask_Tell(const Ask_Tell& y);

  /*! \brief
    If \p p is not top, builds an ask-and-tell system containing only \p p.
    Builds the empty system otherwise.
  */
  explicit Ask_Tell(const Ask_Tell_Pair<D>& p);

  /*! \brief
    If \p ask and \p tell do not constitute a top pair, builds an
    ask-and-tell system containing only that pair.
    Builds the empty system otherwise.
  */
  Ask_Tell(const D& ask, const D& tell);

  //! Destructor.
  ~Ask_Tell();

  //@} // Constructors and Destructor

  //! \name Member Functions that Do Not Modify the Ask_Tell Object
  //@{

  /*! \brief
    Returns <CODE>true</CODE> if \p *this definitely entails \p y.
    Returns <CODE>false</CODE> if \p *this may not entail \p y
    (i.e., if \p *this does not entail \p y or if entailment could
    not be decided).
  */
  bool definitely_entails(const Ask_Tell& y) const;

  /*! \brief
    Returns <CODE>true</CODE> if and only if \p *this is the top
    element of the ask-and-tell constraint system (i.e., it
    represents the universe).
  */
  bool is_top() const;

  /*! \brief
    Returns <CODE>true</CODE> if and only if \p *this is the bottom
    element of the ask-and-tell constraint system (i.e., it
    represents the empty set).
  */
  bool is_bottom() const;

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

  /*! \brief
    Returns a 32-bit hash code for \p *this.

    If \p x and \p y are such that <CODE>x == y</CODE>,
    then <CODE>x.hash_code() == y.hash_code()</CODE>.
  */
  int32_t hash_code() const;

  //! Checks if all the invariants are satisfied.
  bool OK() const;

  //@} // Member Functions that Do Not Modify the Ask_Tell Object

protected:
  //! An ask-tell agent is composed of pairs.
  typedef Ask_Tell_Pair<D> Pair;

  //! An ask-tell agent is implemented as a sequence of ask-tell pairs.
  /*!
    The particular sequence employed must support efficient deletion
    in any position and efficient back insertion.
  */
  typedef std::list<Ask_Tell_Pair<D> > Sequence;

  //! Alias for the low-level iterator on the pairs.
  typedef typename Sequence::iterator Sequence_iterator;

  //! Alias for the low-level %const_iterator on the pairs.
  typedef typename Sequence::const_iterator Sequence_const_iterator;

  //! The sequence container holding the pairs/
  Sequence sequence;

  //! If <CODE>true</CODE>, \p *this is normalized.
  mutable bool normalized;

public:
  // Sequence manipulation types, accessors and modifiers
  typedef typename Sequence::size_type size_type;
  typedef typename Sequence::value_type value_type;

  /*! \brief
    Alias for a <EM>read-only</EM> bidirectional %iterator on the
    pairs an Ask_Tell object.

    By using this iterator type, the pairs cannot be overwritten,
    but they can be removed using methods
    <CODE>drop_pair(iterator position)</CODE> and
    <CODE>drop_pairs(iterator first, iterator last)</CODE>,
    while still ensuring a correct handling of normalization.
  */
  typedef iterator_to_const<Sequence> iterator;

  //! A bidirectional %const_iterator on the disjuncts of a Powerset element.
  typedef const_iterator_to_const<Sequence> const_iterator;

  //! The reverse iterator type built from Powerset::iterator.
  typedef std::reverse_iterator<iterator> reverse_iterator;

  //! The reverse iterator type built from Powerset::const_iterator.
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

  //! \name Member Functions for the Direct Manipulation of Pairs
  //@{

  /*! \brief
    Normalizes the pairs in \p *this.

    This method is declared <CODE>const</CODE> because, even though
    normalization may change the syntactic representation of \p *this,
    its semantics will be unchanged.
  */
  void normalize() const;

  //! Returns the number of pairs.
  size_type size() const;

  //! Returns <CODE>true</CODE> if and only if there are no pairs in \p *this.
  bool empty() const;

  /*! \brief
    Returns an iterator pointing to the first pair, if \p *this
    is not empty; otherwise, returns the past-the-end iterator.
  */
  iterator begin();

  //! Returns the past-the-end iterator.
  iterator end();

  /*! \brief
    Returns a const_iterator pointing to the first pair, if \p *this
    is not empty; otherwise, returns the past-the-end const_iterator.
  */
  const_iterator begin() const;

  //! Returns the past-the-end const_iterator.
  const_iterator end() const;

  /*! \brief
    Returns a reverse_iterator pointing to the last pair, if \p *this
    is not empty; otherwise, returns the before-the-start reverse_iterator.
  */
  reverse_iterator rbegin();

  //! Returns the before-the-start reverse_iterator.
  reverse_iterator rend();

  /*! \brief
    Returns a const_reverse_iterator pointing to the last pair,
    if \p *this is not empty; otherwise, returns the before-the-start
    const_reverse_iterator.
  */
  const_reverse_iterator rbegin() const;

  //! Returns the before-the-start const_reverse_iterator.
  const_reverse_iterator rend() const;

  //! Adds to \p *this the pair \p p.
  Ask_Tell& add_pair(const Ask_Tell_Pair<D>& p);

  //! Adds to \p *this the pair constituted by \p ask and \p tell.
  Ask_Tell& add_pair(const D& ask, const D& tell);

  /*! \brief
    Drops the pair in \p *this pointed to by \p position, returning
    an iterator to the pair following \p position.
  */
  iterator drop_pair(iterator position);

  //! Drops all the pairs from \p first to \p last (excluded).
  void drop_pairs(iterator first, iterator last);

  //! Drops all the pairs, making \p *this an empty powerset.
  void clear();

  //@} // Member Functions for the Direct Manipulation of Pairs

  //! \name Member Functions that May Modify the Ask_Tell Object
  //@{

  /*! \brief
    The assignment operator.
    (\p *this and \p y can be dimension-incompatible.)
  */
  Ask_Tell& operator=(const Ask_Tell& y);

  //! Swaps \p *this with \p y.
  void m_swap(Ask_Tell& y);

  //! Assigns to \p *this an upper bound of \p *this and \p y.
  void upper_bound_assign(const Ask_Tell& y);

  //! Assigns to \p *this the meet of \p *this and \p y.
  void meet_assign(const Ask_Tell& y);

  //@} // Member Functions that May Modify the Ask_Tell element

protected:
  //! Returns <CODE>true</CODE> if and only if \p *this is normalized.
  bool is_normalized() const;

  void pair_insert(const D& a, const D& t);
  void pair_insert_good(const D& a, const D& t);

  /*
    Postcondition:
    the map is well formed and there are no two pairs x and y such that
    x.ASK.definitely_entails(y.ASK) && y.TELL.definitely_entails(x.TELL).
  */
  bool reduce();

  // Preconditions:
  //
  //     the map is well formed and the postcondition of reduce() is satisfied.
  //
  // Postconditions:
  //
  //     the map is well formed, the postcondition of reduce() is satisfied,
  //     and...
  //
  bool deduce();

  bool absorb();

  void deabsorb() const;

  /*! \brief
    Does the hard work of checking whether \p *this is normalized
    and returns <CODE>true</CODE> if and only if it is.
  */
  bool check_normalized() const;

protected:
  bool probe(const D& tellv, const D& askv) const;
};

#include "Ask_Tell_inlines.hh"
#include "Ask_Tell_templates.hh"

#endif // !defined(PPL_Ask_Tell_defs_hh)
