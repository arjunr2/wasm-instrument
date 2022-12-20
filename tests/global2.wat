(module
  (global $g (mut i32) (i32.const 62))
  (func (export "main") (param i32) (result i32)
    (global.set $g (local.get 0))
    (global.get $g)
  )
)
