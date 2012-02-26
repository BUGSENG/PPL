/* Row_Flags class implementation (non-inline functions).
   Copyright (C) 2001-2010 Roberto Bagnara <bagnara@cs.unipr.it>
   Copyright (C) 2010-2012 BUGSENG srl (http://bugseng.com)

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

#include "ppl-config.h"
#include "Row_Flags.defs.hh"
#include <iostream>
#include <iomanip>

namespace PPL = Parma_Polyhedra_Library;

void
PPL::Row_Flags::ascii_dump(std::ostream& s) const {
  s << "0x";
  std::ios::fmtflags old_flags = s.setf(std::ios::hex,
                                        std::ios::basefield);
  const std::streamsize new_sz
    = static_cast<std::streamsize>(2 * sizeof(Row_Flags::base_type));
  s << std::setw(new_sz) << setfill('0') << bits;
  s.flags(old_flags);
}

PPL_OUTPUT_DEFINITIONS_ASCII_ONLY(Row_Flags)

bool
PPL::Row_Flags::ascii_load(std::istream& s) {
  std::string str;
  if (!(s >> std::setw(2) >> str) || str != "0x")
    return false;
  const std::ios::fmtflags old_flags = s.setf(std::ios::hex,
                                              std::ios::basefield);
  s >> bits;
  s.flags(old_flags);
  return !s.fail();
}
