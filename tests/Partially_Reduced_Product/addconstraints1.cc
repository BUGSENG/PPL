/* Test Smash_Product.
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

#include "ppl_test.hh"

using namespace Parma_Polyhedra_Library::IO_Operators;
#define REVERSED_TEST
#include "partially_reduced_product_test.hh"

typedef NNC_Polyhedron DOMAIN1;
typedef Grid DOMAIN2;
typedef Domain_Product<DOMAIN1x, DOMAIN2x>::Constraints_Product CProduct;
typedef Domain_Product<DOMAIN1x, DOMAIN2x>::Smash_Product SProduct;

namespace
{

// Product(dims, type); == and !=
bool
test01 ()
{
    Variable A(0);

    SProduct sp1(3);
    SProduct sp2(3, EMPTY);

    bool ok = (sp1 != sp2);

    if (!ok)
    {
        return false;
    }

    sp1.refine_with_congruence((A %= 0) / 4);
    sp1.refine_with_congruence((A %= 1) / 4);

    ok = (sp1 == sp2);

    ok = ok && sp1.OK() && sp2.OK();

    print_congruences(sp1, "*** sp1 congruences ***");
    print_constraints(sp1, "*** sp1 constraints ***");

    return ok;
}

// operator=
bool
test02 ()
{
    Variable A(0);
    Variable B(1);

    Constraint_System cs(A + B <= 9);

    SProduct sp1(2);

    sp1.refine_with_congruence((A %= 9) / 19);
    sp1.refine_with_congruence((A %= 8) / 19);

    SProduct sp2 = sp1;

    bool ok = (sp1 == sp2);

    ok = ok && sp1.OK() && sp2.OK();

    print_congruences(sp1, "*** sp1 congruences ***");
    print_constraints(sp1, "*** sp1 constraints ***");

    return ok;
}
sp.refine_with_constraint(A <= 0);
sp.refine_with_congruence((A %= 1) / 3);

ok = sp.OK();

print_constraints(sp, "*** sp constraints ***");
print_congruences(sp, "*** sp congruences ***");

return ok;
}

// ok(), is reduced.
bool
test15 ()
{
    Variable A(0);
    Variable B(1);

    bool ok;

    SProduct sp(2);

    sp.refine_with_constraint(A >= 1);
    sp.refine_with_constraint(A <= 0);
    sp.refine_with_congruence((A %= 1) / 3);

    // reduce the product
    Constraint_System sp_cs = sp.constraints();

    ok = sp.OK();

    if (!ok)
    {
        print_constraints(sp_cs, "*** sp.constraints(); ***");
        return false;
    }

    Grid sp_gr(sp_cs);

    if (!ok)
    {
        print_constraints(sp_cs, "*** sp.constraints(); ***");
        return false;
    }

        = sp_gr.is_empty();

    print_constraints(sp, "*** sp constraints ***");
    print_congruences(sp, "*** sp congruences ***");

    // reduce the product
    Constraint_System sp_cs = sp.constraints();

    return ok;
}

// Building from inequality constraints()
bool
test16 ()
{
    Variable A(0);
    Variable B(1);
    Variable C(2);

    Constraint_System cs;

    cs.insert(A - C <= 8);
    cs.insert(A - C >= 9);

    try
    {
        SProduct sp(cs);
    }
    catch (const std::invalid_argument & e)
    {
        nout << "cs contains an inequality constraint: " << e.what() << endl;
        return true;
    }
    catch (...)
    {
    }
    return false;
}

// Building from equality congruences()
bool
test17 ()
{
    Variable A(0);
    Variable B(1);
    Variable C(2);

    Congruence_System cgs1;

    cgs1.insert((A - C %= 8) / 0);

    Congruence_System cgs2;

    cgs2.insert((A - C %= 9) / 0);
    cgs2.insert((B %= 21) / 0);

    SProduct sp(cgs1);


    bool ok = sp.OK();

    print_congruences(sp, "*** sp congruences ***");
    print_constraints(sp, "*** sp constraints ***");

    return ok;
}

// refine_with_congruences
bool
test18 ()
{
    Variable A(0);
    Variable B(1);

    Congruence_System cgs;

    cgs.insert((A %= 0) / 2);
    cgs.insert((A + B %= 0) / 2);
    cgs.insert((B %= 0) / 2);
    cgs.insert(A + B == 0);

    SProduct sp(2);

    sp.refine_with_congruences(cgs);

    Grid gr(cgs);

    SProduct known_sp(gr);

    bool ok = (sp == known_sp);

    print_constraints(sp, "*** sp constraints ***");
    print_congruences(sp, "*** sp congruences ***");

    return ok;
}

// add_recycled_congruences
bool
test19 ()
{
    Variable A(0);
    Variable B(1);

    Congruence_System cgs;

    cgs.insert((A + B %= 0) / 2);
    cgs.insert((A %= 0) / 0);

    SProduct sp(2);
    CProduct cp(2);

    Congruence_System cgs_copy  = cgs;
    Congruence_System cgs_copy2 = cgs;

    sp.add_recycled_congruences(cgs);
    cp.add_recycled_congruences(cgs_copy);

    Grid gr(cgs_copy2);

    SProduct known_sp(gr);
    CProduct known_cp(gr);

    bool ok = (sp == known_sp && cp == known_cp);

    print_constraints(sp, "*** sp constraints ***");
    print_congruences(sp, "*** sp congruences ***");
    print_constraints(cp, "*** cp constraints ***");
    print_congruences(cp, "*** cp congruences ***");

    return ok;
}

// add_recycled_congruences
bool
test20 ()
{
    Variable A(0);
    Variable B(1);

    Congruence_System cgs;

    cgs.insert((B %= 0) / 2);
    cgs.insert((A %= 0) / 2);
    cgs.insert((A %= 0) / 1);
    cgs.insert(A - B == 0);

    SProduct sp(2);
    CProduct cp(2);

    Congruence_System cgs_copy  = cgs;
    Congruence_System cgs_copy2 = cgs;

    sp.add_recycled_congruences(cgs);
    cp.add_recycled_congruences(cgs_copy);

    Grid gr(cgs_copy2);

    SProduct known_sp(gr);
    CProduct known_cp(gr);

    bool ok = (sp == known_sp && cp == known_cp);

    print_constraints(sp, "*** sp constraints ***");
    print_congruences(sp, "*** sp congruences ***");
    print_constraints(cp, "*** cp constraints ***");
    print_congruences(cp, "*** cp congruences ***");

    return ok;
}

} // namespace

BEGIN_MAIN
    DO_TEST(test01);
DO_TEST(test02);
END_MAIN

