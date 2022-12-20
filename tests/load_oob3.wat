(module
  (memory 2)
  (data (i32.const 131068) "\22\33\44\55")
  (func (export "main") (param i32) (result i32)
    (i32.load offset=65536 (local.get 0))
  )
)
