(module
  (func $test (result i32)
    (i32.extend16_s (i32.const 168))
    (i32.extend16_s (i32.const 27449))
    (i32.add)
    (return)
  )
  (memory (;0;) 2 4)
  (export "memory" (memory 0))
  (export "main" (func $test))
)
    
