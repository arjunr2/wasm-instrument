(module
  (func $test (result f64)
    (f64.const 32.43)
    (f64.const -75.2)
    (i32.const 1)
    (select)

    (f64.const 32.43)
    (f64.const -75.2)
    (i32.const 0)
    (select)

    (f64.const 2)
    (f64.mul)
    (f64.add)

    (return)
  )
  (memory (;0;) 2 4)
  (export "memory" (memory 0))
  (export "main" (func $test))
)
    
