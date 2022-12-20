(module
  (import "weewasm" "obj.new" (func $n (result externref)))
  (import "weewasm" "obj.get" (func $g (param externref externref) (result externref)))
  (import "weewasm" "obj.set" (func $s (param externref externref externref)))
  (import "weewasm" "obj.box_i32" (func $b (param i32) (result externref)))
  (import "weewasm" "i32.unbox" (func $u (param externref) (result i32)))
  (func (export "main") (param i32) (result i32)
    (local $itr i32)
    (local $obj externref)
    (local $val i32)
    (local $retval i32)

    (local.set $itr (local.get 0))
    (local.set $obj  (call $n))

    (loop $loop
      (local.set $val (i32.mul (i32.const 33) (local.get $itr)))
      ;; Store obj[itr] = itr * val
      (call $s (local.get $obj) (call $b (local.get $itr)) (call $b (local.get $val))) 
      ;; retval = retval + val
      (local.set $retval (i32.add (local.get $retval) (local.get $val)))
      ;; itr -= 1
      (local.set $itr (i32.sub (local.get $itr) (i32.const 1)))
      local.get $itr
      br_if $loop
    )

    (call $u (call $g (local.get $obj) (call $b (i32.const 5))))
    (call $u (call $g (local.get $obj) (call $b (i32.const 11))))
    (i32.add)
    (call $u (call $g (local.get $obj) (call $b (i32.const 2))))
    (i32.add)
    ;;local.get $retval
  )
)
