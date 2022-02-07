#include "ppl_test.hh"
#include "timings.hh"

namespace
{

bool
test01 ()
{
    const dimension_type first_dim = 10;
    const dimension_type last_dim  = 16;

    for (dimension_type dim = first_dim; dim <= last_dim; ++dim)
    {
        Constraint_System cs;

        for (dimension_type i = 0; i < dim; ++i)
        {
            cs.insert(Variable(i) >= 0);
            cs.insert(Variable(i) <= 1);
        }

        nout << "Converting hypercube of dim " << dim << " | ";

        C_Polyhedron ph(cs);

        start_clock();
        (void) ph.generators();
        print_clock(nout);

        const Constraint_System & min_cs = ph.minimized_constraints();

        nout << " " << std::distance(min_cs.begin(), min_cs.end());

        const Generator_System & min_gs = ph.minimized_generators();

        nout << " " << std::distance(min_gs.begin(), min_gs.end());
        nout << endl;
    }

    return true;
}

} // namespace

BEGIN_MAIN
    DO_TEST(test01);
END_MAIN

