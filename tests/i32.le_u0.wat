(module
  (func (export "main") (param i32 i32) (result i32)
    (i32.le_u (local.get 0) (local.get 1))
  )
)
