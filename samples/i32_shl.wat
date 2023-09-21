(module
  (func $test (result i32)
    (i32.shl (i32.const 345732) (i32.const 20))
    (return)
  )
  (memory (;0;) 2 4)
  (export "memory" (memory 0))
  (export "main" (func $test))
)
    
