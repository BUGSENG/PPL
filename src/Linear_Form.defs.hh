/* Linear_Form class declaration.
   Copyright (C) 2001-2010 Roberto Bagnara <bagnara@cs.unipr.it>

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

#ifndef PPL_Linear_Form_defs_hh
#define PPL_Linear_Form_defs_hh 1

#include "Linear_Form.types.hh"
#include "Linear_Expression.types.hh"
#include "Variable.defs.hh"
#include "Box.types.hh"
#include "Float.defs.hh"
#include <vector>

namespace Parma_Polyhedra_Library {
// Put them in the namespace here to declare them friend later.

//! Returns the linear form \p f1 + \p f2.
/*! \relates Linear_Form */
template <typename C>
Linear_Form<C>
operator+(const Linear_Form<C>& f1, const Linear_Form<C>& f2);

//! Returns the linear form \p v + \p f.
/*! \relates Linear_Form */
template <typename C>
Linear_Form<C>
operator+(Variable v, const Linear_Form<C>& f);

//! Returns the linear form \p f + \p v.
/*! \relates Linear_Form */
template <typename C>
Linear_Form<C>
operator+(const Linear_Form<C>& f, Variable v);

//! Returns the linear form \p n + \p f.
/*! \relates Linear_Form */
template <typename C>
Linear_Form<C>
operator+(const C& n, const Linear_Form<C>& f);

//! Returns the linear form \p f + \p n.
/*! \relates Linear_Form */
template <typename C>
Linear_Form<C>
operator+(const Linear_Form<C>& f, const C& n);

//! Returns the linear form \p f.
/*! \relates Linear_Form */
template <typename C>
Linear_Form<C>
operator+(const Linear_Form<C>& f);

//! Returns the linear form - \p f.
/*! \relates Linear_Form */
template <typename C>
Linear_Form<C>
operator-(const Linear_Form<C>& f);

//! Returns the linear form \p f1 - \p f2.
/*! \relates Linear_Form */
template <typename C>
Linear_Form<C>
operator-(const Linear_Form<C>& f1, const Linear_Form<C>& f2);

//! Returns the linear form \p v - \p f.
/*! \relates Linear_Form */
template <typename C>
Linear_Form<C>
operator-(Variable v, const Linear_Form<C>& f);

//! Returns the linear form \p f - \p v.
/*! \relates Linear_Form */
template <typename C>
Linear_Form<C>
operator-(const Linear_Form<C>& f, Variable v);

//! Returns the linear form \p n - \p f.
/*! \relates Linear_Form */
template <typename C>
Linear_Form<C>
operator-(const C& n, const Linear_Form<C>& f);

//! Returns the linear form \p f - \p n.
/*! \relates Linear_Form */
template <typename C>
Linear_Form<C>
operator-(const Linear_Form<C>& f, const C& n);

//! Returns the linear form \p n * \p f.
/*! \relates Linear_Form */
template <typename C>
Linear_Form<C>
operator*(const C& n, const Linear_Form<C>& f);

//! Returns the linear form \p f * \p n.
/*! \relates Linear_Form */
template <typename C>
Linear_Form<C>
operator*(const Linear_Form<C>& f, const C& n);

//! Returns the linear form \p f1 + \p f2 and assigns it to \p e1.
/*! \relates Linear_Form */
template <typename C>
Linear_Form<C>&
operator+=(Linear_Form<C>& f1, const Linear_Form<C>& f2);

//! Returns the linear form \p f + \p v and assigns it to \p f.
/*! \relates Linear_Form
  \exception std::length_error
  Thrown if the space dimension of \p v exceeds
  <CODE>Linear_Form::max_space_dimension()</CODE>.
 */
template <typename C>
Linear_Form<C>&
operator+=(Linear_Form<C>& f, Variable v);

//! Returns the linear form \p f + \p n and assigns it to \p f.
/*! \relates Linear_Form */
template <typename C>
Linear_Form<C>&
operator+=(Linear_Form<C>& f, const C& n);

//! Returns the linear form \p f1 - \p f2 and assigns it to \p f1.
/*! \relates Linear_Form */
template <typename C>
Linear_Form<C>&
operator-=(Linear_Form<C>& f1, const Linear_Form<C>& f2);

