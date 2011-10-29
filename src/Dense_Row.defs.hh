/* Dense_Row class declaration.
   Copyright (C) 2001-2010 Roberto Bagnara <bagnara@cs.unipr.it>
   Copyright (C) 2010-2011 BUGSENG srl (http://bugseng.com)

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

#ifndef PPL_Dense_Row_defs_hh
#define PPL_Dense_Row_defs_hh 1

#include "Dense_Row.types.hh"

#include "globals.defs.hh"

#include "Sparse_Row.types.hh"
#include "Row_Flags.defs.hh"
#include "Coefficient.defs.hh"
#include <vector>
#include <limits>

#ifdef PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
//! A finite sequence of coefficients.
/*! \ingroup PPL_CXX_interface */
#endif // defined(PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS)
class Parma_Polyhedra_Library::Dense_Row {
public:
  typedef Row_Flags Flags;

  class iterator;
  class const_iterator;

  //! Pre-constructs a row: construction must be completed by construct().
  Dense_Row();

  explicit Dense_Row(const Sparse_Row& row);

  explicit Dense_Row(Flags f);

  //! Tight constructor: resizing may require reallocation.
  /*!
    Constructs a row with size and capacity \p sz, and flags \p f.
  */
  Dense_Row(dimension_type sz, Flags f);

  //! Sizing constructor with capacity.
  /*!
    \param sz
    The size of the row that will be constructed;

    \param capacity
    The capacity of the row that will be constructed;

    \param f
    Flags for the row that will be constructed.

    The row that is constructed has storage for \p capacity elements,
    \p sz of which are default-constructed now.
    The row flags are set to \p f.
  */
  Dense_Row(dimension_type sz, dimension_type capacity, Flags f);

  //! Ordinary copy constructor.
  Dense_Row(const Dense_Row& y);

  //! Copy constructor with specified capacity.
  /*!
    It is assumed that \p capacity is greater than or equal to
    the size of \p y.
  */
  Dense_Row(const Dense_Row& y, dimension_type capacity);

  //! Copy constructor with specified size and capacity.
  /*!
    It is assumed that \p sz is greater than or equal to the size of \p y
    and, of course, that \p sz is less than or equal to \p capacity.
  */
  Dense_Row(const Dense_Row& y, dimension_type sz, dimension_type capacity);

  //! Destructor.
  ~Dense_Row();

  //! Assignment operator.
  Dense_Row& operator=(const Dense_Row& y);

  //! Assignment operator.
  Dense_Row& operator=(const Sparse_Row& y);

  //! Swaps \p *this with \p y.
  void swap(Dense_Row& y);

  //! Resizes the row to \p sz.
  void resize(dimension_type sz);

  //! Resizes the row to \p sz, with capacity \p capacity.
  void resize(dimension_type sz, dimension_type capacity);

  //! Expands the row to size \p new_size.
  /*!
    Adds new positions to the implementation of the row
    obtaining a new row with size \p new_size.
    It is assumed that \p new_size is between the current size
    and capacity of the row.
  */
  void expand_within_capacity(dimension_type new_size);

  //! Shrinks the row by erasing elements at the end.
  /*!
    Destroys elements of the row implementation
    from position \p new_size to the end.
    It is assumed that \p new_size is not greater than the current size.
  */
  void shrink(dimension_type new_size);

  //! Returns the flags of \p *this.
  const Flags flags() const;

  //! Sets \p f as the flags of \p *this.
  void set_flags(Flags f);

  //! Returns the size() of the largest possible Dense_Row.
  static dimension_type max_size();

  //! Gives the number of coefficients currently in use.
  dimension_type size() const;

  //! \name Subscript operators
  //@{
  //! Returns a reference to the element of the row indexed by \p k.
  Coefficient& operator[](dimension_type k);

  //! Returns a constant reference to the element of the row indexed by \p k.
  Coefficient_traits::const_reference operator[](dimension_type k) const;
  //@} // Subscript operators

  //! Normalizes the modulo of coefficients so that they are mutually prime.
  /*!
    Computes the Greatest Common Divisor (GCD) among the elements of
    the row and normalizes them by the GCD itself.
  */
  void normalize();

  //! Swaps the i-th element with the j-th element.
  //! Provided for compatibility with Sparse_Row
  void swap(dimension_type i, dimension_type j);

  //! Swaps the element pointed to by i with the element pointed to by j.
  //! Provided for compatibility with Sparse_Row
  void swap(iterator i, iterator j);

  iterator begin();
  const_iterator begin() const;

  iterator end();
  const_iterator end() const;

  //! Resets the i-th element to 0.
  //! Provided for compatibility with Sparse_Row
  void reset(dimension_type i);

