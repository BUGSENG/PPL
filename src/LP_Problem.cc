/* LP_Problem class implementation: non-inline functions.
   Copyright (C) 2001-2006 Roberto Bagnara <bagnara@cs.unipr.it>

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
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1307, USA.

For the most up-to-date information see the Parma Polyhedra Library
site: http://www.cs.unipr.it/ppl/ . */

#include <config.h>
#include "LP_Problem.defs.hh"
#include "globals.types.hh"
#include "globals.defs.hh"
#include "Row.defs.hh"
#include "Matrix.defs.hh"
#include "Linear_Row.defs.hh"
#include "Linear_System.defs.hh"
#include "Linear_Expression.defs.hh"
#include "Constraint_System.defs.hh"
#include "Constraint_System.inlines.hh"
#include "Generator.defs.hh"
#include <stdexcept>
#include <sstream>
#include <map>
#include <deque>
#include <set>
#include <algorithm>

#ifndef PPL_NOISY_SIMPLEX
#define PPL_NOISY_SIMPLEX 0
#endif

#ifdef PPL_NOISY_SIMPLEX
#include <iostream>
#endif

#ifndef PPL_SIMPLEX_ENABLE_STEEPEST_EDGE
#define PPL_SIMPLEX_ENABLE_STEEPEST_EDGE 1
#endif

namespace PPL = Parma_Polyhedra_Library;

#if PPL_NOISY_SIMPLEX
namespace {

unsigned long num_iterations = 0;

} // namespace
#endif // PPL_NOISY_SIMPLEX

PPL::dimension_type
PPL::LP_Problem::steepest_edge() const {
  const dimension_type tableau_num_rows = tableau.num_rows();
  assert(tableau_num_rows == base.size());
  // The square of the lcm of all the coefficients of variables in base.
  TEMP_INTEGER(squared_lcm_basis);
  // The normalization factor for each coefficient in the tableau.
  std::vector<Coefficient> norm_factor(tableau_num_rows);
  {
    // Compute the lcm of all the coefficients of variables in base.
    TEMP_INTEGER(lcm_basis);
    lcm_basis = 1;
    for (dimension_type i = tableau_num_rows; i-- > 0; )
      lcm_assign(lcm_basis, lcm_basis, tableau[i][base[i]]);
    // Compute normalization factors.
    for (dimension_type i = tableau_num_rows; i-- > 0; )
      exact_div_assign(norm_factor[i], lcm_basis, tableau[i][base[i]]);
    // Compute the square of `lcm_basis', exploiting the fact that
    // `lcm_basis' will no longer be needed.
    lcm_basis *= lcm_basis;
    std::swap(squared_lcm_basis, lcm_basis);
  }

  // Defined here to avoid repeated (de-)allocations.
  TEMP_INTEGER(challenger_num);
  TEMP_INTEGER(scalar_value);
  TEMP_INTEGER(challenger_den);
  TEMP_INTEGER(challenger_value);
  TEMP_INTEGER(current_value);

  TEMP_INTEGER(current_num);
  TEMP_INTEGER(current_den);
  dimension_type entering_index = 0;
  const int cost_sign = sgn(working_cost[working_cost.size() - 1]);
  for (dimension_type j = tableau.num_columns() - 1; j-- > 1; ) {
    const Coefficient& cost_j = working_cost[j];
    if (sgn(cost_j) == cost_sign) {
      // We can't compute the (exact) square root of abs(\Delta x_j).
      // The workaround is to compute the square of `cost[j]'.
      challenger_num = cost_j * cost_j;
      // Due to our integer implementation, the `1' term in the denominator
      // of the original formula has to be replaced by `squared_lcm_basis'.
      challenger_den = squared_lcm_basis;
      for (dimension_type i = tableau_num_rows; i-- > 0; ) {
	const Coefficient& tableau_ij = tableau[i][j];
	// Note: this test speeds up the GMP computation.
	if (tableau_ij != 0) {
	  scalar_value = tableau_ij * norm_factor[i];
	  add_mul_assign(challenger_den, scalar_value, scalar_value);
	}
      }
      // Initialization during the first loop.
      if (entering_index == 0) {
	std::swap(current_num, challenger_num);
	std::swap(current_den, challenger_den);
	entering_index = j;
 	continue;
      }
      challenger_value = challenger_num * current_den;
      current_value = current_num * challenger_den;
      // Update the values, if the challeger wins.
      if (challenger_value > current_value) {
	std::swap(current_num, challenger_num);
	std::swap(current_den, challenger_den);
	entering_index = j;
      }
    }
  }
  return entering_index;
}

// See pag. 47 of Papadimitriou.

