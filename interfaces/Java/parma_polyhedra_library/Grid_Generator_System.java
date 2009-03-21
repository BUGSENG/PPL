/* Grid Generator_System Java class declaration and implementation.
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


package parma_polyhedra_library;

import java.io.Writer;
import java.io.IOException;

//! A system of grid generators.
/*! \ingroup PPL_Java_interface
    An object of the class Grid_Generator_System is a system of grid
    generators, i.e., a multiset of objects of the class Grid_Generator.
*/
public class Grid_Generator_System extends java.util.Vector<Grid_Generator> {

    //! Default constructor: builds an empty system of grid generators.
    public Grid_Generator_System() {
    }

    //! Returns an ascii formatted internal representation of \p this.
    public native String ascii_dump();

    //! Returns a string representation of \p this.
    public native String toString();

    private native String pretty_print(long indent_depth,
                                       long preferred_first_line_length,
                                       long preferred_line_length);

    //! Pretty prints \p this using \p writer.
    public void pretty_print(Writer writer,
                             long indent_depth,
                             long preferred_first_line_length,
                             long preferred_line_length)
        throws IOException {
        String s = pretty_print(indent_depth,
                                preferred_first_line_length,
                                preferred_line_length);
        writer.write(s);
    }
}
