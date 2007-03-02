dnl Copyright (C) 2001-2007 Roberto Bagnara <bagnara@cs.unipr.it>
dnl
dnl This file is part of the Parma Polyhedra Library (PPL).
dnl
dnl The PPL is free software; you can redistribute it and/or modify it
dnl under the terms of the GNU General Public License as published by the
dnl Free Software Foundation; either version 2 of the License, or (at your
dnl option) any later version.
dnl
dnl The PPL is distributed in the hope that it will be useful, but WITHOUT
dnl ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
dnl FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
dnl for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program; if not, write to the Free Software Foundation,
dnl Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1307, USA.
dnl
dnl For the most up-to-date information see the Parma Polyhedra Library
dnl site: http://www.cs.unipr.it/ppl/ .

m4_divert(-1)dnl

m4_define(`ppl_new_@TOPOLOGY@@CLASS@_from_space_dimension_code',
`extern "C" Prolog_foreign_return_type
ppl_new_@TOPOLOGY@@CLASS@_from_space_dimension(Prolog_term_ref t_nd,
                                               Prolog_term_ref t_uoe,
                                               Prolog_term_ref t_ph) {
  static const char* where = "ppl_new_@TOPOLOGY@@CLASS@_from_space_dimension/3";
  try {
    @TOPOLOGY@@CPP_CLASS@* ph;
    Prolog_atom uoe = term_to_universe_or_empty(t_uoe, where);

    if (uoe == a_empty)
      ph = new @TOPOLOGY@@CPP_CLASS@(term_to_unsigned<dimension_type>(t_nd,
                                                                      where),
                                     EMPTY);
    else
      ph = new @TOPOLOGY@@CPP_CLASS@(term_to_unsigned<dimension_type>(t_nd,
                                                                      where),
                                     UNIVERSE);

    Prolog_term_ref tmp = Prolog_new_term_ref();
    Prolog_put_address(tmp, ph);
    if (Prolog_unify(t_ph, tmp)) {
      REGISTER(ph);
      return PROLOG_SUCCESS;
    }
    else
      delete ph;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_new_@TOPOLOGY@@CLASS@_from_@INTOPOLOGY@@FRIEND@_code',
`extern "C" Prolog_foreign_return_type
ppl_new_@TOPOLOGY@@CLASS@_from_@INTOPOLOGY@@FRIEND@(
                     Prolog_term_ref t_ph_source, Prolog_term_ref t_ph) {
  static const char* where =
                   "ppl_new_@TOPOLOGY@@CLASS@_from_@INTOPOLOGY@@FRIEND@/2";
  try {
    @TOPOLOGY@@CPP_CLASS@* ph;
    const @INTOPOLOGY@@ALT_FRIEND@* ph_source
        = static_cast<const @INTOPOLOGY@@ALT_FRIEND@*>
        (term_to_handle<@ALT_FRIEND@ >(t_ph_source, where));
    CHECK(ph_source);
        ph = new @TOPOLOGY@@CPP_CLASS@(*ph_source);
    Prolog_term_ref tmp = Prolog_new_term_ref();
    Prolog_put_address(tmp, ph);
    if (Prolog_unify(t_ph, tmp)) {
      REGISTER(ph);
      return PROLOG_SUCCESS;
    }
    else
      delete ph;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_new_@TOPOLOGY@@CLASS@_from_@BUILD_REPRESENT@s_code',
`extern "C" Prolog_foreign_return_type
ppl_new_@TOPOLOGY@@CLASS@_from_@BUILD_REPRESENT@s(Prolog_term_ref t_clist,
                                                  Prolog_term_ref t_ph) {
  static const char* where =
                     "ppl_new_@TOPOLOGY@@CLASS@_from_@BUILD_REPRESENT@s/2";
  try {
    @UBUILD_REPRESENT@_System cs;
    Prolog_term_ref c = Prolog_new_term_ref();

    while (Prolog_is_cons(t_clist)) {
      Prolog_get_cons(t_clist, c, t_clist);
      cs.insert(build_@BUILD_REPRESENT@(c, where));
    }

    // Check the list is properly terminated.
    check_nil_terminating(t_clist, where);

    @TOPOLOGY@@CPP_CLASS@* ph;
    ph = new @TOPOLOGY@@CPP_CLASS@(cs@RECYCLE@);
    Prolog_term_ref tmp = Prolog_new_term_ref();
    Prolog_put_address(tmp, ph);
    if (Prolog_unify(t_ph, tmp)) {
      REGISTER(ph);
      return PROLOG_SUCCESS;
    }
    else
      delete ph;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_new_@TOPOLOGY@@CLASS@_from_@BOX@_code',
`extern "C" Prolog_foreign_return_type
ppl_new_@TOPOLOGY@@CLASS@_from_@BOX@(Prolog_term_ref t_bb,
                                     Prolog_term_ref t_ph) {
  static const char* where = "ppl_new_@TOPOLOGY@@CLASS@_from_@BOX@/2";
  try {
    // Compute the space dimension.
    Prolog_term_ref t_l = Prolog_new_term_ref();
    Prolog_term_ref t_interval = Prolog_new_term_ref();
    Prolog_put_term(t_l, t_bb);
    dimension_type dimension;
    for (dimension = 0; Prolog_is_cons(t_l); ++dimension)
      Prolog_get_cons(t_l, t_interval, t_l);

    // Check the list is properly terminated.
    check_nil_terminating(t_l, where);

    Rational_Box box(dimension);
    // Set box to reflect its Prolog representation.
    for (dimension_type i = 0; i < dimension; ++i) {
      Prolog_get_cons(t_bb, t_interval, t_bb);
      // An interval is either the atom empty or of the form
      // i(Lower_Bound, Upper_Bound).
      if (Prolog_is_atom(t_interval)) {
        Prolog_atom name;
        if (Prolog_get_atom_name(t_interval, &name) && name == a_empty) {
          box.set_empty();
          continue;
        }
        else
          return PROLOG_FAILURE;
      }

      if (!Prolog_is_compound(t_interval))
        return PROLOG_FAILURE;

      Prolog_atom functor;
      int arity;
      Prolog_get_compound_name_arity(t_interval, &functor, &arity);
      if (arity != 2 || functor != a_i)
        return PROLOG_FAILURE;

      bool finite;
      bool closed;
      Coefficient n;
      Coefficient d;
      Prolog_term_ref t_bound = Prolog_new_term_ref();

      // Get and raise the lower bound.
      Prolog_get_arg(1, t_interval, t_bound);
      if (!term_to_boundary(t_bound, LOWER_BOUNDARY, finite, closed, n, d))
        return PROLOG_FAILURE;
      if (finite)
        box.raise_lower_bound(i, closed, n, d);

      // Get and lower the upper bound.
      Prolog_get_arg(2, t_interval, t_bound);
      if (!term_to_boundary(t_bound, UPPER_BOUNDARY, finite, closed, n, d))
        return PROLOG_FAILURE;
      if (finite)
        box.lower_upper_bound(i, closed, n, d);
    }

    @TOPOLOGY@@CPP_CLASS@* ph;
    ph = new @TOPOLOGY@@CPP_CLASS@(box, From_`'@UBOX@());
    Prolog_term_ref tmp = Prolog_new_term_ref();
    Prolog_put_address(tmp, ph);
    if (Prolog_unify(t_ph, tmp)) {
      REGISTER(ph);
      return PROLOG_SUCCESS;
    }
    else
      delete ph;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_swap_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_swap(Prolog_term_ref t_lhs, Prolog_term_ref t_rhs) {
  static const char* where = "ppl_@CLASS@_swap/2";
  try {
    @CPP_CLASS@* lhs = term_to_handle<@CPP_CLASS@ >(t_lhs, where);
    @CPP_CLASS@* rhs = term_to_handle<@CPP_CLASS@ >(t_rhs, where);
    CHECK(lhs);
    CHECK(rhs);
    lhs->swap(*rhs);
    return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_delete_@CLASS@_code',
`extern "C" Prolog_foreign_return_type
ppl_delete_@CLASS@(Prolog_term_ref t_ph) {
  static const char* where = "ppl_delete_@CLASS@/1";
  try {
    const @CPP_CLASS@* ph = term_to_handle<@CPP_CLASS@ >(t_ph, where);
    UNREGISTER(ph);
    delete ph;
    return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_@DIMENSION@_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_@DIMENSION@(Prolog_term_ref t_ph, Prolog_term_ref t_sd) {
  static const char* where = "ppl_@CLASS@_@DIMENSION@/2";
  try {
    const @CPP_CLASS@* ph = term_to_handle<@CPP_CLASS@ >(t_ph, where);
    CHECK(ph);
    if (unify_ulong(t_sd, ph->@DIMENSION@()))
      return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_get_@GET_REPRESENT@s_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_get_@GET_REPRESENT@s(Prolog_term_ref t_ph,
                                 Prolog_term_ref t_glist) {
  static const char* where = "ppl_@CLASS@_get_@GET_REPRESENT@s/2";
  try {
    const @CPP_CLASS@* ph = term_to_handle<@CPP_CLASS@ >(t_ph, where);
    CHECK(ph);

    Prolog_term_ref tail = Prolog_new_term_ref();
    Prolog_put_atom(tail, a_nil);
    const @UGET_REPRESENT@_System& gs = ph->@GET_REPRESENT@s();
    for (@UGET_REPRESENT@_System::const_iterator i = gs.begin(),
           gs_end = gs.end(); i != gs_end; ++i)
      Prolog_construct_cons(tail, @GET_REPRESENT@_term(*i), tail);

    if (Prolog_unify(t_glist, tail))
      return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_get_minimized_@GET_REPRESENT@s_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_get_minimized_@GET_REPRESENT@s(Prolog_term_ref t_ph,
                                           Prolog_term_ref t_glist) {
  static const char* where = "ppl_@CLASS@_get_minimized_@GET_REPRESENT@s/2";
  try {
    const @CPP_CLASS@* ph = term_to_handle<@CPP_CLASS@ >(t_ph, where);
    CHECK(ph);

    Prolog_term_ref tail = Prolog_new_term_ref();
    Prolog_put_atom(tail, a_nil);
    const @UGET_REPRESENT@_System& gs = ph->minimized_@GET_REPRESENT@s();
    for (@UGET_REPRESENT@_System::const_iterator i = gs.begin(),
           gs_end = gs.end(); i != gs_end; ++i)
      Prolog_construct_cons(tail, @GET_REPRESENT@_term(*i), tail);

    if (Prolog_unify(t_glist, tail))
      return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_begin_iterator_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_begin_iterator(Prolog_term_ref t_pps,
			  Prolog_term_ref t_it) {
  static const char* where = "ppl_@CLASS@_begin_iterator/2";
  try {
    @CPP_CLASS@* pps = term_to_handle<@CPP_CLASS@ >(t_pps, where);
    CHECK(pps);

    @CPP_CLASS@::iterator* i = new @CPP_CLASS@::iterator(pps->begin());
    Prolog_term_ref t_i = Prolog_new_term_ref();
    Prolog_put_address(t_i, i);

    if (Prolog_unify(t_it, t_i)) {
      REGISTER(i);
      return PROLOG_SUCCESS;
    }
    else
      delete i;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_end_iterator_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_end_iterator(Prolog_term_ref t_pps,
			  Prolog_term_ref t_it) {
  static const char* where = "ppl_@CLASS@_end_iterator/2";
  try {
    @CPP_CLASS@* pps = term_to_handle<@CPP_CLASS@ >(t_pps, where);
    CHECK(pps);

    @CPP_CLASS@::iterator* i = new @CPP_CLASS@::iterator(pps->end());
    Prolog_term_ref t_i = Prolog_new_term_ref();
    Prolog_put_address(t_i, i);

    if (Prolog_unify(t_it, t_i)) {
      REGISTER(i);
      return PROLOG_SUCCESS;
    }
    else
      delete i;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_delete_iterator_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_delete_iterator(Prolog_term_ref t_it) {
  static const char* where = "ppl_@CLASS@_delete_iterator/1";
  try {
    const @CPP_CLASS@::iterator* it
         = term_to_handle<@CPP_CLASS@::iterator >(t_it, where);
    UNREGISTER(it);
    delete it;
    return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_increment_iterator_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_increment_iterator(Prolog_term_ref t_it) {
  static const char* where = "ppl_@CLASS@_increment_iterator/2";
  try {
    @CPP_CLASS@::iterator* it
         = term_to_handle<@CPP_CLASS@::iterator >(t_it, where);
    CHECK(it);
    ++(*it);
    return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_iterator_equals_iterator_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_iterator_equals_iterator(Prolog_term_ref t_it1,
			  Prolog_term_ref t_it2) {
  static const char* where = "ppl_@CLASS@_iterator_equals_iterator/2";
  try {
    @CPP_CLASS@::iterator* it1
         = term_to_handle<@CPP_CLASS@::iterator >(t_it1, where);
    CHECK(it1);
    @CPP_CLASS@::iterator* it2
         = term_to_handle<@CPP_CLASS@::iterator >(t_it2, where);
    CHECK(it2);
    if (*it1 == *it2)
      return PROLOG_SUCCESS;
    else
      return PROLOG_FAILURE;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_decrement_iterator_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_decrement_iterator(Prolog_term_ref t_it) {
  static const char* where = "ppl_@CLASS@_increment_iterator/2";
  try {
    @CPP_CLASS@::iterator* it
         = term_to_handle<@CPP_CLASS@::iterator >(t_it, where);
    CHECK(it);
    --(*it);
    return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_get_disjunct_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_get_disjunct(Prolog_term_ref t_it,
			 Prolog_term_ref t_disj) {
  static const char* where = "ppl_@CLASS@_get_disjunct/2";
  try {
    const @CPP_CLASS@::iterator* it
         = term_to_handle<@CPP_CLASS@::iterator >(t_it, where);
    CHECK(it);

    @ALT_CPP_DISJUNCT@* disj
	= const_cast<@ALT_CPP_DISJUNCT@*>(&((*it)->element()));
    Prolog_term_ref t_d = Prolog_new_term_ref();
    Prolog_put_address(t_d, disj);

    if (Prolog_unify(t_disj, t_d)) {
      WEAK_REGISTER(disj);
      return PROLOG_SUCCESS;
    }
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_drop_disjunct_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_drop_disjunct(Prolog_term_ref t_pps,
			  Prolog_term_ref t_it) {
  static const char* where = "ppl_@CLASS@_drop_disjuncts/2";
  try {
    @CPP_CLASS@* pps = term_to_handle<@CPP_CLASS@ >(t_pps, where);
    CHECK(pps);

    @CPP_CLASS@::iterator* it
         = term_to_handle<@CPP_CLASS@::iterator >(t_it, where);
    CHECK(it);

    @CPP_CLASS@::iterator& i = *it;
    pps->drop_disjunct(i);

    return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_size_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_size(Prolog_term_ref t_pps,
		 Prolog_term_ref t_s) {
  static const char* where = "ppl_@CLASS@_size/2";
  try {
    @CPP_CLASS@* pps = term_to_handle<@CPP_CLASS@ >(t_pps, where);
    CHECK(pps);

    if (unify_ulong(t_s, pps->size()))
      return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

extern "C" Prolog_foreign_return_type
ppl_Pointset_Powerset_C_Polyhedron_linear_partition(Prolog_term_ref t_ph,
			 Prolog_term_ref t_qh,
			 Prolog_term_ref t_inters,
			 Prolog_term_ref t_pset) {
  static const char* where = "ppl_Pointset_Powerset_C_Polyhedron_linear_partition/4";
  try {
    const C_Polyhedron* ph =
        term_to_handle<C_Polyhedron >(t_ph, where);
    CHECK(ph);
    const C_Polyhedron* qh =
        term_to_handle<C_Polyhedron >(t_qh, where);
    CHECK(qh);

    std::pair<C_Polyhedron, Pointset_Powerset<NNC_Polyhedron > > r =
      linear_partition(*ph, *qh);

    Prolog_term_ref t_r_first = Prolog_new_term_ref();
    Prolog_term_ref t_r_second = Prolog_new_term_ref();
    Prolog_put_address(t_r_first, &(r.first));
    Prolog_put_address(t_r_second, &(r.second));

    if (Prolog_unify(t_inters, t_r_first)
         && Prolog_unify(t_pset, t_r_second)) {
      return PROLOG_SUCCESS;
    }
  }
  CATCH_ALL;
}

m4_define(`ppl_@CLASS@_@PARTITION@_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_@PARTITION@(Prolog_term_ref t_ph,
			 Prolog_term_ref t_qh,
			 Prolog_term_ref t_inters,
			 Prolog_term_ref t_pset) {
  static const char* where = "ppl_@CLASS@_@PARTITION@/4";
  try {
    const @ALT_CPP_DISJUNCT@* ph =
        term_to_handle<@ALT_CPP_DISJUNCT@>(t_ph, where);
    CHECK(ph);
    const @ALT_CPP_DISJUNCT@* qh =
        term_to_handle<@ALT_CPP_DISJUNCT@>(t_qh, where);
    CHECK(qh);

    std::pair<@ALT_CPP_DISJUNCT@@COMMA@ Pointset_Powerset<@SUPERCLASS@> > r =
      @PARTITION@(*ph, *qh);

    Prolog_term_ref t_r_first = Prolog_new_term_ref();
    Prolog_term_ref t_r_second = Prolog_new_term_ref();
    Prolog_put_address(t_r_first, &(r.first));
    Prolog_put_address(t_r_second, &(r.second));

    if (Prolog_unify(t_inters, t_r_first)
         && Prolog_unify(t_pset, t_r_second)) {
      return PROLOG_SUCCESS;
    }
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_relation_with_@RELATION_REPRESENT@_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_relation_with_@RELATION_REPRESENT@(Prolog_term_ref t_ph,
                                               Prolog_term_ref t_c,
                                               Prolog_term_ref t_r) {
  static const char* where =
                   "ppl_@CLASS@_relation_with_@RELATION_REPRESENT@/3";
  try {
    @CPP_CLASS@* ph = term_to_handle<@CPP_CLASS@ >(t_ph, where);
    CHECK(ph);
relation_with_@RELATION_REPRESENT@_code`'
    if (Prolog_unify(t_r, tail))
      return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`relation_with_constraint_code', `
    Poly_Con_Relation r = ph->relation_with(build_constraint(t_c, where));

    Prolog_term_ref tail = Prolog_new_term_ref();
    Prolog_put_atom(tail, a_nil);
    while (r != Poly_Con_Relation::nothing()) {
      if (r.implies(Poly_Con_Relation::is_disjoint())) {
        Prolog_term_ref t_dis = Prolog_new_term_ref();
        Prolog_put_atom(t_dis, a_is_disjoint);
        Prolog_construct_cons(tail, t_dis, tail);
        r = r - Poly_Con_Relation::is_disjoint();
      }
      else if (r.implies(Poly_Con_Relation::strictly_intersects())) {
        Prolog_term_ref t_sin = Prolog_new_term_ref();
        Prolog_put_atom(t_sin, a_strictly_intersects);
        Prolog_construct_cons(tail, t_sin, tail);
        r = r - Poly_Con_Relation::strictly_intersects();
      }
      else if (r.implies(Poly_Con_Relation::is_included())) {
        Prolog_term_ref t_inc = Prolog_new_term_ref();
        Prolog_put_atom(t_inc, a_is_included);
        Prolog_construct_cons(tail, t_inc, tail);
        r = r - Poly_Con_Relation::is_included();
      }
      else if (r.implies(Poly_Con_Relation::saturates())) {
        Prolog_term_ref t_sat = Prolog_new_term_ref();
        Prolog_put_atom(t_sat, a_saturates);
        Prolog_construct_cons(tail, t_sat, tail);
        r = r - Poly_Con_Relation::saturates();
      }
    }
')

m4_define(`relation_with_congruence_code', `
    Poly_Con_Relation r = ph->relation_with(build_congruence(t_c, where));

    Prolog_term_ref tail = Prolog_new_term_ref();
    Prolog_put_atom(tail, a_nil);
    while (r != Poly_Con_Relation::nothing()) {
      if (r.implies(Poly_Con_Relation::is_disjoint())) {
        Prolog_term_ref t_dis = Prolog_new_term_ref();
        Prolog_put_atom(t_dis, a_is_disjoint);
        Prolog_construct_cons(tail, t_dis, tail);
        r = r - Poly_Con_Relation::is_disjoint();
      }
      else if (r.implies(Poly_Con_Relation::strictly_intersects())) {
        Prolog_term_ref t_sin = Prolog_new_term_ref();
        Prolog_put_atom(t_sin, a_strictly_intersects);
        Prolog_construct_cons(tail, t_sin, tail);
        r = r - Poly_Con_Relation::strictly_intersects();
      }
      else if (r.implies(Poly_Con_Relation::is_included())) {
        Prolog_term_ref t_inc = Prolog_new_term_ref();
        Prolog_put_atom(t_inc, a_is_included);
        Prolog_construct_cons(tail, t_inc, tail);
        r = r - Poly_Con_Relation::is_included();
      }
    }
')

m4_define(`relation_with_generator_code', `
    Poly_Gen_Relation r = ph->relation_with(build_generator(t_c, where));

    Prolog_term_ref tail = Prolog_new_term_ref();
    Prolog_put_atom(tail, a_nil);
    while (r != Poly_Gen_Relation::nothing()) {
      if (r.implies(Poly_Gen_Relation::subsumes())) {
        Prolog_term_ref t_sub = Prolog_new_term_ref();
        Prolog_put_atom(t_sub, a_subsumes);
        Prolog_construct_cons(tail, t_sub, tail);
        r = r - Poly_Gen_Relation::subsumes();
      }
    }
')

m4_define(`relation_with_grid_generator_code', `
    Poly_Gen_Relation r = ph->relation_with(build_grid_generator(t_c, where));

    Prolog_term_ref tail = Prolog_new_term_ref();
    Prolog_put_atom(tail, a_nil);
    while (r != Poly_Gen_Relation::nothing()) {
      if (r.implies(Poly_Gen_Relation::subsumes())) {
        Prolog_term_ref t_sub = Prolog_new_term_ref();
        Prolog_put_atom(t_sub, a_subsumes);
        Prolog_construct_cons(tail, t_sub, tail);
        r = r - Poly_Gen_Relation::subsumes();
      }
    }
')

m4_define(`ppl_@CLASS@_get_covering_box_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_get_covering_box(Prolog_term_ref t_ph, Prolog_term_ref t_bb) {
  static const char* where = "ppl_@CLASS@_get_covering_box/2";
  try {
    @CLASS@* ph = term_to_handle<@CPP_CLASS@ >(t_ph, where);
    CHECK(ph);

    dimension_type dimension = ph->space_dimension();
    Rational_Box box(dimension);
    ph->get_covering_box(box);
    Prolog_term_ref tail = Prolog_new_term_ref();
    Prolog_put_atom(tail, a_nil);
    for (dimension_type i = dimension; i-- > 0; )
      Prolog_construct_cons(tail, interval_term(box[i]), tail);
    if (Prolog_unify(t_bb, tail))
      return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_get_bounding_box_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_get_bounding_box(Prolog_term_ref t_ph,
                             Prolog_term_ref t_cc,
                             Prolog_term_ref t_bb) {
  static const char* where = "ppl_@CLASS@_get_bounding_box/3";
  try {
    @CPP_CLASS@* ph = term_to_handle<@CPP_CLASS@ >(t_ph, where);
    CHECK(ph);

    Prolog_atom p_cc = term_to_complexity_class(t_cc, where);
    Complexity_Class cc;
    if (p_cc == a_polynomial)
      cc = POLYNOMIAL_COMPLEXITY;
    else if (p_cc == a_simplex)
      cc = SIMPLEX_COMPLEXITY;
    else
      cc = ANY_COMPLEXITY;

    Rational_Box box(*ph, cc);
    Prolog_term_ref tail = Prolog_new_term_ref();
    Prolog_put_atom(tail, a_nil);
    if (box.is_empty()) {
      Prolog_term_ref t_empty = Prolog_new_term_ref();
      Prolog_put_atom(t_empty, a_empty);
      Prolog_construct_cons(tail, t_empty, tail);
    }
    else
      for (dimension_type i = ph->space_dimension(); i-- > 0; )
        Prolog_construct_cons(tail, interval_term(box[i]), tail);
    if (Prolog_unify(t_bb, tail))
      return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_@HAS_PROPERTY@_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_@HAS_PROPERTY@(Prolog_term_ref t_ph) {
  static const char* where = "ppl_@CLASS@_@HAS_PROPERTY@/1";
  try {
    const @CPP_CLASS@* ph = term_to_handle<@CPP_CLASS@ >(t_ph, where);
    CHECK(ph);
    if (ph->@HAS_PROPERTY@())
      return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_@SIMPLIFY@_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_@SIMPLIFY@(Prolog_term_ref t_ph) {
  static const char* where = "ppl_@CLASS@_@SIMPLIFY@/1";
  try {
    @CPP_CLASS@* ph = term_to_handle<@CPP_CLASS@ >(t_ph, where);
    CHECK(ph);
    ph->@SIMPLIFY@();
    return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_bounds_from_@ABOVEBELOW@_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_bounds_from_@ABOVEBELOW@(Prolog_term_ref t_ph,
                                     Prolog_term_ref t_expr) {
  static const char* where = "ppl_@CLASS@_bounds_from_@ABOVEBELOW@/2";
  try {
    @CPP_CLASS@* ph = term_to_handle<@CPP_CLASS@ >(t_ph, where);
    CHECK(ph);
    Linear_Expression l = build_linear_expression(t_expr, where);
    if (ph->bounds_from_@ABOVEBELOW@(l))
      return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_@MAXMIN@_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_@MAXMIN@(Prolog_term_ref t_ph, Prolog_term_ref t_le_expr,
 Prolog_term_ref t_n,  Prolog_term_ref t_d,
 Prolog_term_ref t_maxmin) {
  static const char* where = "ppl_@CLASS@_@MAXMIN@/5";
  try {
    const @CPP_CLASS@* ph = term_to_handle<@CPP_CLASS@ >(t_ph, where);
    CHECK(ph);
    const Linear_Expression le = build_linear_expression(t_le_expr, where);
    Coefficient n;
    Coefficient d;
    bool maxmin;
    if (ph->@MAXMIN@(le, n, d, maxmin)) {
      Prolog_term_ref t = Prolog_new_term_ref();
      Prolog_atom a = (maxmin ? a_true : a_false);
      Prolog_put_atom(t, a);
      if (Prolog_unify_Coefficient(t_n, n)
          && Prolog_unify_Coefficient(t_d, d)
          && Prolog_unify(t_maxmin, t))
        return PROLOG_SUCCESS;
    }
 }
 CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_@MAXMIN@_with_point_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_@MAXMIN@_with_point(Prolog_term_ref t_ph,
                                Prolog_term_ref t_le_expr,
 Prolog_term_ref t_n, Prolog_term_ref t_d,
 Prolog_term_ref t_maxmin, Prolog_term_ref t_g) {
  static const char* where = "ppl_@CLASS@_@MAXMIN@_with_point/6";
  try {
    const @CPP_CLASS@* ph = term_to_handle<@CPP_CLASS@ >(t_ph, where);
    CHECK(ph);
    const Linear_Expression le = build_linear_expression(t_le_expr, where);
    Coefficient n;
    Coefficient d;
    bool maxmin;
    @UGENERATOR@ g(@POINT@());
    if (ph->@MAXMIN@(le, n, d, maxmin, g)) {
      Prolog_term_ref t = Prolog_new_term_ref();
      Prolog_atom a = (maxmin ? a_true : a_false);
      Prolog_put_atom(t, a);
      if (Prolog_unify_Coefficient(t_n, n)
          && Prolog_unify_Coefficient(t_d, d)
          && Prolog_unify(t_maxmin, t)
          && Prolog_unify(t_g, @GENERATOR@_term(g)))
        return PROLOG_SUCCESS;
    }
 }
 CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_@COMPARISON@_@CLASS@_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_@COMPARISON@_@CLASS@(Prolog_term_ref t_lhs,
                                 Prolog_term_ref t_rhs) {
  static const char* where = "ppl_@CLASS@_@COMPARISON@_@CLASS@/2";
  try {
    const @CPP_CLASS@* lhs = term_to_handle<@CPP_CLASS@ >(t_lhs, where);
    const @CPP_CLASS@* rhs = term_to_handle<@CPP_CLASS@ >(t_rhs, where);
    CHECK(lhs);
    CHECK(rhs);
    if (lhs->@COMPARISON@(*rhs))
      return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_equals_@CLASS@_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_equals_@CLASS@(Prolog_term_ref t_lhs, Prolog_term_ref t_rhs) {
  static const char* where = "ppl_@CLASS@_equals_@CLASS@/2";
  try {
    const @CPP_CLASS@* lhs = term_to_handle<@CPP_CLASS@ >(t_lhs, where);
    const @CPP_CLASS@* rhs = term_to_handle<@CPP_CLASS@ >(t_rhs, where);
    CHECK(lhs);
    CHECK(rhs);
    if (*lhs == *rhs)
      return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_OK_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_OK(Prolog_term_ref t_ph) {
  static const char* where = "ppl_@CLASS@_OK/1";
  try {
    const @CPP_CLASS@* ph = term_to_handle<@CPP_CLASS@ >(t_ph, where);
    CHECK(ph);
    if (ph->OK())
      return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_add_@ADD_REPRESENT@_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_add_@ADD_REPRESENT@(Prolog_term_ref t_ph, Prolog_term_ref t_c) {
  static const char* where = "ppl_@CLASS@_add_@ADD_REPRESENT@/2";
  try {
    @CPP_CLASS@* ph = term_to_handle<@CPP_CLASS@ >(t_ph, where);
    CHECK(ph);
    ph->add_@ADD_REPRESENT@(build_@ADD_REPRESENT@(t_c, where));
    return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_add_disjunct_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_add_disjunct(Prolog_term_ref t_ph, Prolog_term_ref t_d) {
  static const char* where = "ppl_@CLASS@_add_disjunct/2";
  try {
    @CPP_CLASS@* ph = term_to_handle<@CPP_CLASS@ >(t_ph, where);
    CHECK(ph);
    @ALT_CPP_DISJUNCT@* d =
      static_cast<@ALT_CPP_DISJUNCT@*>(term_to_handle<@ALT_CPP_DISJUNCT@ >(t_d, where));
    CHECK(d);
    ph->add_disjunct(*d);
    return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')


m4_define(`add_assign_code',
`namespace {

Prolog_foreign_return_type
add_assign(Prolog_term_ref t_ph,
           Prolog_term_ref t_c,
           void (@CPP_CLASS@::* add_assign)(const @CPP_CLASS@&),
           const char* where) {
  try {
    @CPP_CLASS@* lhs = term_to_handle<@CPP_CLASS@ >(t_ph, where);
    const @CPP_CLASS@* rhs = term_to_handle<@CPP_CLASS@ >(t_rhs, where);
    CHECK(ph);
    if (ph->add_assign(build_@ADD_REPRESENT@(t_c, where)))
      return PROLOG_SUCCESS;

    CHECK(lhs);
    CHECK(rhs);
    (lhs->*bop_assign)(*rhs);
    return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}


')

m4_define(`ppl_@CLASS@_add_@ADD_REPRESENT@_and_minimize_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_add_@ADD_REPRESENT@_and_minimize(Prolog_term_ref t_ph,
                                             Prolog_term_ref t_c) {
  static const char* where = "ppl_@CLASS@_add_@ADD_REPRESENT@_and_minimize/2";
  try {
    @CPP_CLASS@* ph = term_to_handle<@CPP_CLASS@ >(t_ph, where);
    CHECK(ph);
    if (ph->add_@ADD_REPRESENT@_and_minimize(build_@ADD_REPRESENT@(t_c, where)))
      return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_add_@ADD_REPRESENT@s_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_add_@ADD_REPRESENT@s(Prolog_term_ref t_ph,
                                 Prolog_term_ref t_clist) {
  static const char* where = "ppl_@CLASS@_add_@ADD_REPRESENT@s/2";
  try {
    @CPP_CLASS@* ph = term_to_handle<@CPP_CLASS@ >(t_ph, where);
    CHECK(ph);
    @UADD_REPRESENT@_System cs;
    Prolog_term_ref c = Prolog_new_term_ref();

    while (Prolog_is_cons(t_clist)) {
      Prolog_get_cons(t_clist, c, t_clist);
      cs.insert(build_@ADD_REPRESENT@(c, where));
    }

    // Check the list is properly terminated.
    check_nil_terminating(t_clist, where);

    ph->add_@ADD_REPRESENT@s(cs);
    return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_add_@ADD_REPRESENT@s_and_minimize_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_add_@ADD_REPRESENT@s_and_minimize(Prolog_term_ref t_ph,
                                              Prolog_term_ref t_clist) {
  static const char* where = "ppl_@CLASS@_add_@ADD_REPRESENT@s_and_minimize/2";
  try {
    @CPP_CLASS@* ph = term_to_handle<@CPP_CLASS@ >(t_ph, where);
    CHECK(ph);
    @UADD_REPRESENT@_System cs;
    Prolog_term_ref c = Prolog_new_term_ref();

    while (Prolog_is_cons(t_clist)) {
      Prolog_get_cons(t_clist, c, t_clist);
      cs.insert(build_@ADD_REPRESENT@(c, where));
    }

    // Check the list is properly terminated.
    check_nil_terminating(t_clist, where);

    if (ph->add_@ADD_REPRESENT@s_and_minimize(cs))
      return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`bop_assign_code',
`namespace {

Prolog_foreign_return_type
bop_assign(Prolog_term_ref t_lhs,
           Prolog_term_ref t_rhs,
           void (@CPP_CLASS@::* bop_assign)(const @CPP_CLASS@&),
           const char* where) {
  try {
    @CPP_CLASS@* lhs = term_to_handle<@CPP_CLASS@ >(t_lhs, where);
    const @CPP_CLASS@* rhs = term_to_handle<@CPP_CLASS@ >(t_rhs, where);
    CHECK(lhs);
    CHECK(rhs);
    (lhs->*bop_assign)(*rhs);
    return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

Prolog_foreign_return_type
bop_assign_and_minimize(Prolog_term_ref t_lhs,
                        Prolog_term_ref t_rhs,
                        bool (@CPP_CLASS@::*
                              bop_assign_and_minimize)(const @CPP_CLASS@&),
                        const char* where) {
  try {
    @CPP_CLASS@* lhs = term_to_handle<@CPP_CLASS@ >(t_lhs, where);
    const @CPP_CLASS@* rhs = term_to_handle<@CPP_CLASS@ >(t_rhs, where);
    CHECK(lhs);
    CHECK(rhs);
    if ((lhs->*bop_assign_and_minimize)(*rhs))
      return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

} // namespace

')

m4_define(`ppl_@CLASS@_@BINOP@_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_@BINOP@
(Prolog_term_ref t_lhs, Prolog_term_ref t_rhs) {
  static const char* where = "ppl_@CLASS@_@BINOP@";
  return bop_assign(t_lhs, t_rhs, &@CPP_CLASS@::@BINOP@, where);
}

')
Prolog_foreign_return_type
bop_assign(Prolog_term_ref t_lhs,
           Prolog_term_ref t_rhs,
           void (@CPP_CLASS@::* bop_assign)(const @CPP_CLASS@&),
           const char* where) {
  try {
    @CPP_CLASS@* lhs = term_to_handle<@CPP_CLASS@ >(t_lhs, where);
    const @CPP_CLASS@* rhs = term_to_handle<@CPP_CLASS@ >(t_rhs, where);
    CHECK(lhs);
    CHECK(rhs);
    (lhs->*bop_assign)(*rhs);
    return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

m4_define(`ppl_@CLASS@_@BINMINOP@_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_@BINMINOP@
(Prolog_term_ref t_lhs, Prolog_term_ref t_rhs) {
  static const char* where = "ppl_@CLASS@_@BINMINOP@";
  return bop_assign_and_minimize(t_lhs, t_rhs,
                                 &@CPP_CLASS@::@BINMINOP@, where);
}

')

m4_define(`ppl_@TOPOLOGY@@CLASS@_@UB_EXACT@_code',
`extern "C" Prolog_foreign_return_type
ppl_@TOPOLOGY@@CLASS@_@UB_EXACT@
(Prolog_term_ref t_lhs, Prolog_term_ref t_rhs) {
  static const char* where = "ppl_@TOPOLOGY@@CLASS@_@UB_EXACT@";
  @TOPOLOGY@@CPP_CLASS@* lhs = term_to_handle<@TOPOLOGY@@CPP_CLASS@ >(t_lhs, where);
  const @TOPOLOGY@@CPP_CLASS@* rhs = term_to_handle<@TOPOLOGY@@CPP_CLASS@ >(t_rhs, where);
  CHECK(lhs);
  CHECK(rhs);
  return lhs->@UB_EXACT@(*rhs);
}

')

m4_define(`ppl_@CLASS@_@AFFIMAGE@_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_@AFFIMAGE@
(Prolog_term_ref t_ph,
 Prolog_term_ref t_v, Prolog_term_ref t_le, Prolog_term_ref t_d) {
  const char* where = "ppl_@CLASS@_@AFFIMAGE@/4";
  try {
    @CPP_CLASS@* ph = term_to_handle<@CPP_CLASS@ >(t_ph, where);
    CHECK(ph);
    ph->@AFFIMAGE@(term_to_Variable(t_v, where),
                   build_linear_expression(t_le, where),
                   term_to_Coefficient(t_d, where));
    return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_Grid_generalized_@AFFIMAGE@_code',
`extern "C" Prolog_foreign_return_type
ppl_Grid_generalized_@AFFIMAGE@
(Prolog_term_ref t_ph,
 Prolog_term_ref t_v, Prolog_term_ref t_r, Prolog_term_ref t_le,
 Prolog_term_ref t_d, Prolog_term_ref t_m) {
  static const char* where = "ppl_Grid_generalized_@AFFIMAGE@/6";
  try {
    Grid* ph = term_to_handle<Grid>(t_ph, where);
    CHECK(ph);
    ph->generalized_@AFFIMAGE@(term_to_Variable(t_v, where),
                               term_to_relation_symbol(t_r, where),
                               build_linear_expression(t_le, where),
                               term_to_Coefficient(t_d, where),
                               term_to_Coefficient(t_m, where));
      return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_generalized_@AFFIMAGE@_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_generalized_@AFFIMAGE@
(Prolog_term_ref t_ph,
 Prolog_term_ref t_v, Prolog_term_ref t_r, Prolog_term_ref t_le,
 Prolog_term_ref t_d) {
  static const char* where = "ppl_@CLASS@_generalized_@AFFIMAGE@/5";
  try {
    @CPP_CLASS@* ph = term_to_handle<@CPP_CLASS@ >(t_ph, where);
    CHECK(ph);
    ph->generalized_@AFFIMAGE@(term_to_Variable(t_v, where),
                               term_to_relation_symbol(t_r, where),
                               build_linear_expression(t_le, where),
                               term_to_Coefficient(t_d, where));
      return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_Grid_generalized_@AFFIMAGE@_lhs_rhs_code',
`extern "C" Prolog_foreign_return_type
ppl_Grid_generalized_@AFFIMAGE@_lhs_rhs
(Prolog_term_ref t_ph,
 Prolog_term_ref t_lhs, Prolog_term_ref t_r, Prolog_term_ref t_rhs,
 Prolog_term_ref t_m) {
 static const char* where = "ppl_Grid_generalized_@AFFIMAGE@_lhs_rhs/5";
  try {
    Grid* ph = term_to_handle<Grid>(t_ph, where);
    CHECK(ph);
    ph->generalized_@AFFIMAGE@(build_linear_expression(t_lhs, where),
                               term_to_relation_symbol(t_r, where),
                               build_linear_expression(t_rhs, where),
                               term_to_Coefficient(t_m, where));
    return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_generalized_@AFFIMAGE@_lhs_rhs_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_generalized_@AFFIMAGE@_lhs_rhs
(Prolog_term_ref t_ph,
 Prolog_term_ref t_lhs, Prolog_term_ref t_r, Prolog_term_ref t_rhs) {
  static const char* where = "ppl_@CLASS@_generalized_@AFFIMAGE@_lhs_rhs/4";
  try {
    @CPP_CLASS@* ph = term_to_handle<@CPP_CLASS@ >(t_ph, where);
    CHECK(ph);
    Relation_Symbol r = term_to_relation_symbol(t_r, where);
    ph->generalized_@AFFIMAGE@(build_linear_expression(t_lhs, where),
                               r,
                               build_linear_expression(t_rhs, where));
    return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_bounded_@AFFIMAGE@_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_bounded_@AFFIMAGE@
(Prolog_term_ref t_ph,
 Prolog_term_ref t_v, Prolog_term_ref t_lb_le, Prolog_term_ref t_ub_le,
 Prolog_term_ref t_d) {
  static const char* where = "ppl_@CLASS@_bounded_@AFFIMAGE@/5";
  try {
    @CPP_CLASS@* ph = term_to_handle<@CPP_CLASS@ >(t_ph, where);
    CHECK(ph);
    ph->bounded_@AFFIMAGE@(term_to_Variable(t_v, where),
                           build_linear_expression(t_lb_le, where),
                           build_linear_expression(t_ub_le, where),
                           term_to_Coefficient(t_d, where));
      return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_@WIDEN@_widening_assign_with_tokens_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_@WIDEN@_widening_assign_with_tokens
(Prolog_term_ref t_lhs, Prolog_term_ref t_rhs,
 Prolog_term_ref t_ti, Prolog_term_ref t_to) {
  static const char* where = "ppl_@CLASS@_@WIDEN@_widening_assign_with_tokens/4";
  try {
    @CPP_CLASS@* lhs = term_to_handle<@CPP_CLASS@ >(t_lhs, where);
    const @CPP_CLASS@* rhs = term_to_handle<@CPP_CLASS@ >(t_rhs, where);
    CHECK(lhs);
    CHECK(rhs);
    unsigned t = term_to_unsigned<unsigned>(t_ti, where);
    lhs->@WIDEN@_widening_assign(*rhs, &t);
      if (unify_long(t_to, t))
      return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_@WIDEN@_widening_assign_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_@WIDEN@_widening_assign
(Prolog_term_ref t_lhs, Prolog_term_ref t_rhs) {
  static const char* where = "ppl_@CLASS@_@WIDEN@_widening_assign/2";
  try {
    @CPP_CLASS@* lhs = term_to_handle<@CPP_CLASS@ >(t_lhs, where);
    const @CPP_CLASS@* rhs = term_to_handle<@CPP_CLASS@ >(t_rhs, where);
    CHECK(lhs);
    CHECK(rhs);
    lhs->@WIDEN@_widening_assign(*rhs, 0);
    return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_@EXTRAPOLATION@_extrapolation_assign_with_tokens_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_@EXTRAPOLATION@_extrapolation_assign_with_tokens
(Prolog_term_ref t_lhs, Prolog_term_ref t_rhs,
 Prolog_term_ref t_ti, Prolog_term_ref t_to) {
  static const char* where = "ppl_@CLASS@_@EXTRAPOLATION@_extrapolation_assign_with_tokens/4";
  try {
    @CPP_CLASS@* lhs = term_to_handle<@CPP_CLASS@ >(t_lhs, where);
    const @CPP_CLASS@* rhs = term_to_handle<@CPP_CLASS@ >(t_rhs, where);
    CHECK(lhs);
    CHECK(rhs);

    unsigned t = term_to_unsigned<unsigned>(t_ti, where);
    lhs->@EXTRAPOLATION@_extrapolation_assign(*rhs, &t);
    if (unify_long(t_to, t))
      return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_@EXTRAPOLATION@_extrapolation_assign_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_@EXTRAPOLATION@_extrapolation_assign
(Prolog_term_ref t_lhs, Prolog_term_ref t_rhs) {
  static const char* where = "ppl_@CLASS@_@EXTRAPOLATION@_extrapolation_assign/2";
  try {
    @CPP_CLASS@* lhs = term_to_handle<@CPP_CLASS@ >(t_lhs, where);
    const @CPP_CLASS@* rhs = term_to_handle<@CPP_CLASS@ >(t_rhs, where);
    CHECK(lhs);
    CHECK(rhs);

    lhs->@EXTRAPOLATION@_extrapolation_assign(*rhs, 0);
    return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_@EXTRAPOLATION@_narrowing_assign_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_@EXTRAPOLATION@_narrowing_assign
(Prolog_term_ref t_lhs, Prolog_term_ref t_rhs) {
  static const char* where = "ppl_@CLASS@_@EXTRAPOLATION@_narrowing_assign/2";
  try {
    @CPP_CLASS@* lhs = term_to_handle<@CPP_CLASS@ >(t_lhs, where);
    const @CPP_CLASS@* rhs = term_to_handle<@CPP_CLASS@ >(t_rhs, where);
    CHECK(lhs);
    CHECK(rhs);
    lhs->@EXTRAPOLATION@_narrowing_assign(*rhs);
    return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')


dnl FIXME: This code is redundant as there is no options for tokens
dnl        in the current C++ code.
m4_define(`ppl_@CLASS@_BHZ03_@ALT_DISJUNCT_WIDEN@_@DISJUNCT_WIDEN@_widening_assign_with_tokens_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_BHZ03_@ALT_DISJUNCT_WIDEN@_@DISJUNCT_WIDEN@_widening_assign_with_tokens(
(Prolog_term_ref t_lhs, Prolog_term_ref t_rhs,
 Prolog_term_ref t_ti, Prolog_term_ref t_to) {
  static const char* where = "ppl_@CLASS@_BHZ03_@ALT_DISJUNCT_WIDEN@_@DISJUNCT_WIDEN@_widening_assign_with_tokens/4";
  try {
    @CPP_CLASS@* lhs = term_to_handle<@CPP_CLASS@ >(t_lhs, where);
    CHECK(lhs);
    const @CPP_CLASS@* rhs = term_to_handle<@CPP_CLASS@ >(t_rhs, where);
    CHECK(rhs);

    unsigned t = term_to_unsigned<unsigned>(t_ti, where);
    lhs->BHZ03_widening_assign<@ALT_DISJUNCT_WIDEN@_Certificate>
      (*rhs,
       widen_fun_ref(&@ALT_CPP_DISJUNCT@::@DISJUNCT_WIDEN@_widening_assign),
       &t);
    if (unify_long(t_to, t))
      return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_BHZ03_@ALT_DISJUNCT_WIDEN@_@DISJUNCT_WIDEN@_widening_assign_code',
`extern "C" Prolog_foreign_return_type

ppl_@CLASS@_BHZ03_@ALT_DISJUNCT_WIDEN@_@DISJUNCT_WIDEN@_widening_assign(
                     Prolog_term_ref t_lhs, Prolog_term_ref t_rhs) {
  static const char* where = "ppl_@CLASS@_BHZ03_@ALT_DISJUNCT_WIDEN@_@DISJUNCT_WIDEN@_widening_assign/2";
  try {
    @CPP_CLASS@* lhs = term_to_handle<@CPP_CLASS@ >(t_lhs, where);
    CHECK(lhs);
    const @CPP_CLASS@* rhs = term_to_handle<@CPP_CLASS@ >(t_rhs, where);
    CHECK(rhs);

    lhs->BHZ03_widening_assign<@ALT_DISJUNCT_WIDEN@_Certificate>
      (*rhs,
       widen_fun_ref(&@ALT_CPP_DISJUNCT@::@DISJUNCT_WIDEN@_widening_assign));
    return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_BGP99_@DISJUNCT_WIDEN@_extrapolation_assign_code',
`extern "C" Prolog_foreign_return_type

ppl_@CLASS@_BGP99_@DISJUNCT_WIDEN@_extrapolation_assign(
                     Prolog_term_ref t_lhs, Prolog_term_ref t_rhs,
                     Prolog_term_ref t_d) {
  static const char* where = "ppl_@CLASS@_BGP99_@DISJUNCT_WIDEN@_extrapolation_assign/3";
  try {
    @CPP_CLASS@* lhs = term_to_handle<@CPP_CLASS@ >(t_lhs, where);
    CHECK(lhs);
    const @CPP_CLASS@* rhs = term_to_handle<@CPP_CLASS@ >(t_rhs, where);
    CHECK(rhs);

    lhs->BGP99_extrapolation_assign
      (*rhs,
       widen_fun_ref(&@ALT_CPP_DISJUNCT@::@DISJUNCT_WIDEN@_widening_assign),
       term_to_unsigned<unsigned>(t_d, where));
    return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_BGP99_@DISJUNCT_EXTRAPOLATION@_extrapolation_assign_code',
`extern "C" Prolog_foreign_return_type

ppl_@CLASS@_BGP99_@DISJUNCT_EXTRAPOLATION@_extrapolation_assign(
                     Prolog_term_ref t_lhs, Prolog_term_ref t_rhs,
                     Prolog_term_ref t_d) {
  static const char* where = "ppl_@CLASS@_BGP99_@DISJUNCT_EXTRAPOLATION@_extrapolation_assign/3";
  try {
    @CPP_CLASS@* lhs = term_to_handle<@CPP_CLASS@ >(t_lhs, where);
    CHECK(lhs);
    const @CPP_CLASS@* rhs = term_to_handle<@CPP_CLASS@ >(t_rhs, where);
    CHECK(rhs);

    lhs->BGP99_extrapolation_assign
      (*rhs,
       widen_fun_ref(&@ALT_CPP_DISJUNCT@::@DISJUNCT_EXTRAPOLATION@_extrapolation_assign),
       term_to_unsigned<unsigned>(t_d, where));
    return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_@LIMITEDBOUNDED@_@WIDENEXPN@_extrapolation_assign_with_tokens_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_@LIMITEDBOUNDED@_@WIDENEXPN@_extrapolation_assign_with_tokens
(Prolog_term_ref t_lhs, Prolog_term_ref t_rhs, Prolog_term_ref t_clist,
 Prolog_term_ref t_ti, Prolog_term_ref t_to) {
  static const char* where = "ppl_@CLASS@_@LIMITEDBOUNDED@_@WIDENEXPN@_extrapolation_assign_with_tokens/5";
  try {
    @CPP_CLASS@* lhs = term_to_handle<@CPP_CLASS@ >(t_lhs, where);
    const @CPP_CLASS@* rhs = term_to_handle<@CPP_CLASS@ >(t_rhs, where);
    CHECK(lhs);
    CHECK(rhs);
    @UCONSTRAINER@_System cs;
    Prolog_term_ref c = Prolog_new_term_ref();

    while (Prolog_is_cons(t_clist)) {
      Prolog_get_cons(t_clist, c, t_clist);
      cs.insert(build_@CONSTRAINER@(c, where));
    }

    // Check the list is properly terminated.
    check_nil_terminating(t_clist, where);

    unsigned t = term_to_unsigned<unsigned>(t_ti, where);
    lhs->@LIMITEDBOUNDED@_@WIDENEXPN@_extrapolation_assign(*rhs, cs, &t);
    if (unify_long(t_to, t))
      return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_@LIMITEDBOUNDED@_@WIDENEXPN@_extrapolation_assign_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_@LIMITEDBOUNDED@_@WIDENEXPN@_extrapolation_assign
(Prolog_term_ref t_lhs, Prolog_term_ref t_rhs, Prolog_term_ref t_clist) {
  static const char* where = "ppl_@CLASS@_@LIMITEDBOUNDED@_@WIDENEXPN@_extrapolation_assign/3";
  try {
    @CPP_CLASS@* lhs = term_to_handle<@CPP_CLASS@ >(t_lhs, where);
    const @CPP_CLASS@* rhs = term_to_handle<@CPP_CLASS@ >(t_rhs, where);
    CHECK(lhs);
    CHECK(rhs);
    @UCONSTRAINER@_System cs;
    Prolog_term_ref c = Prolog_new_term_ref();

    while (Prolog_is_cons(t_clist)) {
      Prolog_get_cons(t_clist, c, t_clist);
     cs.insert(build_@CONSTRAINER@(c, where));
    }

    // Check the list is properly terminated.
    check_nil_terminating(t_clist, where);

    lhs->@LIMITEDBOUNDED@_@WIDENEXPN@_extrapolation_assign(*rhs, cs, 0);
    return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_add_space_dimensions_@EMBEDPROJECT@_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_add_space_dimensions_@EMBEDPROJECT@
(Prolog_term_ref t_ph, Prolog_term_ref t_nnd) {
  static const char* where = "ppl_@CLASS@_add_space_dimensions_@EMBEDPROJECT@/2";
  try {
    @CPP_CLASS@* ph = term_to_handle<@CPP_CLASS@ >(t_ph, where);
    CHECK(ph);
    dimension_type d = term_to_unsigned<dimension_type>(t_nnd, where);
    ph->add_space_dimensions_@EMBEDPROJECT@(d);
    return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_remove_space_dimensions_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_remove_space_dimensions
(Prolog_term_ref t_ph, Prolog_term_ref t_vlist) {
  static const char* where = "ppl_@CLASS@_remove_space_dimensions/2";
  try {
    @CPP_CLASS@* ph = term_to_handle<@CPP_CLASS@ >(t_ph, where);
    CHECK(ph);
    Variables_Set dead_variables;
    Prolog_term_ref v = Prolog_new_term_ref();
    while (Prolog_is_cons(t_vlist)) {
      Prolog_get_cons(t_vlist, v, t_vlist);
      dead_variables.insert(term_to_Variable(v, where).id());
    }

    // Check the list is properly terminated.
    check_nil_terminating(t_vlist, where);

    ph->remove_space_dimensions(dead_variables);
    return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_remove_higher_space_dimensions_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_remove_higher_space_dimensions
(Prolog_term_ref t_ph, Prolog_term_ref t_nd) {
  static const char* where = "ppl_@CLASS@_remove_higher_space_dimensions/2";
  try {
    @CPP_CLASS@* ph = term_to_handle<@CPP_CLASS@ >(t_ph, where);
    CHECK(ph);
    ph->remove_higher_space_dimensions(term_to_unsigned<dimension_type>(t_nd,
                                                                        where));
    return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_expand_space_dimension_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_expand_space_dimension
(Prolog_term_ref t_ph, Prolog_term_ref t_v, Prolog_term_ref t_nd) {
  static const char* where = "ppl_@CLASS@_expand_space_dimension/3";
  try {
    @CPP_CLASS@* ph = term_to_handle<@CPP_CLASS@ >(t_ph, where);
    CHECK(ph);
    ph->expand_space_dimension(term_to_Variable(t_v, where),
                               term_to_unsigned<dimension_type>(t_nd, where));
    return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_fold_space_dimensions_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_fold_space_dimensions
(Prolog_term_ref t_ph, Prolog_term_ref t_vlist, Prolog_term_ref t_v) {
  static const char* where = "ppl_@CLASS@_fold_space_dimensions/3";
  try {
    @CPP_CLASS@* ph = term_to_handle<@CPP_CLASS@ >(t_ph, where);
    CHECK(ph);
    Variables_Set fold_variables;
    Prolog_term_ref v = Prolog_new_term_ref();
    while (Prolog_is_cons(t_vlist)) {
      Prolog_get_cons(t_vlist, v, t_vlist);
      fold_variables.insert(term_to_Variable(v, where).id());
    }

    // Check the list is properly terminated.
    check_nil_terminating(t_vlist, where);

    ph->fold_space_dimensions(fold_variables, term_to_Variable(t_v, where));
    return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_map_space_dimensions_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_map_space_dimensions
(Prolog_term_ref t_ph, Prolog_term_ref t_pfunc) {
  static const char* where = "ppl_@CLASS@_map_space_dimensions/2";
  try {
    @CPP_CLASS@* ph = term_to_handle<@CPP_CLASS@ >(t_ph, where);
    dimension_type space_dim = ph->space_dimension();
    CHECK(ph);
    PFunc pfunc;
    Prolog_term_ref t_pair = Prolog_new_term_ref();
    while (Prolog_is_cons(t_pfunc)) {
      Prolog_get_cons(t_pfunc, t_pair, t_pfunc);
      Prolog_atom functor;
      int arity;
      Prolog_get_compound_name_arity(t_pair, &functor, &arity);
      if (arity != 2 || functor != a_minus)
        return PROLOG_FAILURE;
      Prolog_term_ref t_i = Prolog_new_term_ref();
      Prolog_term_ref t_j = Prolog_new_term_ref();
      Prolog_get_arg(1, t_pair, t_i);
      Prolog_get_arg(2, t_pair, t_j);
      dimension_type i = term_to_Variable(t_i, where).id();
      dimension_type j = term_to_Variable(t_j, where).id();
      if (i >= space_dim || !pfunc.insert(i, j))
        return PROLOG_FAILURE;
    }

    // Check the list is properly terminated.
    check_nil_terminating(t_pfunc, where);

    ph->map_space_dimensions(pfunc);
    return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

m4_define(`ppl_@CLASS@_ascii_dump_code',
`extern "C" Prolog_foreign_return_type
ppl_@CLASS@_ascii_dump
(Prolog_term_ref t_ph) {
  static const char* where = "ppl_@CLASS@_ascii_dump/1";
  try {
    const @CPP_CLASS@* ph = term_to_handle<@CPP_CLASS@ >(t_ph, where);
    CHECK(ph);
    ph->ascii_dump(std::cout);
    return PROLOG_SUCCESS;
  }
  CATCH_ALL;
}

')

divert`'dnl
