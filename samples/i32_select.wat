(module
  (func $test (result i32)
    (i32.const 7)
    (i32.const -35)
    (i32.const 0)
    (select)

    (i32.const 7)
    (i32.const -35)
    (i32.const 1)
    (select)

    (i32.const 2)
    (i32.mul)
    (i32.add)

    (return)
  )
  (memory (;0;) 2 4)
  (export "memory" (memory 0))
  (export "main" (func $test))
)
    
