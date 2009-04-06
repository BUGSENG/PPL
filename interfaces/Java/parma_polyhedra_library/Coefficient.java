/* Coefficient Java class declaration and implementation.
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

package parma_polyhedra_library;

import java.math.BigInteger;

//! A PPL coefficient.
/*! \ingroup PPL_Java_interface
  Objects of type Coefficient are used to implement the integral valued
  coefficients occurring in linear expressions, constraints, generators
  and so on.
*/
public class Coefficient {

    //! Holds the value of \p this.
    private BigInteger value;

    //! Builds a coefficient values \p i.
    public Coefficient(int i) {
	value = new BigInteger(Integer.toString(i));
    }

    //! Builds a coefficient valued \p l.
    public Coefficient(long l) {
	value = new BigInteger(Long.toString(l));
    }

    //! Builds a coefficient valued \p bi.
    public Coefficient(BigInteger bi) {
	value = bi;
    }

    //! Builds a coefficient from the decimal representation in \p s.
    /*!
      \exception java.lang.NumberFormatException
      Thrown if \p s does not contain a valid decimal representation.
    */
    public Coefficient(String s) {
	value = new BigInteger(s);
    }

    //! Builds a copy of \p c.
    public Coefficient(Coefficient c) {
	value = new BigInteger(c.value.toString());
    }

    //! Returns the value held by \p this.
    public BigInteger getBigInteger() {
	return value;
    }

    private void set(Coefficient c) {
        value = c.getBigInteger();
    }
}