PPL::dimension_type
PPL::LP_Problem::get_entering_var_index() const {
  // The variable entering the base is the first one whose coefficient
  // in the cost function has the same sign the cost function itself.
  // If no such variable exists, then we met the optimality condition
  // (and return 0 to the caller).

  // Get the "sign" of the cost function.
  const dimension_type cost_sign_index = working_cost.size() - 1;
  const int cost_sign = sgn(working_cost[cost_sign_index]);
  assert(cost_sign != 0);
  for (dimension_type i = 1; i < cost_sign_index; ++i)
    if (sgn(working_cost[i]) == cost_sign)
      return i;
  // No variable has to enter the base:
  // the cost function was optimized.
  return 0;
}

void
PPL::LP_Problem::linear_combine(Row& x,
				const Row& y,
				const dimension_type k) {
  assert(x.size() == y.size());
  assert(y[k] != 0 && x[k] != 0);
  // Let g be the GCD between `x[k]' and `y[k]'.
  // For each i the following computes
  //   x[i] = x[i]*y[k]/g - y[i]*x[k]/g.
  TEMP_INTEGER(normalized_x_k);
  TEMP_INTEGER(normalized_y_k);
  normalize2(x[k], y[k], normalized_x_k, normalized_y_k);
  for (dimension_type i = x.size(); i-- > 0; )
    if (i != k) {
      Coefficient& x_i = x[i];
      x_i *= normalized_y_k;
      // Note: the test speeds up the GMP computation.
      const Coefficient& y_i = y[i];
      if (y_i != 0)
	sub_mul_assign(x_i, y_i, normalized_x_k);
    }
  x[k] = 0;
  x.normalize();
}

// See pag 42-43 of Papadimitriou.

void
PPL::LP_Problem::swap_base(const dimension_type entering_var_index,
			   const dimension_type exiting_base_index) {
  const Row& tableau_out = tableau[exiting_base_index];
  // Linearly combine the constraints.
  for (dimension_type i = tableau.num_rows(); i-- > 0; ) {
    Row& tableau_i = tableau[i];
    if (i != exiting_base_index && tableau_i[entering_var_index] != 0)
      linear_combine(tableau_i, tableau_out, entering_var_index);
  }
  // Linearly combine the cost function.
  if (working_cost[entering_var_index] != 0)
    linear_combine(working_cost, tableau_out, entering_var_index);
  // Adjust the base.
  base[exiting_base_index] = entering_var_index;
}

// See pag. 47 + 50 of Papadimitriou.

PPL::dimension_type
PPL::LP_Problem
::get_exiting_base_index(const dimension_type entering_var_index) const  {
  // The variable exiting the base should be associated to a tableau
  // constraint such that the ratio
  // tableau[i][entering_var_index] / tableau[i][base[i]]
  // is strictly positive and minimal.

  // Find the first tableau constraint `c' having a positive value for
  //   tableau[i][entering_var_index] / tableau[i][base[i]]
  const dimension_type tableau_num_rows = tableau.num_rows();
  dimension_type exiting_base_index = tableau_num_rows;
  for (dimension_type i = 0; i < tableau_num_rows; ++i) {
    const Row& t_i = tableau[i];
    const int num_sign = sgn(t_i[entering_var_index]);
    if (num_sign != 0 && num_sign == sgn(t_i[base[i]])) {
      exiting_base_index = i;
      break;
    }
  }
  // Check for unboundedness.
  if (exiting_base_index == tableau_num_rows)
    return tableau_num_rows;

  // Reaching this point means that a variable will definitely exit the base.
  TEMP_INTEGER(lcm);
  TEMP_INTEGER(current_min);
  TEMP_INTEGER(challenger);
  for (dimension_type i = exiting_base_index + 1; i < tableau_num_rows; ++i) {
    const Row& t_i = tableau[i];
    const Coefficient& t_ie = t_i[entering_var_index];
    const Coefficient& t_ib = t_i[base[i]];
    const int t_ie_sign = sgn(t_ie);
    if (t_ie_sign != 0 && t_ie_sign == sgn(t_ib)) {
      const Row& t_e = tableau[exiting_base_index];
      const Coefficient& t_ee = t_e[entering_var_index];
      lcm_assign(lcm, t_ee, t_ie);
      exact_div_assign(current_min, lcm, t_ee);
      current_min *= t_e[0];
      current_min = abs(current_min);
      exact_div_assign(challenger, lcm, t_ie);
      challenger *= t_i[0];
      challenger = abs(challenger);
      current_min -= challenger;
      const int sign = sgn(current_min);
      if (sign > 0
	  || (sign == 0 && base[i] < base[exiting_base_index]))
	exiting_base_index = i;
    }
  }
  return exiting_base_index;
}

// See pag 49 of Papadimitriou.

