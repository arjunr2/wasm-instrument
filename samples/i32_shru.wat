(module
  (func $test (result i32)
    (i32.shr_u (i32.const 2457323421) (i32.const 4))
    (return)
  )
  (memory (;0;) 2 4)
  (export "memory" (memory 0))
  (export "main" (func $test))
)
    
