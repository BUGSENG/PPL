/* Ask_Tell class implementation: inline functions.
   Copyright (C) 2001-2010 Roberto Bagnara <bagnara@cs.unipr.it>
   Copyright (C) 2010-2020 BUGSENG srl (http://bugseng.com)

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

#ifndef PPL_Ask_Tell_inlines_hh
#define PPL_Ask_Tell_inlines_hh 1

#include <algorithm>
#include "assertions.hh"

// Temporary!!!
#include <iostream>

namespace Parma_Polyhedra_Library {

template <typename D>
Ask_Tell_Pair<D>::Ask_Tell_Pair(const D& ask, const D& tell)
  : a(ask), t(tell) {
}

template <typename D>
const D&
Ask_Tell_Pair<D>::ask() const {
  return a;
}

template <typename D>
D&
Ask_Tell_Pair<D>::ask() {
  return a;
}

template <typename D>
const D&
Ask_Tell_Pair<D>::tell() const {
  return t;
}

template <typename D>
D&
Ask_Tell_Pair<D>::tell() {
  return t;
}

template <typename D>
bool
Ask_Tell_Pair<D>::definitely_entails(const Ask_Tell_Pair& y) const {
  const Ask_Tell_Pair<D>& x = *this;
  const D& x_ask = x.ask();
  const D& x_tell = x.tell();
  const D& y_ask = y.ask();
  const D& y_tell = y.tell();
  if (!y_ask.definitely_entails(x_ask)) {
    return false;
  }
  else if (x_tell.definitely_entails(y_tell)) {
    return true;
  }
  // The following test can be omitted.
  else if (x_tell.definitely_entails(y_ask)) {
    return false;
  }
  else {
    D x_tell_y_ask = x_tell;
    x_tell_y_ask.meet_assign(y_ask);
    return x_tell_y_ask.definitely_entails(y_tell);
  }
}

template <typename D>
Ask_Tell<D>::Ask_Tell()
  : sequence(), normalized(true) {
}

template <typename D>
Ask_Tell<D>::Ask_Tell(const Ask_Tell& y)
  : sequence(y.sequence), normalized(y.normalized) {
}

template <typename D>
Ask_Tell<D>::Ask_Tell(const D& ask, const D& tell)
  : sequence(), normalized(true) {
  if (!tell.is_top()) {
    pair_insert(ask, tell);
  }
}

template <typename D>
inline
Ask_Tell<D>::~Ask_Tell() {
}

template <typename D>
Ask_Tell<D>&
Ask_Tell<D>::operator=(const Ask_Tell& y) {
  sequence = y.sequence;
  normalized = y.normalized;
  return *this;
}

template <typename D>
inline void
Ask_Tell<D>::m_swap(Ask_Tell& y) {
  swap(sequence, y.sequence);
  swap(normalized, y.normalized);
}

template <typename D>
typename Ask_Tell<D>::iterator
Ask_Tell<D>::begin() {
  return sequence.begin();
}

template <typename D>
typename Ask_Tell<D>::const_iterator
Ask_Tell<D>::begin() const {
  return sequence.begin();
}

template <typename D>
typename Ask_Tell<D>::iterator
Ask_Tell<D>::end() {
  return sequence.end();
}

template <typename D>
typename Ask_Tell<D>::const_iterator
Ask_Tell<D>::end() const {
  return sequence.end();
}

template <typename D>
typename Ask_Tell<D>::reverse_iterator
Ask_Tell<D>::rbegin() {
  return sequence.rbegin();
}

template <typename D>
typename Ask_Tell<D>::const_reverse_iterator
Ask_Tell<D>::rbegin() const {
  return sequence.rbegin();
}

template <typename D>
typename Ask_Tell<D>::reverse_iterator
Ask_Tell<D>::rend() {
  return sequence.rend();
}

template <typename D>
typename Ask_Tell<D>::const_reverse_iterator
Ask_Tell<D>::rend() const {
  return sequence.rend();
}

template <typename D>
typename Ask_Tell<D>::size_type
Ask_Tell<D>::size() const {
  return sequence.size();
}

template <typename D>
void
Ask_Tell<D>::pair_insert_good(const D& ask, const D& tell) {
  PPL_ASSERT_HEAVY(!ask.definitely_entails(tell));
  sequence.push_back(Ask_Tell_Pair<D>(ask, tell));
  normalized = false;
}

template <typename D>
void
Ask_Tell<D>::pair_insert(const D& ask, const D& tell) {
  if (tell.definitely_entails(ask)) {
    pair_insert_good(ask, tell);
  }
  else {
    D new_tell = tell;
    new_tell.meet_assign(ask);
    pair_insert_good(ask, new_tell);
  }
}

template <typename D>
void
Ask_Tell<D>::normalize() const {
  if (normalized) {
    return;
  }

  Ask_Tell& x = const_cast<Ask_Tell&>(*this);
  x.reduce();
  x.deduce();
  x.absorb();
  normalized = true;

  PPL_ASSERT_HEAVY(OK());
}

template <typename D>
bool
Ask_Tell<D>::is_normalized() const {
  if (!normalized && check_normalized()) {
    normalized = true;
  }
  return normalized;
}

template <typename D>
bool
Ask_Tell<D>::definitely_entails(const Ask_Tell& y) const {
  const Ask_Tell<D>& x = *this;
  x.normalize();
  y.normalize();
  bool found = true;
  for (const_iterator x_begin = x.begin(), x_end = x.end(), y_end = y.end(),
         yi = y.begin(); found && yi != y_end; ++yi) {
    found = false;
    for (const_iterator xi = x_begin; !found && xi != x_end; ++xi)
      found = xi->definitely_entails(*yi);
  }
  return found;
}

template <typename D>
Ask_Tell<D>&
Ask_Tell<D>::add_pair(const D& ask, const D& tell) {
  if (!ask.definitely_entails(tell)) {
    pair_insert(ask, tell);
  }
  PPL_ASSERT_HEAVY(OK());
  return *this;
}

/*! \relates Ask_Tell */
template <typename D>
inline
bool operator==(const Ask_Tell<D>& x, const Ask_Tell<D>& y) {
  return x.definitely_entails(y) && y.definitely_entails(x);
}

