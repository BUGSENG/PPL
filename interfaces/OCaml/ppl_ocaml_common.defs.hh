/* Domain-independent part of the OCaml interface: declarations.
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

#ifndef PPL_ppl_ocaml_common_defs_hh
#define PPL_ppl_ocaml_common_defs_hh 1

#include "ppl.hh"
#ifdef PPL_WATCHDOG_LIBRARY_ENABLED
#include "pwl.hh"
#endif
#include "interfaced_boxes.hh"
#include "marked_pointers.hh"

// OCaml include files.
extern "C" {

#define CAML_NAME_SPACE
#include "caml/mlvalues.h"
#include "caml/memory.h"
#include "caml/custom.h"
#include "caml/fail.h"
#include "caml/callback.h"
#include "caml/alloc.h"
#undef CAML_NAME_SPACE

} // extern "C"

namespace Parma_Polyhedra_Library {

namespace Interfaces {

namespace OCaml {

template <typename U_Int>
U_Int value_to_unsigned(value v);

value
ppl_dimension_to_value(dimension_type dim);

dimension_type
value_to_ppl_dimension(value dim);

Variable
build_ppl_Variable(value var);

Degenerate_Element
build_ppl_Degenerate_Element(value de);

Complexity_Class
build_ppl_Complexity_Class(value cc);

Relation_Symbol
build_ppl_relsym(value caml_relsym);

Coefficient
build_ppl_Coefficient(value coeff);

Variable
build_ppl_Variable(value caml_var);

Variables_Set
build_ppl_Variables_Set(value caml_vset);

Linear_Expression
build_ppl_Linear_Expression(value e);

Constraint
build_ppl_Constraint(value c);

Congruence
build_ppl_Congruence(value c);

Generator
build_ppl_Generator(value g);

Grid_Generator
build_ppl_Grid_Generator(value gg);

Constraint_System
build_ppl_Constraint_System(value cl);

Congruence_System
build_ppl_Congruence_System(value cgl);

Generator_System
build_ppl_Generator_System(value gl);

Grid_Generator_System
build_ppl_Grid_Generator_System(value ggs);

value
build_ocaml_poly_con_relation(Poly_Con_Relation& r);

value
build_ocaml_poly_gen_relation(Poly_Gen_Relation& r);

value
build_ocaml_coefficient(const Coefficient& c);

value
build_ocaml_constraint(const Constraint& c);

value
build_ocaml_congruence(const Congruence& cg);

value
build_ocaml_generator(const Generator& g);

value
build_ocaml_constraint_system(const Constraint_System& cs);

value
build_ocaml_congruence_system(const Congruence_System& cgs);

value
build_ocaml_grid_generator(const Grid_Generator& gg);

value
build_ocaml_generator_system(const Generator_System& gs);

value
build_ocaml_grid_generator_system(const Grid_Generator_System& ggs);

class Partial_Function {
public:
  Partial_Function();

  bool has_empty_codomain() const;

  dimension_type max_in_codomain() const;

  bool maps(dimension_type i, dimension_type& j) const;

  bool insert(dimension_type i, dimension_type j);

private:
  std::set<dimension_type> codomain;

  std::vector<dimension_type> vec;
};

class timeout_exception : public Parma_Polyhedra_Library::Throwable {
public:
  void throw_me() const {
    throw *this;
  }
  int priority() const {
    return 0;
  }
  timeout_exception() {
  }
};

void reset_timeout();

} // namespace OCaml

} // namespace Interfaces

} // namespace Parma_Polyhedra_Library

#define CATCH_ALL							\
catch(std::bad_alloc&) {						\
  caml_raise_out_of_memory();						\
}									\
catch(std::invalid_argument& e) {					\
  caml_invalid_argument(const_cast<char*>(e.what()));			\
}									\
catch(std::overflow_error& e) {					        \
  caml_raise_with_string(*caml_named_value("PPL_arithmetic_overflow"),  \
                         (const_cast<char*>(e.what())));		\
}									\
catch(std::runtime_error& e) {                                          \
  caml_raise_with_string(*caml_named_value("PPL_internal_error"),	\
                         (const_cast<char*>(e.what())));		\
}									\
catch(std::exception& e) {						\
  caml_raise_with_string(*caml_named_value("PPL_unknown_standard_exception"), \
                         (const_cast<char*>(e.what())));		\
}									\
catch(timeout_exception&) {                                             \
  reset_timeout();                                                      \
  caml_raise_constant(*caml_named_value("PPL_timeout_exception"));      \
}                                                                       \
catch(...) {								\
  caml_raise_constant(*caml_named_value("PPL_unexpected_error"));	\
}

#include "ppl_ocaml_common.inlines.hh"

#endif // !defined(PPL_ppl_prolog_common_defs_hh)
