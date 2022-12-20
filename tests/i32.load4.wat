(module
  (memory 1)
  (data (i32.const 4) "xyzw")
  (func (export "main") (param i32) (result i32)
    (i32.load (local.get 0))
  )
)
