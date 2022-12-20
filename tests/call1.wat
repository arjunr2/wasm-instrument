(module
  (func $id (param i32) (result i32) (local.get 0))
  (func (export "main") (param i32) (result i32)
    (call $id (local.get 0))
  )
)
