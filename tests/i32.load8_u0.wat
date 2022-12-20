(module
  (memory 1)
  (data (i32.const 504) "xyzw")
  (func (export "main") (param i32) (result i32)
    (i32.load8_u (local.get 0))
  )
)
