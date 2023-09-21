(module
  (func $test (result f64)
    (f64.mul (f64.const 26.3) (f64.const -91.87))
    (return)
  )
  (memory (;0;) 2 4)
  (export "memory" (memory 0))
  (export "main" (func $test))
)
    
