(module
  (func $test (result i32)
    (i32.rotr (i32.const 432023521) (i32.const 12))
    (return)
  )
  (memory (;0;) 2 4)
  (export "memory" (memory 0))
  (export "main" (func $test))
)
    