//! Returns the linear form \p f - \p v and assigns it to \p f.
/*! \relates Linear_Form
  \exception std::length_error
  Thrown if the space dimension of \p v exceeds
  <CODE>Linear_Form::max_space_dimension()</CODE>.
 */
template <typename C>
Linear_Form<C>&
operator-=(Linear_Form<C>& f, Variable v);

//! Returns the linear form \p f - \p n and assigns it to \p f.
/*! \relates Linear_Form */
template <typename C>
Linear_Form<C>&
operator-=(Linear_Form<C>& f, const C& n);

//! Returns the linear form \p n * \p f and assigns it to \p f.
/*! \relates Linear_Form */
template <typename C>
Linear_Form<C>&
operator*=(Linear_Form<C>& f, const C& n);

//! Returns the linear form \p f / \p n and addigns it to \p f.
/*!
   \relates Linear_Form
   Performs the division of a linear form by a scalar. It is up to the user to
   ensure that division by 0 is not performed.
*/
template <typename C>
Linear_Form<C>&
operator/=(Linear_Form<C>& f, const C& n);

//! Returns <CODE>true</CODE> if and only if \p x and \p y are equal.
/*! \relates Linear_Form */
template <typename C>
bool
operator==(const Linear_Form<C>& x, const Linear_Form<C>& y);

//! Returns <CODE>true</CODE> if and only if \p x and \p y are different.
/*! \relates Linear_Form */
template <typename C>
bool
operator!=(const Linear_Form<C>& x, const Linear_Form<C>& y);

namespace IO_Operators {

//! Output operator.
/*! \relates Parma_Polyhedra_Library::Linear_Form */
template <typename C>
std::ostream& operator<<(std::ostream& s, const Linear_Form<C>& f);

} // namespace IO_Operators

} // namespace Parma_Polyhedra_Library


namespace std {

//! Specializes <CODE>std::swap</CODE>.
/*! \relates Parma_Polyhedra_Library::Linear_Form */
template <typename C>
void swap(Parma_Polyhedra_Library::Linear_Form<C>& x,
	  Parma_Polyhedra_Library::Linear_Form<C>& y);

} // namespace std

//! A linear form.
/*! \ingroup PPL_CXX_interface
  An object of the class Linear_Form represents the linear form
  \f[
    \sum_{i=0}^{n-1} a_i x_i + b
  \f]
  where \f$n\f$ is the dimension of the vector space,
  each \f$a_i\f$ is the coefficient
  of the \f$i\f$-th variable \f$x_i\f$
  and \f$b\f$ is the inhomogeneous term.
  The coefficiens and the inhomogeneous terms of the linear form
  are element of the template parameter \p C.

  \par How to build a linear form.

  A full set of functions is defined to provide a convenient interface
  for building complex linear forms starting from simpler ones
  and from objects of the classes Variable and \p C:
  available operators include unary negation,
  binary addition and subtraction,
  as well as multiplication by a Coefficient.
  The space dimension of a linear form is defined as the maximum
  space dimension of the arguments used to build it:
  in particular, the space dimension of a Variable <CODE>x</CODE>
  is defined as <CODE>x.id()+1</CODE>,
  whereas all the objects of the class \p C have space dimension zero.

  FIXME: the following needs rewriting.

  \par Example
  The following code builds the linear form \f$4x - 2y - z + 14\f$,
  having space dimension \f$3\f$:
  \code
  Linear_Form e = 4*x - 2*y - z + 14;
  \endcode
  Another way to build the same linear form is:
  \code
  Linear_Form e1 = 4*x;
  Linear_Form e2 = 2*y;
  Linear_Form e3 = z;
  Linear_Form e = Linear_Form(14);
  e += e1 - e2 - e3;
  \endcode
  Note that \p e1, \p e2 and \p e3 have space dimension 1, 2 and 3,
  respectively; also, in the fourth line of code, \p e is created
  with space dimension zero and then extended to space dimension 3
  in the fifth line.
*/
template <typename C>
class Parma_Polyhedra_Library::Linear_Form {
public:
  //! Default constructor: returns a copy of Linear_Form::zero().
  Linear_Form();

