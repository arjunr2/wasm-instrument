(module
  (func $test (param i32 f64 i32) (result f64)
    (local i32 f64)
    (f64.const -25.34)
    (local.set 4)
    (local.get 4)
    (return)
  )
  (memory (;0;) 2 4)
  (export "memory" (memory 0))
  (export "main" (func $test))
)
    