bool
PPL::LP_Problem::compute_simplex() {
  assert(tableau.num_columns() == working_cost.size());
  const dimension_type tableau_num_rows = tableau.num_rows();
  while (true) {
    // Choose the index of the variable entering the base, if any.
    const dimension_type entering_var_index
#if PPL_SIMPLEX_ENABLE_STEEPEST_EDGE
      = steepest_edge();
#else
      = get_entering_var_index();
#endif
    // If no entering index was computed, the problem is solved.
    if (entering_var_index == 0)
      return true;

    // Choose the index of the row exiting the base.
    const dimension_type exiting_base_index
      = get_exiting_base_index(entering_var_index);
    // If no exiting index was computed, the problem is unbounded.
    if (exiting_base_index == tableau_num_rows)
      return false;

    // We have not reached the optimality or unbounded condition:
    // compute the new base and the corresponding vertex of the
    // feasible region.
    swap_base(entering_var_index, exiting_base_index);
#if PPL_NOISY_SIMPLEX
    ++num_iterations;
    if (num_iterations % 200 == 0)
      std::cout << "Primal Simplex: iteration "
		<< num_iterations << "." << std::endl;
#endif
  }
}

// See pag 28  Papadimitriou.

void
PPL::LP_Problem::prepare_first_phase() {
  // We negate the row if tableau[i][0] <= 0 to get the inhomogeneous term > 0.
  // This simplifies the insertion of the slack variables: the value of the
  // slack variable of every constraint will be 1.
  const dimension_type tableau_old_n_cols = tableau.num_columns();
  for (dimension_type i = tableau.num_rows(); i-- > 0 ; ) {
    Row& tableau_i = tableau[i];
    if (tableau_i[0] > 0)
      for (dimension_type j = tableau_old_n_cols; j-- > 0; )
	neg_assign(tableau_i[j]);
  }

  // Add the columns for all the slack variables, plus an additional
  // column for the sign of the cost function, provided we are not going
  // to exceed the maximum number of allowed columns.
  if (tableau.max_num_columns() - tableau_old_n_cols <= tableau.num_rows())
    throw std::length_error("PPL::LP_Problem:\nthe maximum size of an "
			    "internal data structure has been exceeded "
			    "while solving the LP_Problem.");
  tableau.add_zero_columns(tableau.num_rows() + 1);
  // Set the working cost function with the right size.
  working_cost = Row(tableau.num_columns(), Row::Flags());

  // Modify the tableau and the new cost function by adding
  // the slack variables (which enter the base).
  // As for the cost function, all the slack variables should have
  // coefficient -1.
  for (dimension_type i = 0; i < tableau.num_rows(); ++i) {
    const dimension_type j = tableau_old_n_cols + i;
    tableau[i][j] = 1;
    working_cost[j] = -1;
    base[i] = j;
  }

  // Set the extra-coefficient of the cost functions to record its sign.
  // This is done to keep track of the possible sign's inversion.
  const dimension_type last_obj_index = working_cost.size() - 1;
  working_cost[last_obj_index] = 1;

  // Express the problem in terms of the variables in base.
  for (dimension_type i = tableau.num_rows(); i-- > 0; )
    linear_combine(working_cost, tableau[i], base[i]);
}

// See pag 55-56 Papadimitriou.

void
PPL::LP_Problem::erase_slacks() {
  const dimension_type tableau_last_index = tableau.num_columns() - 1;
  dimension_type tableau_n_rows = tableau.num_rows();
  const dimension_type first_slack_index = tableau_last_index - tableau_n_rows;

  // Step 1: try to remove from the base all the remaining slack variables.
  for (dimension_type i = 0; i < tableau_n_rows; ++i)
    if (base[i] >= first_slack_index) {
      // Search for a non-zero element to enter the base.
      Row& tableau_i = tableau[i];
      bool redundant = true;
      for (dimension_type j = first_slack_index; j-- > 1; )
	if (tableau_i[j] != 0) {
	  swap_base(j, i);
	  redundant = false;
	  break;
	}
      if (redundant) {
	// No original variable entered the base:
	// the constraint is redundant and should be deleted.
	--tableau_n_rows;
	if (i < tableau_n_rows) {
	  // Replace the redundant row with the last one,
	  // taking care of adjusting the iteration index.
	  tableau_i.swap(tableau[tableau_n_rows]);
	  base[i] = base[tableau_n_rows];
	  --i;
	}
	tableau.erase_to_end(tableau_n_rows);
	base.pop_back();
      }
    }

  // Step 2: Adjust data structures so as to enter phase 2 of the simplex.

  // Compute the dimensions of the new tableau.
  const dimension_type new_tableau_n_cols = first_slack_index + 1;
  const dimension_type new_tableau_last_index = first_slack_index;

  // Adjust the number of columns of `tableau'.
  tableau.remove_trailing_columns(tableau.num_columns() - new_tableau_n_cols);
  // Zero the last column of the tableau.
  for (dimension_type i = tableau_n_rows; i-- > 0; )
    tableau[i][new_tableau_last_index] = 0;

  // ... then properly set the element in the (new) last column,
  // encoding the kind of optimization; ...
  working_cost[new_tableau_last_index] = working_cost[tableau_last_index];
  // ... and finally remove redundant columns.
  const dimension_type working_cost_new_size = working_cost.size() -
    (tableau_last_index - new_tableau_last_index);
  working_cost.shrink(working_cost_new_size);
}

