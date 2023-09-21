(module
  (func $test (result f64)
    (f64.const 25.23)
    (return)
  )
  (memory (;0;) 2 4)
  (export "memory" (memory 0))
  (export "main" (func $test))
)
    
