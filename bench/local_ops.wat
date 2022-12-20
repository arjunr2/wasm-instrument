(module
  (func $test (param i32 f64 i32) (result f64)
    (local i32 f64)
    (f64.const -25.43)
    (local.tee 4)
    (local.get 1)
    (f64.add)
    (return)
  )
  (memory (;0;) 2 4)
  (export "memory" (memory 0))
  (export "main" (func $test))
)
    
