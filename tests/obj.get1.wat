(module
  (import "weewasm" "obj.new" (func $n (result externref)))
  (import "weewasm" "obj.get" (func $g (param externref externref) (result externref)))
  (func (export "main") (result externref)
    (local $f externref)
    (call $g (call $n) (local.get $f))
  )
)
