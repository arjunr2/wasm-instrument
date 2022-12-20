(module
  (import "weewasm" "obj.box_i32" (func $b (param i32) (result externref)))
  (import "weewasm" "obj.eq" (func $f (param externref externref) (result i32)))
  (func (export "main") (param i32) (result i32)
    (call $f (call $b (i32.const -44)) (call $b (local.get 0)))
  )
)
