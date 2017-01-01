/* Poly_Con_Relation Java class declaration and implementation.
   Copyright (C) 2001-2010 Roberto Bagnara <bagnara@cs.unipr.it>
   Copyright (C) 2010-2017 BUGSENG srl (http://bugseng.com)

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

package parma_polyhedra_library;

//! The relation between a polyhedron and a constraint.
/*! \ingroup PPL_Java_interface
  This class implements conjunctions of assertions on the relation
  between a polyhedron and a constraint.
*/
public class Poly_Con_Relation {

    public static final int NOTHING = 0;
    public static final int IS_DISJOINT = 1;
    public static final int STRICTLY_INTERSECTS = 2;
    public static final int IS_INCLUDED = 4;
    public static final int SATURATES = 8;

    //! Holds the value of the possible relations.
    private int mask_value;

    //! Constructs from a integer value.
    public Poly_Con_Relation(int val) {
        mask_value = val;
    }

    //! The assertion that says nothing.
    public static Poly_Con_Relation nothing() {
        return new Poly_Con_Relation(Poly_Con_Relation.NOTHING);
    }

    /*! \brief
      The polyhedron and the set of points satisfying
      the constraint are disjoint.
    */
    public static Poly_Con_Relation is_disjoint() {
        return new  Poly_Con_Relation(Poly_Con_Relation.IS_DISJOINT);
    }

    /*! \brief
      The polyhedron intersects the set of points satisfying
      the constraint, but it is not included in it.
    */
    public static Poly_Con_Relation strictly_intersects() {
        return new Poly_Con_Relation(Poly_Con_Relation.STRICTLY_INTERSECTS);
    }

    /*! \brief
      The polyhedron is included in the set of points satisfying
      the constraint.
    */
    public static Poly_Con_Relation is_included() {
        return new Poly_Con_Relation(Poly_Con_Relation.IS_INCLUDED);
    }

    /*! \brief
      The polyhedron is included in the set of points saturating
      the constraint.
    */
    public static Poly_Con_Relation saturates() {
        return new Poly_Con_Relation(Poly_Con_Relation.SATURATES);
    }

    //! True if and only if \p this implies \p y.
    public boolean implies(Poly_Con_Relation y) {
        return (this.mask_value & y.mask_value) ==  y.mask_value;
    }

    private static native void initIDs();
    static {
        initIDs();
    }
}
