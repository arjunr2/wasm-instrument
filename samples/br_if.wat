(module
  (func $test (param i32 f64 i32) (result f64)
    (local i32 f64)
    (f64.const -25.43)
    (local.tee 4)
    (local.get 1)
    (f64.add)
    block
      local.get 0
      br_if 0
      local.get 1
      f64.const 2
      f64.mul
      local.set 1
    end
    (f64.const 32.34)
    (f64.mul)
    (local.get 1)
    (f64.mul)
    (return)
  )
  (memory (;0;) 2 4)
  (export "memory" (memory 0))
  (export "main" (func $test))
)
    
