(module
  (func $test (result i32)
    (i32.trunc_f64_s (f64.const 26.23))
    (i32.trunc_f64_s (f64.const -8.89))
    (i32.add)
    (return)
  )
  (memory (;0;) 2 4)
  (export "memory" (memory 0))
  (export "main" (func $test))
)
    