// See pag 55 of Papadimitriou.

PPL::LP_Problem_Status
PPL::LP_Problem::compute_tableau() {
  assert(tableau.num_rows() == 0);
  assert(dim_map.size() == 0);
  // Note: exploiting friendship so as to efficiently access the
  // coefficients of each constraint.
  Linear_System& cs = input_cs;
  const dimension_type cs_num_rows = cs.num_rows();
  const dimension_type cs_num_cols = cs.num_columns();

  // Step 1:
  // determine variables that are constrained to be nonnegative,
  // detect (non-negativity or tautology) constraints that will not
  // be part of the tableau and count the number of slack variables.

  // Counters determining the dimensions of the tableau:
  // initialized here, they will be updated while examining `cs'.
  dimension_type tableau_num_rows = cs_num_rows;
  dimension_type tableau_num_cols = 2*cs_num_cols - 1;
  dimension_type num_slack_variables = 0;

  // On exit, `is_tableau_constraint[i]' will be true if and only if
  // `cs[i]' is neither a tautology (e.g., 1 >= 0) nor a non-negativity
  // constraint (e.g., X >= 0).
  std::deque<bool> is_tableau_constraint(cs_num_rows, true);

  // On exit, `nonnegative_variable[j]' will be true if and only if
  // Variable(j) is bound to be nonnegative in `cs'.
  std::deque<bool> nonnegative_variable(cs_num_cols - 1, false);

  // Process each row of the `cs' matrix.
  for (dimension_type i = cs_num_rows; i-- > 0; ) {
    const Linear_Row& cs_i = cs[i];
    bool found_a_nonzero_coeff = false;
    bool found_many_nonzero_coeffs = false;
    dimension_type nonzero_coeff_column_index = 0;
    for (dimension_type j = cs_num_cols; j-- > 1; ) {
      if (cs_i[j] != 0)
	if (found_a_nonzero_coeff) {
	  found_many_nonzero_coeffs = true;
	  if (cs_i.is_ray_or_point_or_inequality())
	    ++num_slack_variables;
	  break;
	}
	else {
	  nonzero_coeff_column_index = j;
	  found_a_nonzero_coeff = true;
	}
    }
    // If more than one coefficient is nonzero,
    // continue with next constraint.
    if (found_many_nonzero_coeffs)
      continue;

    if (!found_a_nonzero_coeff) {
      // All coefficients are 0.
      // The constraint is either trivially true or trivially false.
      if (cs_i.is_ray_or_point_or_inequality()) {
	if (cs_i[0] < 0)
	  // A constraint such as -1 >= 0 is trivially false.
	  return UNFEASIBLE_LP_PROBLEM;
      }
      else
	// The constraint is an equality.
	if (cs_i[0] != 0)
	  // A constraint such as 1 == 0 is trivially false.
	  return UNFEASIBLE_LP_PROBLEM;
      // Here the constraint is trivially true.
      is_tableau_constraint[i] = false;
      --tableau_num_rows;
      continue;
    }
    else {
      // Here we have only one nonzero coefficient.
      /*

      We have the following methods:
      A) Do split the variable and do add the constraint in the tableau.
      B) Don't split the variable and do add the constraint in the tableau.
      C) Don't split the variable and don't add the constraint in the tableau.

      Let the constraint be (a*v + b relsym 0).
      These are the 12 possible combinations we can have:
                a |  b | relsym | method
      ----------------------------------
      1)       >0 | >0 |   >=   |   A
      2)       >0 | >0 |   ==   |   A
      3)       <0 | <0 |   >=   |   A
      4)       >0 | =0 |   ==   |   B
      5)       >0 | <0 |   ==   |   B
      Note:    <0 | >0 |   ==   | impossible by strong normalization
      Note:    <0 | =0 |   ==   | impossible by strong normalization
      Note:    <0 | <0 |   ==   | impossible by strong normalization
      6)       >0 | <0 |   >=   |   B
      7)       >0 | =0 |   >=   |   C
      8)       <0 | >0 |   >=   |   A
      9)       <0 | =0 |   >=   |   A

      The next lines will apply the correct method to each case.
      */

      // The variable index is not equal to the column index.
      const dimension_type nonzero_var_index = nonzero_coeff_column_index - 1;

      const int sgn_a = sgn(cs_i[nonzero_coeff_column_index]);
      const int sgn_b = sgn(cs_i[0]);
      // Cases 1-3: apply method A.
      if (sgn_a == sgn_b) {
	if (cs_i.is_ray_or_point_or_inequality())
	  ++num_slack_variables;
      }
      // Cases 4-5: apply method B.
      else if (cs_i.is_line_or_equality()) {
	if (!nonnegative_variable[nonzero_var_index]) {
	  nonnegative_variable[nonzero_var_index] = true;
	  --tableau_num_cols;
	}
      }
      // Case 6: apply method B.
      else if (sgn_b < 0) {
	if (!nonnegative_variable[nonzero_var_index]) {
	  nonnegative_variable[nonzero_var_index] = true;
	  --tableau_num_cols;
	}
	++num_slack_variables;
      }
      // Case 7: apply method C.
      else if (sgn_a > 0) {
	if (!nonnegative_variable[nonzero_var_index]) {
	  nonnegative_variable[nonzero_var_index] = true;
	  --tableau_num_cols;
	}
	is_tableau_constraint[i] = false;
	--tableau_num_rows;
      }
      // Cases 8-9: apply method A.
      else
	++num_slack_variables;
    }
  }

  // The slack variables will be columns in the tableau.
  tableau_num_cols += num_slack_variables;

  // Now we can fill the map.
  for (dimension_type i = 0, j = nonnegative_variable.size(),
	 nnv_size = j; i < nnv_size; ++i)
    if (!nonnegative_variable[i]) {
      dim_map.insert(std::make_pair(i, j));
      ++j;
    }

  // Step 2:
  // set the dimensions for the tableau and the cost function.
  if (tableau_num_rows > 0) {
    if (tableau_num_cols > tableau.max_num_columns())
      throw std::length_error("PPL::LP_Problem:\nthe maximum size of an "
			      "internal data structure has been exceeded "
			      "while solving the LP_Problem.");
    tableau.add_zero_rows_and_columns(tableau_num_rows,
				      tableau_num_cols,
				      Row::Flags());
  }

  // Phase 3:
  // insert all the (possibly transformed) constraints that are not
  // nonnegativity constraints. The transformation includes both
  // the variable splitting (for variables that are unconstrained
  // in sign) and the addition of slack variables (for inequalities
  // in the original problem).

  for (dimension_type k = tableau_num_rows, slack_index = tableau_num_cols,
	 i = cs_num_rows; i-- > 0; )
    if (is_tableau_constraint[i]) {
      // Copy the original constraint in the tableau.
      Row& tableau_k = tableau[--k];
      const Linear_Row& cs_i = cs[i];
      for (dimension_type j = cs_num_cols; j-- > 0; )
	tableau_k[j] = cs_i[j];
      // Add the slack variable, if needed.
      if (cs_i.is_ray_or_point_or_inequality())
	tableau_k[--slack_index] = -1;
    }

  // Split the variables in the tableau and cost function.
  typedef std::map<dimension_type, dimension_type>::const_iterator iter;
  for (iter map_itr = dim_map.begin(),
	 map_end = dim_map.end(); map_itr != map_end; ++map_itr) {
    const dimension_type original_var = (map_itr->first) + 1;
    const dimension_type split_var = (map_itr->second) + 1;
    for (dimension_type i = tableau_num_rows; i-- > 0; ) {
      Row& tableau_i = tableau[i];
      tableau_i[split_var] = -tableau_i[original_var];
    }
  }

  // If there is no constraint in the tableau, then the feasible region
  // is only delimited by non-negativity constraints. Therefore,
  // the problem is unbounded as soon as the cost function has
  // a variable with a positive coefficient.
  if (tableau_num_rows == 0)
    for (dimension_type i = tableau_num_cols; i-- > 1; )
      if (input_obj_function[i] > 0){
	status = UNBOUNDED;
	return UNBOUNDED_LP_PROBLEM;
      }
  // The problem is neither trivially unfeasible nor trivially unbounded.
  // The tableau was successfull computed and the caller has to figure
  // out which case applies.
  status = OPTIMIZED;
  return OPTIMIZED_LP_PROBLEM;
}

