/* Polyhedron class implementation: simplify().
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

#include <ppl-config.h>

#include "Linear_Row.defs.hh"
#include "Linear_System.defs.hh"
#include "Bit_Matrix.defs.hh"
#include "Polyhedron.defs.hh"
#include <cstddef>
#include <limits>

namespace PPL = Parma_Polyhedra_Library;

/*!
  \return
  The rank of \p sys.

  \param sys
  The system to simplify: it will be modified;

  \param sat
  The saturation matrix corresponding to \p sys.

  \p sys may be modified by swapping some of its rows and by possibly
  removing some of them, if they turn out to be redundant.

  If \p sys is a system of constraints, then the rows of \p sat are
  indexed by constraints and its columns are indexed by generators;
  otherwise, if \p sys is a system of generators, then the rows of
  \p sat are indexed by generators and its columns by constraints.

  Given a system of constraints or a system of generators, this function
  simplifies it using Gauss' elimination method (to remove redundant
  equalities/lines), deleting redundant inequalities/rays/points and
  making back-substitution.
  The explanation that follows assumes that \p sys is a system of
  constraints. For the case when \p sys is a system of generators,
  a similar explanation can be obtain by applying duality.

  The explanation relies on the notion of <EM>redundancy</EM>.
  (See the Introduction.)

  First we make some observations that can help the reader
  in understanding the function:

  Proposition: An inequality that is saturated by all the generators
  can be transformed to an equality.

  In fact, by combining any number of generators that saturate the
  constraints, we obtain a generator that saturates the constraints too:
  \f[
    \langle \vect{c}, \vect{r}_1 \rangle = 0 \land
    \langle \vect{c}, \vect{r}_2 \rangle = 0
    \Rightarrow
    \langle \vect{c}, (\lambda_1 \vect{r}_1 + \lambda_2 \vect{r}_2) \rangle =
    \lambda_1 \langle \vect{c}, \vect{r}_1 \rangle
    + \lambda_2 \langle \vect{c}, \vect{r}_2 \rangle
    = 0,
  \f]
  where \f$\lambda_1, \lambda_2\f$ can be any real number.
*/
PPL::dimension_type
PPL::Polyhedron::simplify(Linear_System& sys, Bit_Matrix& sat) {
  // This method is only applied to a well-formed system `sys'.
  assert(sys.OK(true));
  assert(sys.num_columns() >= 1);

  dimension_type num_rows = sys.num_rows();
  const dimension_type num_columns = sys.num_columns();
  const dimension_type num_cols_sat = sat.num_columns();

  // Looking for the first inequality in `sys'.
  dimension_type num_lines_or_equalities = 0;
  while (num_lines_or_equalities < num_rows
	 && sys[num_lines_or_equalities].is_line_or_equality())
    ++num_lines_or_equalities;

  // `num_saturators[i]' will contain the number of generators
  // that saturate the constraint `sys[i]'.
  if (num_rows > simplify_num_saturators_size) {
    delete [] simplify_num_saturators_p;
    simplify_num_saturators_p = 0;
    simplify_num_saturators_size = 0;
    const size_t max_size
      = std::numeric_limits<size_t>::max() / sizeof(dimension_type);
    const size_t new_size = compute_capacity(num_rows, max_size);
    simplify_num_saturators_p = new dimension_type[new_size];
    simplify_num_saturators_size = new_size;
  }
  dimension_type* num_saturators = simplify_num_saturators_p;

  // Computing the number of saturators for each inequality,
  // possibly identifying and swapping those that happen to be
  // equalities (see Proposition above).
  for (dimension_type i = num_lines_or_equalities; i < num_rows; ++i) {
    if (sat[i].empty()) {
      // The constraint `sys[i]' is saturated by all the generators.
      // Thus, either it is already an equality or it can be transformed
      // to an equality (see Proposition above).
      sys[i].set_is_line_or_equality();
      // Note: simple normalization already holds.
      sys[i].sign_normalize();
      // We also move it just after all the other equalities,
      // so that system `sys' keeps its partial sortedness.
      if (i != num_lines_or_equalities) {
	std::swap(sys[i], sys[num_lines_or_equalities]);
	std::swap(sat[i], sat[num_lines_or_equalities]);
	std::swap(num_saturators[i], num_saturators[num_lines_or_equalities]);
      }
      ++num_lines_or_equalities;
      // `sys' is no longer sorted.
      sys.set_sorted(false);
    }
    else
      // There exists a generator which does not saturate `sys[i]',
      // so that `sys[i]' is indeed an inequality.
      // We store the number of its saturators.
      num_saturators[i] = num_cols_sat - sat[i].count_ones();
  }

  // At this point, all the equalities of `sys' (included those
  // inequalities that we just transformed to equalities) have
  // indexes between 0 and `num_lines_or_equalities' - 1,
  // which is the property needed by method gauss().
  // We can simplify the system of equalities, obtaining the rank
  // of `sys' as result.
  const dimension_type rank = sys.gauss(num_lines_or_equalities);

  // Now the irredundant equalities of `sys' have indexes from 0
  // to `rank' - 1, whereas the equalities having indexes from `rank'
  // to `num_lines_or_equalities' - 1 are all redundant.
  // (The inequalities in `sys' have been left untouched.)
  // The rows containing equalities are not sorted.

  if (rank < num_lines_or_equalities) {
    // We identified some redundant equalities.
    // Moving them at the bottom of `sys':
    // - index `redundant' runs through the redundant equalities
    // - index `erasing' identifies the first row that should
    //   be erased after this loop.
    // Note that we exit the loop either because we have moved all
    // redundant equalities or because we have moved all the
    // inequalities.
    for (dimension_type redundant = rank,
	   erasing = num_rows;
	 redundant < num_lines_or_equalities
	   && erasing > num_lines_or_equalities;
	 ) {
      --erasing;
      std::swap(sys[redundant], sys[erasing]);
      std::swap(sat[redundant], sat[erasing]);
      std::swap(num_saturators[redundant], num_saturators[erasing]);
      sys.set_sorted(false);
      ++redundant;
    }
    // Adjusting the value of `num_rows' to the number of meaningful
    // rows of `sys': `num_lines_or_equalities' - `rank' is the number of
    // redundant equalities moved to the bottom of `sys', which are
    // no longer meaningful.
    num_rows -= num_lines_or_equalities - rank;
    // Adjusting the value of `num_lines_or_equalities'.
    num_lines_or_equalities = rank;
  }

  // Now we use the definition of redundancy (given in the Introduction)
  // to remove redundant inequalities.

  // First we check the saturation rule, which provides a necessary
  // condition for an inequality to be irredundant (i.e., it provides
  // a sufficient condition for identifying redundant inequalities).
  // Let
  //   num_saturators[i] = num_sat_lines[i] + num_sat_rays_or_points[i];
  //   dim_lin_space = num_irred_lines;
  //   dim_ray_space
  //     = dim_vector_space - num_irred_equalities - dim_lin_space
  //     = num_columns - 1 - num_lines_or_equalities - dim_lin_space;
  //   min_sat_rays_or_points = dim_ray_space.
  //
  // An inequality saturated by less than `dim_ray_space' _rays/points_
  // is redundant. Thus we have the implication
  //
  //   (num_saturators[i] - num_sat_lines[i] < dim_ray_space)
  //      ==>
  //        redundant(sys[i]).
  //
  // Moreover, since every line saturates all inequalities, we also have
  //     dim_lin_space = num_sat_lines[i]
  // so that we can rewrite the condition above as follows:
  //
  //   (num_saturators[i] < num_columns - num_lines_or_equalities - 1)
  //      ==>
  //        redundant(sys[i]).
  //
  const dimension_type min_saturators
    = num_columns - num_lines_or_equalities - 1;
  for (dimension_type i = num_lines_or_equalities; i < num_rows; ) {
    if (num_saturators[i] < min_saturators) {
      // The inequality `sys[i]' is redundant.
      --num_rows;
      std::swap(sys[i], sys[num_rows]);
      std::swap(sat[i], sat[num_rows]);
      std::swap(num_saturators[i], num_saturators[num_rows]);
      sys.set_sorted(false);
    }
    else
      ++i;
  }

  // Now we check the independence rule.
  for (dimension_type i = num_lines_or_equalities; i < num_rows; ) {
    bool redundant = false;
    // NOTE: in the inner loop, index `j' runs through _all_ the
    // inequalities and we do not test if `sat[i]' is strictly
    // contained into `sat[j]'.  Experimentation has shown that this
    // is faster than having `j' only run through the indexes greater
    // than `i' and also doing the test `strict_subset(sat[i],
    // sat[k])'.
    for (dimension_type j = num_lines_or_equalities; j < num_rows; ) {
      if (i == j)
	// We want to compare different rows of `sys'.
	++j;
      else {
	// Let us recall that each generator lies on a facet of the
	// polyhedron (see the Introduction).
	// Given two constraints `c_1' and `c_2', if there are `m'
	// generators lying on the hyper-plane corresponding to `c_1',
	// the same `m' generators lie on the hyper-plane
	// corresponding to `c_2', too, and there is another one lying
	// on the latter but not on the former, then `c_2' is more
	// restrictive than `c_1', i.e., `c_1' is redundant.
	bool strict_subset;
	if (subset_or_equal(sat[j], sat[i], strict_subset))
	  if (strict_subset) {
	    // All the saturators of the inequality `sys[i]' are
	    // saturators of the inequality `sys[j]' too,
	    // and there exists at least one saturator of `sys[j]'
	    // which is not a saturator of `sys[i]'.
	    // It follows that inequality `sys[i]' is redundant.
	    redundant = true;
	    break;
	  }
	  else {
	    // We have `sat[j] == sat[i]'.  Hence inequalities
	    // `sys[i]' and `sys[j]' are saturated by the same set of
	    // generators. Then we can remove either one of the two
	    // inequalities: we remove `sys[j]'.
	    --num_rows;
	    std::swap(sys[j], sys[num_rows]);
	    std::swap(sat[j], sat[num_rows]);
	    std::swap(num_saturators[j], num_saturators[num_rows]);
	    sys.set_sorted(false);
	  }
	else
	  // If we reach this point then we know that `sat[i]' does
	  // not contain (and is different from) `sat[j]', so that
	  // `sys[i]' is not made redundant by inequality `sys[j]'.
	  ++j;
      }
    }
    if (redundant) {
      // The inequality `sys[i]' is redundant.
      --num_rows;
      std::swap(sys[i], sys[num_rows]);
      std::swap(sat[i], sat[num_rows]);
      std::swap(num_saturators[i], num_saturators[num_rows]);
      sys.set_sorted(false);
    }
    else
      // The inequality `sys[i]' is not redundant.
      ++i;
  }

  // Here we physically remove the redundant inequalities previously
  // moved to the bottom of `sys' and the corresponding `sat' rows.
  sys.erase_to_end(num_rows);
  sys.unset_pending_rows();
  sat.rows_erase_to_end(num_rows);
  // At this point the first `num_lines_or_equalities' rows of 'sys'
  // represent the irredundant equalities, while the remaining rows
  // (i.e., those having indexes from `num_lines_or_equalities' to
  // `num_rows' - 1) represent the irredundant inequalities.
#ifndef NDEBUG
  // Check if the flag is set (that of the equalities is already set).
  for (dimension_type i = num_lines_or_equalities; i < num_rows; ++i)
    assert(sys[i].is_ray_or_point_or_inequality());
#endif

  // Finally, since now the sub-system (of `sys') of the irredundant
  // equalities is in triangular form, we back substitute each
  // variables with the expression obtained considering the equalities
  // starting from the last one.
  sys.back_substitute(num_lines_or_equalities);

  // The returned value is the number of irredundant equalities i.e.,
  // the rank of the sub-system of `sys' containing only equalities.
  // (See the Introduction for definition of lineality space dimension.)
  return num_lines_or_equalities;
}
