(module
  (func $test (result f64)
    (f64.convert_i32_s (i32.const 26))
    (f64.convert_i32_u (i32.const 8))
    (f64.add)
    (return)
  )
  (memory (;0;) 2 4)
  (export "memory" (memory 0))
  (export "main" (func $test))
)
    