bool
PPL::LP_Problem::is_in_base(const dimension_type var_index,
			    dimension_type& row_index) const {
  for (row_index = base.size(); row_index-- > 0; )
    if (base[row_index] == var_index)
      return true;
  return false;
}

PPL::Generator
PPL::LP_Problem::compute_generator() const {
  // We will store in num[] and in den[] the numerators and
  // the denominators of every variable of the original problem.
  dimension_type original_space_dim = input_cs.space_dimension();
  std::vector<Coefficient> num(original_space_dim);
  std::vector<Coefficient> den(original_space_dim);
  dimension_type row = 0;

  // We start to compute num[] and den[].
  typedef std::map<dimension_type, dimension_type>::const_iterator iter;
  iter map_end = dim_map.end();

  for (dimension_type i = original_space_dim; i-- > 0; ) {
    Coefficient& num_i = num[i];
    Coefficient& den_i = den[i];
    // Get the value of the variable from the tableau
    // (if it is not a basic variable, the value is 0).
    if (is_in_base(i+1, row)) {
      const Row& t_row = tableau[row];
      if (t_row[i+1] > 0) {
	num_i= -t_row[0];
	den_i= t_row[i+1];
      }
      else {
	num_i= t_row[0];
	den_i= -t_row[i+1];
      }
    }
    else {
      num_i = 0;
      den_i = 1;
    }
    // Check whether the variable was split.
    iter map_iter = dim_map.find(i);
    if (map_iter != map_end) {
      // The variable was split: get the value for the negative component,
      // having index map[i] + 1.
      const dimension_type split_i = map_iter->second;
      // Like before, we he have to check if the variable is in base.
      if (is_in_base(split_i+1, row)) {
	const Row& t_row = tableau[row];
	TEMP_INTEGER(split_num);
	TEMP_INTEGER(split_den);
	if (t_row[split_i+1] > 0) {
	  split_num = -t_row[0];
	  split_den = t_row[split_i+1];
	}
	else {
	  split_num = t_row[0];
	  split_den = -t_row[split_i+1];
	}
	// We compute the lcm to compute subsequently the difference
	// between the 2 variables.
	TEMP_INTEGER(lcm);
	lcm_assign(lcm, den_i, split_den);
	exact_div_assign(den_i, lcm, den_i);
	exact_div_assign(split_den, lcm, split_den);
	num_i *= den_i;
	sub_mul_assign(num_i, split_num, split_den);
	if (num_i == 0)
	  den_i = 1;
	else
	  den_i = lcm;
      }
      // Note: if the negative component was not in base, then
      // it has value zero and there is nothing left to do.
    }
  }

  // Compute the lcm of all denominators.
  TEMP_INTEGER(lcm);
  lcm = den[0];
  for (dimension_type i = 1; i < original_space_dim; ++i)
    lcm_assign(lcm, lcm, den[i]);
  // Use the denominators to store the numerators' multipliers
  // and then compute the normalized numerators.
  for (dimension_type i = original_space_dim; i-- > 0; ) {
    exact_div_assign(den[i], lcm, den[i]);
    num[i] *= den[i];
  }

  // Finally, build the generator.
  Linear_Expression expr;
  for (dimension_type i = original_space_dim; i-- > 0; )
    expr += num[i] * Variable(i);
  return point(expr, lcm);
}

