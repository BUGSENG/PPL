open Ppl_ocaml_types
open Gmp
type mip_problem

external ppl_new_MIP_Problem_from_space_dimension:
  int -> mip_problem = "ppl_new_MIP_Problem_from_space_dimension"

external ppl_new_MIP_Problem:
      int -> constraint_system -> linear_expression
	-> optimization_mode -> mip_problem
	  = "ppl_new_MIP_Problem"

external ppl_MIP_Problem_space_dimension:
  mip_problem -> int = "ppl_MIP_Problem_space_dimension"

external ppl_MIP_Problem_constraints:
  mip_problem -> constraint_system = "ppl_MIP_Problem_constraints"

external ppl_MIP_Problem_add_space_dimensions_and_embed:
  mip_problem -> int -> unit
      = "ppl_MIP_Problem_add_space_dimensions_and_embed"

external ppl_MIP_Problem_add_to_integer_space_dimensions:
  mip_problem -> int list -> unit
      = "ppl_MIP_Problem_add_to_integer_space_dimensions"

external ppl_MIP_Problem_add_constraint:
  mip_problem -> linear_constraint -> unit
      = "ppl_MIP_Problem_add_constraint"

external ppl_MIP_Problem_add_constraints:
  mip_problem -> constraint_system -> unit
      = "ppl_MIP_Problem_add_constraints"

external ppl_MIP_Problem_set_objective_function:
  mip_problem -> linear_expression -> unit
      = "ppl_MIP_Problem_set_objective_function"

external ppl_MIP_Problem_is_satisfiable:
  mip_problem -> bool
      = "ppl_MIP_Problem_is_satisfiable"

external ppl_MIP_Problem_solve:
  mip_problem -> mip_problem_status
      = "ppl_MIP_Problem_solve"

external ppl_MIP_Problem_optimization_mode:
  mip_problem -> optimization_mode
      = "ppl_MIP_Problem_optimization_mode"

external ppl_MIP_Problem_feasible_point:
  mip_problem -> linear_generator
      = "ppl_MIP_Problem_feasible_point"

external ppl_MIP_Problem_optimizing_point:
  mip_problem -> linear_generator
      = "ppl_MIP_Problem_optimizing_point"

external ppl_MIP_Problem_objective_function:
  mip_problem -> linear_expression
      = "ppl_MIP_Problem_objective_function"

external ppl_MIP_Problem_optimal_value:
  mip_problem -> Z.t * Z.t
      = "ppl_MIP_Problem_optimal_value"

external ppl_MIP_Problem_evaluate_objective_function:
  mip_problem -> linear_generator  -> Z.t * Z.t
      = "ppl_MIP_Problem_evaluate_objective_function"

external ppl_MIP_Problem_OK:
  mip_problem -> bool
      = "ppl_MIP_Problem_OK"

external ppl_MIP_Problem_clear:
  mip_problem -> unit
      = "ppl_MIP_Problem_clear"

external ppl_MIP_Problem_set_optimization_mode:
  mip_problem -> optimization_mode -> unit
      = "ppl_MIP_Problem_set_optimization_mode"

external ppl_MIP_Problem_swap:
  mip_problem -> mip_problem -> unit
      = "ppl_MIP_Problem_swap"

external test_linear_expression:
  linear_expression -> unit = "test_linear_expression"

external test_linear_constraint:
  linear_constraint -> unit = "test_linear_constraint"

external test_linear_generator:
  linear_generator -> unit = "test_linear_generator"

external test_constraint_system:
  constraint_system -> unit = "test_constraint_system"

external test_generator_system:
  generator_system -> unit = "test_generator_system"
