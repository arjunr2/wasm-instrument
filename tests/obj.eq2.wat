(module
  (import "weewasm" "obj.box_f64" (func $b (param f64) (result externref)))
  (import "weewasm" "obj.eq" (func $f (param externref externref) (result i32)))
  (func (export "main") (param f64) (result i32)
    (call $f (call $b (f64.const -49.25)) (call $b (local.get 0)))
  )
)