  //! Resets the elements [first,last) to 0.
  //! Provided for compatibility with Sparse_Row
  void reset(dimension_type first, dimension_type last);

  //! Resets the element pointed to by itr to 0.
  //! Provided for compatibility with Sparse_Row.
  iterator reset(iterator itr);

  //! Gets the i-th element.
  //! Provided for compatibility with Sparse_Row.
  Coefficient_traits::const_reference get(dimension_type i) const;

  //! Provided for compatibility with Sparse_Row.
  iterator find(dimension_type i);

  //! Provided for compatibility with Sparse_Row.
  const_iterator find(dimension_type i) const;

  //! Provided for compatibility with Sparse_Row.
  iterator find(iterator itr, dimension_type i);

  //! Provided for compatibility with Sparse_Row.
  const_iterator find(const_iterator itr, dimension_type i) const;

  //! Provided for compatibility with Sparse_Row.
  iterator lower_bound(dimension_type i);

  //! Provided for compatibility with Sparse_Row.
  const_iterator lower_bound(dimension_type i) const;

  //! Provided for compatibility with Sparse_Row.
  iterator lower_bound(iterator itr, dimension_type i);

  //! Provided for compatibility with Sparse_Row.
  const_iterator lower_bound(const_iterator itr, dimension_type i) const;

  //! Provided for compatibility with Sparse_Row.
  iterator insert(dimension_type i, Coefficient_traits::const_reference x);

  //! Provided for compatibility with Sparse_Row.
  iterator insert(dimension_type i);

  //! Provided for compatibility with Sparse_Row.
  iterator insert(iterator itr, dimension_type i,
                       Coefficient_traits::const_reference x);

  //! Provided for compatibility with Sparse_Row.
  iterator insert(iterator itr, dimension_type i);

  //! Calls g(x[i],y[i]), for each i.
  /*!
    \param y
    The row that will be combined with *this.

    \param f
    A functor that should take a Coefficient&.
    f(c1) must be equivalent to g(c1, 0).

    \param g
    A functor that should take a Coefficient& and a
    Coefficient_traits::const_reference.
    g(c1, c2) must do nothing when c1 is zero.

    This method takes \f$O(n)\f$ time.

    \note
    The functors will only be called when necessary, assuming the requested
    properties hold.

    \see combine_needs_second
    \see combine
  */
  template <typename Func1, typename Func2>
  void combine_needs_first(const Dense_Row& y,
                           const Func1& f, const Func2& g);

  //! Calls g(x[i],y[i]), for each i.
  /*!
    \param y
    The row that will be combined with *this.

    \param g
    A functor that should take a Coefficient& and a
    Coefficient_traits::const_reference.
    g(c1, 0) must do nothing, for every c1.

    \param h
    A functor that should take a Coefficient& and a
    Coefficient_traits::const_reference.
    h(c1, c2) must be equivalent to g(c1, c2) when c1 is zero.

    This method takes \f$O(n)\f$ time.

    \note
    The functors will only be called when necessary, assuming the requested
    properties hold.

    \see combine_needs_first
    \see combine
  */
  template <typename Func1, typename Func2>
  void combine_needs_second(const Dense_Row& y,
                            const Func1& g, const Func2& h);

  //! Calls g(x[i],y[i]), for each i.
  /*!
    \param y
    The row that will be combined with *this.

    \param f
    A functor that should take a Coefficient&.
    f(c1) must be equivalent to g(c1, 0).

    \param g
    A functor that should take a Coefficient& and a
    Coefficient_traits::const_reference.
    g(c1, c2) must do nothing when both c1 and c2 are zero.

    \param h
    A functor that should take a Coefficient& and a
    Coefficient_traits::const_reference.
    h(c1, c2) must be equivalent to g(c1, c2) when c1 is zero.

    This method takes \f$O(n)\f$ time.

    \note
    The functors will only be called when necessary, assuming the requested
    properties hold.

    \see combine_needs_first
    \see combine_needs_second
  */
  template <typename Func1, typename Func2, typename Func3>
  void combine(const Dense_Row& y,
               const Func1& f, const Func2& g, const Func3& h);

  //! Executes <CODE>(*this)[i] = (*this)[i]*coeff1 + y[i]*coeff2</CODE>, for
  //! each i.
  /*!
    \param y
    The row that will be combined with *this.

    \param coeff1
    The coefficient used for elements of *this.

    \param coeff2
    The coefficient used for elements of y.

    This method takes \f$O(n)\f$ time.

    \see combine_needs_first
    \see combine_needs_second
    \see combine
  */
  void linear_combine(const Dense_Row& y,
                      Coefficient_traits::const_reference coeff1,
                      Coefficient_traits::const_reference coeff2);

  PPL_OUTPUT_DECLARATIONS

