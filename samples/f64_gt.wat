(module
  (func $test (result i32)
    (f64.gt (f64.const 2.35) (f64.const 2.35))
    (f64.gt (f64.const 2.37) (f64.const 2.36))
    (i32.const 2)
    (i32.mul)
    (i32.add)
    (return)
  )
  (memory (;0;) 2 4)
  (export "memory" (memory 0))
  (export "main" (func $test))
)
    
