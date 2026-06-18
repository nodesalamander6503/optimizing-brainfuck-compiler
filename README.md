# Brainfuck Compiler / Interpreter (IR-based)

This is a single-file Brainfuck interpreter written in C with a simple intermediate representation (IR) and an experimental optimization pass.

The pipeline is simple. Brainfuck source is parsed into a simple IR (represented by `struct op`), which then goes through an optional optimization pass, and is finally executed via interpreter.

The IR is currently tape-based and supports the following operations:
- cell increment/decrement
- pointer movement
- conditional absolute indexed jump instructions
- input/output

Loop jumps are precomputed before execution for efficiency. Optimization is currently optional and only performs local transformations (e.g., folding consecutive increments/decrements into arithmetic operations). Optimizations may interact with jump structure and are considered experimental.

# Current features

- Brainfuck interpreter implemented in C
- Intermediate representation (IR) layer
- Precomputed jump addresses (as an alternative to scanning until a matching `[` or `]` is found)
- Basic optimization pass (run-length folding)

# Limitations

- IR is tape-based, not SSA or variable-based
- Optimizations are minimal and not fully verified against all control-flow interactions
- Jump precomputation assumes unmodified instruction layout

Summarily, this is not intended for production use. Fortunately, as a Brainfuck compiler, there's no reason anyone would *want* a production version of it.

# Design goal

This project is an exploration of simple compiler construction techniques (IR design, basic optimization, and execution models) in a minimal Brainfuck compiler.

