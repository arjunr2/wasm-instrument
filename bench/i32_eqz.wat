(module
  (func $test (result i32)
    (i32.eqz (i32.const 32462))
    (i32.eqz (i32.const 0))
    (i32.const 2)
    (i32.mul)
    (i32.add)
    (return)
  )
  (memory (;0;) 2 4)
  (export "memory" (memory 0))
  (export "main" (func $test))
)
    