  //! Ordinary copy constructor.
  Linear_Form(const Linear_Form& f);

  //! Destructor.
  ~Linear_Form();

  //! Builds the linear form corresponding to the inhomogeneous term \p n.
  explicit Linear_Form(const C& n);

  //! Builds the linear form corresponding to the variable \p v.
  /*!
    \exception std::length_error
    Thrown if the space dimension of \p v exceeds
    <CODE>Linear_Form::max_space_dimension()</CODE>.
  */
  Linear_Form(Variable v);

  //! Builds a linear form approximating the linear expression \p e.
  Linear_Form(const Linear_Expression& e);

  //! Returns the maximum space dimension a Linear_Form can handle.
  static dimension_type max_space_dimension();

  //! Returns the dimension of the vector space enclosing \p *this.
  dimension_type space_dimension() const;

  //! Returns the coefficient of \p v in \p *this.
  const C& coefficient(Variable v) const;

  //! Returns the inhomogeneous term of \p *this.
  const C& inhomogeneous_term() const;

  //! Negates all the coefficients of \p *this.
  void negate();

  /*! \brief
    Returns a lower bound to the total size in bytes of the memory
    occupied by \p *this.
  */
  memory_size_type total_memory_in_bytes() const;

  //! Returns the size in bytes of the memory managed by \p *this.
  memory_size_type external_memory_in_bytes() const;

  PPL_OUTPUT_DECLARATIONS

  /*! \brief
    Loads from \p s an ASCII representation (as produced by
    ascii_dump(std::ostream&) const) and sets \p *this accordingly.
    Returns <CODE>true</CODE> if successful, <CODE>false</CODE> otherwise.
  */
  bool ascii_load(std::istream& s);

  //! Checks if all the invariants are satisfied.
  bool OK() const;

  //! Swaps \p *this with \p y.
  void swap(Linear_Form& y);

  // Floating point analysis related methods.

  /*! \brief
    Verifies if the linear form overflows.

    \return
    Returns <CODE>false</CODE> if all coefficients in \p lf are bounded,
    <CODE>true</CODE> otherwise.

    \p T must be the type of possibly unbounded quantities.
  */
  bool overflows() const;

  /*! \brief
    Computes the relative error associated to floating point computations
    that operate on a quantity that is overapproximated by \p *this.

    \param analyzed_format The floating point format used by the analyzed
    program.
    \param result Becomes the linear form corresponding to the relative
    error committed.

    This method makes <CODE>result</CODE> become a linear form
    obtained by evaluating the function \f$\varepsilon_{\mathbf{f}}(l)\f$
    on the linear form. This function is defined as:
    \f[
    \varepsilon_{\mathbf{f}}\left([a;b]+\sum_{v \in \cV}[a_{v};b_{v}]v\right)
    \defeq
    (\textrm{max}(|a|,|b|) \amifp [-\beta^{-\textrm{p}};\beta^{-\textrm{p}}])
    +
    \sum_{v \in \cV}(\textrm{max}(|a_{v}|,|b_{v}|)
    \amifp
    [-\beta^{-\textrm{p}};\beta^{-\textrm{p}}])v
    \f]
    where p is the fraction size in bits for the format \f$\mathbf{f}\f$ and
    \f$\beta\f$ the base.

    The result is undefined if \p T is not the type of an interval with
    floating point boundaries.
  */
  void relative_error(Floating_Point_Format analyzed_format,
                      Linear_Form& result) const;

  /*! \brief
    Makes \p result become an interval that overapproximates all the
    possible values of \p *this.

    \param oracle The FP_Oracle to be queried.
    \param result The linear form that will store the result.

    \return <CODE>true</CODE> if the operation was succesful,
    <CODE>false</CODE> otherwise (the possibility of failure
    depends on the oracle's implementation).

    \par Template type parameters

    - The class template parameter \p Target specifies the implementation
    of Concrete_Expression to be used.

    This method makes <CODE>result</CODE> become
    \f$\iota(lf)\rho^{\#}\f$, that is an interval defined as:
    \f[
    \iota\left(i + \sum_{v \in \cV}i_{v}v\right)\rho^{\#}
    \defeq
    i \asifp \left(\bigoplus_{v \in \cV}{}^{\#}i_{v} \amifp
    \rho^{\#}(v)\right)
    \f]
    where \f$\rho^{\#}(v)\f$ is an interval (provided by the oracle)
    that correctly approximates the value of \f$v\f$.

    The result is undefined if \p C is not the type of an interval with
    floating point boundaries.
  */
  template <typename Target>
  bool intervalize(const FP_Oracle<Target,C>& oracle, C& result) const;

private:
  //! The generic coefficient equal to the singleton zero.
  static C zero;