  /*! \brief
    Loads from \p s an ASCII representation (as produced by
    ascii_dump(std::ostream&) const) and sets \p *this accordingly.
    Returns <CODE>true</CODE> if successful, <CODE>false</CODE> otherwise.
  */
  bool ascii_load(std::istream& s);

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
    Returns the total size in bytes of the memory occupied by \p *this,
    provided the capacity of \p *this is given by \p capacity.
  */
  memory_size_type total_memory_in_bytes(dimension_type capacity) const;

  /*! \brief
    Returns the size in bytes of the memory managed by \p *this,
    provided the capacity of \p *this is given by \p capacity.
  */
  memory_size_type external_memory_in_bytes(dimension_type capacity) const;

  //! Checks if all the invariants are satisfied.
  bool OK() const;

  /*! \brief
    Checks if all the invariants are satisfied and that the actual
    size and capacity match the values provided as arguments.
  */
  bool OK(dimension_type row_size, dimension_type row_capacity) const;

private:
  void init(const Sparse_Row& row);

  void destroy();

  struct Impl {

    Impl();

    ~Impl();

    //! The number of coefficients in the row.
    dimension_type size;

    //! The capacity of the row.
    dimension_type capacity;

    //! The flags of this row.
    Row_Flags flags;

    //! The vector of coefficients.
    //! An empty vector may be stored as NULL instead of using a valid pointer.
    Coefficient* vec;
  };

  Impl impl;

  //! Returns the capacity of the row.
  dimension_type capacity() const;
};

class Parma_Polyhedra_Library::Dense_Row::iterator {
public:

  typedef std::bidirectional_iterator_tag iterator_category;
  typedef Coefficient value_type;
  typedef ptrdiff_t difference_type;
  typedef value_type* pointer;
  typedef value_type& reference;

  iterator();
  iterator(Dense_Row& row1, dimension_type i1);

  Coefficient& operator*();
  Coefficient_traits::const_reference operator*() const;

  //! Returns the index of the element pointed to by \c *this.
  /*!
    If itr is a valid iterator for row, <CODE>row[itr.index()]</CODE> is
    equivalent to *itr.

    \returns the index of the element pointed to by \c *this.
  */
  dimension_type index() const;

  iterator& operator++();
  iterator operator++(int);

  iterator& operator--();
  iterator operator--(int);

  bool operator==(const iterator& x) const;
  bool operator!=(const iterator& x) const;

  operator const_iterator() const;

  bool OK() const;

private:
  Dense_Row* row;
  dimension_type i;
};

class Parma_Polyhedra_Library::Dense_Row::const_iterator {
public:

  typedef const Coefficient value_type;
  typedef ptrdiff_t difference_type;
  typedef value_type* pointer;
  typedef Coefficient_traits::const_reference reference;

  const_iterator();
  const_iterator(const Dense_Row& row1, dimension_type i1);

  Coefficient_traits::const_reference operator*() const;

  //! Returns the index of the element pointed to by \c *this.
  /*!
    If itr is a valid iterator for row, <CODE>row[itr.index()]</CODE> is
    equivalent to *itr.

    \returns the index of the element pointed to by \c *this.
  */
  dimension_type index() const;

  const_iterator& operator++();
  const_iterator operator++(int);

  const_iterator& operator--();
  const_iterator operator--(int);

  bool operator==(const const_iterator& x) const;
  bool operator!=(const const_iterator& x) const;

  bool OK() const;

private:
  const Dense_Row* row;
  dimension_type i;
};


namespace Parma_Polyhedra_Library {

//! Returns <CODE>true</CODE> if and only if \p x and \p y are equal.
/*! \relates Dense_Row */
bool operator==(const Dense_Row& x, const Dense_Row& y);

//! Returns <CODE>true</CODE> if and only if \p x and \p y are different.
/*! \relates Dense_Row */
bool operator!=(const Dense_Row& x, const Dense_Row& y);

} // namespace Parma_Polyhedra_Library


namespace std {

#ifdef PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
//! Specializes <CODE>std::swap</CODE>.
/*! \relates Parma_Polyhedra_Library::Dense_Row */
#endif // defined(PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS)
void swap(Parma_Polyhedra_Library::Dense_Row& x,
          Parma_Polyhedra_Library::Dense_Row& y);

#ifdef PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
//! Specializes <CODE>std::iter_swap</CODE>.
/*! \relates Parma_Polyhedra_Library::Dense_Row */
#endif // defined(PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS)
void iter_swap(std::vector<Parma_Polyhedra_Library::Dense_Row>::iterator x,
               std::vector<Parma_Polyhedra_Library::Dense_Row>::iterator y);

} // namespace std

#include "Dense_Row.inlines.hh"
#include "Dense_Row.templates.hh"

#endif // !defined(PPL_Dense_Row_defs_hh)
