(module
  (func $test (param i32 i32) (result i32)
    (i32.div_s (local.get 0) (local.get 1))
    (return)
  )
  (memory (;0;) 2 4)
  (export "memory" (memory 0))
  (export "main" (func $test))
)
    
