(module
  (memory 10)
  (func (export "main") (param i32) (result i32)
    (i32.load offset=655300 (local.get 0))
  )
)
