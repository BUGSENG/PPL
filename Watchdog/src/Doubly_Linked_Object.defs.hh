/* Doubly_Linked_Object class declaration.
   Copyright (C) 2001-2009 Roberto Bagnara <bagnara@cs.unipr.it>

This file is part of the Parma Watchdog Library (PWL).

The PWL is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

The PWL is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1307, USA.

For the most up-to-date information see the Parma Polyhedra Library
site: http://www.cs.unipr.it/ppl/ . */

#ifndef PWL_Doubly_Linked_Object_defs_hh
#define PWL_Doubly_Linked_Object_defs_hh 1

#include "Doubly_Linked_Object.types.hh"
#include "EList.types.hh"
#include "EList_Iterator.types.hh"

//! A (base) class for doubly linked objects.
class Parma_Watchdog_Library::Doubly_Linked_Object {
public:
  //! Default constructor.
  Doubly_Linked_Object();

  //! Creates a chain element with forward link \p f and backward link \p b.
  Doubly_Linked_Object(Doubly_Linked_Object* f, Doubly_Linked_Object* b);

  //! Inserts \p y before \p *this.
  void insert_before(Doubly_Linked_Object& y);

  //! Inserts \p y after \p *this.
  void insert_after(Doubly_Linked_Object& y);

  //! Erases \p *this from the chain and returns a pointer to the next element.
  Doubly_Linked_Object* erase();

  //! Erases \p *this from the chain.
  ~Doubly_Linked_Object();

private:
  //! Forward link.
  Doubly_Linked_Object* next;

  //! Backward link.
  Doubly_Linked_Object* prev;

  template <typename T> friend class EList;
  template <typename T> friend class EList_Iterator;
};

#include "Doubly_Linked_Object.inlines.hh"

#endif // !defined(PWL_Doubly_Linked_Object_defs_hh)
