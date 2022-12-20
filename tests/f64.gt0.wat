(module
  (func (export "main") (param f64 f64) (result i32)
    (f64.gt (local.get 0) (local.get 1))
  )
)
