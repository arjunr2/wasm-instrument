(module
  (func $test (result i32)
    (i32.div_s (i32.const -672) (i32.const 5))
    (return)
  )
  (memory (;0;) 2 4)
  (export "memory" (memory 0))
  (export "main" (func $test))
)
    
