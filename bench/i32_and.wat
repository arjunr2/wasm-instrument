(module
  (func $test (result i32)
    (i32.and (i32.const 4263634) (i32.const 244241))
    (return)
  )
  (memory (;0;) 2 4)
  (export "memory" (memory 0))
  (export "main" (func $test))
)
    