void
PPL::LP_Problem::second_phase() {
  // Second_phase requires that *this is satisfiable.
  assert(status == SATISFIABLE || status == UNBOUNDED || status == OPTIMIZED);
  // In the following cases the problem is already solved.
  if (status == UNBOUNDED || status == OPTIMIZED)
    return;

  // Negate the cost function if we are minimizing.
  Row new_cost = input_obj_function;
  if (opt_mode == MINIMIZATION)
    for (dimension_type i = new_cost.size(); i-- > 0; )
      neg_assign(new_cost[i]);

  // Substitute properly the cost funcion in the `costs' Matrix.
  const dimension_type cost_zero_size = working_cost.size();
  Row tmp_cost = Row(new_cost, cost_zero_size, cost_zero_size);
  tmp_cost.swap(working_cost);
  working_cost[cost_zero_size-1] = 1;
  // Split the variable in the original cost function as defined in the
  // `dim_map' variable.
  typedef std::map<dimension_type, dimension_type>::const_iterator iter;
  for (iter map_itr = dim_map.begin(),
	 map_end = dim_map.end(); map_itr != map_end; ++map_itr){
    const dimension_type original_var = (map_itr->first) + 1;
    const dimension_type split_var = (map_itr->second) + 1;
    working_cost[split_var] = -working_cost[original_var];
  }

  // Here the first phase problem succeeded with optimum value zero.
  // Express the old cost function in terms of the computed base.
  for (dimension_type i = tableau.num_rows(); i-- > 0; ) {
    const dimension_type base_i = base[i];
    if (working_cost[base_i] != 0)
      linear_combine(working_cost, tableau[i], base_i);
  }
  // Solve the second phase problem.
  bool second_phase_successful = compute_simplex();

#if PPL_NOISY_SIMPLEX
  std::cout << "LP_Problem::solve: 2nd phase ended at iteration "
	    << num_iterations << "." << std::endl;
#endif
  if (second_phase_successful) {
    last_generator = compute_generator();
    status = OPTIMIZED;
  }
  else
    status = UNBOUNDED;
  assert(OK());
}

