(module
  (func $test (result i32)
    (i32.const 42)
    (f64.const -54.54)
    (drop)

    (f64.const -54.54)
    (i32.const 42)
    (drop)

    (i32.trunc_f64_s)
    (i32.mul)

    (return)
  )
  (memory (;0;) 2 4)
  (export "memory" (memory 0))
  (export "main" (func $test))
)
    
