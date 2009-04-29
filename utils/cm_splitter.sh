#!/bin/sh

# Splits stdin according to cutting markers.
# Copyright (C) 2001-2009 Roberto Bagnara <bagnara@cs.unipr.it>
#
# This file is part of the Parma Polyhedra Library (PPL).
#
# The PPL is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 3 of the License, or (at your
# option) any later version.
#
# The PPL is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1307, USA.
#
# For the most up-to-date information see the Parma Polyhedra Library
# site: http://www.cs.unipr.it/ppl/ .

CUTTING_MARKER_PREFIX="%<--%<--%<-- "

open="false"

while IFS= read -r line
do
  case "$line" in
      $CUTTING_MARKER_PREFIX*)
	  read dummy file <<EOF
$line
EOF
	  exec 3>> "$file"
	  open="true"
	  ;;
      *)
	  [ $open = "true" ] && echo "$line" >&3
	  ;;
  esac
done
