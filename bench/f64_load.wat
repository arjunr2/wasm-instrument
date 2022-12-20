(module
  (func $test (result f64)
    (i32.const 6806)
    (f64.load offset=4)
    (return)
  )
  (memory (;0;) 2 4)
  (data (;2;) (i32.const 6808) "\0a\00\ef\be\ad\de34\7d4au\02\43")
  (export "memory" (memory 0))
  (export "main" (func $test))
)
    