/*! \relates Ask_Tell */
template <typename D>
inline
bool operator!=(const Ask_Tell<D>& x, const Ask_Tell<D>& y) {
  return !(x == y);
}

template <typename D>
bool
Ask_Tell<D>::is_top() const {
  return sequence.empty();
}

template <typename D>
bool
Ask_Tell<D>::is_bottom() const {
  // Must normalize for correctness.
  const_iterator xi = begin();
  const_iterator x_end = end();
  return xi != x_end
    && xi->ask().is_top() && xi->tell().is_bottom()
    && ++xi == x_end;
}

template <typename D>
bool
Ask_Tell<D>::empty() const {
  return sequence.empty();
}

template <typename D>
void
Ask_Tell<D>::meet_assign(const Ask_Tell& y) {
  if (!y.empty()) {
    std::copy(y.begin(), y.end(), back_inserter(sequence));
    normalized = false;
  }
  PPL_ASSERT_HEAVY(OK());
}

// Hiding

template <typename D>
bool
Ask_Tell<D>::probe(const D& tell, const D& ask) const {
  const_iterator yi;
  bool tell_changed;

  D xtell(tell);
  tell_changed = true;
  while (tell_changed) {
    tell_changed = false;
    for (yi = begin(); yi != end(); ++yi) {
      if (xtell.definitely_entails(yi->ask())
          && !xtell.definitely_entails(yi->tell())) {
          xtell.meet_assign(yi->tell());
          if (xtell.definitely_entails(ask)) {
            return true;
          }
          tell_changed = true;
      }
    }
  }
  return false;
}

/*! \relates Ask_Tell */
template <typename D>
inline void
swap(Ask_Tell<D>& x, Ask_Tell<D>& y) {
  x.m_swap(y);
}

} // namespace Parma_Polyhedra_Library

#endif // !defined(PPL_Ask_Tell_inlines_hh)
