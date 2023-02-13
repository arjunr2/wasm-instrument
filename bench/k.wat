(module
  (type $ft (func (param i32) (result i32)))
  (type $ft2 (func (param i32) (result f64)))
  (import "spectest" "print_i32" (func $print_i32))
  (import "mytest" "print_f64" (func $print_f64 (param i32 externref) (result externref)))
  (func $print_i64 (param f64 i32) (result i32)
    (i32.add (local.get 1) (i32.const -30))
    (f64.add (local.get 0) (f64.const -40))
    (drop)
    (return)
  )
  (func $hello_world (param i32) (result i32)
    (local i32 f64 f64 i32)
    (i32.add (local.get 0) (i32.const 8))
    (f64.add (local.get 2) (f64.const 9))
    (f64.add (local.get 2) (f64.const 10)) ;; i32 f64 f64
    (local.set 3)
    (i32.add (local.get 0) (i32.const 11))
    (local.set 1)
    (i32.add (local.get 1) (i32.const 12)) ;; i32 f64 i32
    (call $print_i64) ;; i32 i32
    (i32.add)

    (i32.const 3398)
    (i32.const 612406984)
    (i32.store8 offset=10)

    (i32.const 3398)
    (i32.load offset=10)
    (drop)
    (i32.const 3398)
    (i32.load8_s offset=10)
    (drop)
    (i32.const 3398)
    (i32.load8_u offset=10)
    (drop)
    (i32.const 3398)
    (i32.load16_s offset=10)
    (drop)
    (i32.const 3398)
    (i32.load16_u offset=10)
    (drop)

    (i32.const 3398)
    (i32.const 612406984)
    (i32.store16 offset=10)

    (i32.const 3398)
    (i32.load offset=10)
    (drop)
    (i32.const 3398)
    (i32.load8_s offset=10)
    (drop)
    (i32.const 3398)
    (i32.load8_u offset=10)
    (drop)
    (i32.const 3398)
    (i32.load16_s offset=10)
    (drop)
    (i32.const 3398)
    (i32.load16_u offset=10)
    (drop)

    (i32.const 3398)
    (i32.const 612406984)
    (i32.store offset=10)

    (i32.const 3398)
    (i32.load offset=10)
    (drop)
    (i32.const 3398)
    (i32.load8_s offset=10)
    (drop)
    (i32.const 3398)
    (i32.load8_u offset=10)
    (drop)
    (i32.const 3398)
    (i32.load16_s offset=10)
    (drop)
    (i32.const 3398)
    (i32.load16_u offset=10)
    (drop)

    (i32.const 3470)
    (f64.const -30)
    (f64.store offset=25)

    (i32.const 3485)
    (f64.load offset=10)
    (drop)


    (i32.const 612406984)
    (i32.popcnt)
    (i32.const -1)
    (i32.popcnt)
    (i32.const 612406984)
    (i32.clz)
    (i32.const 0)
    (i32.clz)
    (i32.const 612406984)
    (i32.ctz)
    (i32.const 0)
    (i32.ctz)

    (drop)
    (drop)
    (drop)
    (drop)
    (drop)
    (drop)

    (f64.const -632.1)
    (i32.trunc_f64_s)
    (drop)
    (f64.const 632.1)
    (i32.trunc_f64_u)
    (drop)

    (i32.const 2759890632)
    (f64.convert_i32_s)
    (drop)
    (i32.const 2759890632)
    (f64.convert_i32_u)
    (drop)

    (i32.div_s (i32.const 0) (i32.const 35))
    (drop)

    (i32.const 8) ;; i32 i32
    (call_indirect 0 (type $ft2)) ;; i32 f64
    (i32.trunc_f64_u) ;; i32 i32
    (i32.const 33)
    (i32.sub)
    (return)
  )
  (func $hello_world2 (param i32) (result f64)
    (local f64 i32 f64 i32)
    block
      i32.const 2
      i32.const 3
      i32.add
      loop
        i32.const 5
        i32.const 3
        i32.div_u
        br_if 0
      end
      i32.const 45
      block
        i32.const 0
        br_if 0
        local.get 2
        drop
      end
      drop
      drop
    end
    (i32.add (local.get 0) (i32.const 8))
    (f64.add (local.get 1) (f64.const 9))
    (f64.add (local.get 1) (f64.const 10))
    (local.set 3)
    (i32.add (local.get 0) (i32.const 11))
    (local.set 2)
    (i32.add (local.get 2) (i32.const 12)) ;; i32 f64 i32
    (call $print_i64) ;; i32 i32
    (local.set 4)
    (drop)
    (f64.add (local.get 1) (f64.const 10))
    (return)
  )
  (table (;0;) 10 10 funcref)
  (elem (i32.const 6) $hello_world $print_i32 $hello_world2 $print_i32)
  (memory (;0;) 2 4)
  (global (;0;) (mut i32) (i32.const 22))
  (global (;1;) f64 (f64.const 3408))
  (global (;2;) i32 (i32.const 5))
  (export "memory" (memory 0))
  (export "main" (func $hello_world))
  (export "main2" (func $hello_world2))
  (export "glob" (global 2))
  (export "hello_world2" (func $hello_world2))
  ;;(data (;0;) (i32.const 1024) "%d\0a\00Instruction Count: %lu\0a\00\00\00\00\00\00\19\12D;\02?,G\14=30\0a\1b\06FKE7\0fI\0e\17\03@\1d<+6\1fJ-\1c\01 %)!\08\0c\15\16\22.\108>\0b41\18/A\099\11#C2B:\05\04&('\0d*\1e5\07\1aH\13$L\ff\00\00Success\00Illegal byte sequence\00Domain error\00Result not representable\00Not a tty\00Permission denied\00Operation not permitted\00No such file or directory\00No such process\00File exists\00Value too large for data type\00No space left on device\00Out of memory\00Resource busy\00Interrupted system call\00Resource temporarily unavailable\00Invalid seek\00Cross-device link\00Read-only file system\00Directory not empty\00Connection reset by peer\00Operation timed out\00Connection refused\00Host is unreachable\00Address in use\00Broken pipe\00I/O error\00No such device or address\00No such device\00Not a directory\00Is a directory\00Text file busy\00Exec format error\00Invalid argument\00Argument list too long\00Symbolic link loop\00Filename too long\00Too many open files in system\00No file descriptors available\00Bad file descriptor\00No child process\00Bad address\00File too large\00Too many links\00No locks available\00Resource deadlock would occur\00State not recoverable\00Previous owner died\00Operation canceled\00Function not implemented\00No message of desired type\00Identifier removed\00Link has been severed\00Protocol error\00Bad message\00Not a socket\00Destination address required\00Message too large\00Protocol wrong type for socket\00Protocol not available\00Protocol not supported\00Not supported\00Address family not supported by protocol\00Address not available\00Network is down\00Network unreachable\00Connection reset by network\00Connection aborted\00No buffer space available\00Socket is connected\00Socket not connected\00Operation already in progress\00Operation in progress\00Stale file handle\00Quota exceeded\00Multihop attempted\00Capabilities insufficient\00No error information\00\00-+   0X0x\00(null)\00\00\00\00\00\00\00\00\00\00\19\00\0a\00\19\19\19\00\00\00\00\05\00\00\00\00\00\00\09\00\00\00\00\0b\00\00\00\00\00\00\00\00\19\00\11\0a\19\19\19\03\0a\07\00\01\1b\09\0b\18\00\00\09\06\0b\00\00\0b\00\06\19\00\00\00\19\19\19\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\0e\00\00\00\00\00\00\00\00\19\00\0a\0d\19\19\19\00\0d\00\00\02\00\09\0e\00\00\00\09\00\0e\00\00\0e\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\0c\00\00\00\00\00\00\00\00\00\00\00\13\00\00\00\00\13\00\00\00\00\09\0c\00\00\00\00\00\0c\00\00\0c\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\10\00\00\00\00\00\00\00\00\00\00\00\0f\00\00\00\04\0f\00\00\00\00\09\10\00\00\00\00\00\10\00\00\10\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\12\00\00\00\00\00\00\00\00\00\00\00\11\00\00\00\00\11\00\00\00\00\09\12\00\00\00\00\00\12\00\00\12\00\00\1a\00\00\00\1a\1a\1a\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\1a\00\00\00\1a\1a\1a\00\00\00\00\00\00\09\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\14\00\00\00\00\00\00\00\00\00\00\00\17\00\00\00\00\17\00\00\00\00\09\14\00\00\00\00\00\14\00\00\14\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\16\00\00\00\00\00\00\00\00\00\00\00\15\00\00\00\00\15\00\00\00\00\09\16\00\00\00\00\00\16\00\00\16\00\00Support for formatting long double values is currently disabled.\0aTo enable it, add -lc-printscan-long-double to the link command.\0a\00\00\00\00\00\00\00\00\00\00\00\00\00\000123456789ABCDEF-0X+0X 0X-0x+0x 0x\00inf\00INF\00nan\00NAN\00.\00\00\00\00X\0d\00\00\d0\0d\00\00")
  (data (;1;) (i32.const 3408) "\7f\be\ad\de\00\00\00\00\05\00\00\00\00\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\02\00\00\00\03\00\00\00\88\0e\00\00\00\04\00\00\00\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\0a\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00X\0d\00\00\00\00\00\00\05\00\00\00\00\00\00\00\00\00\00\00\01\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\04\00\00\00\03\00\00\00\94\12\00\00\00\00\00\00\00\00\00\00\00\00\00\00\02\00\00\00\00\00\00\00\ff\ff\ff\ff\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\d0\0d\00\00")
  (data (;2;) (i32.const 6808) "%d\0a\00Instruction Count: %lu")
)
    
