(module
  (func (export "main") (param f64 f64) (result f64)
    (f64.mul (local.get 0) (local.get 1))
  )
)
