(module
  (func $test (result f64)
    (f64.const -54.54)
    (i32.const 42)
    (drop)

    (i32.const 42)
    (f64.const -54.54)
    (drop)

    (f64.convert_i32_u)
    (f64.mul)

    (return)
  )
  (memory (;0;) 2 4)
  (export "memory" (memory 0))
  (export "main" (func $test))
)
    
