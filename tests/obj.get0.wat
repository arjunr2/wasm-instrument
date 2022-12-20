(module
  (import "weewasm" "obj.get" (func $f (param externref externref) (result externref)))
  (func (export "main") (result i32)
    (local $f externref)
    (drop (call $f (local.get $f) (local.get $f)))
    (i32.const 47)
  )
)
