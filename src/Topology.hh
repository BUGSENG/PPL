/* Copyright (C) 2001-2009 Roberto Bagnara <bagnara@cs.unipr.it>

This file is free software; as a special exception the author gives
unlimited permission to copy and/or distribute it, with or without
modifications, as long as this notice is preserved.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. */

#ifndef PPL_Topology_hh
#define PPL_Topology_hh 1

namespace Parma_Polyhedra_Library {

#ifdef PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS
//! Kinds of polyhedra domains.
/*! \ingroup PPL_CXX_interface */
#endif // defined(PPL_DOXYGEN_INCLUDE_IMPLEMENTATION_DETAILS)
enum Topology {
  NECESSARILY_CLOSED = 0,
  NOT_NECESSARILY_CLOSED = 1
};

} // namespace Parma_Polyhedra_Library

#endif // !defined(PPL_Topology_hh)
