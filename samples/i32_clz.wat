(module
  (func $test (result i32)
    (i32.clz (i32.const 0))
    (i32.clz (i32.const 3242352))
    (i32.add)
    (return)
  )
  (memory (;0;) 2 4)
  (export "memory" (memory 0))
  (export "main" (func $test))
)
    
