/* BD_Shape<T>::Status class declaration.
   Copyright (C) 2001-2005 Roberto Bagnara <bagnara@cs.unipr.it>

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
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
USA.

For the most up-to-date information see the Parma Polyhedra Library
site: http://www.cs.unipr.it/ppl/ . */

#ifndef PPL_BD_Status_defs_hh
#define PPL_BD_Status_defs_hh 1

//! A conjunctive assertion about a BD_Shape<T> object.
/*!
  The assertions supported are:
  - <EM>zero-dim universe</EM>: the polyhedron is the zero-dimension
    vector space \f$\Rset^0 = \{\cdot\}\f$;
  - <EM>empty</EM>: the polyhedron is the empty set;
  - <EM>transitively closed</EM>: the BD_Shape object is transitively
    closed, so that all the constraints are as tight as possible.

  Not all the conjunctions of these elementary assertions constitute
  a legal Status.  In fact:
  - <EM>zero-dim universe</EM> excludes any other assertion;
  - <EM>empty</EM>: excludes any other assertion.
*/
class Status {
public:
  //! By default Status is the <EM>zero-dim universe</EM> assertion.
  Status();

  //! \name Test, remove or add an individual assertion from the conjunction.
  //@{
  bool test_zero_dim_univ() const;
  void reset_zero_dim_univ();
  void set_zero_dim_univ();

  bool test_empty() const;
  void reset_empty();
  void set_empty();

  bool test_transitively_closed() const;
  void reset_transitively_closed();
  void set_transitively_closed();
  //@}

  //! Checks if all the invariants are satisfied.
  bool OK() const;

  //! \brief
  //! Writes to \p s an ASCII representation of the internal
  //! representation of \p *this.
  void ascii_dump(std::ostream& s) const;

  //! \brief
  //! Loads from \p s an ASCII representation (as produced by \ref
  //! ascii_dump) and sets \p *this accordingly.  Returns <CODE>true</CODE>
  //! if successful, <CODE>false</CODE> otherwise.
  bool ascii_load(std::istream& s);

private:
  //! Status is implemented by means of a finite bitset.
  typedef unsigned int flags_t;

  //! \name Bit-masks for the individual assertions.
  //@{
  static const flags_t ZERO_DIM_UNIV       = 0U;
  static const flags_t EMPTY               = 1U << 0;
  static const flags_t TRANSITIVELY_CLOSED = 1U << 1;
  //@}

  //! This holds the current bitset.
  flags_t flags;

  //! Construct from a bit-mask.
  Status(flags_t mask);

  //! Check whether <EM>all</EM> bits in \p mask are set.
  bool test_all(flags_t mask) const;

  //! Check whether <EM>at least one</EM> bit in \p mask is set.
  bool test_any(flags_t mask) const;

  //! Set the bits in \p mask.
  void set(flags_t mask);

  //! Reset the bits in \p mask.
  void reset(flags_t mask);
};

#endif // !defined(PPL_BD_Status_defs_hh)
