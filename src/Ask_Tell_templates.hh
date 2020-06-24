/* Ask_Tell class implementation: non-inline template functions.
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

#ifndef PPL_Ask_Tell_templates_hh
#define PPL_Ask_Tell_templates_hh 1

#include <iostream>

namespace Parma_Polyhedra_Library {

template <typename D>
bool
Ask_Tell<D>::check_normalized() const {
  for (const_iterator x_begin = begin(),
         x_end = end(), xi = x_begin; xi != x_end; ++xi)
    for (const_iterator yi = x_begin; yi != x_end; ++yi)
      if (xi != yi) {
        if (xi->tell().definitely_entails(yi->tell())) {
          if (yi->ask().definitely_entails(xi->ask())) {
            return false;
          }
        }
        else if (xi->tell().definitely_entails(yi->ask())) {
          return false;
        }
        if (xi->ask().definitely_entails(yi->ask())
            && !xi->ask().definitely_entails(yi->tell())) {
          return false;
        }
      }
  return true;
}

template <typename D>
bool
Ask_Tell<D>::reduce() {
  bool changed = false;
  for (Sequence_iterator x_begin = sequence.begin(),
         x_end = sequence.end(), xi = x_begin; xi != x_end; ++xi)
    for (Sequence_iterator yi = x_begin; yi != x_end; ) {
      if (xi != yi
          && yi->ask().definitely_entails(xi->ask())
          && xi->tell().definitely_entails(yi->tell())) {
        yi = sequence.erase(yi);
        x_begin = sequence.begin();
        x_end = sequence.end();
        changed = true;
      }
      else {
        ++yi;
      }
    }
  PPL_ASSERT_HEAVY(OK());
  return changed;
}

template <typename D>
bool
Ask_Tell<D>::deduce() {
  bool changed = false;
  for (Sequence_iterator x_begin = sequence.begin(),
         x_end = sequence.end(), xi = x_begin; xi != x_end; ++xi) {
    D& xi_tell = xi->tell();
    bool tell_changed;
    do {
      tell_changed = false;
      for (Sequence_iterator yi = x_begin; yi != x_end; ++yi) {
        if (xi != yi
            && xi_tell.definitely_entails(yi->ask())
            && !xi_tell.definitely_entails(yi->tell())) {
          xi_tell.meet_assign(yi->tell());
          changed = tell_changed = true;
          }
        }
    } while (tell_changed);
  }
  if (changed) {
    (void) reduce();
  }
  PPL_ASSERT_HEAVY(OK());
  return changed;
}

template <typename D>
bool
Ask_Tell<D>::absorb() {
  bool changed = false;
  for (Sequence_iterator x_begin = sequence.begin(),
         x_end = sequence.end(), xi = x_begin; xi != x_end; ) {
    D& xi_ask = xi->ask();
    D& xi_tell = xi->tell();
    // We may strengthen the ask component of the pair referenced by `xi'.
    // If we do it, the pair may become useless (i.e., with the ask
    // component entailing the tell component) and thus to be
    // discarded.
    bool must_check_xi_pair = false;
    bool ask_changed;
    do {
      ask_changed = false;
      for (Sequence_iterator yi = x_begin; yi != x_end; ++yi) {
        if (xi != yi) {
          D& yi_ask = yi->ask();
          D& yi_tell = yi->tell();
          if (xi_ask.definitely_entails(yi_ask)
              && !xi_ask.definitely_entails(yi_tell)) {
            xi_ask.meet_assign(yi_tell);
            must_check_xi_pair = true;
            ask_changed = true;
          }
        }
      }
    } while (ask_changed);
    if (must_check_xi_pair) {
      changed = true;
      if (xi_ask.definitely_entails(xi_tell)) {
        xi = sequence.erase(xi);
        x_begin = sequence.begin();
        x_end = sequence.end();
      }
      else {
        ++xi;
      }
    }
    else {
      ++xi;
    }
  }
  if (changed) {
    (void) reduce();
  }
  PPL_ASSERT_HEAVY(OK());
  return changed;
}

template <typename D>
void
Ask_Tell<D>::deabsorb() const {
  if (D::has_nontrivial_weakening()) {
    Sequence new_sequence;
    for (Sequence_const_iterator x_begin = sequence.begin(),
           x_end = sequence.end(), xi = x_begin; xi != x_end; ++xi)
      for (Sequence_const_iterator yi = x_begin; yi != x_end; ++yi) {
        if (xi != yi) {
          const D& xi_ask = xi->ask();
          const D& yi_ask = yi->ask();
          if (xi_ask.definitely_entails(yi_ask)) {
            D new_ask = xi_ask;
            new_ask.weakening_assign(yi->tell());
            new_ask.meet_assign(yi_ask);
            if (!new_ask.definitely_entails(xi_ask)) {
              new_sequence.push_back(Pair(new_ask, xi->tell()));
            }
          }
        }
      }
    if (!new_sequence.empty()) {
      Ask_Tell& x = const_cast<Ask_Tell&>(*this);
      std::copy(new_sequence.begin(), new_sequence.end(),
                back_inserter(x.sequence));
      x.reduce();
      x.deduce();
      normalized = false;
    }
  }
  else if (!normalized) {
    Ask_Tell& x = const_cast<Ask_Tell&>(*this);
    x.reduce();
    x.deduce();
  }
  PPL_ASSERT_HEAVY(OK());
}

template <typename D>
void
Ask_Tell<D>::upper_bound_assign(const Ask_Tell& y) {
  const Ask_Tell& x = *this;
  x.deabsorb();
  y.deabsorb();
  Ask_Tell<D> z;
  for (typename Ask_Tell<D>::const_iterator xi = x.begin(),
         x_end = x.end(); xi != x_end; ++xi)
    for (typename Ask_Tell<D>::const_iterator yi = y.begin(),
           y_end = y.end(); yi != y_end; ++yi) {
      D tell = xi->tell();
      tell.upper_bound_assign(yi->tell());
      D ask = xi->ask();
      ask.meet_assign(yi->ask());
      if (!ask.definitely_entails(tell)) {
        z.pair_insert(ask, tell);
      }
    }
  *this = z;
  PPL_ASSERT_HEAVY(OK());
}

template <typename D>
bool
Ask_Tell<D>::OK() const {
  for (typename Ask_Tell<D>::const_iterator xi = begin(),
         x_end = end(); xi != x_end; ++xi) {
    const Ask_Tell_Pair<D>& p = *xi;
    if (!p.ask().OK()) {
      return false;
    }
    if (!p.tell().OK()) {
      return false;
    }
    if (p.ask().definitely_entails(p.tell())) {
#ifndef NDEBUG
      using namespace IO_Operators;
      std::cerr << "Illegal agent in ask-and-tell: "
                << p.ask() << " -> " << p.tell()
                << std::endl;
#endif
      return false;
    }
  }
  if (normalized && !check_normalized()) {
#ifndef NDEBUG
    std::cerr << "Ask_Tell claims to be normalized, but it is not!"
              << std::endl;
#endif
    return false;
  }
  return true;
}

namespace IO_Operators {

template <typename D>
std::ostream&
operator<<(std::ostream& s, const Ask_Tell<D>& x) {
  if (x.is_top()) {
    s << "true";
  }
  else if (x.is_bottom()) {
    s << "false";
  }
  else {
    for (typename Ask_Tell<D>::const_iterator xi = x.begin(),
           x_end = x.end(); xi != x_end; ++xi)
      s << "(" << xi->ask() << " -> " << xi->tell() << ")";
  }
  return s;
}

} // namespace IO_Operators

} // namespace Parma_Polyhedra_Library

#endif // !defined(PPL_Ask_Tell_templates_hh)