  //! Type of the container vector.
  typedef std::vector<C> vec_type;

  //! The container vector.
  vec_type vec;

  //! Implementation sizing constructor.
  /*!
    The bool parameter is just to avoid problems with
    the constructor Linear_Form(const C& n).
  */
  Linear_Form(dimension_type sz, bool);

  /*! \brief
    Builds the linear form corresponding to the difference of
    \p v and \p w.

    \exception std::length_error
    Thrown if the space dimension of \p v or the one of \p w exceed
    <CODE>Linear_Form::max_space_dimension()</CODE>.
  */
  Linear_Form(Variable v, Variable w);

  //! Gives the number of generic coefficients currently in use.
  dimension_type size() const;

  //! Extends the vector of \p *this to size \p sz.
  void extend(dimension_type sz);

  //! Returns a reference to \p vec[i].
  C& operator[](dimension_type i);

  //! Returns a const reference to \p vec[i].
  const C& operator[](dimension_type i) const;

  friend Linear_Form<C>
  operator+<C>(const Linear_Form<C>& f1, const Linear_Form<C>& f2);
  friend Linear_Form<C>
  operator+<C>(const C& n, const Linear_Form<C>& f);
  friend Linear_Form<C>
  operator+<C>(const Linear_Form<C>& f, const C& n);
  friend Linear_Form<C>
  operator+<C>(Variable v, const Linear_Form<C>& f);

  friend Linear_Form<C>
  operator-<C>(const Linear_Form<C>& f);

  friend Linear_Form<C>
  operator-<C>(const Linear_Form<C>& f1, const Linear_Form<C>& f2);
  friend Linear_Form<C>
  operator-<C>(const C& n, const Linear_Form<C>& f);
  friend Linear_Form<C>
  operator-<C>(const Linear_Form<C>& f, const C& n);
  friend Linear_Form<C>
  operator-<C>(Variable v, const Linear_Form<C>& f);
  friend Linear_Form<C>
  operator-<C>(const Linear_Form<C>& f, Variable v);

  friend Linear_Form<C>
  operator*<C>(const C& n, const Linear_Form<C>& f);
  friend Linear_Form<C>
  operator*<C>(const Linear_Form<C>& f, const C& n);

  friend Linear_Form<C>&
  operator+=<C>(Linear_Form<C>& f1, const Linear_Form<C>& f2);
  friend Linear_Form<C>&
  operator+=<C>(Linear_Form<C>& f, Variable v);
  friend Linear_Form<C>&
  operator+=<C>(Linear_Form<C>& f, const C& n);

  friend Linear_Form<C>&
  operator-=<C>(Linear_Form<C>& f1, const Linear_Form<C>& f2);
  friend Linear_Form<C>&
  operator-=<C>(Linear_Form<C>& f, Variable v);
  friend Linear_Form<C>&
  operator-=<C>(Linear_Form<C>& f, const C& n);

  friend Linear_Form<C>&
  operator*=<C>(Linear_Form<C>& f, const C& n);

  friend Linear_Form<C>&
  operator/=<C>(Linear_Form<C>& f, const C& n);

  friend bool
  operator==<C>(const Linear_Form<C>& x, const Linear_Form<C>& y);

  friend std::ostream&
  Parma_Polyhedra_Library::IO_Operators
  ::operator<<<C>(std::ostream& s, const Linear_Form<C>& f);
};

#include "Linear_Form.inlines.hh"
// Linear_Form.templates.hh is not included here on purpose.

#endif // !defined(PPL_Linear_Form_defs_hh)
