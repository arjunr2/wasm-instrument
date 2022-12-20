(module
  (func $test (result i32)
    (i32.xor (i32.const 324436325) (i32.const 594387))
    (return)
  )
  (memory (;0;) 2 4)
  (export "memory" (memory 0))
  (export "main" (func $test))
)
    
