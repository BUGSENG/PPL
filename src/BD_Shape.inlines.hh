/* BD_Shape class implementation: inline functions.
   Copyright (C) 2001-2005 Roberto Bagnara <bagnara@cs.unipr.it>

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
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
USA.

For the most up-to-date information see the Parma Polyhedra Library
site: http://www.cs.unipr.it/ppl/ . */

#ifndef PPL_BD_Shape_inlines_hh
#define PPL_BD_Shape_inlines_hh 1

#include <cassert>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

namespace Parma_Polyhedra_Library {

template <typename T>
inline void
min_assign(T& x, const T& y) {
  // This is an auxiliary function to compute the minimum 
  // between two T-values.
  if (x > y)
    x = y;
}

template <typename T>
inline dimension_type
BD_Shape<T>::max_space_dimension() {
  using std::min;
  // One dimension is reserved to have a value of type dimension_type
  // that does not represent a legal dimension.
  return min(DB_Matrix<T>::max_num_rows() - 1, DB_Matrix<T>::max_num_columns() - 1);
}

template <typename T>
inline void
BD_Shape<T>::init() {
  dimension_type space_dim = space_dimension();
  // The matrix `dbm' is initialized a plus infinity to 
  // haven't any constraints.
  for (dimension_type i = 0; i <= space_dim; ++i) {
    DB_Row<T>& dbm_i = dbm[i];
    for (dimension_type j = 0; j <= space_dim; ++j)
      dbm_i[j] = PLUS_INFINITY;
  }
}

template <typename T>
inline
BD_Shape<T>::BD_Shape(dimension_type num_dimensions,
		  Polyhedron::Degenerate_Kind kind)
  : dbm(num_dimensions + 1), status() {
  if (kind == Polyhedron::EMPTY)
    set_empty();
  else {
    init();
    if (num_dimensions > 0)
    // A (non zero-dim) universe system of bounded differences
    // is transitively closed.
    status.set_transitively_closed();
  }
  assert(OK());
}

template <typename T>
inline
BD_Shape<T>::BD_Shape(const BD_Shape& y)
  : dbm(y.dbm), status(y.status) {
}

template <typename T>
inline
BD_Shape<T>::BD_Shape(const Constraint_System& cs)
  : dbm(cs.space_dimension() + 1) {
  init();
  if (cs.space_dimension() > 0)
    // A (non zero-dim) universe system of bounded differences
    // is transitively closed.
    status.set_transitively_closed();
  add_constraints(cs);
  assert(OK());
}

template <typename T>
inline BD_Shape<T>&
BD_Shape<T>::operator=(const BD_Shape& y) {
  status = y.status;
  dbm = y.dbm;
  return *this;
}

template <typename T>
inline
BD_Shape<T>::~BD_Shape() {
}

template <typename T>
inline void
BD_Shape<T>::swap(BD_Shape& y) {
  std::swap(status, y.status);
  std::swap(dbm, y.dbm);
}

template <typename T>
inline bool
BD_Shape<T>::marked_empty() const {
  return status.test_empty();
}

template <typename T>
inline bool
BD_Shape<T>::marked_transitively_closed() const {
  return status.test_transitively_closed();
}

template <typename T>
inline dimension_type
BD_Shape<T>::space_dimension() const {
  return dbm.num_rows() - 1;
}

template <typename T>
inline bool
operator==(const BD_Shape<T>& x, const BD_Shape<T>& y) {
  // Dimension-compatibility check.
  if (x.space_dimension() != y.space_dimension())
    return false;
  
  // Two zero-dim polyhedra are equal if and only if they are 
  // both empty or universe.
  if (x.space_dimension() == 0) {
    if (x.marked_empty())
      return y.marked_empty();
    else 
      return !y.marked_empty();
  }

  /* 
     The two systems of bounded differences need be closed.     
     In fact if, for example, in `x' we have the constraints:
     
     x1 - x2 <= 1;
     x2      <= 2;

     and in `y' we have the constraints:

     x1 - x2 <= 1;
     x2      <= 2;
     x1      <= 3;

     without closure it returns "false" instead of "true".
 */
  x.closure_assign();
  y.closure_assign();

  // If one of two polyhedron is empty, then they are equal if and only if
  // also other polyhedron is empty.
  if (x.marked_empty())
    return y.marked_empty();
  if (y.marked_empty())
    return false;

  return x.dbm == y.dbm;
}

template <typename T>
inline bool
operator!=(const BD_Shape<T>& x, const BD_Shape<T>& y) {
  return !(x == y);
}

template <typename T>
inline void
BD_Shape<T>::set_empty() {
  status.set_empty();
  assert(OK());
  assert(is_empty());
}

template <typename T>
inline void
BD_Shape<T>::set_zero_dim_univ() {
  status.set_zero_dim_univ();
}

template <typename T>
inline void
BD_Shape<T>::add_constraint(const Constraint& c) {
  /* 
     This method not preserve the closure.
     If we consider a closed system of bounded differences bd(2)
     with these constraint:

     x - y <= 2
     x     <= 8

     and then we add a new constraint:

     y     <= 3

     it is not longer closed.
     Its closure is:

     x - y <= 2
     y     <= 3
     x     <= 5.
  */
  dimension_type c_space_dimension = c.space_dimension();
  
  // Dimension-compatibility check.
  if (c_space_dimension > space_dimension())
    throw_dimension_incompatible("add_constraint(c)", c);
  // Not strict-inequality check.
  if (c.is_strict_inequality())
    throw_constraint_incompatible("add_constraint(c)");

  // Store the indices of the non-zero components of `c',
  dimension_type j[2] = { 0, 0 };

  // Number of non-zero components of `c'.
  dimension_type t = 0;

  // Collect the non-zero components of `c'.
  for (dimension_type i = c_space_dimension; i-- > 0; )
    if (c.coefficient(Variable(i)) != 0) {
      if (t >= 2)
	// Constraints that are not "bounded differences" are ignored.
	return;
      else
	j[t++] = i;
    }

  // We will now make sure `c' has one of the following forms:
  //
  //           0 <=/= b, if t == 0;
  //   a*x       <=/= b, if t == 1;
  //   a*x - a*y <=/= b, if t == 2.
  //
  // In addition, j[0] and (if t >= 1) j[1] will contain the indices
  // of the cell(s) of `dbm' to be modified.
  Coefficient a;
  Coefficient b = c.inhomogeneous_term();
  switch (t) {
  case 2:
    a = c.coefficient(Variable(j[1]));
    if (a != -c.coefficient(Variable(j[0])))
      // Constraints that are not "bounded differences" are ignored.
      return;
    ++j[1];
    ++j[0];
    break;

  case 1:
    a = -c.coefficient(Variable(j[0]));
    ++j[0];
    break;

  case 0:
    if (b < 0)
      set_empty();
    return;
  }

  // Select the cell to be modified for the "<=" part of the constraint,
  // and set `a' to the absolute value of itself.
  T& dbm_j_0_j_1 = dbm[j[0]][j[1]];
  T& dbm_j_1_j_0 = dbm[j[1]][j[0]];
  T* dbm_cellp;
  if (a < 0) {
    dbm_cellp = &dbm_j_0_j_1;
    a = -a;
  }
  else
    dbm_cellp = &dbm_j_1_j_0;
  
  bool check_change = false;
  // Compute b/a into `d', rounding the result towards plus infinity.
  T d;
  div_round_up(d, b, a);
  if (*dbm_cellp > d) {
    *dbm_cellp = d;
    check_change = true;
  }

  if (c.is_equality()) {
    // The symmetric cell is the one to be possibly modified.
    dbm_cellp = (dbm_cellp == &dbm_j_0_j_1) ? &dbm_j_1_j_0 : &dbm_j_0_j_1;
    // Compute -b/a into `d', rounding the result towards plus infinity.
    T d;
    div_round_up(d, -b, a);
    if (*dbm_cellp > d) {
      *dbm_cellp = d;
      check_change = true;
    }
  }
  if (check_change && marked_transitively_closed())
    status.reset_transitively_closed();
  assert(OK());
}

template <typename T>
inline bool
BD_Shape<T>::add_constraint_and_minimize(const Constraint& c) {
  add_constraint(c);
  closure_assign();
  return !marked_empty();
}

template <typename T>
inline void
BD_Shape<T>::add_constraints(const Constraint_System& cs) {
  // This method not preserve closure.
  // Seen add_constraint().
  Constraint_System::const_iterator iend = cs.end();
  for (Constraint_System::const_iterator i = cs.begin(); i != iend; ++i)
    add_constraint(*i);
  assert(OK());
}

template <typename T>
inline bool
BD_Shape<T>::add_constraints_and_minimize(const Constraint_System& cs) {
  add_constraints(cs);
  closure_assign();
  return !marked_empty();
}

template <typename T>
inline void
BD_Shape<T>::concatenate_assign(const BD_Shape<T>& y) {
  assert(OK());
  assert(y.OK());

  dimension_type k = space_dimension();
  dimension_type h = y.space_dimension();

  // If `y' is an empty 0-dim space system of bounded differences,
  // let `*this' become empty.
  if (h == 0 && y.marked_empty()) {
    set_empty();
    return;
  }

  // If `*this' is an empty 0-dim space 
  // system of bounded differences, it is sufficient to adjust
  // the dimension of the space.
  if (k == 0 && marked_empty()) {
    dbm.grow(h + 1);
    return;
  }
  // First we increase the space dimension of `*this' by adding 
  // `y.space_dimension()' new dimensions.
  // The matrix for the new system of constraints is obtained
  // by leaving the old system of constraints in the upper left-hand side
  // and placing the constraints of `y' in the lower right-hand side,
  // except the constraints as `y(i) >= cost' or `y(i) <= cost', that are
  // placed in the right position on the new matrix.
  add_space_dimensions_and_embed(h);
  for (dimension_type i = k + 1; i <= k + h; ++i) {
    DB_Row<T>& dbm_i = dbm[i];
    dbm_i[0] = y.dbm[i-k][0];
    dbm[0][i] = y.dbm[0][i-k];
    for (dimension_type j = k + 1; j <= k + h; ++j)
      dbm_i[j] = y.dbm[i-k][j-k];
  }

  if (marked_transitively_closed())
    status.reset_transitively_closed();
  assert(OK());
}

template <typename T>
inline bool
BD_Shape<T>::contains(const BD_Shape& y) const {
  // Dimension-compatibility check.
  if (space_dimension() != y.space_dimension())
    throw_dimension_incompatible("contains(y)", y);

  // The zero-dimensional universal system of bounded differences 
  // always contains a zero-dimensional universal system of 
  // bounded differences and a zero-dimensional empty system 
  // of bounded differences.
  // The zero-dimensional empty system of bounded differences 
  // only contains a zero-dimensional empty system of bounded differences.
  if (space_dimension() == 0) {
    if (!marked_empty())
      return true;
    else 
      return y.marked_empty();
  }

  /* 
    The `y' system of bounded differences need be closed.
    In fact if, for example, in `*this' we have the constraints:
    
    x1 - x2 <= 1;
    x1      <= 3;
    x2      <= 2;
    
    in `y' the constraints are:
    
    x1 - x2 <= 0;
    x2      <= 1;
    
    without closure it returns "false", instead if we close `y' we have
    the implicit constraint
    
    x1      <= 1;
    
    and so we obtain the right result "true".
  */
  y.closure_assign();
  
  // If `y' is empty, then it is always contained by
  // every polyhedra.
  if (y.marked_empty())
    return true;

  // `*this' contains `y' if and only if every cell of `dbm'
  // is greater than or equal to the correspondent one of `y.dbm'. 
  dimension_type n = space_dimension();
  for (dimension_type i = 0; i <= n; ++i)
    for (dimension_type j = 0; j <= n; ++j)
      if (dbm[i][j] < y.dbm[i][j])
	return false;
  return true;
}

template <typename T>
inline bool
BD_Shape<T>::is_empty() const {
  // If `*this' is already empty, then returns "true".
  if (marked_empty())
    return true;

  // If `*this' is close, then it is never empty.
  if (marked_transitively_closed())
    return false;
  
  dimension_type k = space_dimension();
  // A zero-dim universal polyhedron it is never empty.
  if (k == 0)
    return false;

  // Remark that the matrix `dbm' can be seen as the adjacent matrix
  // of a weighted directed graph, where the nodes are the variables
  // and the weights are the inhomogeneous terms of the constraints.
  // Here we use a modification of Bellman-Ford algorithm for
  // not-necessary-connected graph.
  // Since the graph is not necessary connected we consider a new
  // node, called source, that is joined with all other nodes with 
  // zero-weight arcs.
  
  // Values of the minimum path, from source to all nodes.
  DB_Row<T> z(k + 1);
  for (dimension_type i = 0; i <= k; ++i)
    z[i].assign(0, ROUND_IGNORE);

  // The relax-technique: given an arc (j,h), it tries to improve
  // the value of minimum path for h passing by j.
  T sum1;
  for (dimension_type i = 0; i <= k; ++i) 
    for (dimension_type j = 0; j <= k; ++j) {
      const DB_Row<T>& dbm_j = dbm[j]; 
      T& z_j = z[j];
      for (dimension_type h = 0; h <= k; ++h) {
	add_round_up(sum1, dbm_j[h], z_j);
	min_assign(z[h], sum1);
      }
    }

  // `*this' is empty if and only if it has at least a cycles 
  // with a negative weight.
  // This happens when vector `z' don't contain really the 
  // smaller values of minimum path, from source to all nodes.
  T sum2;
  for (dimension_type j = 0; j <= k; ++j) {
    const DB_Row<T>& dbm_j = dbm[j]; 
    T& z_j = z[j];
    for (dimension_type h = 0; h <= k; ++h) {
      add_round_up(sum2, dbm_j[h], z_j);
      if (z[h] > sum2) {
	Status& nstatus = const_cast<Status&>(status);
	nstatus.set_empty();
	return true; 
      }
    }
  }

  return false;
}

template <typename T>
inline bool
BD_Shape<T>::is_universe() const {
  
  // If system of bounded differences is empty 
  // then is not a universe system of bounded differences. 
  if (marked_empty())
    return false;

  dimension_type n = space_dimension();
  
  // If system of bounded differences is zero-dimensional, 
  // then it is necessarily an universe system of bounded differences.
  if (n == 0)
    return true;

  // An universe system of bounded differences has not constraints:
  // in all the cells of matrix `dbm' must have `plus infinity'.
  for (dimension_type i = 0; i <= n; ++i) {
    const DB_Row<T>& dbm_i = dbm[i]; 
    for (dimension_type j = 0; j <= n; ++j)
      if (!is_plus_infinity(dbm_i[j]))
	return false;
  }
  return true;
}

template <typename T>
inline bool
BD_Shape<T>::is_transitively_reduced() const {
  // If system of bounded differences is already empty
  // is transitively reduced.
  if (marked_empty())
    return true; 

  dimension_type space_dim = space_dimension();

  const BD_Shape x_copy = *this;
  x_copy.closure_assign();

  // The vector `leader' is used to indicate which variables are equivalent. 
  std::vector<dimension_type> leader(space_dim + 1);
  
  // We store the leader.
  for (dimension_type i = space_dim + 1; i-- > 0; )
    leader[i] = i;
  
  // Step 1: we store really the leader with the corrected value.
  // We search for the equivalent or zero-equivalent variables.
  // The variable(i-1) and variable(j-1) are equivalent if and only if 
  // m_i_j == -(m_j_i). 
  for (dimension_type i = 0; i < space_dim; ++i) {
    const DB_Row<T>& dbm_i = x_copy.dbm[i];
    for (dimension_type j = i + 1; j <= space_dim; ++j) {
      T negated_dbm_ji;
      if (exact_neg(negated_dbm_ji, x_copy.dbm[j][i]) &&
	  negated_dbm_ji == dbm_i[j])
	// Two equivalent variables have got the same leader 
	// (the smaller variable).
	leader[j] = leader[i];
    }
  }

  // Step 2: we check if there are redundant constraints in the zero_cycle 
  // free systems of bounded differences, considering only the leaders.
  // A constraint `c' is redundant, when there are two constraints such that
  // their sum is the same constraint with the inhomogeneous term
  // less than or equal to the `c' one.
  T c;
  for (dimension_type k = 0; k <= space_dim; ++k) {
    if (leader[k] == k) {
      const DB_Row<T>& x_k = dbm[k];
      for (dimension_type i = 0; i <= space_dim; ++i) { 
	if (leader[i] == i) {
	  const DB_Row<T>& x_i = dbm[i];
	  const T& x_i_k = x_i[k];
	  for (dimension_type j = 0; j <= space_dim; ++j) { 
	    if (leader[j] == j) {
	      const T& x_i_j = x_i[j];
	      if (!is_plus_infinity(x_i_j)) {
		add_round_up(c, x_i_k, x_k[j]);
		if (x_i_j >= c)
		  return false;
	      }
	    }
	  }
	} 
      }
    }
  }

  // The vector `var_conn' is used to check if there is a single cycle
  // that connected all zero-equivalent variables between them.
  // The value `space_dim + 1' is used to indicate that the equivalence
  // class contains a single variable. 
  std::vector<dimension_type> var_conn(space_dim + 1);
  for (dimension_type i = space_dim + 1; i-- > 0; )
    var_conn[i] = space_dim + 1; 

  // Step 3: we store really the `var_conn' with the right value, putting 
  // the variable with the selected variable is connected:
  // we check the row of each variable:
  // a- each leader could be connected with only zero-equivalent one,
  // b- each no-leader with only another zero-equivalent one.
  for (dimension_type i = 0; i <= space_dim; ++i) {
    const DB_Row<T>& x_i = dbm[i];
    // It count with how many variables the selected variable is
    // connected.
    dimension_type t = 0;
    dimension_type ld_i = leader[i];
    // Case a: leader.
    if (ld_i == i) {
      for (dimension_type j = 0; j <= space_dim; ++j) {
	dimension_type ld_j = leader[j];
	// Only the connectedness with equivalent variables 
	// is considered.
	if (j != ld_j)
	  if (!is_plus_infinity(x_i[j])) {
	    if (t == 1)
	      // Two no-leaders couldn't connected with the same leader.
	      return false;
	    else 
	      if (ld_j != i)
		// The variables isn't in the same equivalence class.
		return false;
	      else {
		++t;
		var_conn[i] = j;
	      }
	  }
      }
    }
    // Case b: no-leader.
    else {
      for (dimension_type j = 0; j <= space_dim; ++j) {
	if (!is_plus_infinity(x_i[j])) {
	  dimension_type ld_j = leader[j];
	  if (ld_i != ld_j)
	    // The variables isn't in the same equivalence class.
	    return false;
	  else {
	    if (t == 1)
	      // Two variables couldn't connected with the same leader.
	      return false;
	    else {
	      ++t;
	      var_conn[i] = j;
	    }
	  }
	  // A no-leader must be connected with 
	  // another variable.
	  if (t == 0)
	    return false;
	}
      }
    }
  }

  // The vector `just_checked' is used to check if 
  // a variable is already checked. 
  std::vector<bool> just_checked(space_dim + 1);
  for (dimension_type i = space_dim + 1; i-- > 0; )
    just_checked[i] = false;

  // Step 4: we check if there are single cycles that
  // connected all the zero-equivalent variables between them.
  for (dimension_type i = 0; i <= space_dim; ++i) {
    bool jc_i = just_checked[i];
    // We don't re-control the already considered single cycles.
    if (!jc_i) {
      dimension_type v_con = var_conn[i];
      // We consider only the equivalence classes with 
      // 2 or plus variables.
      if (v_con != space_dim + 1) {
	// There is a single cycle if taken a variable, 
	// we return to this same variable. 
	while (v_con != i) {
	  just_checked[v_con] = true;
	  v_con = var_conn[v_con];
	  // If we re-pass to an already considered variable, 
	  // then we haven't a single cycle.
	  if (just_checked[v_con])
	    return false;  
	}
      }
    }
    just_checked[i] = true;
  }

  // The system bounded differences is just reduced.
  return true;
}

template <typename T>
inline Poly_Con_Relation
BD_Shape<T>::relation_with(const Constraint& c) const {
  dimension_type c_space_dimension = c.space_dimension();
  
  // Dimension-compatibility check.
  if (c_space_dimension > space_dimension())
    throw_dimension_incompatible("relation_with(c)", c);

  closure_assign();

  if (marked_empty())
    return Poly_Con_Relation::saturates()
      && Poly_Con_Relation::is_included()
      && Poly_Con_Relation::is_disjoint();

  if (space_dimension() == 0) {
    if ((c.is_equality() && c.inhomogeneous_term() != 0)
	|| (c.is_inequality() && c.inhomogeneous_term() < 0)) 
      return Poly_Con_Relation::is_disjoint();
    else 
      if (c.is_strict_inequality() && c.inhomogeneous_term() == 0)
	// The constraint 0 > 0 implicitly defines the hyperplane 0 = 0;
	// thus, the zero-dimensional point also saturates it.
	return Poly_Con_Relation::saturates()
	  && Poly_Con_Relation::is_disjoint();
      
      else if (c.is_equality() || c.inhomogeneous_term() == 0)
	return Poly_Con_Relation::saturates()
	  && Poly_Con_Relation::is_included();
    
      else
	// The zero-dimensional point saturates
	// neither the positivity constraint 1 >= 0,
	// nor the strict positivity constraint 1 > 0.
	return Poly_Con_Relation::is_included();
  }
  
  // Store the indices of the non-zero components of `c',
  dimension_type j[2] = { 0, 0 };
  
  // Number of non-zero components of `c'.
  dimension_type t = 0;
  
  // Collect the non-zero components of `c'.
  for (dimension_type i = c_space_dimension; i-- > 0; )
    if (c.coefficient(Variable(i)) != 0) {
      if (t >= 2)
	throw_constraint_incompatible("relation_with(c)");
      
      else
	j[t++] = i;
    }
  
  // We will now make sure `c' has one of the following forms:
  //
  //           0 <=/= b, if t == 0;
  //   a*x       <=/= b, if t == 1;
  //   a*x - a*y <=/= b, if t == 2.
  //
  // In addition, j[0] and (if t >= 1) j[1] will contain the indices
  // of the cell(s) of `dbm' to be checked.
  Coefficient a;
  Coefficient b = c.inhomogeneous_term();
  switch (t) {
  case 2:
    a = c.coefficient(Variable(j[1]));
    if (a != -c.coefficient(Variable(j[0])))
      throw_constraint_incompatible("relation_with(c)");
    
    ++j[1];
    ++j[0];
    break;
    
  case 1:
    a = -c.coefficient(Variable(j[0]));
    ++j[0];
    break;
    
  case 0:
    if (b < 0) 
      return Poly_Con_Relation::is_disjoint();
    else if (b == 0) {
      if (c.is_strict_inequality())
	return Poly_Con_Relation::saturates()
	  && Poly_Con_Relation::is_disjoint();
      else
	return Poly_Con_Relation::saturates()
	  && Poly_Con_Relation::is_included();
    }
    else 
      return Poly_Con_Relation::is_included();
    break;
  }
  

  // Select the cell to be checked for the "<=" part of the constraint,
  // and set `a' to the absolute value of itself.
  const T& dbm_j_0_j_1 = dbm[j[0]][j[1]];
  const T& dbm_j_1_j_0 = dbm[j[1]][j[0]];
  if (a < 0) {
    a = -a;
    T d;
    div_round_up(d, b, a);
    T d1;
    div_round_up(d1, -b, a);
    if (c.is_equality()) {
      if (d == dbm_j_0_j_1 && d1 == dbm_j_1_j_0)
	return Poly_Con_Relation::saturates()
	  && Poly_Con_Relation::is_included();
      else if (d < dbm_j_1_j_0 && d1 > dbm_j_0_j_1)
	return Poly_Con_Relation::is_disjoint();
      else 
	return Poly_Con_Relation::strictly_intersects();
    }
    else 
      if (c.is_nonstrict_inequality()) {
	if (d >= dbm_j_0_j_1 && d1 >= dbm_j_1_j_0)
	  return Poly_Con_Relation::saturates()
	    && Poly_Con_Relation::is_included();
	else if (d >= dbm_j_0_j_1) 
	  return Poly_Con_Relation::is_included();
	else if (d < dbm_j_0_j_1 && d1 > dbm_j_1_j_0)
	  return Poly_Con_Relation::is_disjoint();
	else 
	  return Poly_Con_Relation::strictly_intersects(); 
      }
      else {
	if (d >= dbm_j_0_j_1 && d1 >= dbm_j_1_j_0)
	  return Poly_Con_Relation::saturates()
	    && Poly_Con_Relation::is_disjoint();
	else if (d > dbm_j_0_j_1) 
	  return Poly_Con_Relation::is_included();
	else if (d <= dbm_j_0_j_1 && d1 >= dbm_j_1_j_0)
	  return Poly_Con_Relation::is_disjoint();
	else
	  return Poly_Con_Relation::strictly_intersects();
      }
  }
  else {
    T d;
    div_round_up(d, b, a);
    T d1;
    div_round_up(d1, -b, a);
    if (c.is_equality()) {
      if (d == dbm_j_1_j_0 && d1 == dbm_j_0_j_1)
	return Poly_Con_Relation::saturates()
	  && Poly_Con_Relation::is_included();
      else if (d < dbm_j_0_j_1 && d1 > dbm_j_1_j_0)
	return Poly_Con_Relation::is_disjoint();
      else 
	return Poly_Con_Relation::strictly_intersects();
    }
    else
      if (c.is_nonstrict_inequality()) {
	if (d >= dbm_j_1_j_0 && d1 >= dbm_j_0_j_1)
	  return Poly_Con_Relation::saturates()
	    && Poly_Con_Relation::is_included();
	else if (d >= dbm_j_1_j_0) 
	  return Poly_Con_Relation::is_included();
	else if (d < dbm_j_1_j_0 && d1 > dbm_j_0_j_1)
	  return Poly_Con_Relation::is_disjoint();
	else 
	  return Poly_Con_Relation::strictly_intersects(); 
      }
      else {
	if (d >= dbm_j_1_j_0 && d1 >= dbm_j_0_j_1)
	  return Poly_Con_Relation::saturates()
	    && Poly_Con_Relation::is_disjoint();
	else if (d > dbm_j_1_j_0) 
	  return Poly_Con_Relation::is_included();
	else if (d <= dbm_j_1_j_0 && d1 >= dbm_j_0_j_1)
	  return Poly_Con_Relation::is_disjoint();
	else  
	  return Poly_Con_Relation::strictly_intersects();
      }
  }
}

template <typename T>
inline Poly_Gen_Relation
BD_Shape<T>::relation_with(const Generator& g) const {
  // Dimension-compatibility check.
  if (space_dimension() < g.space_dimension())
    throw_dimension_incompatible("relation_with(g)", g);
  
  // The empty bdiff cannot subsume a generator.
  if (marked_empty())
    return Poly_Gen_Relation::nothing();

  // A universe BD shape in a zero-dimensional space subsumes
  // all the generators of a zero-dimensional space.
  if (space_dimension() == 0)
    return Poly_Gen_Relation::subsumes();

  dimension_type space_dim = space_dimension();  
  dimension_type g_space_dim = g.space_dimension();
  
  bool is_line = g.is_line();
  
  // The relation between the bdiff and the given generator is obtained
  // checking if the generator satisfies all the constraints in the bdiff.
  // To check if the generator satisfies all the constraints it's enough
  // studying the sign of the scalar product between the generator and
  // all the constraints in the bdiff.
  
  // We find in `*this' all the constraints.
  for (dimension_type i = 0; i <= space_dim; ++i) {
    for (dimension_type j = i + 1; j <= space_dim; ++j) {
      const Variable x(j - 1);
      const bool x_dimension_incompatible = x.space_dimension() > g_space_dim;
      T dbm_i_j = dbm[i][j];
      T dbm_j_i = dbm[j][i];
      T negated_dbm_ji;
      const bool is_equality = exact_neg(negated_dbm_ji, dbm_j_i) &&
	negated_dbm_ji == dbm_i_j;
      const bool dbm_i_j_is_infinity = is_plus_infinity(dbm_i_j); 
      const bool dbm_j_i_is_infinity = is_plus_infinity(dbm_j_i); 
      if (i != 0) {
	const Variable y(i - 1);
	const bool y_dimension_incompatible = y.space_dimension() > g_space_dim;
	const bool is_trivial_zero = (x_dimension_incompatible && g.coefficient(y) == 0)
	  || (y_dimension_incompatible && g.coefficient(x) == 0)
	  || (x_dimension_incompatible && y_dimension_incompatible);
	if (is_equality) {
	  // We have one equality constraint.
	  // The constraint has form ax - ay = b.
	  // The scalar product has the form 
	  // 'a * y_i - a * x_j' 
	  // where y_i = g.coefficient(y) and x_j = g.coefficient(x).
	  // It is not zero when both the coefficients of the 
	  // variables x and y are not zero or when these coefficients
 	  if (!is_trivial_zero && g.coefficient(x) != g.coefficient(y))
	    return Poly_Gen_Relation::nothing();
	}
	else 
	  // We have the binary inequality constraints.
	  if (!dbm_i_j_is_infinity) {
	  // The constraint has form ax - ay <= b.
	  // The scalar product has the form 
	  // 'a * y_i - a * x_j' 
	    if (is_line && (!is_trivial_zero && g.coefficient(x) != g.coefficient(y)))
	      return Poly_Gen_Relation::nothing();
	    else
	      if (g.coefficient(y) < g.coefficient(x))
		return Poly_Gen_Relation::nothing();
	  }

	  else 
	    if (!dbm_j_i_is_infinity) {
	      // The constraint has form ay - ax <= b.
	      // The scalar product has the form 
	      // 'a * x_j - a* y_i'.
	      if (is_line && (!is_trivial_zero && g.coefficient(x) != g.coefficient(y)))
		return Poly_Gen_Relation::nothing();
	      else
		if (g.coefficient(x) < g.coefficient(y))
		  return Poly_Gen_Relation::nothing();
	    }
      }
      else {
	if (is_equality) {
	  // The constraint has form ax = b.
	  // To satisfy the constraint it's necessary that the scalar product
	  // is not zero.It happens when the coefficient of the variable 'x'
	  // in the generator is not zero, because the scalar 
	  // product has the form:
	  // 'a * x_i' where x_i = g.coefficient(x)..
	  if (!x_dimension_incompatible && g.coefficient(x) != 0)
	    return Poly_Gen_Relation::nothing();
	}
	else 
	  // We have the unary inequality constraints.
	  if (!dbm_i_j_is_infinity) {
	    // The constraint has form ax <= b.
	    // The scalar product has the form:
	    // '-a * x_i' where x_i = g.coefficient(x).
	    if (is_line && (!x_dimension_incompatible && g.coefficient(x) != 0))
	      return Poly_Gen_Relation::nothing();
	    else
	      if (g.coefficient(x) > 0)
		return Poly_Gen_Relation::nothing();
	  }
	  else 
	    if (!dbm_j_i_is_infinity) {
	      // The constraint has form -ax <= b.
	      // The scalar product has the form:
	      // 'a * x_i' where x_i = g.coefficient(x).
	      if (is_line && (!x_dimension_incompatible && g.coefficient(x) != 0))
		return Poly_Gen_Relation::nothing();
	      else
		if (g.coefficient(x) < 0)
		  return Poly_Gen_Relation::nothing();
	    }
      }
    }
  }
  return Poly_Gen_Relation::subsumes();
}

template <typename T>
inline void
BD_Shape<T>::closure_assign() const {
  
  // If system of bounded differences is already empty
  // or closed, then the closure_assign is not necessary.
  if (marked_empty() || marked_transitively_closed())
    return;
  dimension_type n = space_dimension();

  // If system of bounded differences is zero-dimensional, 
  // then it is necessarily a closed system of bounded differences.
  if (n == 0)
    return;

  // To enforce the closure we use the Floyd-Warshall algorithm.
  BD_Shape& x = const_cast<BD_Shape<T>&>(*this);

  // Fill the diagonal with zeros.
  for (dimension_type h = 0; h <= n; ++h)
    x.dbm[h][h].assign(0, ROUND_IGNORE);

  // Algorithm is described in the following way:
  // indicated with `m' the matrix `dbm' we have
  // m_i_j = min(m_i_j, 
  //             m_i_k + m_k_j).
  T sum;
  for (dimension_type k = 0; k <= n; ++k) {
    DB_Row<T>& xdbm_k = x.dbm[k]; 
    for (dimension_type i = 0; i <= n; ++i) {
      DB_Row<T>& xdbm_i = x.dbm[i];
      T& xdbm_i_k = xdbm_i[k];
      if (!is_plus_infinity(xdbm_i_k)) {
	for (dimension_type j = 0; j <= n; j++) {
	  T& xdbm_k_j = xdbm_k[j]; 
	  if (!is_plus_infinity(xdbm_k_j)) {
	    // Round upward. In fact, for example, we take this system:
	    // x3      <= 2.5 
	    // x2 - x3 <= 0.71
	    // x1 - x2 <= 0
	    //    - x1 <= 3.2  (x1 >= 3.2)
	    //
	    // we have got also these implicit constraints:
	    // x1 <= 3.21
	    // x2 <= 3.21
	    //
	    // But if the number 3.21 doesn't exist and we round downward to
	    // 3.2, we get for x1 a point and not a interval.(3.2 <= x1 <= 3.21).
	    add_round_up(sum, xdbm_i_k, xdbm_k_j);
	    min_assign(xdbm_i[j], sum);
	  }
	}
      }
    }
  }

  x.status.set_transitively_closed();
  for (dimension_type h = 0; h <= n; ++h) {
    T& xdbm_h_h = x.dbm[h][h];
    // We can check the emptiness of the system of bounded differences.
    // If the system of bounded differences is empty 
    // on the diagonal there is a negative value. 
    if (is_negative(xdbm_h_h)) 
      x.status.set_empty();
    // Fill the diagonal with plus_infinity.
    xdbm_h_h = PLUS_INFINITY;
  }
}

template <typename T>
inline void
BD_Shape<T>::transitive_reduction_assign() const {
  dimension_type space_dim = space_dimension();

  // We close for find tighter constraints.
  closure_assign();

  // If `*this' is empty, then this operation is not necessary.
  if (marked_empty())
    return;

  // The vector `leader' is used to indicate which variables are equivalent. 
  std::vector<dimension_type> leader(space_dim + 1);
  
  // We store the leader.
  for (dimension_type i = space_dim + 1; i-- > 0; )
    leader[i] = i;
    
  // Step 1: we store really the leader with the corrected value.
  // We search for the equivalent or zero-equivalent variables.
  // The variable(i-1) and variable(j-1) are equivalent if and only if 
  // m_i_j == -(m_j_i). 
  for (dimension_type i = 0; i < space_dim; ++i) {
    const DB_Row<T>& dbm_i = dbm[i];
    for (dimension_type j = i + 1; j <= space_dim; ++j) {
      T negated_dbm_ji;
      if (exact_neg(negated_dbm_ji, dbm[j][i]) &&
	  negated_dbm_ji == dbm_i[j])
	// When there are two equivalent variables, the greater one has
	// got the other variable as leader.
	leader[j] = i;
    }
  }

  // Step 2: we remove the redundant constraint in the zero_cycle 
  // free systems of bounded differences, considering only the leaders.
  // A constraint `c' is redundant when there are two constraints such that
  // their sum is the same constraint with the inhomogeneous term
  // less than or equal to the `c' one.
  T c;
  BD_Shape<T>& x = const_cast<BD_Shape<T>&>(*this);
  for (dimension_type k = 0; k <= space_dim; ++k) {
    if (leader[k] == k) {
      DB_Row<T>& x_k = x.dbm[k];
      for (dimension_type i = 0; i <= space_dim; ++i) { 
	if (leader[i] == i) {
	  DB_Row<T>& x_i = x.dbm[i];
	  T& x_i_k = x_i[k];
	  for (dimension_type j = 0; j <= space_dim; ++j) { 
	    if (leader[j] == j) {
	      add_round_up(c, x_i_k, x_k[j]);
	      T& x_i_j = x_i[j];
	      if (x_i_j >= c)
		x_i_j = PLUS_INFINITY;
	    }
	  }
	} 
      }
    }
  }
  x.status.reset_transitively_closed();

  // Step 3: we remove all the redundant constraints in 0-cycles.
  // Each equivalence class must have an only cycle that connected
  // all the equivalent variables.
  for (dimension_type i = 2; i <= space_dim; ++i) {
    dimension_type ld_i = leader[i];
    DB_Row<T>& x_i = x.dbm[i];
    if (ld_i == i) {
      // When we have a leader, we remove all constraints 
      // that this variable has got with the no-leaders variables 
      // less than it. This is possible because the smaller variables
      // don't belong to the same equivalence class. 
      for (dimension_type j = 0; j < i; ++j)
	if (leader[j] != j) {
	  x_i[j] = PLUS_INFINITY;
	  x.dbm[j][i] = PLUS_INFINITY;
	}
    }
    else {
      dimension_type ld_ld_i = leader[ld_i];
      if (ld_i != ld_ld_i) {
	leader[i] = ld_ld_i;
	x.dbm[ld_ld_i][ld_i] = PLUS_INFINITY;
      }
      for (dimension_type j = 0; j < i; ++j) {
	// We remove in the row all the constraints with all variables
	// except its leader.
	if (j != ld_i)
	  x_i[j] = PLUS_INFINITY;
        // We remove in the columns all the constraints with all variables
	// except the leader of its leader.
	if (j != ld_ld_i)
	  x.dbm[j][i] = PLUS_INFINITY;
      }
    }
  }
  
  assert(is_transitively_reduced());
}

template <typename T>
inline void
BD_Shape<T>::poly_hull_assign(const BD_Shape& y) {
  // Dimension-compatibility check.
  if (space_dimension() != y.space_dimension())
    throw_dimension_incompatible("poly_hull_assign(y)", y);

  // The poly-hull of a polyhedron `bd' with an empty polyhedron is `bd'.
  y.closure_assign();
  if (y.marked_empty())
    return;
  closure_assign();
  if (marked_empty()) {
    *this = y;
    return;
  }

  // The poly_hull consist to construct `*this' with the maximum
  // elements selected from `*this' or `y'.
  dimension_type k = space_dimension();
  assert(k == 0 || marked_transitively_closed());
  for (dimension_type i = 0; i <= k; ++i) {
    DB_Row<T>& dbm_i = dbm[i];
    const DB_Row<T>& y_dbm_i = y.dbm[i];
    for (dimension_type j = 0; j <= k; ++j) {
      T& dbm_ij = dbm_i[j];
      const T& y_dbm_ij = y_dbm_i[j];
      if (dbm_ij < y_dbm_ij) 
	dbm_ij = y_dbm_ij;
    }
  }
  // The result is still transitively closed.
  assert(OK());
}

template <typename T>
inline bool
BD_Shape<T>::poly_hull_assign_and_minimize(const BD_Shape& y) {
  poly_hull_assign(y);
  assert(marked_empty()
	 || space_dimension() == 0 || marked_transitively_closed()); 
  return !marked_empty();
}

template <typename T>
inline void
BD_Shape<T>::poly_difference_assign(const BD_Shape& y) {
  // Dimension-compatibility check.
  if (space_dimension() != y.space_dimension())
    throw_dimension_incompatible("poly_difference_assign(y)", y);
  
  BD_Shape new_bdiffs(space_dimension(), Polyhedron::EMPTY);
  
  BD_Shape& x = *this;
  
  x.closure_assign();
  // The difference of an empty system of bounded differences
  // and of a system of bounded differences `p' is empty.
  if (x.marked_empty())
    return;
  y.closure_assign();
  // The difference of a system of bounded differences `p' 
  // and an empty system of bounded differences is `p'.
  if (y.marked_empty())
    return;

  // If both systems of bounded differences are zero-dimensional,
  // then at this point they are necessarily universe system of bounded differences,
  // so that their difference is empty.
  if (x.space_dimension() == 0) {
    x.set_empty();
    return;
  }

  // TODO: This is just an executable specification.
  //       Have to find a more efficient method.
  if (y.contains(x)) {
    x.set_empty();
    return;
  }

  // We take a constraint of the system y at the time and we 
  // consider its complementary. Then we intersect the union
  // of these complementaries with the system x.
  const Constraint_System& y_cs = y.constraints();
  for (Constraint_System::const_iterator i = y_cs.begin(),
	 y_cs_end = y_cs.end(); i != y_cs_end; ++i) {
    BD_Shape z = x;
    const Constraint& c = *i;
    const Linear_Expression e = Linear_Expression(c);      
    bool change = false;
    if (c.is_nonstrict_inequality()) 
      change = z.add_constraint_and_minimize(e <= 0); 
    if (c.is_equality()) {
      BD_Shape w = x;
      if (w.add_constraint_and_minimize(e <= 0))
	new_bdiffs.poly_hull_assign(w);
      change = z.add_constraint_and_minimize(e >= 0); 
    }
    if (change)
      new_bdiffs.poly_hull_assign(z);
  }
  *this = new_bdiffs;
  // The result is still transitively closed, because both
  // poly_hull_assign() and add_constraint_and_minimize()
  // preserve the transitive closure.
  assert(OK());
}
  

template <typename T>
inline void
BD_Shape<T>::add_space_dimensions_and_embed(dimension_type m) {
  // Adding no dimensions to any system of bounded 
  // differences is no-op.
  if (m == 0)
    return;

  bool was_zero_dim_univ = (!marked_empty() && space_dimension() == 0);

  dimension_type h = dbm.num_rows();
  // To embed an n-dimension space system of bounded differences
  // in a (n+m)-dimension space, we just add `m' infinity-columns
  // and rows in the matrix of constraints.
  dbm.grow(h + m);
  // Fill top-right corner of the matrix with plus infinity. 
  for (dimension_type i = 0; i < h; ++i) {
    DB_Row<T>& dbm_i = dbm[i];
    for (dimension_type j = h; j < h + m; ++j)
      dbm_i[j] = PLUS_INFINITY;
  }
  // Fill bottom of the matrix with plus infinity.
  for (dimension_type i = h; i < h + m; ++i) {
    DB_Row<T>& dbm_i = dbm[i];
    for (dimension_type j = 0; j < h + m; ++j)
      dbm_i[j] = PLUS_INFINITY;
  }

  // If `*this' was zero-dim-universe, since a non zero-dim universe polyhedron
  // is always closed, we must set the flag.  
  if (was_zero_dim_univ) 
    status.set_transitively_closed(); 
  
  assert(OK());
}


template <typename T>
inline void
BD_Shape<T>::add_space_dimensions_and_project(dimension_type m) {
  // Adding no dimensions to any system of bounded 
  // differences is no-op.
  if (m == 0)
    return;

  dimension_type k = space_dimension();
  dimension_type h = dbm.num_rows();

  // If `*this' was zero-dimensional, then we simply add `m' zero-rows
  // and columns. If `*this' was also universal, then we fill diagonal
  // with plus_infinity and set the flag (a non zero-dim universe 
  // polyhedron is closed).
  if (k == 0) {
    dbm.grow(m + 1);
    if (!marked_empty()) {
      for (dimension_type i = 0; i <= m; ++i)
	dbm[i][i] = PLUS_INFINITY;
      status.set_transitively_closed();
    }
    assert(OK());
    return;
  }

  // To project an n-dimension space system of bounded differences
  // in a (n+m)-dimension space, we just add `m' infinity-columns
  // and rows in the matrix of constraints.
  // In the first row and column of the matrix we add `zero' from 
  // the h-position to the end.
  dbm.grow(h + m);

  // Fill top-right corner of the matrix with plus_infinity. 
  for (dimension_type i = 1; i < h; ++i){
    DB_Row<T>& dbm_i = dbm[i];
    for (dimension_type j = h; j < h + m; ++j)
      dbm_i[j] = PLUS_INFINITY;
  }
  // Bottom of the matrix and first row.
  for (dimension_type i = h; i < h + m; ++i) {
    DB_Row<T>& dbm_i = dbm[i];
    dbm_i[0] = 0;
    dbm[0][i] = 0;
    for (dimension_type j = 1; j < h + m; ++j) 
      dbm_i[j] = PLUS_INFINITY;
  }
  
  if (marked_transitively_closed())
    status.reset_transitively_closed();
  assert(OK());
}

template <typename T>
inline void
BD_Shape<T>::remove_space_dimensions(const Variables_Set& to_be_removed) {
  // The removal of no dimensions from any polyhedron is a no-op.
  // Note that this case also captures the only legal removal of
  // dimensions from a system of bounded differences in a 0-dim space.
  if (to_be_removed.empty()) {
    assert(OK());
    return;
  }

  // Dimension-compatibility check: the variable having
  // maximum cardinality is the one occurring last in the set.
  dimension_type max_dim_to_be_removed = to_be_removed.rbegin()->id();
  if (max_dim_to_be_removed >= space_dimension())
    throw_dimension_incompatible("remove_space_dimensions(vs)",
				 max_dim_to_be_removed);

  dimension_type new_space_dim = space_dimension() - to_be_removed.size();
  /* The closure is necessary.
     For example.
     In `*this' we have the constraints:

     x1 - x2 <= 1;
     x2 <= 2;
     x3 <= 3;

     We suppose to remove x2.
     The result is:

     x3 <= 3;

     But if we close before the removing we obtain:

     x1 <= 3;
     x3 <= 3;

     which is the right result.
     Without closure we lose the constraint

     x1 <= 3;
   */
  closure_assign();
  // When removing _all_ dimensions from a system of bounded differences,
  // we obtain the zero-dimensional system of bounded differences.
  if (new_space_dim == 0) {
    dbm.resize_no_copy(1);
    if (!marked_empty())
      // We set the zero_dim_univ flag.
      set_zero_dim_univ();
    assert(OK());
    return;
  }

  // For each variable to remove, we erase the corresponding column and
  // row by shifting the other columns and rows, than are not removed, 
  // respectively left and above.
  dimension_type old_space_dim = space_dimension();
  Variables_Set::const_iterator tbr = to_be_removed.begin();
  Variables_Set::const_iterator tbr_end = to_be_removed.end();
  dimension_type dst = tbr->id() + 1;
  dimension_type src = dst + 1;
  for (++tbr; tbr != tbr_end; ++tbr) {
    dimension_type tbr_next = tbr->id() + 1;
    // All other columns and rows are moved respectively to the left
    // and above.
    while (src < tbr_next) {
      dbm[dst] = dbm[src];
      for (dimension_type i = 0; i <= old_space_dim; ++i) {
	DB_Row<T>& dbm_i = dbm[i];
	dbm_i[dst] = dbm_i[src];
      }
      ++dst;
      ++src;
    }
    ++src;
  }

  // Moving the remaining rows and columns.
  while (src <= old_space_dim) {
    dbm[dst] = dbm[src];
    for (dimension_type i = 0; i <= old_space_dim; ++i) {
      DB_Row<T>& dbm_i = dbm[i];
      dbm_i[dst] = dbm_i[src];
    }
    ++src;
    ++dst;
  }

  // Update the space dimension.
  dbm.resize_no_copy(new_space_dim + 1);
  assert(OK());
}

template <typename T>
inline void
BD_Shape<T>::remove_higher_space_dimensions(dimension_type new_dimension) {
  // Dimension-compatibility check: the variable having
  // maximum cardinality is the one occurring last in the set.
  if (new_dimension > space_dimension())
    throw_dimension_incompatible("remove_higher_space_dimensions(nd)",
				 new_dimension);
  
  // The removal of no dimensions from any polyhedron is a no-op.
  // Note that this case also captures the only legal removal of
  // dimensions from a system of bounded differences in a 0-dim space.
  if(new_dimension == space_dimension()) {
    assert(OK());
    return;
  }

  // The closure is necessary as in remove_space_dimensions().
  closure_assign();
  dbm.resize_no_copy(new_dimension + 1);

  // When we remove _all_ dimensions from a not-empty polyhedron,
  // we obtain the zero-dimensional universe system of bounded differences.
  if (new_dimension == 0 && !marked_empty())
    set_zero_dim_univ();
  assert(OK());
}

template <typename T>
template <typename PartialFunction>
inline void
BD_Shape<T>::map_space_dimensions(const PartialFunction& pfunc) {
  dimension_type space_dim = space_dimension();
  // TODO: this implementation is just an executable specification.
  if (space_dim == 0)
    return;
  
  if (pfunc.has_empty_codomain()) {
    // All dimensions vanish: the system of bounded differences
    // becomes zero_dimensional.
    remove_higher_space_dimensions(0);
    assert(OK());
    return;
  }

  dimension_type new_space_dim = pfunc.max_in_codomain() + 1; 
  // If the new dimension of space is strict less than the old one, 
  // since we don't want to loose solutions, we must close. 
  // In fact, we have this system of bounded differences:
  // x - y <= 1;
  // y     <= 2.
  // and we have this function:
  // x --> x.
  // If we don't close, we loose the constraint: x <= 3.
  if (new_space_dim < space_dim) 
    closure_assign();

  // If we have got an empty system of bounded differences, 
  // then we must only adjust the dimension of the system of bounded differences,
  // but it must remain empty.
  if (marked_empty()) {
    remove_higher_space_dimensions(new_space_dim);
    return;
  }

  // We create a new matrix with the new space dimension.
  DB_Matrix<T> x(new_space_dim+1);
  // First of all we must map the unary constraints, because 
  // there is the fictitious variable `zero', that can't be mapped 
  // at all. 
  for (dimension_type i = new_space_dim + 1; i-- > 0; )
    x[i][i] = PLUS_INFINITY;
  DB_Row<T>& dbm_0 = dbm[0];
  DB_Row<T>& x_0 = x[0];
  for (dimension_type j = 1; j <= space_dim; ++j) {
    T& dbm_0_j = dbm_0[j];
    T& dbm_j_0 = dbm[j][0];
    dimension_type new_j;
    if (pfunc.maps(j - 1, new_j)) {
      x_0[new_j + 1] = dbm_0_j;
      x[new_j + 1][0] = dbm_j_0;
    }
  }
  // Now we map the binary constraints, exchanging the indexes.
  for (dimension_type i = 1; i <= space_dim; ++i) {
    dimension_type new_i;
    if (pfunc.maps(i - 1, new_i)) {
      DB_Row<T>& dbm_i = dbm[i];
      ++new_i;
      DB_Row<T>& x_new_i = x[new_i];
      for (dimension_type j = i+1; j <= space_dim; ++j) {
	dimension_type new_j;
	if (pfunc.maps(j - 1, new_j)) {
	  ++new_j;
	  x_new_i[new_j] = dbm_i[j];
	  x[new_j][new_i] = dbm[j][i];
	}
      }
    }
  } 
  
  std::swap(dbm, x);
  assert(OK());
}

template <typename T>
inline void
BD_Shape<T>::intersection_assign(const BD_Shape& y) {
  // Dimension-compatibility check.
  if (space_dimension() != y.space_dimension())
    throw_dimension_incompatible("intersection_assign(y)", y);

  // If one of the two systems of bounded differences is empty, 
  // the intersection is empty.
  if (marked_empty())
    return;
  if (y.marked_empty()) {
    set_empty();
    return;
  }

  dimension_type k = space_dimension();
  // If both systems of bounded differences are zero-dimensional,
  // then at this point they are necessarily non-empty,
  // so that their intersection is non-empty too.
  if (k == 0)
    return;

  // To intersect two systems of bounded differences we compare
  // the constraints and we choose the less values. 
  bool changed = false;
  for (dimension_type i = 0; i <= k; ++i) {
    DB_Row<T>& dbm_i = dbm[i];
    const DB_Row<T>& y_dbm_i = y.dbm[i];
    for (dimension_type j = 0; j <= k; ++j) {
      T& dbm_ij = dbm_i[j];
      const T& y_dbm_ij = y_dbm_i[j];
      if (dbm_ij > y_dbm_ij) {
	dbm_ij = y_dbm_ij;
	changed = true;
      }
    }
  }

  if (changed && marked_transitively_closed())
    status.reset_transitively_closed();
  assert(OK());
}

template <typename T>
inline bool
BD_Shape<T>::intersection_assign_and_minimize(const BD_Shape& y) {
  intersection_assign(y);
  closure_assign();
  return !marked_empty();
}

template <typename T>
template <typename Iterator>
inline void
BD_Shape<T>::CC76_extrapolation_assign(const BD_Shape& y,
				     Iterator first, Iterator last) {
  // Dimension-compatibility check.
  if (space_dimension() != y.space_dimension())
    throw_dimension_incompatible("CC76_extrapolation_assign(y)", y);

#ifndef NDEBUG
  {
    // We assume that `y' is contained in or equal to `*this'.
    const BD_Shape x_copy = *this;
    const BD_Shape y_copy = y;
    assert(x_copy.contains(y_copy));
  }
#endif

  dimension_type k = space_dimension();
  // If both systems of bounded differences are zero-dimensional, 
  // since `*this' contains `y', we simply return `*this'.
  if (k == 0)
    return;

  closure_assign();
  // If `*this' is empty, since `*this' contains `y', `y' is empty too.
  if (marked_empty()) 
    return;
  y.closure_assign();
  // If `y' is empty, we return.
  if (y.marked_empty())
    return;

  /* The closure is necessary.
     For example.
     In `*this' there are the constraints:
     x1 - x2 <= 1;
     x2      <= 2; 
     x1      <= 4;   (with the closure x1 <= 3).

     In `y' there are the constraints:
     x1 - x2 <= 1;
     x2      <= 2;
     x1      <= 3.

     Without closure of `*this' the result is:
     x1 - x2 <= 1;
     x2      <= 2.

     With closure the result has also the constraint:
     x1      <= 3.

     We must close `y' too. In fact, if we have for `*this':
     x1      <= 4
    
     and for `y':
     x2      <= 2;
     x1 - x2 <= 1;
     x1      <= 4; (with the closure x1 <= 3).

     The result without the closure is the same  `*this', instead of universe. 
  */
  // We compare a constraint of `y' at the time to the corresponding
  // constraint of `*this'. If the value of constraint of `y' is 
  // less than of `*this' one, we further compare the constraint of
  // `*this' to elements in a sorted container, given by the user, 
  // and, if in the container there is a value that is greater than
  // or equal to the value of the constraint, we take this value,
  // otherwise we remove this constraint.
  for (dimension_type i = 0; i <= k; ++i) {
    DB_Row<T>& dbm_i = dbm[i];
    const DB_Row<T>& y_dbm_i = y.dbm[i];
    for (dimension_type j = 0; j <= k; ++j) {
      T& dbm_ij = dbm_i[j];
      const T& y_dbm_ij = y_dbm_i[j];
      if (y_dbm_ij < dbm_ij) {
	Iterator k = std::lower_bound(first, last, dbm_ij);
	if (k != last) {
	  if (dbm_ij < *k)
	    dbm_ij = *k;
	}
	else
	  dbm_ij = PLUS_INFINITY;
      }
    }
  }
  status.reset_transitively_closed();
  assert(OK());
}

template <typename T>
T BD_Shape<T>::default_stop_points[] = { T(-2), T(-1), T(0), T(1), T(2) };

template <typename T>
inline void
BD_Shape<T>::CC76_extrapolation_assign(const BD_Shape& y) {
  assert(OK());
  assert(y.OK());
  // Recalled the CC76_extrapolation_assign(const BD_Shape& y,
  //                                        Iterator first, Iterator last)
  // where the container is given by default.
  CC76_extrapolation_assign
    (y,
     default_stop_points,
     default_stop_points
     + sizeof(default_stop_points)/sizeof(default_stop_points[0]));
}

template <typename T>
inline void
BD_Shape<T>::limited_CC76_extrapolation_assign(const BD_Shape& y,
					     const Constraint_System& cs,
					     unsigned* /*tp*/) {
  dimension_type space_dim = space_dimension(); 
  // Not strict-inequality check.
  Constraint_System::const_iterator iend = cs.end();
  for (Constraint_System::const_iterator i = cs.begin(); i != iend; ++i) 
    if ((*i).is_strict_inequality()) 
      throw_constraint_incompatible("limited_CC76_extrapolation_assign(y, cs)");
    
  // Dimension-compatibility check.
  if (space_dim != y.space_dimension())
    throw_dimension_incompatible("limited_CC76_extrapolation_assign(y, cs)",
				 y);
  // `cs' must be dimension-compatible with the two systems 
  // of bounded differences.
  const dimension_type cs_space_dim = cs.space_dimension();
  if (space_dim < cs_space_dim)
    throw_constraint_incompatible("limited_CC76_extrapolation_assign(y, cs)");
  
  // The limited CC76-extrapolation between two systems of bounded differences in a
  // zero-dimensional space is a system of bounded differences in a zero-dimensional
  // space, too.
  if (space_dim == 0)
    return;
  
#ifndef NDEBUG
  {
    // We assume that `y' is contained in or equal to `*this'.
    const BD_Shape x_copy = *this;
    const BD_Shape y_copy = y;
    assert(x_copy.contains(y_copy));
  }
#endif

  // If `*this' is empty, since `*this' contains `y', `y' is empty too.
  if (marked_empty()) 
    return;
  // If `y' is empty, we return.
  if (y.marked_empty())
    return;

  T d;
  T d1;
  Constraint_System add_cons;
  for (Constraint_System::const_iterator i = cs.begin(); i != iend; ++i) {
    const Constraint& c = *i; 
      
    // Store the indices of the non-zero components of `c',
    dimension_type j[2] = { 0, 0 };
      
    // Number of non-zero components of `c'.
    dimension_type t = 0;

    // Controlled if the constraint must be to add.
    bool right_cons = true;
      
    // Collect the non-zero components of `c'.
    for (dimension_type i = cs_space_dim; i-- > 0; )
      if (c.coefficient(Variable(i)) != 0) {
	if (t >= 2) { 
	  // Constraints that are not "bounded differences" are ignored.
	  right_cons = false;
	  break;
	}
	else
	  j[t++] = i;
      }
    // Constraints that are not "bounded differences" are ignored.  
    if (right_cons && t == 2)  
      if (c.coefficient(Variable(j[1])) != -c.coefficient(Variable(j[0])))
	  right_cons = false;
      
    // We will now make sure `c' has one of the following forms:
    //
    //           0 <=/= b, if t == 0;
    //   a*x       <=/= b, if t == 1;
    //   a*x - a*y <=/= b, if t == 2.
    //
    // In addition, j[0] and (if t >= 1) j[1] will contain the indices
    // of the cell(s) of `dbm' to be modified.
    if (right_cons && t != 0) {    
      Coefficient a;
      Coefficient b = c.inhomogeneous_term();
      switch (t) {
      case 2:
	a = c.coefficient(Variable(j[1]));
	++j[1];
	++j[0];
	break;
	
      case 1:
	a = -c.coefficient(Variable(j[0]));
	++j[0];
	break;
      }
      // Select the cell to be modified for the "<=" part of the constraint,
      // and set `a' to the absolute value of itself.
      T& dbm_j_0_j_1 = dbm[j[0]][j[1]];
      T& dbm_j_1_j_0 = dbm[j[1]][j[0]];
      T* dbm_cellp;
      if (a < 0) {
	dbm_cellp = &dbm_j_0_j_1;
	a = -a;
      }
      else
	dbm_cellp = &dbm_j_1_j_0;
      
      // Compute b/a into `d', rounding the result towards plus infinity.
      div_round_up(d, b, a);
      if (*dbm_cellp <= d) {
	if (c.is_inequality()) 
	  add_cons.insert(c);
	else {
	  T* dbm_cellp1;
	  // The symmetric cell is the one to be possibly modified.
	  dbm_cellp1 = (dbm_cellp == &dbm_j_0_j_1) ? &dbm_j_1_j_0 : &dbm_j_0_j_1;
	  // Compute -b/a into `d', rounding the result towards plus infinity.
	  div_round_up(d1, -b, a);
	  if (*dbm_cellp1 <= d1)
	    add_cons.insert(c);
	}
      }
    }
  }
  CC76_extrapolation_assign(y);
  add_constraints(add_cons);
  assert(OK());
}

template <typename T>
inline void
BD_Shape<T>::CH78_widening_assign(const BD_Shape& y) {
  // Dimension-compatibility check.
  if (space_dimension() != y.space_dimension())
    throw_dimension_incompatible("CH78_widening_assign(y)", y);

#ifndef NDEBUG
  {
    // We assume that `y' is contained in or equal to `*this'.
    const BD_Shape x_copy = *this;
    const BD_Shape y_copy = y;
    assert(x_copy.contains(y_copy));
  }
#endif

  dimension_type k = space_dimension();
  // If both systems of bounded differences are zero-dimensional, 
  // since `*this' contains `y', we simply return `*this'.
  if (k == 0)
    return;
  
  closure_assign();
  // If `*this' is empty, since `*this' contains `y', `y' is empty too.
  if (marked_empty())
    return;

  // Minimize `y'.
  y.transitive_reduction_assign();
  // If `y' is empty, we return.
  if (y.marked_empty())
    return;

  // Extrapolate unstable bounds.
  for (dimension_type i = 0; i <= k; ++i) {
    DB_Row<T>& dbm_i = dbm[i];
    const DB_Row<T>& y_dbm_i = y.dbm[i];
    for (dimension_type j = 0; j <= k; ++j) {
      T& dbm_ij = dbm_i[j];
      const T& y_dbm_ij = y_dbm_i[j];
      // Note: in the following line the use of `!=' (as opposed to
      // the use of `<' that would seem -but is not- equivalent) is
      // intentional.
      if (y_dbm_ij != dbm_ij) 
	dbm_ij = PLUS_INFINITY;
    }
  }
  status.reset_transitively_closed();
  assert(OK());
}

template <typename T>
inline void
BD_Shape<T>::limited_CH78_extrapolation_assign(const BD_Shape& y,
					     const Constraint_System& cs,
					     unsigned* /*tp*/) {
  dimension_type space_dim = space_dimension();
  // Not strict-inequality check. 
  Constraint_System::const_iterator iend = cs.end();
  for (Constraint_System::const_iterator i = cs.begin(); i != iend; ++i) 
    if ((*i).is_strict_inequality()) 
      throw_constraint_incompatible("limited_CH78_extrapolation_assign(y, cs)");
    
  // Dimension-compatibility check.
  if (space_dim != y.space_dimension())
    throw_dimension_incompatible("limited_CH78_extrapolation_assign(y, cs)",
				 y);
  // `cs' must be dimension-compatible with the two systems 
  // of bounded differences.
  const dimension_type cs_space_dim = cs.space_dimension();
  if (space_dim < cs_space_dim)
    throw_constraint_incompatible("limited_CH78_extrapolation_assign(y, cs)");
  
  // The limited CH78-extrapolation between two systems of bounded 
  // differences in a zero-dimensional space is a system of bounded 
  // differences in a zero-dimensional space, too.
  if (space_dim == 0)
    return;
  
#ifndef NDEBUG
  {
    // We assume that `y' is contained in or equal to `*this'.
    const BD_Shape x_copy = *this;
    const BD_Shape y_copy = y;
    assert(x_copy.contains(y_copy));
  }
#endif

  // If `*this' is empty, since `*this' contains `y', `y' is empty too.
  if (marked_empty()) 
    return;
  // If `y' is empty, we return.
  if (y.marked_empty())
    return;

  T d;
  T d1;
  Constraint_System add_cons;
  for (Constraint_System::const_iterator i = cs.begin(); i != iend; ++i) {
    const Constraint& c = *i; 
      
    // Store the indices of the non-zero components of `c',
    dimension_type j[2] = { 0, 0 };
      
    // Number of non-zero components of `c'.
    dimension_type t = 0;

    // Controlled if the constraint must be to add.
    bool right_cons = true;
      
    // Collect the non-zero components of `c'.
    for (dimension_type i = cs_space_dim; i-- > 0; )
      if (c.coefficient(Variable(i)) != 0) {
	if (t >= 2) { 
	  // Constraints that are not "bounded differences" are ignored.
	  right_cons = false;
	  break;
	}
	else
	  j[t++] = i;
      }
    // Constraints that are not "bounded differences" are ignored.  
    if (right_cons && t == 2)  
      if (c.coefficient(Variable(j[1])) != -c.coefficient(Variable(j[0])))
	  right_cons = false;
      
    // We will now make sure `c' has one of the following forms:
    //
    //           0 <=/= b, if t == 0;
    //   a*x       <=/= b, if t == 1;
    //   a*x - a*y <=/= b, if t == 2.
    //
    // In addition, j[0] and (if t >= 1) j[1] will contain the indices
    // of the cell(s) of `dbm' to be modified.
    if (right_cons && t != 0) {    
      Coefficient a;
      Coefficient b = c.inhomogeneous_term();
      switch (t) {
      case 2:
	a = c.coefficient(Variable(j[1]));
	++j[1];
	++j[0];
	break;
	
      case 1:
	a = -c.coefficient(Variable(j[0]));
	++j[0];
	break;
      }
      // Select the cell to be modified for the "<=" part of the constraint,
      // and set `a' to the absolute value of itself.
      T& dbm_j_0_j_1 = dbm[j[0]][j[1]];
      T& dbm_j_1_j_0 = dbm[j[1]][j[0]];
      T* dbm_cellp;
      if (a < 0) {
	dbm_cellp = &dbm_j_0_j_1;
	a = -a;
      }
      else
	dbm_cellp = &dbm_j_1_j_0;
      
      // Compute b/a into `d', rounding the result towards plus infinity.
      div_round_up(d, b, a);
      if (*dbm_cellp <= d) {
	if (c.is_inequality()) 
	  add_cons.insert(c);
	else {
	  T* dbm_cellp1;
	  // The symmetric cell is the one to be possibly modified.
	  dbm_cellp1 = (dbm_cellp == &dbm_j_0_j_1) ? &dbm_j_1_j_0 : &dbm_j_0_j_1;
	  // Compute -b/a into `d', rounding the result towards plus infinity.
	  div_round_up(d1, -b, a);
	  if (*dbm_cellp1 <= d1)
	    add_cons.insert(c);
	}
      }
    }
  }
  CH78_widening_assign(y);
  add_constraints(add_cons);
  assert(OK());
}

  
template <typename T>
inline void
BD_Shape<T>::CC76_narrowing_assign(const BD_Shape& y) {
  // Dimension-compatibility check.
  if (space_dimension() != y.space_dimension())
    throw_dimension_incompatible("CC76_narrowing_assign(y)", y);

#ifndef NDEBUG
  {
    // We assume that `y' is contained in or equal to `*this'.
    const BD_Shape x_copy = *this;
    const BD_Shape y_copy = y;
    assert(x_copy.contains(y_copy));
  }
#endif

  dimension_type k = space_dimension();
  // If both systems of bounded differences are zero-dimensional, 
  // since `*this' contains `y', we simply return `*this'.
  if (k == 0)
    return;
   
  closure_assign();
  // If `*this' is empty, since `*this' contains `y', `y' is empty too.
  if (marked_empty()) 
    return;
  y.closure_assign();
  // If `y' is empty, we return.
  if (y.marked_empty())
    return;

  /* The closure is necessary.
     For example.
     In `*this' there are the constraints:

     x1 - x2 <= 2;
   
     In y there are the constraints:

     x1      <= 2;
     x1 - x2 <= 1;
     x2      <= 0   

     If we don't close y the result is:

     x1      <= 2;
     x1 - x2 <= 2;
     x2      <= 0.

     If we before close y, the result is:

     x1 - x2 <= 2;
     x2 <= 0;
     x1 <= 1;

     So we observe that
     x1 <= 1
     is more tight than
     x1 <= 2
     
     We must close `*this' too, in fact, if we have these constraint for *this:
     x1 - x2 <= 1;
     x2      <= 5    
     (and the implicit constraint x1 <= 6)

     and for y we have:
     x1      <= 1;
     x1 - x2 <= 1;
     x2      <= 0;

     we obtain:
     x1      <= 1;
     x1 - x2 <= 1;
     x2      <= 5;
    
     instead of: 
     x1      <= 6;
     x1 - x2 <= 1;
     x2      <= 5;
   */
  // We consider a constraint of `*this' at the time, if its value is 
  // `plus_infinity', we replace it with the value the corresponding 
  // constraint of `y'. 
  bool changed = false;
  for (dimension_type i = 0; i <= k; ++i) {
    DB_Row<T>& dbm_i = dbm[i];
    const DB_Row<T>& y_dbm_i = y.dbm[i];
    for (dimension_type j = 0; j <= k; ++j) {
      T& dbm_ij = dbm_i[j];
      const T& y_dbm_ij = y_dbm_i[j];
      if (is_plus_infinity(dbm_ij)) {
	dbm_ij = y_dbm_ij;
	changed = true;
      }
    }
  }
  if (changed && marked_transitively_closed())
    status.reset_transitively_closed();
  assert(OK());
}

template <typename T>
inline void
BD_Shape<T>::H79_widening_assign(const BD_Shape& y) {
  // Seen the polyhedra documentation.
  C_Polyhedron px(constraints());
  C_Polyhedron py(y.constraints());
  px.H79_widening_assign(py);
  BD_Shape x(px.constraints());
  swap(x);
  assert(OK());
}

template <typename T>
inline void
BD_Shape<T>::limited_H79_extrapolation_assign(const BD_Shape& y,
					    const Constraint_System& cs,
					    unsigned* tp) {
  // Seen the polyhedra documentation.
  C_Polyhedron px(constraints());
  C_Polyhedron py(y.constraints());
  px.limited_H79_extrapolation_assign(py, cs, tp);
  BD_Shape x(px.constraints());
  swap(x);
  assert(OK());
}

template <typename T>
inline void
BD_Shape<T>::affine_image(const Variable var,
			const Linear_Expression& expr,
			const Coefficient& denominator) {

  // The denominator cannot be zero.
  if (denominator == 0)
    throw_generic("affine_image(v, e, d)", "d == 0");

  // Dimension-compatibility checks.
  // The dimension of `expr' should not be greater than the dimension
  // of `*this'.
  dimension_type space_dim = space_dimension();
  dimension_type expr_space_dim = expr.space_dimension();
  if (space_dim < expr_space_dim)
    throw_dimension_incompatible("affine_image(v, e, d)", "e", expr);
  
  // `var' should be one of the dimensions of the systems of bounded differences.
  dimension_type num_var = var.id() + 1;
  if (num_var > space_dim)
    throw_dimension_incompatible("affine_image(v, e, d)", var.id());
  
  // If `*this' is empty, then its image is empty.
  if (marked_empty())
    return;
  
  // Index of the non-zero component of `expr'.
  dimension_type j = 0;
  
  // Number of non-zero components of `expr'.
  dimension_type t = 0;
  
  // Compute the number of the non-zero components of `expr'.
  // The `expr' must not be in two or plus variables.
  for (dimension_type i = expr_space_dim; i-- > 0; )
    if (expr.coefficient(Variable(i)) != 0) {
      if (t >= 1)
        throw_expression_too_complex("affine_image(v, e, d)", expr);
      else {
	++t;
	j = i;
      }
    }
  
  // Now we have got a form of `expr':
  // if t == 0, expr = n, with n integer.
  // if t == 1, expr = a*z + n, where z can be `var' or another variable.
  // Attention: in the second case the coefficient of variable must
  // equal to denominator.
  Coefficient b = expr.inhomogeneous_term();
  if (t == 0) {
    closure_assign();
    // If `*this' is empty, then its image is empty.
    if (marked_empty())
      return;
    else {
      // Case 1: expr = n.
      // We lose(remove) all constraints on `var' and we add the new constraint
      // `var = n/denominator'.
      for (dimension_type i = 0; i <= space_dim; ++i) {
	dbm[num_var][i] = PLUS_INFINITY;
	dbm[i][num_var] = PLUS_INFINITY;
      }
      add_constraint(denominator*var == b);
    }
  }
  else {
    // Case 2: expr = a*z + n. 
    closure_assign();
    // If `*this' is empty, then its image is empty.
    if (marked_empty())
      return;
    else {
      Coefficient a = expr.coefficient(Variable(j));
      if (a != denominator) 
	throw_expression_too_complex("affine_image(v, e, d)", expr);
      else {
	// We have got an expression of the following form: var + n.
	if (j == num_var - 1) {
	  // If b = 0, then the image is an identity of `*this'.
	  if (b == 0)
	    return;
	  else {
	    // We translate all the constraints on `var' adding or
	    // subtracting the value `n/denominator'.
	    T d;
	    div_round_up(d, b, denominator);
	    T c;
	    div_round_up(c, -b, denominator);
	    for (dimension_type i = 0; i <= space_dim; ++i) {
	      T& dbm_v_i = dbm[num_var][i];
	      T& dbm_i_v = dbm[i][num_var];
	      add_round_up(dbm_v_i, dbm_v_i, c);
	      add_round_up(dbm_i_v, dbm_i_v, d);
	    }
	  }
	}
	else {
	  // We have got an expression of the following form: 
	  // var1 + n, with `var1' != `var'.
	  // We lose(remove) all constraints on `var' and we add the new 
          // constraint `var - var1 = n/denominator'.
	  for (dimension_type i = 0; i <= space_dim; ++i) {
	    dbm[num_var][i] = PLUS_INFINITY;
	    dbm[i][num_var] = PLUS_INFINITY;
	  }
	  add_constraint(denominator*var - denominator*Variable(j) == b); 
	}
      }
    }
  }
  assert(OK());
}

template <typename T>
inline void
BD_Shape<T>::affine_preimage(const Variable var,
			   const Linear_Expression& expr,
			   const Coefficient& denominator) {

  // The denominator cannot be zero.
  if (denominator == 0)
    throw_generic("affine_preimage(v, e, d)", "d == 0");

  // Dimension-compatibility checks.
  // The dimension of `expr' should not be greater than the dimension
  // of `*this'.
  const dimension_type space_dim = space_dimension();
  const dimension_type expr_space_dim = expr.space_dimension();
  if (space_dim < expr_space_dim)
    throw_dimension_incompatible("affine_preimage(v, e, d)", "e", expr);

  // `var' should be one of the dimensions of 
  // the systems of bounded differences.
  const dimension_type num_var = var.id() + 1;
  if (num_var > space_dim)
    throw_dimension_incompatible("affine_preimage(v, e, d)", var.id());
  
   // If `*this' is empty, then its image is empty.
  if (marked_empty())
    return;

  // Index of the non-zero component of `expr'.
  dimension_type j = 0;
  
  // Number of non-zero components of `expr'.
  dimension_type t = 0;
  
  // Compute the number of the non-zero components of `expr'.
  // The `expr' must not be in two or plus variables.
  for (dimension_type i = expr_space_dim; i-- > 0; )
    if (expr.coefficient(Variable(i)) != 0) {
      if (t >= 1)
        throw_expression_too_complex("affine_preimage(v, e, d)", expr);
      else {
	++t;
        j = i;
      }
    }
  
  // Now we have got a form of `expr':
  // if t == 0, expr = n, with n integer.
  // if t == 1, expr = a*z + n, where z can be `var' or another variable.
  // Attention: in the second case the coefficient of variable must
  // equal to denominator.
  Coefficient b = expr.inhomogeneous_term();
  DB_Row<T>& dbm_nv = dbm[num_var];
  if (t == 0) {
    closure_assign();
    // If `*this' is empty, then its image is empty.
    if (marked_empty())
      return;
    else {
      // Case 1: expr = n.
      // We remove all constraints on `var'. 
      for (dimension_type i = 0; i <= space_dim; ++i) {
	dbm_nv[i] = PLUS_INFINITY;
	dbm[i][num_var] = PLUS_INFINITY;
      }
    }
  }
  else {
    // Case 2: expr = a*z + n.
    closure_assign();
    // If `*this' is empty, then its image is empty.
    if (marked_empty())
      return;
    else {
      Coefficient a = expr.coefficient(Variable(j));
      if (a != denominator) 
	throw_expression_too_complex("affine_preimage(v, e, d)", expr);
      else {
        // We have got an expression of the following form: var + n.
	if (j == num_var - 1) {
          // If b = 0, then the image is an identity of `*this'.
	  if (b == 0)
	    return;
	  else
	    // We recall the affine_image to invert the transformation.
	    affine_image(var, a*var - b, denominator);
	}
	else {
	  // We have got an expression of the following form: 
	  // var1 + n, with `var1' != `var'.
	  // We remove all constraints on `var'.
	  for (dimension_type i = 0; i <= space_dim; ++i) {
	    dbm_nv[i] = PLUS_INFINITY;
	    dbm[i][num_var] = PLUS_INFINITY;
	  }
	}
      }
    }
  }
  assert(OK());
}

template <typename T>
inline void 
BD_Shape<T>::generalized_affine_image(Variable var,
				    const Relation_Symbol relsym,
				    const Linear_Expression& expr,
				    const Coefficient& denominator) {

  // The denominator cannot be zero.
  if (denominator == 0)
    throw_generic("generalized_affine_image(v, r, e, d)", "d == 0");

  // Dimension-compatibility checks.
  // The dimension of `expr' should not be greater than the dimension
  // of `*this'.
  dimension_type space_dim = space_dimension();
  dimension_type expr_space_dim = expr.space_dimension();
  if (space_dim < expr_space_dim)
    throw_dimension_incompatible("generalized_affine_image(v, r, e, d)", "e", expr);
  
  // `var' should be one of the dimensions of the system of bounded differences.
  dimension_type num_var = var.id() + 1;
  if (num_var > space_dim)
    throw_dimension_incompatible("generalized_affine_image(v, r, e, d)", var.id());
  
  // The relation symbol cannot be a strict relation symbol.
  if (relsym == LESS_THAN || relsym == GREATER_THAN)
    throw_generic("generalized_affine_image(v, r, e, d)",
  		  "r is a strict relation symbol and "
  		  "*this is a BD_Shape");

  // Index of the non-zero component of `expr'.
  dimension_type j = 0;
  
  // Number of non-zero components of `expr'.
  dimension_type t = 0;
  
  // Compute the number of the non-zero components of `expr'.
  // The `expr' must not be in two or plus variables.
  for (dimension_type i = expr_space_dim; i-- > 0; )
    if (expr.coefficient(Variable(i)) != 0) {
      if (t >= 1)
        throw_expression_too_complex("generalized_affine_image(v, r, e, d)",
				     expr);
      else {
	++t;
	j = i;
      }
    }

  // We need the closure.
  closure_assign();
  // If `*this' is empty, then its image is empty.
  if (marked_empty())
    return;

  // Now we have got a form of `expr':
  // if t == 0, expr = n, with n integer.
  // if t == 1, expr = a*z + n, where z can be `var' or another variable.
  // Attention: in the second case the coefficient of variable must
  // equal to denominator.
  DB_Row<T>& n_v = dbm[num_var]; 
  Coefficient a = expr.coefficient(Variable(j));
  Coefficient b = expr.inhomogeneous_term();
  if (t == 0) { 
    // Case 1: expr = n.
    switch (relsym) {
    case LESS_THAN_OR_EQUAL:
      // We lose(remove) all constraints of the  form `var (- var1) <= const '
      // and we add the new constraint `var <= n/denominator'.
	for (dimension_type i = 0; i <= space_dim; ++i) {
	  dbm[i][num_var] = PLUS_INFINITY;
	}
	add_constraint(denominator*var <= b);
	break;
    case EQUAL:
      // The relation symbol is "==":
      // this is just an affine image computation.
      affine_image(var, expr, denominator);
      break;
    case GREATER_THAN_OR_EQUAL:
      // We lose(remove) all constraints of the  form `var (- var1) >= const '
      // and we add the new constraint `var >= n/denominator'.
      for (dimension_type i = 0; i <= space_dim; ++i) 
	dbm[num_var][i] = PLUS_INFINITY;
	add_constraint(denominator*var >= b);
      break;
    default:
      // We already dealt with the case of a strict relation symbol.
      throw std::runtime_error("PPL internal error");
      break;
    }
  }
  else {
    // Case 2: expr = a*z + n.
    switch (relsym) {
    case LESS_THAN_OR_EQUAL:
      if (a != denominator) 
	throw_expression_too_complex("generalized_affine_image(v, r, e, d)",
				     expr);
      else {
	// We have got an expression of the following form: var + n.
	if (j == num_var - 1) {
	  if (b == 0)
	    return;
	  else {
	    // We translate all the constraints of the form `var (- var1) <= const' 
	    // adding the value `n/denominator'.
	    T d;
	    div_round_up(d, b, denominator);
	    for (dimension_type i = 0; i <= space_dim; ++i) {
	      T& dbm_i_v = dbm[i][num_var];
	      add_round_up(dbm_i_v, dbm_i_v, d);
	    }
	  }
	}
	else {
	  // We have got an expression of the following form: 
	  // var1 + n, with `var1' != `var'.
	  // We lose(remove) all constraints of the form `var (- var1) <= const' 
	  // and we add the new constraint `var - var1 <= n/denominator'.
	  for (dimension_type i = 0; i <= space_dim; ++i) 
	    dbm[i][num_var] = PLUS_INFINITY;
	  add_constraint(denominator*var - denominator*Variable(j) <= b); 
	}
      }
      break;
    case EQUAL:
      // The relation symbol is "==":
      // this is just an affine image computation.
      affine_image(var, expr, denominator);
      break;
    case GREATER_THAN_OR_EQUAL:
      if (a != denominator) 
	throw_expression_too_complex("generalized_affine_image(v, r, e, d)",
				     expr);
      else {
	// We have got an expression of the following form: var + n.
	if (j == num_var - 1) {
	  if (b == 0)
	    return;
	  else {
	    // We translate all the constraints of the form 
            // `var (- var1) >= const' subtracting the value `n/denominator'.
	    T c;
	    div_round_up(c, -b, denominator);
	    for (dimension_type i = 0; i <= space_dim; ++i) {
	      T& dbm_v_i = n_v[i];
	      add_round_up(dbm_v_i, dbm_v_i, c);
	    }
	  }
	}
	else {
	  // We have got an expression of the following form: 
	  // var1 + n, with `var1' != `var'.
	  // We lose(remove) all constraints of the form `var (- var1) >= const' 
	  // and we add the new constraint `var - var1 >= n/denominator'.
	  for (dimension_type i = 0; i <= space_dim; ++i) 
	    n_v[i] = PLUS_INFINITY;
	  add_constraint(denominator*var - denominator*Variable(j) >= b); 
	}
      }
      break;
    default:
      // We already dealt with the case of a strict relation symbol.
      throw std::runtime_error("PPL internal error");
      break;
    }
  }
  assert(OK());
}
 
template <typename T>
inline void 
BD_Shape<T>::generalized_affine_image(const Linear_Expression& lhs,
				     const Relation_Symbol relsym,
				     const Linear_Expression& rhs) {

  // Dimension-compatibility checks.
  // The dimension of `lhs' should not be greater than the dimension
  // of `*this'.
  dimension_type space_dim = space_dimension();
  dimension_type lhs_space_dim = lhs.space_dimension();
  if (space_dim < lhs_space_dim)
    throw_dimension_incompatible("generalized_affine_image(e1, r, e2)",
				 "e1", lhs);
  // The dimension of `rhs' should not be greater than the dimension
  // of `*this'.
  const dimension_type rhs_space_dim = rhs.space_dimension();
  if (space_dim < rhs_space_dim)
    throw_dimension_incompatible("generalized_affine_image(e1, r, e2)",
				 "e2", rhs);
  // Strict relation symbols are not admitted for system of bounded differences.
  if (relsym == LESS_THAN || relsym == GREATER_THAN)
    throw_generic("generalized_affine_image(e1, r, e2)",
		  "r is a strict relation symbol and "
		  "*this is a BD_Shape");

  // We need the closure.
  closure_assign();
  // Any image of an empty system of bounded differences is empty.
  if (marked_empty())
    return;

  // Number of non-zero components of `lhs'.
  dimension_type t = 0;
  dimension_type j = 0;
  
  // Compute the number of the non-zero components of `lhs'.
  for (dimension_type i = lhs_space_dim; i-- > 0; )
    if (lhs.coefficient(Variable(i)) != 0) {
	++t;
	j = i;
    }

 // Number of non-zero components of `rhs'.
  dimension_type t1 = 0;
  dimension_type j1 = 0;  

  // Compute the number of the non-zero components of `rhs'.
  for (dimension_type i = rhs_space_dim; i-- > 0; )
    if (rhs.coefficient(Variable(i)) != 0) {
	++t1;
	j1 = i;
    }

  // TODO: we want all the possible linear expressions without
  // exceptions.
  if (t == 1) {    
    // Case 1: lhs = var + n.
    Coefficient d = lhs.coefficient(Variable(j));
    // We compute a new expression `e = rhs - n', so we recall 
    // BD_Shape<T>::generalized_affine_image(Variable var,
    //				           const Relation_Symbol relsym,
    //				           const Linear_Expression& e,
    //			                   const Coefficient& d).
    Linear_Expression e = rhs - lhs.inhomogeneous_term();
    // If `d < 0', then we use inverted relation symbol.
    if (d < 0) {
      d = -d;
      if (relsym == GREATER_THAN_OR_EQUAL)
	generalized_affine_image(Variable(j), LESS_THAN_OR_EQUAL, -e, d); 
      else if (relsym == LESS_THAN_OR_EQUAL)
	generalized_affine_image(Variable(j), GREATER_THAN_OR_EQUAL, -e, d);
      else
	generalized_affine_image(Variable(j), relsym, -e, d); 
    }
    else
      generalized_affine_image(Variable(j), relsym, e, d);
  }
  else if (t1 == 1) {
    // Case 2: rhs = var + n.
    // We compute a new expression `e = lhs - n', so we recall 
    // BD_Shape<T>::generalized_affine_image(Variable var,
    //				           const Relation_Symbol relsym,
    //				           const Linear_Expression& e,
    //			                   const Coefficient& denominator).
   Coefficient d = rhs.coefficient(Variable(j1));
   Linear_Expression e = lhs - rhs.inhomogeneous_term();
   // If `d > 0', then we use inverted relation symbol.
   if (d < 0) {
     d = -d;
     generalized_affine_image(Variable(j1), relsym, -e, d);
   }
   else {
     if (relsym == GREATER_THAN_OR_EQUAL)
       generalized_affine_image(Variable(j1), LESS_THAN_OR_EQUAL, e, d); 
     else if (relsym == LESS_THAN_OR_EQUAL)
       generalized_affine_image(Variable(j1), GREATER_THAN_OR_EQUAL, e, d);
     else
       generalized_affine_image(Variable(j1), relsym, e, d);           
   }
  }
  else
    
    // `lhs' and `rhs' are not acceptable if they has two or
    // plus variables or if they are both constants.
    throw_generic("generalized_affine_image(e1, r, e2)", 
		  "The expressions are not compatible!");

  assert(OK()); 
}

template <typename T>
inline void 
BD_Shape<T>::time_elapse_assign(const BD_Shape& y) {
  // Dimension-compatibility check.
  if (space_dimension() != y.space_dimension())
    throw_dimension_incompatible("time_elapse_assign(y)", y);
  // Seen the polyhedra documentation.
  C_Polyhedron px(constraints());
  C_Polyhedron py(y.constraints());
  px.time_elapse_assign(py);
  BD_Shape x(px.constraints());
  swap(x);
  assert(OK());
}
 
template <typename T>
inline Constraint_System
BD_Shape<T>::constraints() const {
  Constraint_System cs;
  dimension_type space_dim = space_dimension();
  if (space_dim == 0) {
    if (marked_empty())
      cs = Constraint_System::zero_dim_empty();
  }
  else if (marked_empty())
    cs.insert(0*Variable(space_dim-1) <= -1);
  else {
    // KLUDGE: in the future `cs' will be constructed of the right dimension.
    // For the time being, we force the dimension with the following line.
    cs.insert(0*Variable(space_dim-1) <= 0);
    
    // We find in `*this' all the constraints.
    for (dimension_type i = 0; i <= space_dim; ++i) {
      for (dimension_type j = i + 1; j <= space_dim; ++j) {
	Variable x(j - 1);
	T dbm_i_j = dbm[i][j];
	T dbm_j_i = dbm[j][i];
	T negated_dbm_ji;
	if (exact_neg(negated_dbm_ji, dbm_j_i) &&
	    negated_dbm_ji == dbm_i_j) {
	  // We have one equality constraint.
	  Coefficient a;
	  Coefficient b;
	  numer_denom(dbm_i_j, b, a);
	  if (i != 0) {
	    Variable y(i - 1);
	    cs.insert(a*x - a*y == b);
	  }
	  else
	    cs.insert(a*x == b);
	}
	else {
	  // We have 0, 1 or 2 inequality constraints.
	  if (!is_plus_infinity(dbm_i_j)) {
	    Coefficient a;
	    Coefficient b;
	    numer_denom(dbm_i_j, b, a);
	    if (i != 0) {
	      Variable y(i - 1);
	      cs.insert(a*x - a*y <= b);
	    }
	    else
	      cs.insert(a*x <= b);
	  }
	  if (!is_plus_infinity(dbm_j_i)) {
	    Coefficient a;
	    Coefficient b;
	    numer_denom(dbm_j_i, b, a);
	    if (i != 0) {
	      Variable y(i - 1);
	      cs.insert(a*y - a*x <= b);
	    }
	    else
	      cs.insert(-a*x <= b);
	  }
	}
      }
    }
  }
  return cs;
}

template <typename T>
inline void
BD_Shape<T>::ascii_dump(std::ostream& s) const {
  status.ascii_dump(s);
  s << std::endl;
  dbm.ascii_dump(s);
}

template <typename T>
inline bool
BD_Shape<T>::ascii_load(std::istream& s) {
  if(!status.ascii_load(s))
    return false;
  if(!dbm.ascii_load(s))
    return false;
  return true;
}


/*! \relates Parma_Polyhedra_Library::BD_Shape */
template <typename T>
inline std::ostream&
IO_Operators::operator<<(std::ostream& s, const BD_Shape<T>& c) {
  if (c.is_universe())
    s << "true" << std::endl;
  else {
    // We control empty system of bounded differences.
    dimension_type n = c.space_dimension();
    if (c.marked_empty())
      s << "false" << std::endl;
    else {
      bool first = true;
      for (dimension_type i = 0; i <= n; ++i) 
	for (dimension_type j = i + 1; j <= n; ++j) {
	  const T& c_i_j = c.dbm[i][j];
	  const T& c_j_i = c.dbm[j][i];
	  T negated_c_ji;
	  if (exact_neg(negated_c_ji, c_j_i) &&
	      negated_c_ji == c_i_j) {
	    // We will print an equality.
	    if (first)
	      first = false;
	    else 
	      s << ", ";
	    if (i == 0) {
	      // We have got a equality constraint with one Variable.
	      s << Variable(j - 1);
	      s << " == " << c_i_j;
	    }
	    else {
	      // We have got a equality constraint with two Variables.
	      if (is_nonnegative(c_i_j)) {
		s << Variable(j - 1);
		s << " - ";
		s << Variable(i - 1);
		s << " == " << c_i_j;
	      }
	      else {
		s << Variable(i - 1);
		s << " - ";
		s << Variable(j - 1);
		s << " == " << c_j_i;
	      }
	    }
	  }
	  else {
	    // We will print a non-strict inequality.
	    if (!is_plus_infinity(c_j_i)) {
	      if (first)
		first = false;
	      else 
		s << ", ";
	      if (i == 0) {
		// We have got a constraint with an only Variable.
		s << Variable(j - 1);
		T v;
		negate_round_down(v, c_j_i);
		s << " >= " << v;
	      }
	      else {
		// We have got a constraint with two Variables.
		if (is_nonnegative(c_j_i)) {
		  s << Variable(i - 1);
		  s << " - ";
		  s << Variable(j - 1);
		  s << " <= " << c_j_i;
		}
		else {
		  s << Variable(j - 1);
		  s << " - ";
		  s << Variable(i - 1);
		  T v;
		  negate_round_down(v, c_j_i);
		  s << " >= " << v;
		}
	      }
	    }
	    if (!is_plus_infinity(c_i_j)) {
	      if (first)
		first = false;
	      else 
		s << ", ";
	      if (i == 0) {
		// We have got a constraint with an only Variable.
		s << Variable(j - 1);
		s << " <= " << c_i_j;
	      }
	      else {
		// We have got a constraint with two Variables.
		if (is_nonnegative(c_i_j)) {
		  s << Variable(j - 1);
		  s << " - ";
		  s << Variable(i - 1);
		  s << " <= " << c_i_j;
		}
		else {
		  s << Variable(i - 1);
		  s << " - ";
		  s << Variable(j - 1);
		  T v;
		  negate_round_down(v, c_i_j);
		  s << " >= " << v;
		}
	      }
	    }
	  }
	}
    }
  }
  return s;
}

template <typename T>
inline bool
BD_Shape<T>::OK() const {
  // Check whether the difference-bound matrix is well-formed.
  if (!dbm.OK())
    return false;

  // Check whether the status information is legal.
  if (!status.OK())
    return false;

  // If `*this' is empty bdiffs are OK. 
  if (marked_empty())
    return true;

  // On the diagonal we must have plus infinity.
  for (dimension_type i = dbm.num_rows(); i-- > 0;) {
    if (!is_plus_infinity(dbm[i][i])) {
#ifndef NDEBUG
      using namespace Parma_Polyhedra_Library::IO_Operators;
      std::cerr << "BD_Shape::dbm[" << i << "][" << i << "] = "
		<< dbm[i][i] << "!  (+inf was expected.)"
		<< std::endl;
#endif
      return false;
    }
  }
 
  // Check whether the closure information is legal.
  if (marked_transitively_closed()) {
    BD_Shape x = *this;
    x.status.reset_transitively_closed();
    x.closure_assign();
    if (x.dbm != dbm) {
#ifndef NDEBUG
      std::cerr << "BD_Shape is marked as transitively closed but is it not!"
		<< std::endl;
#endif
      return false;
    }
  }
  
  // All checks passed.
  return true;
}
  

template <typename T>
inline void
BD_Shape<T>::throw_dimension_incompatible(const char* method,
				       const BD_Shape& y) const {
  std::ostringstream s;
  s << "PPL::";
  s << "BD_Shape::" << method << ":" << std::endl
    << "this->space_dimension() == " << space_dimension()
    << ", y->space_dimension() == " << y.space_dimension() << ".";
  throw std::invalid_argument(s.str());
}

template <typename T>
inline void
BD_Shape<T>::throw_dimension_incompatible(const char* method,
				       dimension_type required_dim) const {
  std::ostringstream s;
  s << "PPL::";
  s << "BD_Shape::" << method << ":" << std::endl
    << "this->space_dimension() == " << space_dimension()
    << ", required dimension == " << required_dim << ".";
  throw std::invalid_argument(s.str());
}

template <typename T>
inline void
BD_Shape<T>::throw_dimension_incompatible(const char* method,
				       const Constraint& c) const {
  std::ostringstream s;
  s << "PPL::";
  s << "BD_Shape::" << method << ":" << std::endl
    << "this->space_dimension() == " << space_dimension()
    << ", c->space_dimension == " << c.space_dimension() << ".";
  throw std::invalid_argument(s.str());
}

template <typename T>
inline void
BD_Shape<T>::throw_dimension_incompatible(const char* method,
				       const Generator& g) const {
  std::ostringstream s;
  s << "PPL::";
  s << "BD_Shape::" << method << ":" << std::endl
    << "this->space_dimension() == " << space_dimension()
    << ", g->space_dimension == " << g.space_dimension() << ".";
  throw std::invalid_argument(s.str());
}

template <typename T>
inline void
BD_Shape<T>::throw_constraint_incompatible(const char* method) const {

  std::ostringstream s;
  s << "PPL::BD_Shape::" << method << ":" << std::endl
    << "the constraint is incompatible.";
  throw std::invalid_argument(s.str());
}

template <typename T>
inline void
BD_Shape<T>::throw_expression_too_complex(const char* method,
					const Linear_Expression& e) const {
  using namespace IO_Operators;
  std::ostringstream s;
  s << "PPL::BD_Shape::" << method << ":" << std::endl
    << e << " is too complex.";
  throw std::invalid_argument(s.str());
}


template <typename T>
inline void
BD_Shape<T>::throw_dimension_incompatible(const char* method,
					const char* name_row,
					const Linear_Expression& y) const {
  std::ostringstream s;
  s << "PPL::";
  s << "BD_Shape::" << method << ":" << std::endl
    << "this->space_dimension() == " << space_dimension()
    << ", " << name_row << "->space_dimension() == "
    << y.space_dimension() << ".";
  throw std::invalid_argument(s.str());
}


template <typename T>
inline void
BD_Shape<T>::throw_generic(const char* method,
			 const char* reason) const {
  std::ostringstream s;
  s << "PPL::";
  s << "BD_Shape::" << method << ":" << std::endl
    << reason;
  throw std::invalid_argument(s.str());
}

} // namespace Parma_Polyhedra_Library

namespace std {

/*! \relates Parma_Polyhedra_Library::BD_Shape */
template <typename T>
inline void
swap(Parma_Polyhedra_Library::BD_Shape<T>& x,
     Parma_Polyhedra_Library::BD_Shape<T>& y) {
  x.swap(y);
}

} // namespace std

#endif // !defined(PPL_BD_Shape_inlines_hh)
