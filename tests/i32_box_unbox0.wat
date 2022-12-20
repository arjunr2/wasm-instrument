(module
  (import "weewasm" "obj.box_i32" (func $b (param i32) (result externref)))
  (import "weewasm" "i32.unbox" (func $u (param externref) (result i32)))
  (func (export "main") (param i32) (result i32)
    (call $u (call $b (local.get 0)))
  )
)
