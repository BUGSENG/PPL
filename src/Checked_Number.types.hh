/* Copyright (C) 2001-2004 Roberto Bagnara <bagnara@cs.unipr.it>

This file is free software; as a special exception the author gives
unlimited permission to copy and/or distribute it, with or without
modifications, as long as this notice is preserved.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. */

#ifndef PPL_Checked_Number_types_hh
#define PPL_Checked_Number_types_hh 1

namespace Parma_Polyhedra_Library {

struct Check_Overflow_Policy;

template <typename T, typename Policy = Check_Overflow_Policy>
class Checked_Number;

} // namespace Parma_Polyhedra_Library

#endif // !defined(PPL_Checked_Number_types_hh)
