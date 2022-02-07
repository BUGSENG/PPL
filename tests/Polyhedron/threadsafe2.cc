/* Tests library thread safety: interrupting a working thread.
   Copyright (C) 2016 Enea Zaffanella <enea.zaffanella@unipr.it>
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
#include <algorithm>
#include <chrono>
#include <future>
#include <sstream>
#include <thread>

namespace
{

thread_local int worker_index;

struct My_Throwable : public Throwable
{
    void throw_me () const override
    {
        std::stringstream ss;

        ss << "Worker thread " << worker_index << ": ";
        ss << "expensive computation abandoned";
        throw ss.str();
    }

};

dimension_type
build_polyhedron (dimension_type space_dim)
{
    C_Polyhedron ph(space_dim, UNIVERSE);

    for (dimension_type i = 0; i < space_dim; ++i)
    {
        ph.add_constraint(Variable(i) >= 0);
        ph.add_constraint(Variable(i) <= 1);
    }

    const Generator_System & gs = ph.generators();

    return std::distance(gs.begin(), gs.end());
}

std::string
worker_code (int index, dimension_type space_dim)
{
    worker_index = index;

    std::stringstream ss;

    ss << "Worker thread " << worker_index << ": ";

    const dimension_type num_gens = build_polyhedron(space_dim);

    ss << "hypercube of dimension " << space_dim
       << " has " << num_gens << " generators";
    return ss.str();
}

bool
test01 ()
{
    std::vector<std::thread>               threads;
    std::vector<std::future<std::string> > futures;

    // Note: we need to propagate exceptions from the worker thread(s)
    // to the master thread. Use packaged_task/future.
    using Task = std::packaged_task<std::string (int, dimension_type)>;

    nout << "Main thread: launching 10 worker threads\n";

    for (int index = 0; index < 10; ++index)
    {
        Task task(make_interruptible(worker_code));

        futures.push_back(task.get_future());

        const dimension_type space_dim = index + 5;

        threads.push_back(std::thread(std::move(task), index, space_dim));
    }

    nout << "Main thread: some sleeping going on\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(25));

    nout << "Main thread: requesting interruption of worker threads\n";

    My_Throwable throwable;

    for (int index = 0; index < 10; ++index)
    {
        nout << "  Call to interrupt_thread for worker " << index;

        if (interrupt_thread(threads[index], &throwable))
        {
            nout << " succeded\n";
        }
        else
        {
            nout << " failed (hint: worker already finished?)\n";
        }
    }

    nout << "Main thread: getting results from worker threads\n";

    for (int index = 0; index < 10; ++index)
    {
        try
        {
            std::string result = futures[index].get();

            nout << "  " << result;
            threads[index].join();
        }
        catch (const std::string & s)
        {
            nout << "  Exception caught (" << s << ")";

            if (threads[index].joinable())
            {
                threads[index].join();
            }
        }
        nout << std::endl;
    }

    return true;
}

} // namespace

BEGIN_MAIN
    DO_TEST(test01);
END_MAIN

