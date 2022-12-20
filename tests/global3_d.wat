(module
  (global $g (mut f64) (f64.const 62))
  (func (export "main") (param f64) (result f64)
    (global.set $g (local.get 0))
    (global.get $g)
  )
)
