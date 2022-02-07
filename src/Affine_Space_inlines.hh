/* Affine_Space class implementation: inline functions.
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

#ifndef PPL_Affine_Space_inlines_hh
#define PPL_Affine_Space_inlines_hh 1

namespace Parma_Polyhedra_Library
{

inline dimension_type
Affine_Space::max_space_dimension ()
{
    return Grid::max_space_dimension();
}

inline
Affine_Space::Affine_Space (dimension_type           num_dimensions,
                            const Degenerate_Element kind)
    : gr(num_dimensions, kind)
{
}

inline
Affine_Space::Affine_Space (const Affine_Space & y,
                            Complexity_Class     complexity)
    : gr(y.gr, complexity)
{
}

inline
Affine_Space::Affine_Space (const Constraint_System & cs)
    : gr(cs)
{
}

inline
Affine_Space::Affine_Space (Constraint_System & cs, Recycle_Input ri)
    : gr(cs, ri)
{
}

inline
Affine_Space::Affine_Space (const Polyhedron & ph,
                            Complexity_Class   complexity)
    : gr(ph, complexity)
{
}

inline
Affine_Space::Affine_Space (Generator_System & gs, Recycle_Input)
    : gr(EMPTY)
{
    Affine_Space(gs).m_swap(*this);
}

template <typename U>
inline
Affine_Space::Affine_Space (const BD_Shape<U> & bd,
                            Complexity_Class    complexity)
    : gr(bd, complexity)
{
}

template <typename U>
inline
Affine_Space::Affine_Space (const Octagonal_Shape<U> & os,
                            Complexity_Class           complexity)
    : gr(os, complexity)
{
}

inline
Affine_Space::~Affine_Space ()
{
}

inline dimension_type
Affine_Space::space_dimension () const
{
    return gr.space_dimension();
}

inline memory_size_type
Affine_Space::total_memory_in_bytes () const
{
    return gr.total_memory_in_bytes();
}

inline int32_t
Affine_Space::hash_code () const
{
    return gr.hash_code();
}

inline Constraint_System
Affine_Space::constraints () const
{
    return gr.constraints();
}

inline Constraint_System
Affine_Space::minimized_constraints () const
{
    return gr.minimized_constraints();
}

inline void
Affine_Space::m_swap (Affine_Space & y)
{
    swap(gr, y.gr);
}

inline void
Affine_Space::add_congruence (const Congruence & cg)
{
    gr.add_congruence(cg);
}

inline void
Affine_Space::add_congruences (const Congruence_System & cgs)
{
    gr.add_congruences(cgs);
}

inline void
Affine_Space::refine_with_congruence (const Congruence & cg)
{
    gr.refine_with_congruence(cg);
}

inline void
Affine_Space::refine_with_congruences (const Congruence_System & cgs)
{
    gr.refine_with_congruences(cgs);
}

inline bool
Affine_Space::can_recycle_constraint_systems ()
{
    return Grid::can_recycle_constraint_systems();
}

inline bool
Affine_Space::can_recycle_congruence_systems ()
{
    return Grid::can_recycle_congruence_systems();
}

inline void
Affine_Space::add_constraint (const Constraint & c)
{
    gr.add_constraint(c);
}

inline void
Affine_Space::add_recycled_constraints (Constraint_System & cs)
{
    gr.add_recycled_constraints(cs);
}

inline bool
Affine_Space::bounds_from_above (const Linear_Expression & expr) const
{
    return gr.bounds_from_above(expr);
}

inline bool
Affine_Space::bounds_from_below (const Linear_Expression & expr) const
{
    return gr.bounds_from_below(expr);
}

inline bool
Affine_Space::maximize (const Linear_Expression & expr,
                        Coefficient & sup_n, Coefficient & sup_d,
                        bool & maximum) const
{
    return gr.maximize(expr, sup_n, sup_d, maximum);
}

inline bool
Affine_Space::maximize (const Linear_Expression & expr,
                        Coefficient & sup_n, Coefficient & sup_d,
                        bool & maximum, Generator & point) const
{
    return gr.maximize(expr, sup_n, sup_d, maximum, point);
}

inline bool
Affine_Space::minimize (const Linear_Expression & expr,
                        Coefficient & inf_n, Coefficient & inf_d,
                        bool & minimum) const
{
    return gr.minimize(expr, inf_n, inf_d, minimum);
}

inline bool
Affine_Space::minimize (const Linear_Expression & expr,
                        Coefficient & inf_n, Coefficient & inf_d,
                        bool & minimum, Generator & point) const
{
    return gr.minimize(expr, inf_n, inf_d, minimum, point);
}

inline bool
Affine_Space::frequency (const Linear_Expression & expr,
                         Coefficient & freq_n, Coefficient & freq_d,
                         Coefficient & val_n, Coefficient & val_d) const
{
    return gr.frequency(expr, freq_n, freq_d, val_n, val_d);
}

/*! \relates Affine_Space */
inline bool
operator!= (const Affine_Space & x, const Affine_Space & y)
{
    return !(x == y);
}

inline bool
Affine_Space::strictly_contains (const Affine_Space & y) const
{
    return gr.strictly_contains(y.gr);
}

inline void
Affine_Space::topological_closure_assign ()
{
}

/*! \relates Affine_Space */
inline void
swap (Affine_Space & x, Affine_Space & y)
{
    x.m_swap(y);
}

} // namespace Parma_Polyhedra_Library

#endif // !defined(PPL_Affine_Space_inlines_hh)

