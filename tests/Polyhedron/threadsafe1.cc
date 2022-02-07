/* Tests library thread safety.
   Copyright (C) 2014-2016 Enea Zaffanella <enea.zaffanella@unipr.it>
   Copyright (C) 2016-2016 BUGSENG srl (http://bugseng.com)

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

#include "ppl_test.hh"
#include <thread>

namespace
{

C_Polyhedron
build_polyhedron ()
{
    const dimension_type space_dim = 10;
    C_Polyhedron         ph(space_dim, UNIVERSE);

    for (dimension_type i = 0; i < space_dim; ++i)
    {
        ph.add_constraint(Variable(i) >= 0);
        ph.add_constraint(Variable(i) <= 1);
    }

    return ph;
}

// The work to be done in a separate thread.
void
compute_image (C_Polyhedron * ph)
{
    ph->affine_image(Variable(0), Linear_Expression(5));
}

void
test01_aux (C_Polyhedron * ph)
{
    // Perform thread initialization (and finalization on exit).
    Parma_Polyhedra_Library::Thread_Init thread_init;

    compute_image(ph);
}

bool
test01 ()
{
    C_Polyhedron ph1 = build_polyhedron();
    C_Polyhedron ph2(ph1);
    C_Polyhedron ph3(ph1);
    C_Polyhedron ph4(ph1);

    std::thread t1(test01_aux, &ph1);
    std::thread t2(test01_aux, &ph2);
    std::thread t3(test01_aux, &ph3);
    std::thread t4(test01_aux, &ph4);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    const bool ok = (ph1 == ph2) && (ph2 == ph3) && (ph3 == ph4);

    return ok;
}

// Same as test01, but using make_threadable helper.
bool
test02 ()
{
    C_Polyhedron ph1 = build_polyhedron();
    C_Polyhedron ph2(ph1);
    C_Polyhedron ph3(ph1);
    C_Polyhedron ph4(ph1);

    std::thread t1(make_threadable(compute_image), &ph1);
    std::thread t2(make_threadable(compute_image), &ph2);
    std::thread t3(make_threadable(compute_image), &ph3);
    std::thread t4(make_threadable(compute_image), &ph4);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    const bool ok = (ph1 == ph2) && (ph2 == ph3) && (ph3 == ph4);

    return ok;
}

} // namespace

BEGIN_MAIN
    DO_TEST(test01);
DO_TEST(test02);
END_MAIN