void
PPL::LP_Problem::evaluate_objective_function(const Generator& evaluating_point,
					     Coefficient& ext_n,
					     Coefficient& ext_d) const {
  const dimension_type ep_space_dim = evaluating_point.space_dimension();
  if (space_dimension() < ep_space_dim)
    throw std::invalid_argument("PPL::LP_Problem::"
				"evaluate_objective_function(p, n, d):\n"
				"*this and p are dimension incompatible.");
  if (!evaluating_point.is_point())
    throw std::invalid_argument("PPL::LP_Problem::"
				"evaluate_objective_function(p, n, d):\n"
				"p is not a point.");

  // Compute the smallest space dimension  between `input_obj_function'
  // and `evaluating_point'.
  const dimension_type space_dim
    = std::min(ep_space_dim, input_obj_function.space_dimension());
  // Compute the optimal value of the cost function.
  ext_n = input_obj_function.inhomogeneous_term();
  for (dimension_type i = space_dim; i-- > 0; )
    ext_n += evaluating_point.coefficient(Variable(i))
      * input_obj_function.coefficient(Variable(i));
  // Numerator and denominator should be coprime.
  normalize2(ext_n, evaluating_point.divisor(), ext_n, ext_d);
}

bool
PPL::LP_Problem::is_satisfiable() const {
#if PPL_NOISY_SIMPLEX
  num_iterations = 0;
#endif
  // Check for the `status' attribute in trivial cases.
  switch (status) {
  case UNSATISFIABLE:
    return false;
  case SATISFIABLE:
    return true;
  case UNBOUNDED:
    return true;
  case OPTIMIZED:
    return true;
  case PARTIALLY_SATISFIABLE:
    return false;
  case UNSOLVED:
    break;
  }

  LP_Problem& x = const_cast<LP_Problem&>(*this);

  // The space dimension of the solution to be computed.
  // Note: here we can not use method Constraint_System::space_dimension(),
  // because if the constraint system is NNC, then even the epsilon
  // dimension has to be interpreted as a normal dimension.
  const dimension_type space_dim = x.input_cs.num_columns() - 1;

  // Reset internal objects.
  x.tableau.clear();
  x.dim_map.clear();
  // Compute the initial tableau.
  LP_Problem_Status s_status = x.compute_tableau();

  // Check for trivial cases.
  switch (s_status) {
  case UNFEASIBLE_LP_PROBLEM:
    return false;
  case UNBOUNDED_LP_PROBLEM:
    // A feasible point has to be returned: the origin.
    // Ensure the right space dimension is obtained.
    x.last_generator = point(0*Variable(space_dim-1));
    return true;
  case OPTIMIZED_LP_PROBLEM:
    // Check for the special case of an empty tableau,
    // in which case an optimizing solution is the origin.
    if (x.tableau.num_rows() == 0) {
      // Ensure the right space dimension is obtained.
      x.last_generator = point(0*Variable(space_dim-1));
      return true;
    }
    break;
  }

#if PPL_NOISY_SIMPLEX
  num_iterations = 0;
#endif

  // Actually solve the LP problem.
  x.base = std::vector<dimension_type> (x.tableau.num_rows());

  // This will contain the new cost function for the 1st phase problem.
  // Adds the necessary slack variables to get the 1st phase problem.
  x.prepare_first_phase();
  // Solve the first phase of the primal simplex algorithm.
  bool first_phase_successful = x.compute_simplex();

#if PPL_NOISY_SIMPLEX
  std::cout << "LP_Problem::solve: 1st phase ended at iteration "
	    << num_iterations << "." << std::endl;
#endif
  // If the first phase problem was not solved or if we found an optimum
  // value different from zero, then the origianl problem is unfeasible.
  if (!first_phase_successful || x.working_cost[0] != 0){
    x.status = UNSATISFIABLE;
    return false;
  }

  // The first phase has found a feasible solution. If only a satisfiability
  // check was requested, we can return that feasible solution.
  // Store the last succesfully computed generator.
  x.last_generator = compute_generator();
  x.status = SATISFIABLE;
  // Erase the slack variables.
  x.erase_slacks();
  return true;
}

