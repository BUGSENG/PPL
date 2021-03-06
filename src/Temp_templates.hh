/* Temp_* classes implementation: non-inline template members.
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

#ifndef PPL_Temp_templates_hh
#define PPL_Temp_templates_hh 1

namespace Parma_Polyhedra_Library {

template <typename T>
Temp_Item<T>::Free_List::~Free_List() {
  while (head_ptr != nullptr) {
    Temp_Item* const p = head_ptr;
    head_ptr = head_ptr->next;
    delete p;
  }
}

} // namespace Parma_Polyhedra_Library

#endif // !defined(PPL_Temp_templates_hh)
