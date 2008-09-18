dnl  -*- C++ -*-
m4_divert(-1)

This m4 file contains the program implementation code for generating the
files ppl_ocaml_DOMAIN.cc for each interface domain DOMAIN
in ppl_interface instantiations.m4.

dnl Copyright (C) 2001-2008 Roberto Bagnara <bagnara@cs.unipr.it>
dnl
dnl This file is part of the Parma Polyhedra Library (PPL).
dnl
dnl The PPL is free software; you can redistribute it and/or modify it
dnl under the terms of the GNU General Public License as published by the
dnl Free Software Foundation; either version 3 of the License, or (at your
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

dnl No code is needed for these procedure schemas in the OCaml interface.
dnl
m4_define(`ppl_delete_@CLASS@_code', `')
m4_define(`ppl_@CLASS@_delete_iterator_code', `')

dnl There is no code at present for these procedures in the OCaml interface.
dnl Remove the macro if its definition is added.
dnl
m4_define(`ppl_new_@TOPOLOGY@@CLASS@_from_@FRIEND@_with_complexity_code', `')
m4_define(`ppl_@CLASS@_ascii_dump_code', `')
m4_define(`ppl_@CLASS@_widening_assign_with_tokens_code', `')
m4_define(`ppl_@CLASS@_widening_assign_code', `')
m4_define(`ppl_@CLASS@_@PARTITION@_code', `')
m4_define(`ppl_@CLASS@_approximate_partition_code', `')

  m4_define(`ppl_new_@TOPOLOGY@@CLASS@_from_space_dimension_code',
`dnl
extern "C"
CAMLprim value
ppl_new_@TOPOLOGY@@CLASS@_from_space_dimension(value d, value caml_de) try {
  CAMLparam2(d, caml_de);
  int dd = Int_val(d);
  check_int_is_unsigned(dd);
  Degenerate_Element ppl_de = build_ppl_degenerate_element(caml_de);
  CAMLreturn(val_p_@CLASS@(*new @TOPOLOGY@@CPP_CLASS@(dd, ppl_de)));
}
CATCH_ALL

')

  m4_define(`ppl_new_@TOPOLOGY@@CLASS@_from_@BUILD_REPRESENT@s_code',
`dnl
extern "C"
CAMLprim value
ppl_new_@TOPOLOGY@@CLASS@_from_@BUILD_REPRESENT@s(value cl) try {
  CAMLparam1(cl);
  @UBUILD_REPRESENT@_System cs = build_ppl_@UBUILD_REPRESENT@_System(cl);
  CAMLreturn(val_p_@CLASS@(*new @TOPOLOGY@@CPP_CLASS@(cs)));
}
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_relation_with_@RELATION_REPRESENT@_code',
`dnl
extern "C"
CAMLprim value
ppl_@CLASS@_relation_with_@RELATION_REPRESENT@(value ph, value c) try {
  CAMLparam2(ph, c);
  const @CPP_CLASS@& pph = *p_@CLASS@_val(ph);
  @URELATION_REPRESENT@ ppl_c = build_ppl_@URELATION_REPRESENT@(c);
  Poly_@UALT_RELATION_REPRESENT@_Relation r = pph.relation_with(ppl_c);
  CAMLreturn(build_ocaml_poly_@ALT_RELATION_REPRESENT@_relation(r));
}
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_@DIMENSION@_code',
`dnl
extern "C"
CAMLprim value
ppl_@CLASS@_@DIMENSION@(value ph) try {
  CAMLparam1(ph);
  const @CPP_CLASS@& pph = *p_@CLASS@_val(ph);
  dimension_type d = pph.@DIMENSION@();
  if (d > INT_MAX)
    abort();
  CAMLreturn(Val_int(d));
}
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_@HAS_PROPERTY@_code',
`dnl
extern "C"
CAMLprim value
ppl_@CLASS@_@HAS_PROPERTY@(value ph) try {
  CAMLparam1(ph);
  const @CPP_CLASS@& pph = *p_@CLASS@_val(ph);
  CAMLreturn(Val_bool(pph.@HAS_PROPERTY@()));
}
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_@SIMPLIFY@_code',
`dnl
extern "C"
void
ppl_@CLASS@_@SIMPLIFY@(value ph) try {
  CAMLparam1(ph);
  @CPP_CLASS@& pph = *p_@CLASS@_val(ph);
  pph.@SIMPLIFY@();
  CAMLreturn0;
}
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_bounds_from_@ABOVEBELOW@_code',
`dnl
extern "C"
CAMLprim value
ppl_@CLASS@_bounds_from_@ABOVEBELOW@(value ph, value le) try {
  CAMLparam2(ph, le);
  const @CPP_CLASS@& pph = *p_@CLASS@_val(ph);
  Linear_Expression ple = build_ppl_Linear_Expression(le);
  CAMLreturn(Val_bool(pph.bounds_from_@ABOVEBELOW@(ple)));
}
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_add_@ADD_REPRESENT@_code',
`dnl
extern "C"
void
ppl_@CLASS@_add_@ADD_REPRESENT@(value ph, value c) try {
  CAMLparam2(ph, c);
  @CPP_CLASS@& pph = *p_@CLASS@_val(ph);
  @UADD_REPRESENT@ pc = build_ppl_@UADD_REPRESENT@(c);
  pph.add_@ADD_REPRESENT@(pc);
  CAMLreturn0;
}
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_add_@ADD_REPRESENT@_and_minimize_code',
`dnl
extern "C"
CAMLprim value
ppl_@CLASS@_add_@ADD_REPRESENT@_and_minimize(value ph, value c) try {
  CAMLparam2(ph, c);
  @CPP_CLASS@& pph = *p_@CLASS@_val(ph);
  @UADD_REPRESENT@ pc = build_ppl_@UADD_REPRESENT@(c);
  CAMLreturn(Val_bool(pph.add_@ADD_REPRESENT@_and_minimize(pc)));
}
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_add_@ADD_REPRESENT@s_code',
`dnl
extern "C"
void
ppl_@CLASS@_add_@ADD_REPRESENT@s(value ph, value cs) try {
  CAMLparam2(ph, cs);
  @CPP_CLASS@& pph = *p_@CLASS@_val(ph);
  @UADD_REPRESENT@_System pcs = build_ppl_@UADD_REPRESENT@_System(cs);
  pph.add_@ADD_REPRESENT@s(pcs);
  CAMLreturn0;
}
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_add_@ADD_REPRESENT@s_and_minimize_code',
`dnl
extern "C"
CAMLprim value
ppl_@CLASS@_add_@ADD_REPRESENT@s_and_minimize(value ph, value cs) try {
  CAMLparam2(ph, cs);
  @CPP_CLASS@& pph = *p_@CLASS@_val(ph);
  @UADD_REPRESENT@_System pcs = build_ppl_@UADD_REPRESENT@_System(cs);
  CAMLreturn(Val_bool(pph.add_@ADD_REPRESENT@s_and_minimize(pcs)));
}
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_refine_with_@REFINE_REPRESENT@_code',
`dnl
extern "C"
void
ppl_@CLASS@_refine_with_@REFINE_REPRESENT@(value ph, value c) try {
  CAMLparam2(ph, c);
  @CPP_CLASS@& pph = *p_@CLASS@_val(ph);
  @UREFINE_REPRESENT@ pc = build_ppl_@UREFINE_REPRESENT@(c);
  pph.refine_with_@REFINE_REPRESENT@(pc);
  CAMLreturn0;
}
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_refine_with_@REFINE_REPRESENT@s_code',
`dnl
extern "C"
void
ppl_@CLASS@_refine_with_@REFINE_REPRESENT@s(value ph, value cs) try {
  CAMLparam2(ph, cs);
  @CPP_CLASS@& pph = *p_@CLASS@_val(ph);
  @UREFINE_REPRESENT@_System pcs = build_ppl_@UREFINE_REPRESENT@_System(cs);
  pph.refine_with_@REFINE_REPRESENT@s(pcs);
  CAMLreturn0;
}
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_@COMPARISON@_@CLASS@_code',
`dnl
extern "C"
CAMLprim value
ppl_@CLASS@_@COMPARISON@_@CLASS@(value ph1, value ph2) try {
  CAMLparam2(ph1, ph2);
  const @CPP_CLASS@& pph1 = *p_@CLASS@_val(ph1);
  const @CPP_CLASS@& pph2 = *p_@CLASS@_val(ph2);
  CAMLreturn(Val_bool(pph1.@COMPARISON@(pph2)));
}
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_equals_@CLASS@_code',
`dnl
extern "C"
CAMLprim value
ppl_@CLASS@_equals_@CLASS@(value ph1, value ph2) try {
  CAMLparam2(ph1, ph2);
  const @CPP_CLASS@& pph1 = *p_@CLASS@_val(ph1);
  const @CPP_CLASS@& pph2 = *p_@CLASS@_val(ph2);
  CAMLreturn(Val_bool(pph1 == pph2));
}
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_@BINOP@_code',
`dnl
extern "C"
void
ppl_@CLASS@_@BINOP@(value ph1, value ph2) try {
  CAMLparam2(ph1, ph2);
  @CPP_CLASS@& pph1 = *p_@CLASS@_val(ph1);
  @CPP_CLASS@& pph2 = *p_@CLASS@_val(ph2);
  pph1.@BINOP@(pph2);
  CAMLreturn0;
}
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_@BINMINOP@_code',
`dnl
extern "C"
CAMLprim value
ppl_@CLASS@_@BINMINOP@(value ph1, value ph2) try {
  CAMLparam2(ph1, ph2);
  @CPP_CLASS@& pph1 = *p_@CLASS@_val(ph1);
  @CPP_CLASS@& pph2 = *p_@CLASS@_val(ph2);
  CAMLreturn(Val_bool(pph1.@BINMINOP@(pph2)));
}
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_simplify_using_context_assign_code',
`dnl
extern "C"
CAMLprim value
ppl_@CLASS@_simplify_using_context_assign(value ph1, value ph2) try {
  CAMLparam2(ph1, ph2);
  @CPP_CLASS@& pph1 = *p_@CLASS@_val(ph1);
  @CPP_CLASS@& pph2 = *p_@CLASS@_val(ph2);
  CAMLreturn(Val_bool(pph1.simplify_using_context_assign(pph2)));
}
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_add_space_dimensions_@EMBEDPROJECT@_code',
`dnl
extern "C"
void
ppl_@CLASS@_add_space_dimensions_@EMBEDPROJECT@(value ph,
							     value d) try {
  CAMLparam2(ph, d);
  int dd = Int_val(d);
  check_int_is_unsigned(dd);
  @CPP_CLASS@& pph = *p_@CLASS@_val(ph);
  pph.add_space_dimensions_and_embed(dd);
  CAMLreturn0;
							     }
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_remove_space_dimensions_code',
`dnl
extern "C"
void
ppl_@CLASS@_remove_space_dimensions(value ph, value caml_vset) try {
  CAMLparam2(ph, caml_vset);
  @CPP_CLASS@& pph = *p_@CLASS@_val(ph);
  pph.remove_space_dimensions(build_ppl_Variables_Set(caml_vset));
  CAMLreturn0;
}
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_remove_higher_space_dimensions_code',
`dnl
extern "C"
void
ppl_@CLASS@_remove_higher_space_dimensions(value ph,
							     value d) try {
  CAMLparam2(ph, d);
  int dd = Int_val(d);
  check_int_is_unsigned(dd);
  @CPP_CLASS@& pph = *p_@CLASS@_val(ph);
  pph.remove_higher_space_dimensions(dd);
  CAMLreturn0;
}
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_fold_space_dimensions_code',
`dnl
extern "C"
void
ppl_@CLASS@_fold_space_dimensions(value ph, value caml_vset, value caml_dim)
  try {
  CAMLparam1(ph);
  dimension_type ppl_dim = Int_val(caml_dim);
  @CPP_CLASS@& pph = *p_@CLASS@_val(ph);
  Variables_Set ppl_vset;
  if (Int_val(caml_vset) == 0)
    CAMLreturn0;
  while (true) {
    ppl_vset.insert(Int_val(Field(caml_vset, 0)));
    if (Int_val(Field(caml_vset, 1)) == 0)
      break;
    caml_vset = Field(caml_vset, 1);
  }
  pph.fold_space_dimensions(ppl_vset, Variable(ppl_dim));
  CAMLreturn0;
}
CATCH_ALL

')


  m4_define(`ppl_@CLASS@_map_space_dimensions_code',
`dnl
extern "C"
void
ppl_@CLASS@_map_space_dimensions(value ph, value caml_mapped_dims) try {
  CAMLparam2(ph, caml_mapped_dims);
  @CPP_CLASS@& pph = *p_@CLASS@_val(ph);
  PFunc pfunc;
  while (caml_mapped_dims != Val_int(0)) {
    Int_val(Field(Field(caml_mapped_dims, 0),0));
    int domain_value = Int_val(Field(Field(caml_mapped_dims, 0),0));
    int codomain_value = Int_val(Field(Field(caml_mapped_dims, 0),1));
    pfunc.insert(domain_value,
		 codomain_value);
    caml_mapped_dims = Field(caml_mapped_dims, 1);
  }
  pph.map_space_dimensions(pfunc);
  CAMLreturn0;
}
CATCH_ALL

')


  m4_define(`ppl_@CLASS@_expand_space_dimension_code',
`dnl
extern "C"
void
ppl_@CLASS@_expand_space_dimension(value ph,
						     value var_index,
						     value m) try {
  CAMLparam3(ph, var_index, m);
  int c_m = Int_val(m);
  check_int_is_unsigned(c_m);
  @CPP_CLASS@& pph = *p_@CLASS@_val(ph);
  pph.expand_space_dimension(build_ppl_Variable(var_index), c_m);
  CAMLreturn0;
}
CATCH_ALL

')


  m4_define(`ppl_@CLASS@_get_@GET_REPRESENT@s_code',
`dnl
extern "C"
CAMLprim value
ppl_@CLASS@_get_@GET_REPRESENT@s(value ph) try {
  CAMLparam1(ph);
  @CPP_CLASS@& pph = *p_@CLASS@_val(ph);
  CAMLreturn(build_caml_@GET_REPRESENT@_system(pph.@GET_REPRESENT@s()));
}
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_get_minimized_@GET_REPRESENT@s_code',
`dnl
extern "C"
CAMLprim value
ppl_@CLASS@_get_minimized_@GET_REPRESENT@s(value ph) try {
  CAMLparam1(ph);
  @CPP_CLASS@& pph = *p_@CLASS@_val(ph);
  CAMLreturn(build_caml_@GET_REPRESENT@_system(pph.minimized_@GET_REPRESENT@s()));
}
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_constrains_code',
`dnl
extern "C"
CAMLprim value
ppl_@CLASS@_constrains(value ph, value var) try {
  CAMLparam2(ph, var);
  @CPP_CLASS@& pph = *p_@CLASS@_val(ph);
  CAMLreturn(Val_bool(pph.constrains(build_ppl_Variable(var))));
			    }
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_unconstrain_space_dimension_code',
`dnl
extern "C"
void
ppl_@CLASS@_unconstrain_space_dimension(value ph, value var) try {
  CAMLparam2(ph, var);
  @CPP_CLASS@& pph = *p_@CLASS@_val(ph);
  pph.unconstrain(build_ppl_Variable(var));
  CAMLreturn0;
			    }
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_unconstrain_space_dimensions_code',
`dnl
extern "C"
void
ppl_@CLASS@_unconstrain_space_dimensions(value ph, value caml_vset)
  try {
    CAMLparam2(ph, caml_vset);
    @CPP_CLASS@& pph = *p_@CLASS@_val(ph);
    pph.unconstrain(build_ppl_Variables_Set(caml_vset));
    CAMLreturn0;
			    }
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_bounded_@AFFIMAGE@_code',
`dnl
extern "C"
void
ppl_@CLASS@_bounded_@AFFIMAGE@(value ph, value var, value lb_expr,
				    value ub_expr, value coeff) try {
  CAMLparam5(ph, var, lb_expr, ub_expr, coeff);
  @CPP_CLASS@& pph = *p_@CLASS@_val(ph);
  pph.bounded_@AFFIMAGE@(build_ppl_Variable(var),
			   build_ppl_Linear_Expression(lb_expr),
 			   build_ppl_Linear_Expression(ub_expr),
 			   build_ppl_Coefficient(coeff));
  CAMLreturn0;
}
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_@AFFIMAGE@_code',
`dnl
extern "C"
void
ppl_@CLASS@_@AFFIMAGE@(value ph, value var, value expr,
			    value coeff) try {
  CAMLparam4(ph, var, expr, coeff);
  @CPP_CLASS@& pph = *p_@CLASS@_val(ph);
  build_ppl_Linear_Expression(expr);
  pph.@AFFIMAGE@(build_ppl_Variable(var),
		   build_ppl_Linear_Expression(expr),
		   build_ppl_Coefficient(coeff));
  CAMLreturn0;
			    }
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_generalized_@AFFIMAGE@_lhs_rhs_code',
`dnl
extern "C"
void
ppl_@CLASS@_generalized_@AFFIMAGE@1(value ph, value le1, value rel_sym,
					 value le2) try {
  CAMLparam4(ph, le1, rel_sym, le2);
  build_ppl_relsym(rel_sym);
  @CPP_CLASS@& pph = *p_@CLASS@_val(ph);
  pph.generalized_@AFFIMAGE@(build_ppl_Linear_Expression(le1),
			       build_ppl_relsym(rel_sym),
			       build_ppl_Linear_Expression(le2));
  CAMLreturn0;
 }
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_generalized_@AFFIMAGE@_code',
`dnl
extern "C"
void
ppl_@CLASS@_generalized_@AFFIMAGE@2(value ph, value int_val,
					 value rel_sym,
					 value le, value caml_coeff) try {
  CAMLparam5(ph, int_val, rel_sym, le, caml_coeff);
  @CPP_CLASS@& pph = *p_@CLASS@_val(ph);
  pph.generalized_@AFFIMAGE@(build_ppl_Variable(int_val),
			       build_ppl_relsym(rel_sym),
			       build_ppl_Linear_Expression(le),
			       build_ppl_Coefficient(caml_coeff));
  CAMLreturn0;
 }
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_generalized_@AFFIMAGE@_with_congruence_code',
`dnl
extern "C"
void
ppl_@CLASS@_generalized_@AFFIMAGE@1_with_congruence(value ph,
                                         value int_val,
                                         value rel_sym,
					 value le,
                                         value caml_coeff,
                                         value caml_modulus) try {
  CAMLparam5(ph, int_val, rel_sym, le, caml_coeff);
  CAMLxparam1(caml_modulus);
  build_ppl_relsym(rel_sym);
  @CPP_CLASS@& pph = *p_@CLASS@_val(ph);
  pph.generalized_@AFFIMAGE@(build_ppl_Variable(int_val),
			       build_ppl_relsym(rel_sym),
			       build_ppl_Linear_Expression(le),
			       build_ppl_Coefficient(caml_coeff),
			       build_ppl_Coefficient(caml_modulus));
  CAMLreturn0;
 }
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_generalized_@AFFIMAGE@_lhs_rhs_with_congruence_code',
`dnl
extern "C"
void
ppl_@CLASS@_generalized_@AFFIMAGE@1_lhs_rhs_with_congruence(value ph,
                                         value le1,
                                         value rel_sym,
					 value le2,
                                         value caml_modulus) try {
  CAMLparam5(ph, le1, rel_sym, le2, caml_modulus);
  build_ppl_relsym(rel_sym);
  @CPP_CLASS@& pph = *p_@CLASS@_val(ph);
  pph.generalized_@AFFIMAGE@(build_ppl_Linear_Expression(le1),
			     build_ppl_relsym(rel_sym),
			     build_ppl_Linear_Expression(le2),
			     build_ppl_Coefficient(caml_modulus));
  CAMLreturn0;
 }
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_@WIDEN@_widening_assign_code',
`dnl
extern "C"
void
ppl_@CLASS@_@WIDEN@_widening_assign(value ph1, value ph2) try {
  CAMLparam2(ph1, ph2);
  @CPP_CLASS@& pph1 = *p_@CLASS@_val(ph1);
  @CPP_CLASS@& pph2 = *p_@CLASS@_val(ph2);
  pph1.@WIDEN@_widening_assign(pph2);
  CAMLreturn0;
}
CATCH_ALL

')


  m4_define(`ppl_@CLASS@_@WIDEN@_widening_assign_with_tokens_code',
`dnl
extern "C"
CAMLprim value
ppl_@CLASS@_@WIDEN@_widening_assign_with_tokens(value ph1, value ph2,
						     value integer) try {
  CAMLparam3(ph1, ph2, integer);
  @CPP_CLASS@& pph1 = *p_@CLASS@_val(ph1);
  @CPP_CLASS@& pph2 = *p_@CLASS@_val(ph2);
  int cpp_int = Val_int(integer);
  check_int_is_unsigned(cpp_int);
  unsigned int unsigned_value = cpp_int;
  pph1.@WIDEN@_widening_assign(pph2, &unsigned_value);
  CAMLreturn(Int_val(unsigned_value));
}
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_@LIMITEDBOUNDED@_@WIDENEXPN@_extrapolation_assign_with_tokens_code',
`dnl
extern "C"
CAMLprim value
ppl_@CLASS@_@LIMITEDBOUNDED@_@WIDENEXPN@_extrapolation_assign_with_tokens(value ph1,
						   value ph2,
						   value caml_cs,
						   value integer) try {
  CAMLparam4(ph1, ph2, caml_cs, integer);
  @CPP_CLASS@& pph1 = *p_@CLASS@_val(ph1);
  @CPP_CLASS@& pph2 = *p_@CLASS@_val(ph2);
  @UCONSTRAINER@_System ppl_cs = build_ppl_@UCONSTRAINER@_System(caml_cs);
  int cpp_int = Val_int(integer);
  check_int_is_unsigned(cpp_int);
  unsigned int unsigned_value = cpp_int;
  pph1.@LIMITEDBOUNDED@_@WIDENEXPN@_extrapolation_assign(pph2, ppl_cs,
							 &unsigned_value);
  CAMLreturn(Int_val(unsigned_value));
}
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_@LIMITEDBOUNDED@_@WIDENEXPN@_extrapolation_assign_code',
`dnl
extern "C"
void
ppl_@CLASS@_@LIMITEDBOUNDED@_@WIDENEXPN@_extrapolation_assign(value ph1,
						   value ph2,
						   value caml_cs) try {
  CAMLparam3(ph1, ph2, caml_cs);
  @CPP_CLASS@& pph1 = *p_@CLASS@_val(ph1);
  @CPP_CLASS@& pph2 = *p_@CLASS@_val(ph2);
  @UCONSTRAINER@_System ppl_cs = build_ppl_@UCONSTRAINER@_System(caml_cs);
  pph1.@LIMITEDBOUNDED@_@WIDENEXPN@_extrapolation_assign(pph2, ppl_cs);
  CAMLreturn0;
}
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_@MAXMIN@_code',
`dnl
extern "C"
CAMLprim value
ppl_@CLASS@_@MAXMIN@(value ph, value caml_le) try {
  CAMLparam2(ph, caml_le);
  TEMP_INTEGER(num);
  TEMP_INTEGER(den);
  bool is_supremum = false;
  @CPP_CLASS@& pph = *p_@CLASS@_val(ph);
  bool ppl_return_value = pph.@MAXMIN@(build_ppl_Linear_Expression(caml_le),
				      num, den, is_supremum);
  value caml_return_value = caml_alloc(4,0);
  Field(caml_return_value, 0) = Val_bool(ppl_return_value);
  Field(caml_return_value, 1) = build_caml_coefficient(num);
  Field(caml_return_value, 2) = build_caml_coefficient(den);
  Field(caml_return_value, 3) = Val_bool(is_supremum);
  CAMLreturn(caml_return_value);
}
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_@MAXMIN@_with_point_code',
`dnl
extern "C"
CAMLprim value
ppl_@CLASS@_@MAXMIN@_with_point(value ph, value caml_le) try {
  CAMLparam2(ph, caml_le);
  TEMP_INTEGER(num);
  TEMP_INTEGER(den);
  bool is_supremum = false;
  Generator g = point();
  @CPP_CLASS@& pph = *p_@CLASS@_val(ph);
  bool ppl_return_value = pph.@MAXMIN@(build_ppl_Linear_Expression(caml_le),
				      num, den, is_supremum, g);
  value caml_return_value = caml_alloc(5,0);
  Field(caml_return_value, 0) = Val_bool(ppl_return_value);
  Field(caml_return_value, 1) = build_caml_coefficient(num);
  Field(caml_return_value, 2) = build_caml_coefficient(den);
  Field(caml_return_value, 3) = Val_bool(is_supremum);
  Field(caml_return_value, 4) = build_caml_generator(g);
  CAMLreturn(caml_return_value);
}
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_OK_code',
`dnl
extern "C"
CAMLprim value
ppl_@CLASS@_OK(value ph) try {
  CAMLparam1(ph);
  @CPP_CLASS@& pph = *p_@CLASS@_val(ph);
  CAMLreturn(Bool_val(pph.OK()));
}
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_@MEMBYTES@_code',
`dnl
extern "C"
CAMLprim value
ppl_@CLASS@_@MEMBYTES@(value ph) try {
  CAMLparam1(ph);
  @CPP_CLASS@& pph = *p_@CLASS@_val(ph);
  CAMLreturn(Val_int(pph.@MEMBYTES@()));
}
CATCH_ALL

 ')

  m4_define(`ppl_@CLASS@_swap_code',
`dnl
extern "C"
void
ppl_@CLASS@_swap(value ph1, value ph2) try {
  CAMLparam2(ph1, ph2);
  @CPP_CLASS@& pph1 = *p_@CLASS@_val(ph1);
  @CPP_CLASS@& pph2 = *p_@CLASS@_val(ph2);
  pph1.swap(pph2);
  CAMLreturn0;
}
CATCH_ALL

')

  m4_define(`ppl_new_@TOPOLOGY@@CLASS@_from_@FRIEND@_code',
`dnl
extern "C"
CAMLprim value
ppl_new_@TOPOLOGY@@CLASS@_from_@FRIEND@(value ph) try {
  CAMLparam1(ph);
  @CPPX_FRIEND@& pph = *(reinterpret_cast<@CPPX_FRIEND@*>( p_@ALT_FRIEND@_val(ph)));
  CAMLreturn(val_p_@CLASS@(*(reinterpret_cast<@CPP_CLASS@*>(new @TOPOLOGY@@CPP_CLASS@(pph)))));
}
CATCH_ALL

')

#   m4_define(`ppl_@CLASS@_@UB_EXACT@_code',
# `dnl
# extern "C"
# CAMLprim value
# ppl_@TOPOLOGY@@CLASS@_@UB_EXACT@(value ph1, value ph2) try {
#   CAMLparam2(ph1, ph2);
#   @TOPOLOGY@@CPP_CLASS@& pph1 = *p_@TOPOLOGY@@CLASS@_val(ph1);
#   const @TOPOLOGY@@CPP_CLASS@& pph2 = *p_@TOPOLOGY@@CLASS@_val(ph2);
#   CAMLreturn(Val_bool(pph1.@UB_EXACT@(pph2)));
# }
# CATCH_ALL

# ')

  m4_define(`ppl_@CLASS@_@EXTRAPOLATION@_extrapolation_assign_code',
`dnl
extern "C"
void
ppl_@CLASS@_@EXTRAPOLATION@_extrapolation_assign(value ph1,
                                                 value ph2) try {
  CAMLparam2(ph1, ph2);
  @CPP_CLASS@& pph1 = *p_@CLASS@_val(ph1);
  @CPP_CLASS@& pph2 = *p_@CLASS@_val(ph2);
  pph1.@EXTRAPOLATION@_extrapolation_assign(pph2);
  CAMLreturn0;
}
CATCH_ALL

')


  m4_define(`ppl_@CLASS@_@EXTRAPOLATION@_extrapolation_assign_with_tokens_code',
`dnl
extern "C"
CAMLprim value
ppl_@CLASS@_@EXTRAPOLATION@_extrapolation_assign_with_tokens(
                                                     value ph1, value ph2,
						     value integer) try {
  CAMLparam3(ph1, ph2, integer);
  @CPP_CLASS@& pph1 = *p_@CLASS@_val(ph1);
  @CPP_CLASS@& pph2 = *p_@CLASS@_val(ph2);
  int cpp_int = Val_int(integer);
  check_int_is_unsigned(cpp_int);
  unsigned int unsigned_value = cpp_int;
  pph1.@EXTRAPOLATION@_extrapolation_assign(pph2, &unsigned_value);
  CAMLreturn(Int_val(unsigned_value));
}
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_@EXTRAPOLATION@_narrowing_assign_code',
`dnl
extern "C"
void
ppl_@CLASS@_@EXTRAPOLATION@_narrowing_assign(value ph1,
                                                       value ph2) try {
  CAMLparam2(ph1, ph2);
  @CPP_CLASS@& pph1 = *p_@CLASS@_val(ph1);
  const @CPP_CLASS@& pph2 = *p_@CLASS@_val(ph2);
  pph1.@EXTRAPOLATION@_narrowing_assign(pph2);
  CAMLreturn0;
}
CATCH_ALL

 ')

  m4_define(`ppl_@CLASS@_@BEGINEND@_iterator_code',
 `dnl
 extern "C"
 CAMLprim value
 ppl_@CLASS@_@BEGINEND@_iterator(value t_pps) try {
   CAMLparam1(t_pps);
   @CPP_CLASS@& pps = *p_@CLASS@_val(t_pps);
  CAMLreturn(val_p_@CLASS@_iterator(*new @CPP_CLASS@::iterator(pps.@BEGINEND@())));

 }
 CATCH_ALL

 ')


  m4_define(`ppl_@CLASS@_get_disjunct_code',
`dnl
 extern "C"
 CAMLprim value
ppl_@CLASS@_get_disjunct(value caml_it) {
   CAMLparam1(caml_it);
   @CPP_CLASS@::iterator& cpp_it  = *p_@CLASS@_iterator_val(caml_it);
   @CLASSTOPOLOGY@@CPP_DISJUNCT@ disjunct = cpp_it->element();
   value value_to_return = val_p_@DISJUNCT@(disjunct);
   actual_p_@CPP_DISJUNCT@_val(value_to_return) =
                                              mark(&disjunct);
   CAMLreturn(value_to_return);

}

')

  m4_define(`ppl_@CLASS@_add_disjunct_code',
`dnl
extern "C"
void
ppl_@CLASS@_add_disjunct(value t_pps, value caml_item_to_add) try {
   CAMLparam2(t_pps, caml_item_to_add);
   @CPP_CLASS@& pps = *p_@CLASS@_val(t_pps);
   @CLASSTOPOLOGY@@CPP_DISJUNCT@& item = * (reinterpret_cast<@CLASSTOPOLOGY@@DISJUNCT@*> (p_@DISJUNCT@_val(caml_item_to_add)));
   pps.add_disjunct(item);
CAMLreturn0;
}
CATCH_ALL

 ')

  m4_define(`ppl_@CLASS@_drop_disjunct_code',
`dnl
 extern "C"
void
 ppl_@CLASS@_drop_disjunct(value t_pps, value caml_item_to_drop) try {
   CAMLparam2(t_pps, caml_item_to_drop);
   @CPP_CLASS@& pps = *p_@CLASS@_val(t_pps);
   @CPP_CLASS@::iterator& itr = *p_@CLASS@_iterator_val(caml_item_to_drop);
 pps.drop_disjunct(itr);
CAMLreturn0;
 }
 CATCH_ALL

 ')

  m4_define(`ppl_@CLASS@_@INCDEC@_iterator_code',
`dnl
 extern "C"
void
 ppl_@CLASS@_@INCDEC@_iterator(value caml_itr) try {
   CAMLparam1(caml_itr);
   @CPP_CLASS@::iterator& itr = *p_@CLASS@_iterator_val(caml_itr);
    @CPPX_INCDEC@itr;
   CAMLreturn0;
 }
 CATCH_ALL

 ')

  m4_define(`ppl_@CLASS@_iterator_equals_iterator_code',
`dnl
//! Give access to the embedded @CLASS@* in \p v.
inline @CPP_CLASS@::iterator*&
p_@CLASS@_iterator_val(value v) {
  return *reinterpret_cast<@CPP_CLASS@::iterator**>(Data_custom_val(v));
}

void
custom_@CLASS@_iterator_finalize(value v) {
  delete p_@CLASS@_iterator_val(v);
}

static struct custom_operations @CLASS@_iterator_custom_operations = {
  "it.unipr.cs.ppl" "." PPL_VERSION "." "@CLASS@_iterator"@COMMA@
  custom_@CLASS@_iterator_finalize@COMMA@
  custom_compare_default@COMMA@
  custom_hash_default@COMMA@
  custom_serialize_default@COMMA@
  custom_deserialize_default
};

inline value
val_p_@CLASS@_iterator(const @CPP_CLASS@::iterator& ph) {
  value v = caml_alloc_custom(&@CLASS@_iterator_custom_operations,
			      sizeof(@CPP_CLASS@::iterator*), 0, 1);
  p_@CLASS@_iterator_val(v) = const_cast<@CPP_CLASS@::iterator*>(&ph);
  return(v);
}

 extern "C"
CAMLprim value
 ppl_@CLASS@_iterator_equals_iterator(value caml_itr1, value caml_itr2) try {
   CAMLparam2(caml_itr1, caml_itr2);
  @CPP_CLASS@::iterator& itr1 = *p_@CLASS@_iterator_val(caml_itr1);
  @CPP_CLASS@::iterator& itr2 = *p_@CLASS@_iterator_val(caml_itr2);
 if (itr1 == itr2)
   CAMLreturn(Val_bool(true));
 else
   CAMLreturn(Val_bool(false));
 }
 CATCH_ALL

 ')


  m4_define(`ppl_@CLASS@_BHZ03_@ALT_DISJUNCT_WIDEN@_@DISJUNCT_WIDEN@_widening_assign_code',
`dnl
extern "C"
void
ppl_@CLASS@_BHZ03_@ALT_DISJUNCT_WIDEN@_@DISJUNCT_WIDEN@_widening_assign(
                                               value ph1,
                                               value ph2) try {
  CAMLparam2(ph1, ph2);
  @CPP_CLASS@& pph1 = *p_@CLASS@_val(ph1);
  @CPP_CLASS@& pph2 = *p_@CLASS@_val(ph2);
  pph1.BHZ03_widening_assign<@ALT_DISJUNCT_WIDEN@_Certificate>
      (pph2,
       widen_fun_ref(&@CLASSTOPOLOGY@@CPP_DISJUNCT@::@DISJUNCT_WIDEN@_widening_assign));;
  CAMLreturn0;
}
CATCH_ALL

')

  m4_define(`ppl_@CLASS@_BGP99_@DISJUNCT_WIDEN@_extrapolation_assign_code',
`dnl
extern "C"
void
ppl_@CLASS@_BGP99_@DISJUNCT_WIDEN@_extrapolation_assign(value ph1,
                                              value ph2,
                                              value integer) try {
  CAMLparam2(ph1, ph2);
  @CPP_CLASS@& pph1 = *p_@CLASS@_val(ph1);
  @CPP_CLASS@& pph2 = *p_@CLASS@_val(ph2);
  int cpp_int = Val_int(integer);
  check_int_is_unsigned(cpp_int);
  pph1.BGP99_extrapolation_assign(
       pph2,
       widen_fun_ref(&@CLASSTOPOLOGY@@CPP_DISJUNCT@::@DISJUNCT_WIDEN@_widening_assign), cpp_int);
  CAMLreturn0;
}
CATCH_ALL

')
