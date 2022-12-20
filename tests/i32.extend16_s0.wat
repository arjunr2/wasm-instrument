(module
  (func (export "main") (param i32) (result i32)
    (i32.extend16_s (local.get 0))
  )
)