bool
PPL::LP_Problem::OK() const {
#ifndef NDEBUG
  using std::endl;
  using std::cerr;
#endif

  // Constraint system should contain no strict inequalities.
  if (input_cs.has_strict_inequalities()) {
#ifndef NDEBUG
    cerr << "The feasible region of the LP_Problem is defined by "
	 << "a constraint system containing strict inequalities."
	 << endl;
#endif
    return false;
  }

  // Constraint system and objective function should be dimension compatible.
  const dimension_type space_dim = input_cs.space_dimension();
  if (space_dim < input_obj_function.space_dimension()) {
#ifndef NDEBUG
    cerr << "The LP_Problem and the objective function have "
	 << "incompatible space dimensions ("
	 << space_dim << " < " << input_obj_function.space_dimension() << ")."
	 << endl;
#endif
    return false;
  }

  if (status == SATISFIABLE || status == UNBOUNDED || status == OPTIMIZED) {
    // Here `last_generator' has to be meaningful.
    // Check for dimension compatibility and actual feasibility.
    if (space_dim != last_generator.space_dimension()) {
#ifndef NDEBUG
      cerr << "The LP_Problem and the cached feasible point have "
	   << "incompatible space dimensions ("
	   << space_dim << " != " << last_generator.space_dimension() << ")."
	   << endl;
#endif
      return false;
    }
    if (!input_cs.satisfies_all_constraints(last_generator)) {
#ifndef NDEBUG
      cerr << "The cached feasible point does not belong to "
	   << "the feasible region of the LP_Problem."
	   << endl;
#endif
      return false;
    }

    const dimension_type tableau_nrows = tableau.num_rows();
    const dimension_type tableau_ncols = tableau.num_columns();

    // The number of rows in the tableau and base should be equal.
    if (tableau_nrows != base.size()) {
#ifndef NDEBUG
      cerr << "tableau and base have incompatible sizes" << endl;
#endif
      return false;
    }

    // The number of columns in the tableau and working_cost should be equal.
    if (tableau_ncols != working_cost.size()) {
#ifndef NDEBUG
      cerr << "tableau and working_cost have incompatible sizes" << endl;
#endif
      return false;
    }

    // The vector base should contain indices of tableau's columns.
    for (dimension_type i = base.size(); i-- > 0; )
      if (base[i] > tableau_ncols) {
#ifndef NDEBUG
	cerr << "base contains an invalid column index" << endl;
#endif
	return false;
      }

    // dim_map should encode an injective function having
    // disjoint domain and range.
    std::set<dimension_type> domain;
    std::set<dimension_type> range;
    typedef std::map<dimension_type, dimension_type>::const_iterator Iter;
    for (Iter i = dim_map.begin(), iend = dim_map.end(); i != iend; ++i) {
      domain.insert(i->first);
      range.insert(i->second);
    }
    if (domain.size() != range.size()
	|| domain.end() != std::find_first_of(domain.begin(), domain.end(),
					      range.begin(), range.end())) {
#ifndef NDEBUG
      cerr << "dim_map encodes an invalid map" << endl;
#endif
      return false;
    }
  }
  // TODO: further tests will be added when supporting incremental
  // computations.

  // All checks passed.
  return true;
}

void
PPL::LP_Problem::ascii_dump(std::ostream& s) const {
  using namespace IO_Operators;

  s << "input_cs\n";
  input_cs.ascii_dump(s);
  s << "\ninput_obj_function\n";
  input_obj_function.ascii_dump(s);
  s << "\nopt_mode " << (opt_mode == MAXIMIZATION ? "MAX" : "MIN") << "\n";

  s << "\nstatus: ";
  switch (status) {
  case UNSATISFIABLE:
    s << "UNSAT";
    break;
  case SATISFIABLE:
    s << "SATIS";
    break;
  case UNBOUNDED:
    s << "UNBOU";
    break;
  case OPTIMIZED:
    s << "OPTIM";
    break;
  case PARTIALLY_SATISFIABLE:
    s << "P_SAT";
    break;
  case UNSOLVED:
    s << "UNSOL";
    break;
  }
  s << "\n";

  s << "\ntableau\n";
  tableau.ascii_dump(s);
  s << "\nworking_cost\n";
  working_cost.ascii_dump(s);

  const dimension_type base_size = base.size();
  s << "\nbase (" << base_size << ")\n";
  for (dimension_type i = 0; i != base_size; ++i)
    s << base[i] << ' ';

  const dimension_type dim_map_size = dim_map.size();
  s << "\ndim_map (" << dim_map_size << ")\n";
  for (std::map<dimension_type, dimension_type>::const_iterator
	 i = dim_map.begin(), iend = dim_map.end(); i != iend; ++i)
    s << Variable(i->first) << "->" << Variable(i->second) << ' ';

  // FIXME: no ascii_dump() for Generator?
  // last_generator.ascii_dump(s);
  s << "\nlast_generator\n";
  s << last_generator;
  s << "\n";
}

PPL_OUTPUT_DEFINITIONS_ASCII_ONLY(LP_Problem);
