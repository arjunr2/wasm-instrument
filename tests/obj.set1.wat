(module
  (import "weewasm" "obj.new" (func $n (result externref)))
  (import "weewasm" "obj.set" (func $s (param externref externref externref)))
  (func (export "main") (result i32)
    (local $f externref)
    (call $s (call $n) (local.get $f) (local.get $f))
    (i32.const 55)
  )
)
