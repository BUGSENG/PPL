/* Utilities for termination analysis: non-inline, non-template functions.
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
site: http://www.cs.unipr.it/ppl/ . */

#include <ppl-config.h>

#include "termination.defs.hh"
#include "NNC_Polyhedron.defs.hh"

namespace Parma_Polyhedra_Library {

namespace Implementation {

namespace Termination {

void
assign_all_inequalities_approximation(const Constraint_System& cs_in,
				      Constraint_System& cs_out) {
  if (cs_in.has_strict_inequalities() || cs_in.has_equalities()) {
    // Here we have some strict inequality and/or equality constraints:
    // translate them into non-strict inequality constraints.
    for (Constraint_System::const_iterator i = cs_in.begin(),
           i_end = cs_in.end(); i != i_end; ++i) {
      const Constraint& c = *i;
      if (c.is_equality()) {
        // Insert the two corresponding opposing inequalities.
        cs_out.insert(Linear_Expression(c) >= 0);
        cs_out.insert(Linear_Expression(c) <= 0);
      }
      else if (c.is_strict_inequality())
        // Insert the non-strict approximation.
        cs_out.insert(Linear_Expression(c) >= 0);
      else
        // Insert as is.
        cs_out.insert(c);
    }
  }
  else
    // No strict inequality and no equality constraints.
    cs_out = cs_in;
}

template <>
void
assign_all_inequalities_approximation(const C_Polyhedron& ph,
				      Constraint_System& cs) {
  const Constraint_System& ph_cs = ph.minimized_constraints();
  if (ph_cs.has_equalities()) {
    // Translate equalities into inequalities.
    for (Constraint_System::const_iterator i = ph_cs.begin(),
	   i_end = ph_cs.end(); i != i_end; ++i) {
      const Constraint& c = *i;
      if (c.is_equality()) {
	// Insert the two corresponding opposing inequalities.
	cs.insert(Linear_Expression(c) >= 0);
	cs.insert(Linear_Expression(c) <= 0);
      }
      else
	// Insert as is.
	cs.insert(c);
    }
  }
  else
    // No equality constraints (and no strict inequalities).
    cs = ph_cs;
}

void
shift_unprimed_variables(Constraint_System& cs) {
  const dimension_type cs_space_dim = cs.space_dimension();
  Constraint_System cs_shifted;
  for (Constraint_System::const_iterator i = cs.begin(),
	 cs_end = cs.end(); i != cs_end; ++i) {
    const Constraint& c_i = *i;
    Linear_Expression le_i_shifted;
    for (dimension_type j = cs_space_dim; j-- > 0; ) {
      Coefficient_traits::const_reference a_i_j
	= c_i.coefficient(Variable(j));
      if (a_i_j != 0)
	add_mul_assign(le_i_shifted, a_i_j, Variable(cs_space_dim + j));
    }
    le_i_shifted += c_i.inhomogeneous_term();
    cs_shifted.insert(le_i_shifted >= 0);
  }
  cs.swap(cs_shifted);
}

/*! \brief
  Fill the constraint system(s) for the application of the
  Mesnard and Serebrenik improved termination tests.

  \param cs
  The input constraint system, where variables indices are allocated
  as follows:
  - \f$ x'_1, \ldots, x'_n \f$ go onto space dimensions
    \f$ 0, \ldots, n-1 \f$,
  - \f$ x_1, \ldots, x_n \f$ go onto space dimensions
    \f$ n, \ldots, 2n-1 \f$.
  .
  The system does not contain any equality.

  \param cs_out1
  The first output constraint system.

  \param cs_out2
  The second output constraint system, if any: it may be an alias
  for \p cs_out1.

  The allocation of variable indices in the output constraint
  systems \p cs_out1 and \p cs_out2 is as follows,
  where \f$m\f$ is the number of constraints in \p cs:
  - \f$ \mu_1, \ldots, \mu_n \f$ go onto space dimensions
    \f$ 0, \ldots, n-1 \f$;
  - \f$ \mu_0\f$ goes onto space dimension \f$ n \f$;
  - \f$ y_1, \ldots, y_m \f$ go onto space dimensions
    \f$ n+1, \ldots, n+m \f$;
  .
  if we use the same constraint system, that is
  <CODE>&cs_out1 == &cs_out2</CODE>, then
  - \f$ z_1, ..., z_m, z_{m+1}, z_{m+2} \f$ go onto space dimensions
    \f$ n+m+1, ..., n+2*m+2 \f$;
  .
  otherwise
  - \f$ z_1, ..., z_m, z_{m+1}, z_{m+2} \f$ go onto space dimensions
    \f$ n+1, ..., n+m+2 \f$.
*/
void
fill_constraint_systems_MS(const Constraint_System& cs,
			   Constraint_System& cs_out1,
			   Constraint_System& cs_out2) {
  PPL_ASSERT(cs.space_dimension() % 2 == 0);
  const dimension_type n = cs.space_dimension() / 2;
  const dimension_type m = std::distance(cs.begin(), cs.end());

#if PRINT_DEBUG_INFO
  Variable::output_function_type* p_default_output_function
    = Variable::get_output_function();
  Variable::set_output_function(output_function_MS);

  output_function_MS_n = n;
  output_function_MS_m = m;

  std::cout << "*** cs ***" << std::endl;
  output_function_MS_which = 0;
  using namespace IO_Operators;
  std::cout << cs << std::endl;
#endif

  dimension_type y_begin = n+1;
  dimension_type z_begin = y_begin + ((&cs_out1 == &cs_out2) ? m : 0);

  // Make sure linear expressions are not reallocated multiple times.
  Linear_Expression y_le(0*Variable(y_begin + m - 1));
  Linear_Expression z_le(0*Variable(z_begin + m + 2 - 1));
  std::vector<Linear_Expression> y_les(2*n, y_le);
  std::vector<Linear_Expression> z_les(2*n + 1, z_le);

  dimension_type y = y_begin;
  dimension_type z = z_begin;
  for (Constraint_System::const_iterator i = cs.begin(),
	 cs_end = cs.end(); i != cs_end; ++i) {
    Variable vy(y);
    Variable vz(z);
    ++y;
    ++z;
    cs_out1.insert(vy >= 0);
    cs_out2.insert(vz >= 0);
    const Constraint& c_i = *i;
    Coefficient_traits::const_reference b_i = c_i.inhomogeneous_term();
    if (b_i != 0) {
      // Note that b_i is to the left ot the relation sign, hence here
      // we have -= and not += just to avoid negating b_i.
      sub_mul_assign(y_le, b_i, vy);
      sub_mul_assign(z_le, b_i, vz);
    }
    for (dimension_type j = 2*n; j-- > 0; ) {
      Coefficient_traits::const_reference a_i_j = c_i.coefficient(Variable(j));
      if (a_i_j != 0) {
        add_mul_assign(y_les[j], a_i_j, vy);
        add_mul_assign(z_les[j], a_i_j, vz);
      }
    }
  }
  z_le += Variable(z);
  z_les[2*n] += Variable(z);
  cs_out2.insert(Variable(z) >= 0);
  ++z;
  z_le -= Variable(z);
  z_les[2*n] -= Variable(z);
  cs_out2.insert(Variable(z) >= 0);
  cs_out1.insert(y_le >= 1);
  cs_out2.insert(z_le >= 0);
  dimension_type j = 2*n;
  while (j-- > n) {
    cs_out1.insert(y_les[j] == Variable(j-n));
    cs_out2.insert(z_les[j] == Variable(j-n));
  }
  ++j;
  while (j-- > 0) {
    cs_out1.insert(y_les[j] == -Variable(j));
    cs_out2.insert(z_les[j] == 0);
  }
  cs_out2.insert(z_les[2*n] == Variable(n));

#if PRINT_DEBUG_INFO
  if (&cs_out1 == &cs_out2) {
    std::cout << "*** cs_mip ***" << std::endl;
    output_function_MS_which = 3;
    using namespace IO_Operators;
    std::cout << cs_mip << std::endl;
  }
  else {
    std::cout << "*** cs_out1 ***" << std::endl;
    output_function_MS_which = 1;
    using namespace IO_Operators;
    std::cout << cs_out1 << std::endl;

    std::cout << "*** cs_out2 ***" << std::endl;
    output_function_MS_which = 2;
    using namespace IO_Operators;
    std::cout << cs_out2 << std::endl;
  }

  Variable::set_output_function(p_default_output_function);
#endif
}

/*! \brief
  Fill the constraint system(s) for the application of the
  Podelski and Rybalchenko improved termination tests.

  \param cs_before
  The input constraint system describing the state <EM>before</EM>
  the execution of the loop body, where variables indices are allocated
  as follows:
  - \f$ x_1, \ldots, x_n \f$ go onto space dimensions
    \f$ 0, \ldots, n-1 \f$.
  .
  The system does not contain any equality.

  \param cs_after
  The input constraint system describing the state <EM>after</EM>
  the execution of the loop body, where variables indices are allocated
  as follows:
  - \f$ x'_1, \ldots, x'_n \f$ go onto space dimensions
    \f$ 0, \ldots, n-1 \f$,
  - \f$ x_1, \ldots, x_n \f$ go onto space dimensions
    \f$ n, \ldots, 2n-1 \f$.
  .
  The system does not contain any equality.

  \param cs_out
  The output constraint system, where variables indices are allocated
  as follows:
  - \f$ u_3 \f$ goes onto space dimensions \f$ 0, \ldots, s-1 \f$;
  - \f$ u_2 \f$ goes onto space dimensions \f$ s, \ldots, s+r-1 \f$;
  - \f$ u_1 \f$ goes onto space dimensions \f$ s+r, \ldots, s+2r-1 \f$.

  The improved Podelski-Rybalchenko method described in the paper
  is based on a loop encoding of the form
  \f[
    \begin{pmatrix}
      A_B  & \vect{0}    \\
      A_C  & A'_C
    \end{pmatrix}
    \begin{pmatrix}
     \vect{x} \\ \vect{x}'
    \end{pmatrix}
    \leq
    \begin{pmatrix}
     \vect{b}_B \\ \vect{b}_C
    \end{pmatrix},
  \f]
  where
  \f$ A_B \in \Qset^{r \times n} \f$,
  \f$ A_C \in \Qset^{s \times n} \f$,
  \f$ A'_C \in \Qset^{s \times n} \f$,
  \f$ \vect{b}_B \in \Qset^r \f$,
  \f$ \vect{b}_C \in \Qset^s \f$.
  The corresponding system is:
  \f[
    \begin{aligned}
      (\vect{v}_1-\vect{v}_2)^\transpose A_B
        - \vect{v}_3^\transpose A_C
          &= \vect{0}^\transpose, \\
      \vect{v}_2^\transpose A_B
        + \vect{v}_3^\transpose (A_C+A_C')
          &= \vect{0}^\transpose, \\
      \vect{v}_2 \vect{b}_B + \vect{v}_3 \vect{b}_C
          &< 0,
    \end{aligned}
  \f]
  where \f$ \vect{v}_1 \in \Qset_+^r \f$, \f$ \vect{v}_2 \in \Qset_+^r \f$,
  \f$ \vect{v}_3 \in \Qset_+^s \f$.
  The space of ranking functions is then spanned by
  \f$ \vect{v}_3^\transpose A_C' \vect x \f$.

  In contrast, our encoding is of the form
  \f[
    \begin{pmatrix}
      \vect{0}    & E_B \\
      E'_C & E_C
    \end{pmatrix}
    \begin{pmatrix}
     \vect{x}' \\ \vect{x}
    \end{pmatrix}
    +
    \begin{pmatrix}
     \vect{d}_B \\ \vect{d}_C
    \end{pmatrix}
    \geq
    \vect{0},
  \f]
  where \f$ {E}_B = -{A}_B \f$, \f$ {E}_C = -{A}_C \f$,
  \f$ {E}'_C = -{A}'_C \f$, \f$ \vect{d}_B = \vect{b}_B \f$
  and \f$ \vect{d}_C = \vect{b}_C \f$.
  The corresponding system is:
  \f[
    \begin{aligned}
      (\vect{u}_1-\vect{u}_2)^\transpose E_B
        - \vect{u}_3^\transpose E_C
          &= \vect{0}^\transpose, \\
      \vect{u}_2^\transpose E_B
        + \vect{u}_3^\transpose (E_C+E_C')
          &= \vect{0}^\transpose, \\
      \vect{u}_2 \vect{d}_B + \vect{u}_3 \vect{d}_C
          &> 0,
    \end{aligned}
  \f]
  where \f$ \vect{u}_1 \in \Qset_-^r \f$, \f$ \vect{u}_2 \in \Qset_-^r \f$,
  \f$ \vect{u}_3 \in \Qset_-^s \f$.
  The space of ranking functions is then spanned by
  \f$ \vect{u}_3^\transpose E_C' \vect x \f$.

  \param le_out
  The expression to be minimized in the context of \p cs_out:
  a value of \f$ -1 \f$ or less entails termination.
*/
void
fill_constraint_system_PR(const Constraint_System& cs_before,
			  const Constraint_System& cs_after,
			  Constraint_System& cs_out,
			  Linear_Expression& le_out) {
  PPL_ASSERT(cs_after.space_dimension() % 2 == 0);
  PPL_ASSERT(2*cs_before.space_dimension() == cs_after.space_dimension());
  const dimension_type n = cs_before.space_dimension();
  const dimension_type r = distance(cs_before.begin(), cs_before.end());
  const dimension_type s = distance(cs_after.begin(), cs_after.end());
  const dimension_type m = r + s;

  // Make sure linear expressions are not reallocated multiple times.
  if (m > 0)
    le_out = 0 * Variable(m + r - 1);
  std::vector<Linear_Expression> les_eq(2*n, le_out);

  dimension_type row_index = 0;
  for (Constraint_System::const_iterator i = cs_before.begin(),
	 cs_before_end = cs_before.end();
       i != cs_before_end;
       ++i, ++row_index) {
    Variable u1_i(m + row_index);
    Variable u2_i(s + row_index);
    const Constraint& c_i = *i;
    for (dimension_type j = n; j-- > 0; ) {
      Coefficient_traits::const_reference A_ij_B = c_i.coefficient(Variable(j));
      if (A_ij_B != 0) {
        // (u1 - u2) A_B, in the context of j-th constraint.
        add_mul_assign(les_eq[j], A_ij_B, u1_i);
        sub_mul_assign(les_eq[j], A_ij_B, u2_i);
        // u2 A_B, in the context of (j+n)-th constraint.
        add_mul_assign(les_eq[j + n], A_ij_B, u2_i);
      }
    }
    Coefficient_traits::const_reference b_B = c_i.inhomogeneous_term();
    if (b_B != 0)
      // u2 b_B, in the context of the strict inequality constraint.
      add_mul_assign(le_out, b_B, u2_i);
  }

  row_index = 0;
  for (Constraint_System::const_iterator i = cs_after.begin(),
	 cs_after_end = cs_after.end();
       i != cs_after_end;
       ++i, ++row_index) {
    Variable u3_i(row_index);
    const Constraint& c_i = *i;
    for (dimension_type j = n; j-- > 0; ) {
      Coefficient_traits::const_reference
        A_ij_C = c_i.coefficient(Variable(j + n));
      if (A_ij_C != 0) {
        // - u3 A_C, in the context of the j-th constraint.
        sub_mul_assign(les_eq[j], A_ij_C, u3_i);
        // u3 A_C, in the context of the (j+n)-th constraint.
        add_mul_assign(les_eq[j+n], A_ij_C, u3_i);
      }
      Coefficient_traits::const_reference
        Ap_ij_C = c_i.coefficient(Variable(j));
      if (Ap_ij_C != 0)
        // u3 Ap_C, in the context of the (j+n)-th constraint.
        add_mul_assign(les_eq[j+n], Ap_ij_C, u3_i);
    }
    Coefficient_traits::const_reference b_C = c_i.inhomogeneous_term();
    if (b_C != 0)
      // u3 b_C, in the context of the strict inequality constraint.
      add_mul_assign(le_out, b_C, u3_i);
  }

  // Add the nonnegativity constraints for u_1, u_2 and u_3.
  for (dimension_type i = s + 2*r; i-- > 0; )
    cs_out.insert(Variable(i) >= 0);

  // FIXME: iterate backwards once the debugging phase is over.
  //for (dimension_type j = 2*n; j-- > 0; )
  for (dimension_type j = 0; j < 2*n; ++j)
    cs_out.insert(les_eq[j] == 0);
}

void
fill_constraint_system_PR_original(const Constraint_System& cs,
                                   Constraint_System& cs_out,
                                   Linear_Expression& le_out) {
  PPL_ASSERT(cs.space_dimension() % 2 == 0);
  const dimension_type n = cs.space_dimension() / 2;
  const dimension_type m = distance(cs.begin(), cs.end());

  // Make sure linear expressions are not reallocated multiple times.
  if (m > 0)
    le_out = 0 * Variable(2*m - 1);
  std::vector<Linear_Expression> les_eq(3*n, le_out);

  dimension_type row_index = 0;
  for (Constraint_System::const_iterator i = cs.begin(),
	 cs_end = cs.end(); i != cs_end; ++i, ++row_index) {
    const Constraint& c_i = *i;
    const Variable lambda1_i(row_index);
    const Variable lambda2_i(m + row_index);
    for (dimension_type j = n; j-- > 0; ) {
      Coefficient_traits::const_reference Ap_ij = c_i.coefficient(Variable(j));
      if (Ap_ij != 0) {
        // lambda_1 A'
        add_mul_assign(les_eq[j], Ap_ij, lambda1_i);
        // lambda_2 A'
        add_mul_assign(les_eq[j+n+n], Ap_ij, lambda2_i);
      }
      Coefficient_traits::const_reference A_ij = c_i.coefficient(Variable(j+n));
      if (A_ij != 0) {
        // (lambda_1 - lambda_2) A
        add_mul_assign(les_eq[j+n], A_ij, lambda1_i);
        sub_mul_assign(les_eq[j+n], A_ij, lambda2_i);
        // lambda_2 A
        add_mul_assign(les_eq[j+n+n], A_ij, lambda2_i);
      }
    }
    Coefficient_traits::const_reference b = c_i.inhomogeneous_term();
    if (b != 0)
      // lambda2 b
      add_mul_assign(le_out, b, lambda2_i);
  }

  // Add the non-negativity constraints for lambda_1 and lambda_2.
  for (dimension_type i = 2*m; i-- > 0; )
    cs_out.insert(Variable(i) >= 0);

  // FIXME: iterate backwards once the debugging phase is over.
  //for (dimension_type j = 3*n; j-- > 0; )
  for (dimension_type j = 0; j < 3*n; ++j)
    cs_out.insert(les_eq[j] == 0);
}

bool
termination_test_MS(const Constraint_System& cs) {
  Constraint_System cs_mip;
  fill_constraint_systems_MS(cs, cs_mip, cs_mip);

  MIP_Problem mip = MIP_Problem(cs_mip.space_dimension(), cs_mip);
  return mip.is_satisfiable();
}

bool
one_affine_ranking_function_MS(const Constraint_System& cs, Generator& mu) {
  Constraint_System cs_mip;
  fill_constraint_systems_MS(cs, cs_mip, cs_mip);

  MIP_Problem mip = MIP_Problem(cs_mip.space_dimension(), cs_mip);
  if (!mip.is_satisfiable())
    return false;

  Generator fp = mip.feasible_point();
  PPL_ASSERT(fp.is_point());
  Linear_Expression le;
  const dimension_type n = cs.space_dimension() / 2;
  for (dimension_type i = n+1; i-- > 0; ) {
    Variable vi(i);
    add_mul_assign(le, fp.coefficient(vi), vi);
  }
  mu = point(le, fp.divisor());
  return true;
}

void
all_affine_ranking_functions_MS(const Constraint_System& cs,
				C_Polyhedron& mu_space) {
  Constraint_System cs_out1;
  Constraint_System cs_out2;
  fill_constraint_systems_MS(cs, cs_out1, cs_out2);

  C_Polyhedron ph1(cs_out1);
  C_Polyhedron ph2(cs_out2);
  const dimension_type n = cs.space_dimension() / 2;
  ph1.remove_higher_space_dimensions(n);
  ph1.add_space_dimensions_and_embed(1);
  ph2.remove_higher_space_dimensions(n+1);

#if PRINT_DEBUG_INFO
  Variable::output_function_type* p_default_output_function
    = Variable::get_output_function();
  Variable::set_output_function(output_function_MS);

  output_function_MS_n = n;
  output_function_MS_m = std::distance(cs.begin(), cs.end());

  std::cout << "*** ph1 projected ***" << std::endl;
  output_function_MS_which = 4;
  using namespace IO_Operators;
  std::cout << ph1.minimized_constraints() << std::endl;

  std::cout << "*** ph2 projected ***" << std::endl;
  std::cout << ph2.minimized_constraints() << std::endl;
#endif

  ph1.intersection_assign(ph2);

#if PRINT_DEBUG_INFO
  std::cout << "*** intersection ***" << std::endl;
  using namespace IO_Operators;
  std::cout << ph1.minimized_constraints() << std::endl;

  Variable::set_output_function(p_default_output_function);
#endif

  mu_space.swap(ph1);
}

void
all_affine_quasi_ranking_functions_MS(const Constraint_System& cs,
                                      C_Polyhedron& decreasing_mu_space,
                                      C_Polyhedron& bounded_mu_space) {
  Constraint_System cs_out1;
  Constraint_System cs_out2;
  fill_constraint_systems_MS(cs, cs_out1, cs_out2);

  C_Polyhedron ph1(cs_out1);
  C_Polyhedron ph2(cs_out2);
  const dimension_type n = cs.space_dimension() / 2;
  ph1.remove_higher_space_dimensions(n);
  ph1.add_space_dimensions_and_embed(1);
  ph2.remove_higher_space_dimensions(n+1);

#if PRINT_DEBUG_INFO
  Variable::output_function_type* p_default_output_function
    = Variable::get_output_function();
  Variable::set_output_function(output_function_MS);

  output_function_MS_n = n;
  output_function_MS_m = std::distance(cs.begin(), cs.end());

  std::cout << "*** ph1 projected ***" << std::endl;
  output_function_MS_which = 4;
  using namespace IO_Operators;
  std::cout << ph1.minimized_constraints() << std::endl;

  std::cout << "*** ph2 projected ***" << std::endl;
  std::cout << ph2.minimized_constraints() << std::endl;

  Variable::set_output_function(p_default_output_function);
#endif

  decreasing_mu_space.swap(ph1);
  bounded_mu_space.swap(ph2);
}

bool
termination_test_PR_original(const Constraint_System& cs) {
  PPL_ASSERT(cs.space_dimension() % 2 == 0);

  Constraint_System cs_mip;
  Linear_Expression le_ineq;
  fill_constraint_system_PR_original(cs, cs_mip, le_ineq);

  // Turn minimization problem into satisfiability.
  cs_mip.insert(le_ineq <= -1);

  MIP_Problem mip(cs_mip.space_dimension(), cs_mip);
  return mip.is_satisfiable();
}

bool
termination_test_PR(const Constraint_System& cs_before,
		    const Constraint_System& cs_after) {
  Constraint_System cs_mip;
  Linear_Expression le_ineq;
  fill_constraint_system_PR(cs_before, cs_after, cs_mip, le_ineq);

#if PRINT_DEBUG_INFO
  Variable::output_function_type* p_default_output_function
    = Variable::get_output_function();
  Variable::set_output_function(output_function_PR);

  output_function_PR_r = distance(cs_before.begin(), cs_before.end());
  output_function_PR_s = distance(cs_after.begin(), cs_after.end());

  std::cout << "*** cs_mip ***" << std::endl;
  using namespace IO_Operators;
  std::cout << cs_mip << std::endl;
  std::cout << "*** le_ineq ***" << std::endl;
  std::cout << le_ineq << std::endl;

  Variable::set_output_function(p_default_output_function);
#endif

  // Turn minimization problem into satisfiability.
  cs_mip.insert(le_ineq <= -1);

  MIP_Problem mip(cs_mip.space_dimension(), cs_mip);
  return mip.is_satisfiable();
}

bool
one_affine_ranking_function_PR(const Constraint_System& cs_before,
			       const Constraint_System& cs_after,
			       Generator& mu) {
  Constraint_System cs_mip;
  Linear_Expression le_ineq;
  fill_constraint_system_PR(cs_before, cs_after, cs_mip, le_ineq);

#if PRINT_DEBUG_INFO
  Variable::output_function_type* p_default_output_function
    = Variable::get_output_function();
  Variable::set_output_function(output_function_PR);

  output_function_PR_r = distance(cs_before.begin(), cs_before.end());
  output_function_PR_s = distance(cs_after.begin(), cs_after.end());

  std::cout << "*** cs_mip ***" << std::endl;
  using namespace IO_Operators;
  std::cout << cs_mip << std::endl;
  std::cout << "*** le_ineq ***" << std::endl;
  std::cout << le_ineq << std::endl;

  Variable::set_output_function(p_default_output_function);
#endif

  // Turn minimization problem into satisfiability.
  cs_mip.insert(le_ineq <= -1);

  MIP_Problem mip(cs_mip.space_dimension(), cs_mip);
  if (!mip.is_satisfiable())
    return false;

  Generator fp = mip.feasible_point();
  PPL_ASSERT(fp.is_point());

  // u_3 corresponds to space dimensions 0, ..., s - 1.
  const dimension_type n = cs_before.space_dimension();
  Linear_Expression le;
  // mu_0 is zero: do this first to avoid reallocations.
  le += 0*Variable(n);
  // Multiply u_3 by E'_C to obtain mu_1, ..., mu_n.
  dimension_type row_index = 0;
  PPL_DIRTY_TEMP_COEFFICIENT(k);
  for (Constraint_System::const_iterator i = cs_after.begin(),
         cs_after_end = cs_after.end();
       i != cs_after_end;
       ++i, ++row_index) {
    Variable vi(row_index);
    Coefficient_traits::const_reference fp_i = fp.coefficient(vi);
    const Constraint& c_i = *i;
    for (dimension_type j = n; j-- > 0; ) {
      Variable vj(j);
      k = fp_i * c_i.coefficient(vj);
      sub_mul_assign(le, k, vj);
    }
  }
  // Note that we can neglect the divisor of `fp' since it is positive.
  mu = point(le);
  return true;
}

bool
one_affine_ranking_function_PR_original(const Constraint_System& cs,
                                        Generator& mu) {
  PPL_ASSERT(cs.space_dimension() % 2 == 0);
  const dimension_type n = cs.space_dimension() / 2;
  const dimension_type m = std::distance(cs.begin(), cs.end());

  Constraint_System cs_mip;
  Linear_Expression le_ineq;
  fill_constraint_system_PR_original(cs, cs_mip, le_ineq);

#if PRINT_DEBUG_INFO
  std::cout << "*** cs_mip ***" << std::endl;
  using namespace IO_Operators;
  std::cout << cs_mip << std::endl;
  std::cout << "*** le_ineq ***" << std::endl;
  std::cout << le_ineq << std::endl;
#endif

  // Turn minimization problem into satisfiability.
  cs_mip.insert(le_ineq <= -1);

  MIP_Problem mip = MIP_Problem(cs_mip.space_dimension(), cs_mip);
  if (!mip.is_satisfiable())
    return false;

  Generator fp = mip.feasible_point();
  PPL_ASSERT(fp.is_point());
  Linear_Expression le;
  // mu_0 is zero: do this first to avoid reallocations.
  le += 0*Variable(n);
  // Multiply -lambda_2 by A' to obtain mu_1, ..., mu_n.
  // lambda_2 corresponds to space dimensions m, ..., 2*m - 1.
  dimension_type row_index = m;
  PPL_DIRTY_TEMP_COEFFICIENT(k);
  for (Constraint_System::const_iterator i = cs.begin(),
         cs_end = cs.end(); i != cs_end; ++i, ++row_index) {
    Variable lambda_2(row_index);
    Coefficient_traits::const_reference fp_i = fp.coefficient(lambda_2);
    if (fp_i != 0) {
      const Constraint& c_i = *i;
      for (dimension_type j = n; j-- > 0; ) {
        Variable vj(j);
        Coefficient_traits::const_reference Ap_ij = c_i.coefficient(vj);
        k = fp_i * Ap_ij;
        sub_mul_assign(le, k, vj);
      }
    }
  }
  // Note that we can neglect the divisor of `fp' since it is positive.
  mu = point(le);
  return true;
}

void
all_affine_ranking_functions_PR(const Constraint_System& cs_before,
				const Constraint_System& cs_after,
				NNC_Polyhedron& mu_space) {
  Constraint_System cs_eqs;
  Linear_Expression le_ineq;
  fill_constraint_system_PR(cs_before, cs_after, cs_eqs, le_ineq);

#if PRINT_DEBUG_INFO
  Variable::output_function_type* p_default_output_function
    = Variable::get_output_function();
  Variable::set_output_function(output_function_PR);

  output_function_PR_r = distance(cs_before.begin(), cs_before.end());
  output_function_PR_s = distance(cs_after.begin(), cs_after.end());

  std::cout << "*** cs_eqs ***" << std::endl;
  using namespace IO_Operators;
  std::cout << cs_eqs << std::endl;
  std::cout << "*** le_ineq ***" << std::endl;
  std::cout << le_ineq << std::endl;
#endif

  NNC_Polyhedron ph(cs_eqs);
  ph.add_constraint(le_ineq < 0);
  // u_3 corresponds to space dimensions 0, ..., s - 1.
  const dimension_type s = distance(cs_after.begin(), cs_after.end());
  ph.remove_higher_space_dimensions(s);

#if PRINT_DEBUG_INFO
  std::cout << "*** ph ***" << std::endl;
  std::cout << ph << std::endl;

  Variable::set_output_function(p_default_output_function);
#endif

  const dimension_type n = cs_before.space_dimension();

  const Generator_System& gs_in = ph.generators();
  Generator_System gs_out;
  Generator_System::const_iterator gs_in_it = gs_in.begin();
  Generator_System::const_iterator gs_in_end = gs_in.end();
  if (gs_in_it == gs_in_end)
    // The system is unsatisfiable.
    mu_space = NNC_Polyhedron(n + 1, EMPTY);
  else {
    for ( ; gs_in_it != gs_in_end; ++gs_in_it) {
      const Generator& g = *gs_in_it;
      Linear_Expression le;
      // Set le to the multiplication of Linear_Expression(g) by E'_C.
      dimension_type row_index = 0;
      PPL_DIRTY_TEMP_COEFFICIENT(k);
      for (Constraint_System::const_iterator i = cs_after.begin(),
	     cs_after_end = cs_after.end();
	   i != cs_after_end;
	   ++i, ++row_index) {
	Variable vi(row_index);
	Coefficient_traits::const_reference g_i = g.coefficient(vi);
	if (g_i != 0) {
	  const Constraint& c_i = *i;
	  for (dimension_type j = n; j-- > 0; ) {
	    Variable vj(j);
	    k = g_i * c_i.coefficient(vj);
	    sub_mul_assign(le, k, vj);
	  }
	}
      }

      // Add to gs_out the transformed generator.
      switch (g.type()) {
      case Generator::LINE:
	if (!le.all_homogeneous_terms_are_zero())
	  gs_out.insert(line(le));
	break;
      case Generator::RAY:
	if (!le.all_homogeneous_terms_are_zero())
	  gs_out.insert(ray(le));
	break;
      case Generator::POINT:
	gs_out.insert(point(le, g.divisor()));
	break;
      case Generator::CLOSURE_POINT:
	gs_out.insert(closure_point(le, g.divisor()));
	break;
      }
    }

    mu_space = NNC_Polyhedron(gs_out);
    // mu_0 is zero.
    mu_space.add_space_dimensions_and_embed(1);
  }
}

void
all_affine_ranking_functions_PR_original(const Constraint_System& cs,
                                         NNC_Polyhedron& mu_space) {
  PPL_ASSERT(cs.space_dimension() % 2 == 0);
  const dimension_type n = cs.space_dimension() / 2;
  const dimension_type m = distance(cs.begin(), cs.end());

  if (m == 0) {
    // If there are no constraints at all, we have non-termination,
    // i.e., no affine ranking function at all.
    mu_space = NNC_Polyhedron(n + 1, EMPTY);
    return;
  }

  Constraint_System cs_eqs;
  Linear_Expression le_ineq;
  fill_constraint_system_PR_original(cs, cs_eqs, le_ineq);

  NNC_Polyhedron ph(cs_eqs);
  ph.add_constraint(le_ineq < 0);
  // lambda_2 corresponds to space dimensions m, ..., 2*m-1.
  Variables_Set lambda1(Variable(0), Variable(m-1));
  ph.remove_space_dimensions(lambda1);
  //ph.remove_higher_space_dimensions(m);

#if PRINT_DEBUG_INFO
  std::cout << "*** ph ***" << std::endl;
  std::cout << ph << std::endl;

  Variable::set_output_function(p_default_output_function);
#endif

  const Generator_System& gs_in = ph.generators();
  Generator_System gs_out;
  Generator_System::const_iterator gs_in_it = gs_in.begin();
  Generator_System::const_iterator gs_in_end = gs_in.end();
  if (gs_in_it == gs_in_end)
    // The system is unsatisfiable.
    mu_space = NNC_Polyhedron(n + 1, EMPTY);
  else {
    for ( ; gs_in_it != gs_in_end; ++gs_in_it) {
      const Generator& g = *gs_in_it;
      Linear_Expression le;
      // Set le to the multiplication of Linear_Expression(g) by E'_C.
      dimension_type row_index = 0;
      PPL_DIRTY_TEMP_COEFFICIENT(k);
      for (Constraint_System::const_iterator i = cs.begin(),
	     cs_end = cs.end(); i != cs_end; ++i, ++row_index) {
	Variable lambda2_i(row_index);
	Coefficient_traits::const_reference g_i = g.coefficient(lambda2_i);
	if (g_i != 0) {
	  const Constraint& c_i = *i;
	  for (dimension_type j = n; j-- > 0; ) {
	    Variable vj(j);
	    Coefficient_traits::const_reference Ap_ij = c_i.coefficient(vj);
	    k = g_i * Ap_ij;
	    sub_mul_assign(le, k, vj);
	  }
	}
      }

      // Add to gs_out the transformed generator.
      switch (g.type()) {
      case Generator::LINE:
	if (!le.all_homogeneous_terms_are_zero())
	  gs_out.insert(line(le));
	break;
      case Generator::RAY:
	if (!le.all_homogeneous_terms_are_zero())
	  gs_out.insert(ray(le));
	break;
      case Generator::POINT:
	gs_out.insert(point(le, g.divisor()));
	break;
      case Generator::CLOSURE_POINT:
	gs_out.insert(closure_point(le, g.divisor()));
	break;
      }
    }

    mu_space = NNC_Polyhedron(gs_out);
    // mu_0 is zero.
    mu_space.add_space_dimensions_and_embed(1);
  }
}

} // namespace Termination

} // namespace Implementation

} // namespace Parma_Polyhedra_Library
