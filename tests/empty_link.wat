(module
  (type (;0;) (func))
  (type (;1;) (func (param i32)))
  (type (;2;) (func (param i32 i32 i32) (result i32)))
  (type (;3;) (func (param i32) (result i32)))
  (type (;4;) (func (param i32 i32) (result i32)))
  (type (;5;) (func (param i32 i32 i32 i32 i32 i32) (result i32)))
  (type (;6;) (func (param i32 i32 i32 i32) (result i32)))
  (type (;7;) (func (param i32 i32 i32 i32 i32) (result i32)))
  (type (;8;) (func (result i32)))
  (import "env" "__wasm_call_dtors" (func $__wasm_call_dtors (type 0)))
  (import "env" "__wasi_proc_exit" (func $__wasi_proc_exit (type 1)))
  (import "wali" "__syscall_SYS_unlinkat" (func $__syscall_SYS_unlinkat (type 2)))
  (import "wali" "__syscall_SYS_read" (func $__syscall_SYS_read (type 2)))
  (import "wali" "__syscall_SYS_write" (func $__syscall_SYS_write (type 2)))
  (import "wali" "__syscall_SYS_open" (func $__syscall_SYS_open (type 2)))
  (import "wali" "__syscall_SYS_close" (func $__syscall_SYS_close (type 3)))
  (import "wali" "__syscall_SYS_stat" (func $__syscall_SYS_stat (type 4)))
  (import "wali" "__syscall_SYS_fstat" (func $__syscall_SYS_fstat (type 4)))
  (import "wali" "__syscall_SYS_lstat" (func $__syscall_SYS_lstat (type 4)))
  (import "wali" "__syscall_SYS_poll" (func $__syscall_SYS_poll (type 2)))
  (import "wali" "__syscall_SYS_lseek" (func $__syscall_SYS_lseek (type 2)))
  (import "wali" "__syscall_SYS_mmap" (func $__syscall_SYS_mmap (type 5)))
  (import "wali" "__syscall_SYS_mprotect" (func $__syscall_SYS_mprotect (type 2)))
  (import "wali" "__syscall_SYS_munmap" (func $__syscall_SYS_munmap (type 4)))
  (import "wali" "__syscall_SYS_brk" (func $__syscall_SYS_brk (type 3)))
  (import "wali" "__syscall_SYS_rt_sigaction" (func $__syscall_SYS_rt_sigaction (type 6)))
  (import "wali" "__syscall_SYS_rt_sigprocmask" (func $__syscall_SYS_rt_sigprocmask (type 6)))
  (import "wali" "__syscall_SYS_rt_sigreturn" (func $__syscall_SYS_rt_sigreturn (type 3)))
  (import "wali" "__syscall_SYS_ioctl" (func $__syscall_SYS_ioctl (type 2)))
  (import "wali" "__syscall_SYS_pread64" (func $__syscall_SYS_pread64 (type 6)))
  (import "wali" "__syscall_SYS_pwrite64" (func $__syscall_SYS_pwrite64 (type 6)))
  (import "wali" "__syscall_SYS_readv" (func $__syscall_SYS_readv (type 2)))
  (import "wali" "__syscall_SYS_writev" (func $__syscall_SYS_writev (type 2)))
  (import "wali" "__syscall_SYS_access" (func $__syscall_SYS_access (type 4)))
  (import "wali" "__syscall_SYS_pipe" (func $__syscall_SYS_pipe (type 3)))
  (import "wali" "__syscall_SYS_select" (func $__syscall_SYS_select (type 7)))
  (import "wali" "__syscall_SYS_sched_yield" (func $__syscall_SYS_sched_yield (type 8)))
  (import "wali" "__syscall_SYS_mremap" (func $__syscall_SYS_mremap (type 7)))
  (import "wali" "__syscall_SYS_msync" (func $__syscall_SYS_msync (type 2)))
  (import "wali" "__syscall_SYS_madvise" (func $__syscall_SYS_madvise (type 2)))
  (import "wali" "__syscall_SYS_dup" (func $__syscall_SYS_dup (type 3)))
  (import "wali" "__syscall_SYS_dup2" (func $__syscall_SYS_dup2 (type 4)))
  (import "wali" "__syscall_SYS_nanosleep" (func $__syscall_SYS_nanosleep (type 4)))
  (import "wali" "__syscall_SYS_alarm" (func $__syscall_SYS_alarm (type 3)))
  (import "wali" "__syscall_SYS_getpid" (func $__syscall_SYS_getpid (type 8)))
  (import "wali" "__syscall_SYS_socket" (func $__syscall_SYS_socket (type 2)))
  (import "wali" "__syscall_SYS_connect" (func $__syscall_SYS_connect (type 2)))
  (import "wali" "__syscall_SYS_accept" (func $__syscall_SYS_accept (type 2)))
  (import "wali" "__syscall_SYS_sendto" (func $__syscall_SYS_sendto (type 6)))
  (import "wali" "__syscall_SYS_recvfrom" (func $__syscall_SYS_recvfrom (type 5)))
  (import "wali" "__syscall_SYS_sendmsg" (func $__syscall_SYS_sendmsg (type 2)))
  (import "wali" "__syscall_SYS_recvmsg" (func $__syscall_SYS_recvmsg (type 2)))
  (import "wali" "__syscall_SYS_shutdown" (func $__syscall_SYS_shutdown (type 4)))
  (import "wali" "__syscall_SYS_bind" (func $__syscall_SYS_bind (type 2)))
  (import "wali" "__syscall_SYS_listen" (func $__syscall_SYS_listen (type 4)))
  (import "wali" "__syscall_SYS_getsockname" (func $__syscall_SYS_getsockname (type 2)))
  (import "wali" "__syscall_SYS_getpeername" (func $__syscall_SYS_getpeername (type 2)))
  (import "wali" "__syscall_SYS_setsockopt" (func $__syscall_SYS_setsockopt (type 7)))
  (import "wali" "__syscall_SYS_getsockopt" (func $__syscall_SYS_getsockopt (type 7)))
  (import "wali" "__syscall_SYS_wait4" (func $__syscall_SYS_wait4 (type 6)))
  (import "wali" "__syscall_SYS_kill" (func $__syscall_SYS_kill (type 4)))
  (import "wali" "__syscall_SYS_uname" (func $__syscall_SYS_uname (type 3)))
  (import "wali" "__syscall_SYS_fcntl" (func $__syscall_SYS_fcntl (type 2)))
  (import "wali" "__syscall_SYS_flock" (func $__syscall_SYS_flock (type 4)))
  (import "wali" "__syscall_SYS_fsync" (func $__syscall_SYS_fsync (type 3)))
  (import "wali" "__syscall_SYS_getcwd" (func $__syscall_SYS_getcwd (type 4)))
  (import "wali" "__syscall_SYS_chdir" (func $__syscall_SYS_chdir (type 3)))
  (import "wali" "__syscall_SYS_mkdir" (func $__syscall_SYS_mkdir (type 4)))
  (import "wali" "__syscall_SYS_rmdir" (func $__syscall_SYS_rmdir (type 3)))
  (import "wali" "__syscall_SYS_link" (func $__syscall_SYS_link (type 4)))
  (import "wali" "__syscall_SYS_unlink" (func $__syscall_SYS_unlink (type 3)))
  (import "wali" "__syscall_SYS_symlink" (func $__syscall_SYS_symlink (type 4)))
  (import "wali" "__syscall_SYS_readlink" (func $__syscall_SYS_readlink (type 2)))
  (import "wali" "__syscall_SYS_chmod" (func $__syscall_SYS_chmod (type 4)))
  (import "wali" "__syscall_SYS_fchmod" (func $__syscall_SYS_fchmod (type 4)))
  (import "wali" "__syscall_SYS_chown" (func $__syscall_SYS_chown (type 2)))
  (import "wali" "__syscall_SYS_fchown" (func $__syscall_SYS_fchown (type 2)))
  (import "wali" "__syscall_SYS_umask" (func $__syscall_SYS_umask (type 3)))
  (import "wali" "__syscall_SYS_utime" (func $__syscall_SYS_utime (type 4)))
  (import "wali" "__syscall_SYS_statfs" (func $__syscall_SYS_statfs (type 4)))
  (import "wali" "__syscall_SYS_fstatfs" (func $__syscall_SYS_fstatfs (type 4)))
  (import "wali" "__syscall_SYS_getdents64" (func $__syscall_SYS_getdents64 (type 2)))
  (import "wali" "__syscall_SYS_fadvise" (func $__syscall_SYS_fadvise (type 6)))
  (import "wali" "__syscall_SYS_openat" (func $__syscall_SYS_openat (type 6)))
  (import "wali" "__syscall_SYS_faccessat" (func $__syscall_SYS_faccessat (type 6)))
  (import "wali" "__syscall_SYS_utimensat" (func $__syscall_SYS_utimensat (type 6)))
  (import "wali" "__syscall_SYS_eventfd" (func $__syscall_SYS_eventfd (type 3)))
  (import "wali" "__syscall_SYS_eventfd2" (func $__syscall_SYS_eventfd2 (type 4)))
  (import "wali" "__syscall_SYS_pipe2" (func $__syscall_SYS_pipe2 (type 4)))
  (import "wali" "__syscall_SYS_fork" (func $__syscall_SYS_fork (type 8)))
  (import "wali" "__get_tp" (func $__get_tp (type 8)))
  (func $__wasm_call_ctors (type 0))
  (func $print (type 3) (param i32) (result i32)
    (local i32)
    global.get $__stack_pointer
    i32.const 16
    i32.sub
    local.tee 1
    global.set $__stack_pointer
    local.get 1
    local.get 0
    i32.store offset=12
    local.get 1
    local.get 1
    i32.load offset=12
    call $strlen
    i32.store offset=8
    i32.const 1
    local.get 1
    i32.load offset=12
    local.get 1
    i32.load offset=8
    call $write
    drop
    local.get 1
    i32.load offset=8
    local.set 0
    local.get 1
    i32.const 16
    i32.add
    global.set $__stack_pointer
    local.get 0)
  (func $__original_main (type 8) (result i32)
    (local i32 i32)
    global.get $__stack_pointer
    i32.const 112
    i32.sub
    local.tee 0
    global.set $__stack_pointer
    local.get 0
    i32.const 0
    i32.store offset=108
    local.get 0
    i32.const 1024
    i32.const 100
    call $memcpy
    local.set 0
    i32.const 57
    i32.const 0
    call $syscall
    drop
    local.get 0
    call $print
    drop
    i32.const 0
    i32.load offset=1124
    local.set 1
    local.get 0
    i32.const 112
    i32.add
    global.set $__stack_pointer
    local.get 1)
  (func $_start (type 0)
    (local i32)
    block  ;; label = @1
      block  ;; label = @2
        i32.const 0
        i32.load offset=1128
        br_if 0 (;@2;)
        i32.const 0
        i32.const 1
        i32.store offset=1128
        call $__wasm_call_ctors
        call $__original_main
        local.set 0
        call $__wasm_call_dtors
        local.get 0
        br_if 1 (;@1;)
        return
      end
      unreachable
      unreachable
    end
    local.get 0
    call $__wasi_proc_exit
    unreachable)
  (func $__errno_location (type 8) (result i32)
    call $__get_tp
    i32.const 28
    i32.add)
  (func $__syscall_ret (type 3) (param i32) (result i32)
    block  ;; label = @1
      local.get 0
      i32.const -4095
      i32.lt_u
      br_if 0 (;@1;)
      call $__errno_location
      i32.const 0
      local.get 0
      i32.sub
      i32.store
      i32.const -1
      local.set 0
    end
    local.get 0)
  (func $syscall (type 4) (param i32 i32) (result i32)
    (local i32 i32 i32 i32 i32 i32 i32)
    global.get $__stack_pointer
    i32.const 80
    i32.sub
    local.tee 2
    global.set $__stack_pointer
    local.get 2
    local.get 1
    i32.const 4
    i32.add
    i32.store offset=76
    local.get 1
    i32.load
    local.set 3
    local.get 2
    local.get 1
    i32.const 24
    i32.add
    i32.store offset=76
    local.get 1
    i32.load offset=12
    local.set 4
    local.get 1
    i32.load offset=8
    local.set 5
    local.get 1
    i32.load offset=4
    local.set 6
    i32.const -1
    local.set 7
    block  ;; label = @1
      block  ;; label = @2
        block  ;; label = @3
          block  ;; label = @4
            block  ;; label = @5
              block  ;; label = @6
                block  ;; label = @7
                  block  ;; label = @8
                    block  ;; label = @9
                      block  ;; label = @10
                        block  ;; label = @11
                          block  ;; label = @12
                            block  ;; label = @13
                              block  ;; label = @14
                                block  ;; label = @15
                                  block  ;; label = @16
                                    block  ;; label = @17
                                      block  ;; label = @18
                                        block  ;; label = @19
                                          block  ;; label = @20
                                            block  ;; label = @21
                                              block  ;; label = @22
                                                block  ;; label = @23
                                                  block  ;; label = @24
                                                    block  ;; label = @25
                                                      block  ;; label = @26
                                                        block  ;; label = @27
                                                          block  ;; label = @28
                                                            block  ;; label = @29
                                                              block  ;; label = @30
                                                                block  ;; label = @31
                                                                  block  ;; label = @32
                                                                    block  ;; label = @33
                                                                      block  ;; label = @34
                                                                        block  ;; label = @35
                                                                          block  ;; label = @36
                                                                            block  ;; label = @37
                                                                              block  ;; label = @38
                                                                                block  ;; label = @39
                                                                                  block  ;; label = @40
                                                                                    block  ;; label = @41
                                                                                      block  ;; label = @42
                                                                                        block  ;; label = @43
                                                                                          block  ;; label = @44
                                                                                            block  ;; label = @45
                                                                                              block  ;; label = @46
                                                                                                block  ;; label = @47
                                                                                                  block  ;; label = @48
                                                                                                    block  ;; label = @49
                                                                                                      block  ;; label = @50
                                                                                                        block  ;; label = @51
                                                                                                          block  ;; label = @52
                                                                                                            block  ;; label = @53
                                                                                                              block  ;; label = @54
                                                                                                                block  ;; label = @55
                                                                                                                  block  ;; label = @56
                                                                                                                    block  ;; label = @57
                                                                                                                      block  ;; label = @58
                                                                                                                        block  ;; label = @59
                                                                                                                          block  ;; label = @60
                                                                                                                            block  ;; label = @61
                                                                                                                              block  ;; label = @62
                                                                                                                                block  ;; label = @63
                                                                                                                                  block  ;; label = @64
                                                                                                                                    block  ;; label = @65
                                                                                                                                      block  ;; label = @66
                                                                                                                                        block  ;; label = @67
                                                                                                                                          block  ;; label = @68
                                                                                                                                            block  ;; label = @69
                                                                                                                                              block  ;; label = @70
                                                                                                                                                block  ;; label = @71
                                                                                                                                                  block  ;; label = @72
                                                                                                                                                    block  ;; label = @73
                                                                                                                                                      block  ;; label = @74
                                                                                                                                                        block  ;; label = @75
                                                                                                                                                          block  ;; label = @76
                                                                                                                                                            block  ;; label = @77
                                                                                                                                                              block  ;; label = @78
                                                                                                                                                                block  ;; label = @79
                                                                                                                                                                  block  ;; label = @80
                                                                                                                                                                    local.get 0
                                                                                                                                                                    i32.const 216
                                                                                                                                                                    i32.gt_s
                                                                                                                                                                    br_if 0 (;@80;)
                                                                                                                                                                    local.get 1
                                                                                                                                                                    i32.load offset=20
                                                                                                                                                                    local.set 8
                                                                                                                                                                    local.get 1
                                                                                                                                                                    i32.load offset=16
                                                                                                                                                                    local.set 1
                                                                                                                                                                    local.get 0
                                                                                                                                                                    br_table 1 (;@79;) 2 (;@78;) 3 (;@77;) 4 (;@76;) 5 (;@75;) 6 (;@74;) 7 (;@73;) 8 (;@72;) 9 (;@71;) 10 (;@70;) 11 (;@69;) 12 (;@68;) 13 (;@67;) 14 (;@66;) 15 (;@65;) 16 (;@64;) 17 (;@63;) 18 (;@62;) 19 (;@61;) 20 (;@60;) 21 (;@59;) 22 (;@58;) 23 (;@57;) 24 (;@56;) 25 (;@55;) 26 (;@54;) 27 (;@53;) 79 (;@1;) 28 (;@52;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 29 (;@51;) 30 (;@50;) 79 (;@1;) 31 (;@49;) 79 (;@1;) 32 (;@48;) 79 (;@1;) 33 (;@47;) 79 (;@1;) 34 (;@46;) 35 (;@45;) 36 (;@44;) 37 (;@43;) 38 (;@42;) 39 (;@41;) 40 (;@40;) 41 (;@39;) 42 (;@38;) 43 (;@37;) 44 (;@36;) 45 (;@35;) 79 (;@1;) 46 (;@34;) 47 (;@33;) 79 (;@1;) 78 (;@2;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 48 (;@32;) 49 (;@31;) 50 (;@30;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 51 (;@29;) 52 (;@28;) 53 (;@27;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 54 (;@26;) 55 (;@25;) 79 (;@1;) 79 (;@1;) 56 (;@24;) 57 (;@23;) 79 (;@1;) 58 (;@22;) 59 (;@21;) 60 (;@20;) 61 (;@19;) 62 (;@18;) 63 (;@17;) 64 (;@16;) 65 (;@15;) 79 (;@1;) 66 (;@14;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 67 (;@13;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 68 (;@12;) 69 (;@11;) 79 (;@1;)
                                                                                                                                                                  end
                                                                                                                                                                  block  ;; label = @80
                                                                                                                                                                    local.get 0
                                                                                                                                                                    i32.const 268
                                                                                                                                                                    i32.gt_s
                                                                                                                                                                    br_if 0 (;@80;)
                                                                                                                                                                    block  ;; label = @81
                                                                                                                                                                      local.get 0
                                                                                                                                                                      i32.const -217
                                                                                                                                                                      i32.add
                                                                                                                                                                      br_table 71 (;@10;) 80 (;@1;) 80 (;@1;) 80 (;@1;) 72 (;@9;) 0 (;@81;)
                                                                                                                                                                    end
                                                                                                                                                                    local.get 0
                                                                                                                                                                    i32.const 257
                                                                                                                                                                    i32.eq
                                                                                                                                                                    br_if 72 (;@8;)
                                                                                                                                                                    local.get 0
                                                                                                                                                                    i32.const 263
                                                                                                                                                                    i32.ne
                                                                                                                                                                    br_if 79 (;@1;)
                                                                                                                                                                    local.get 3
                                                                                                                                                                    local.get 6
                                                                                                                                                                    local.get 5
                                                                                                                                                                    call $__syscall_SYS_unlinkat
                                                                                                                                                                    local.set 7
                                                                                                                                                                    br 79 (;@1;)
                                                                                                                                                                  end
                                                                                                                                                                  block  ;; label = @80
                                                                                                                                                                    local.get 0
                                                                                                                                                                    i32.const -280
                                                                                                                                                                    i32.add
                                                                                                                                                                    br_table 74 (;@6;) 79 (;@1;) 79 (;@1;) 79 (;@1;) 75 (;@5;) 0 (;@80;)
                                                                                                                                                                  end
                                                                                                                                                                  local.get 0
                                                                                                                                                                  i32.const -290
                                                                                                                                                                  i32.add
                                                                                                                                                                  br_table 75 (;@4;) 78 (;@1;) 78 (;@1;) 76 (;@3;) 72 (;@7;)
                                                                                                                                                                end
                                                                                                                                                                local.get 3
                                                                                                                                                                local.get 6
                                                                                                                                                                local.get 5
                                                                                                                                                                call $__syscall_SYS_read
                                                                                                                                                                local.set 7
                                                                                                                                                                br 77 (;@1;)
                                                                                                                                                              end
                                                                                                                                                              local.get 3
                                                                                                                                                              local.get 6
                                                                                                                                                              local.get 5
                                                                                                                                                              call $__syscall_SYS_write
                                                                                                                                                              local.set 7
                                                                                                                                                              br 76 (;@1;)
                                                                                                                                                            end
                                                                                                                                                            local.get 3
                                                                                                                                                            local.get 6
                                                                                                                                                            local.get 5
                                                                                                                                                            call $__syscall_SYS_open
                                                                                                                                                            local.set 7
                                                                                                                                                            br 75 (;@1;)
                                                                                                                                                          end
                                                                                                                                                          local.get 3
                                                                                                                                                          call $__syscall_SYS_close
                                                                                                                                                          local.set 7
                                                                                                                                                          br 74 (;@1;)
                                                                                                                                                        end
                                                                                                                                                        local.get 3
                                                                                                                                                        local.get 6
                                                                                                                                                        call $__syscall_SYS_stat
                                                                                                                                                        local.set 7
                                                                                                                                                        br 73 (;@1;)
                                                                                                                                                      end
                                                                                                                                                      local.get 3
                                                                                                                                                      local.get 6
                                                                                                                                                      call $__syscall_SYS_fstat
                                                                                                                                                      local.set 7
                                                                                                                                                      br 72 (;@1;)
                                                                                                                                                    end
                                                                                                                                                    local.get 3
                                                                                                                                                    local.get 6
                                                                                                                                                    call $__syscall_SYS_lstat
                                                                                                                                                    local.set 7
                                                                                                                                                    br 71 (;@1;)
                                                                                                                                                  end
                                                                                                                                                  local.get 3
                                                                                                                                                  local.get 6
                                                                                                                                                  local.get 5
                                                                                                                                                  call $__syscall_SYS_poll
                                                                                                                                                  local.set 7
                                                                                                                                                  br 70 (;@1;)
                                                                                                                                                end
                                                                                                                                                local.get 3
                                                                                                                                                local.get 6
                                                                                                                                                local.get 5
                                                                                                                                                call $__syscall_SYS_lseek
                                                                                                                                                local.set 7
                                                                                                                                                br 69 (;@1;)
                                                                                                                                              end
                                                                                                                                              local.get 3
                                                                                                                                              local.get 6
                                                                                                                                              local.get 5
                                                                                                                                              local.get 4
                                                                                                                                              local.get 1
                                                                                                                                              local.get 8
                                                                                                                                              call $__syscall_SYS_mmap
                                                                                                                                              local.set 7
                                                                                                                                              br 68 (;@1;)
                                                                                                                                            end
                                                                                                                                            local.get 3
                                                                                                                                            local.get 6
                                                                                                                                            local.get 5
                                                                                                                                            call $__syscall_SYS_mprotect
                                                                                                                                            local.set 7
                                                                                                                                            br 67 (;@1;)
                                                                                                                                          end
                                                                                                                                          local.get 3
                                                                                                                                          local.get 6
                                                                                                                                          call $__syscall_SYS_munmap
                                                                                                                                          local.set 7
                                                                                                                                          br 66 (;@1;)
                                                                                                                                        end
                                                                                                                                        local.get 3
                                                                                                                                        call $__syscall_SYS_brk
                                                                                                                                        local.set 7
                                                                                                                                        br 65 (;@1;)
                                                                                                                                      end
                                                                                                                                      local.get 3
                                                                                                                                      local.get 6
                                                                                                                                      local.get 5
                                                                                                                                      local.get 4
                                                                                                                                      call $__syscall_SYS_rt_sigaction
                                                                                                                                      local.set 7
                                                                                                                                      br 64 (;@1;)
                                                                                                                                    end
                                                                                                                                    local.get 3
                                                                                                                                    local.get 6
                                                                                                                                    local.get 5
                                                                                                                                    local.get 4
                                                                                                                                    call $__syscall_SYS_rt_sigprocmask
                                                                                                                                    local.set 7
                                                                                                                                    br 63 (;@1;)
                                                                                                                                  end
                                                                                                                                  local.get 3
                                                                                                                                  call $__syscall_SYS_rt_sigreturn
                                                                                                                                  local.set 7
                                                                                                                                  br 62 (;@1;)
                                                                                                                                end
                                                                                                                                local.get 2
                                                                                                                                local.get 5
                                                                                                                                i32.store
                                                                                                                                local.get 3
                                                                                                                                local.get 6
                                                                                                                                local.get 2
                                                                                                                                call $__syscall_SYS_ioctl
                                                                                                                                local.set 7
                                                                                                                                br 61 (;@1;)
                                                                                                                              end
                                                                                                                              local.get 3
                                                                                                                              local.get 6
                                                                                                                              local.get 5
                                                                                                                              local.get 4
                                                                                                                              call $__syscall_SYS_pread64
                                                                                                                              local.set 7
                                                                                                                              br 60 (;@1;)
                                                                                                                            end
                                                                                                                            local.get 3
                                                                                                                            local.get 6
                                                                                                                            local.get 5
                                                                                                                            local.get 4
                                                                                                                            call $__syscall_SYS_pwrite64
                                                                                                                            local.set 7
                                                                                                                            br 59 (;@1;)
                                                                                                                          end
                                                                                                                          local.get 3
                                                                                                                          local.get 6
                                                                                                                          local.get 5
                                                                                                                          call $__syscall_SYS_readv
                                                                                                                          local.set 7
                                                                                                                          br 58 (;@1;)
                                                                                                                        end
                                                                                                                        local.get 3
                                                                                                                        local.get 6
                                                                                                                        local.get 5
                                                                                                                        call $__syscall_SYS_writev
                                                                                                                        local.set 7
                                                                                                                        br 57 (;@1;)
                                                                                                                      end
                                                                                                                      local.get 3
                                                                                                                      local.get 6
                                                                                                                      call $__syscall_SYS_access
                                                                                                                      local.set 7
                                                                                                                      br 56 (;@1;)
                                                                                                                    end
                                                                                                                    local.get 3
                                                                                                                    call $__syscall_SYS_pipe
                                                                                                                    local.set 7
                                                                                                                    br 55 (;@1;)
                                                                                                                  end
                                                                                                                  local.get 3
                                                                                                                  local.get 6
                                                                                                                  local.get 5
                                                                                                                  local.get 4
                                                                                                                  local.get 1
                                                                                                                  call $__syscall_SYS_select
                                                                                                                  local.set 7
                                                                                                                  br 54 (;@1;)
                                                                                                                end
                                                                                                                call $__syscall_SYS_sched_yield
                                                                                                                local.set 7
                                                                                                                br 53 (;@1;)
                                                                                                              end
                                                                                                              local.get 2
                                                                                                              local.get 1
                                                                                                              i32.store offset=16
                                                                                                              local.get 3
                                                                                                              local.get 6
                                                                                                              local.get 5
                                                                                                              local.get 4
                                                                                                              local.get 2
                                                                                                              i32.const 16
                                                                                                              i32.add
                                                                                                              call $__syscall_SYS_mremap
                                                                                                              local.set 7
                                                                                                              br 52 (;@1;)
                                                                                                            end
                                                                                                            local.get 3
                                                                                                            local.get 6
                                                                                                            local.get 5
                                                                                                            call $__syscall_SYS_msync
                                                                                                            local.set 7
                                                                                                            br 51 (;@1;)
                                                                                                          end
                                                                                                          local.get 3
                                                                                                          local.get 6
                                                                                                          local.get 5
                                                                                                          call $__syscall_SYS_madvise
                                                                                                          local.set 7
                                                                                                          br 50 (;@1;)
                                                                                                        end
                                                                                                        local.get 3
                                                                                                        call $__syscall_SYS_dup
                                                                                                        local.set 7
                                                                                                        br 49 (;@1;)
                                                                                                      end
                                                                                                      local.get 3
                                                                                                      local.get 6
                                                                                                      call $__syscall_SYS_dup2
                                                                                                      local.set 7
                                                                                                      br 48 (;@1;)
                                                                                                    end
                                                                                                    local.get 3
                                                                                                    local.get 6
                                                                                                    call $__syscall_SYS_nanosleep
                                                                                                    local.set 7
                                                                                                    br 47 (;@1;)
                                                                                                  end
                                                                                                  local.get 3
                                                                                                  call $__syscall_SYS_alarm
                                                                                                  local.set 7
                                                                                                  br 46 (;@1;)
                                                                                                end
                                                                                                call $__syscall_SYS_getpid
                                                                                                local.set 7
                                                                                                br 45 (;@1;)
                                                                                              end
                                                                                              local.get 3
                                                                                              local.get 6
                                                                                              local.get 5
                                                                                              call $__syscall_SYS_socket
                                                                                              local.set 7
                                                                                              br 44 (;@1;)
                                                                                            end
                                                                                            local.get 3
                                                                                            local.get 6
                                                                                            local.get 5
                                                                                            call $__syscall_SYS_connect
                                                                                            local.set 7
                                                                                            br 43 (;@1;)
                                                                                          end
                                                                                          local.get 3
                                                                                          local.get 6
                                                                                          local.get 5
                                                                                          call $__syscall_SYS_accept
                                                                                          local.set 7
                                                                                          br 42 (;@1;)
                                                                                        end
                                                                                        local.get 3
                                                                                        local.get 6
                                                                                        local.get 5
                                                                                        local.get 4
                                                                                        call $__syscall_SYS_sendto
                                                                                        local.set 7
                                                                                        br 41 (;@1;)
                                                                                      end
                                                                                      local.get 3
                                                                                      local.get 6
                                                                                      local.get 5
                                                                                      local.get 4
                                                                                      local.get 1
                                                                                      local.get 8
                                                                                      call $__syscall_SYS_recvfrom
                                                                                      local.set 7
                                                                                      br 40 (;@1;)
                                                                                    end
                                                                                    local.get 3
                                                                                    local.get 6
                                                                                    local.get 5
                                                                                    call $__syscall_SYS_sendmsg
                                                                                    local.set 7
                                                                                    br 39 (;@1;)
                                                                                  end
                                                                                  local.get 3
                                                                                  local.get 6
                                                                                  local.get 5
                                                                                  call $__syscall_SYS_recvmsg
                                                                                  local.set 7
                                                                                  br 38 (;@1;)
                                                                                end
                                                                                local.get 3
                                                                                local.get 6
                                                                                call $__syscall_SYS_shutdown
                                                                                local.set 7
                                                                                br 37 (;@1;)
                                                                              end
                                                                              local.get 3
                                                                              local.get 6
                                                                              local.get 5
                                                                              call $__syscall_SYS_bind
                                                                              local.set 7
                                                                              br 36 (;@1;)
                                                                            end
                                                                            local.get 3
                                                                            local.get 6
                                                                            call $__syscall_SYS_listen
                                                                            local.set 7
                                                                            br 35 (;@1;)
                                                                          end
                                                                          local.get 3
                                                                          local.get 6
                                                                          local.get 5
                                                                          call $__syscall_SYS_getsockname
                                                                          local.set 7
                                                                          br 34 (;@1;)
                                                                        end
                                                                        local.get 3
                                                                        local.get 6
                                                                        local.get 5
                                                                        call $__syscall_SYS_getpeername
                                                                        local.set 7
                                                                        br 33 (;@1;)
                                                                      end
                                                                      local.get 3
                                                                      local.get 6
                                                                      local.get 5
                                                                      local.get 4
                                                                      local.get 1
                                                                      call $__syscall_SYS_setsockopt
                                                                      local.set 7
                                                                      br 32 (;@1;)
                                                                    end
                                                                    local.get 3
                                                                    local.get 6
                                                                    local.get 5
                                                                    local.get 4
                                                                    local.get 1
                                                                    call $__syscall_SYS_getsockopt
                                                                    local.set 7
                                                                    br 31 (;@1;)
                                                                  end
                                                                  local.get 3
                                                                  local.get 6
                                                                  local.get 5
                                                                  local.get 4
                                                                  call $__syscall_SYS_wait4
                                                                  local.set 7
                                                                  br 30 (;@1;)
                                                                end
                                                                local.get 3
                                                                local.get 6
                                                                call $__syscall_SYS_kill
                                                                local.set 7
                                                                br 29 (;@1;)
                                                              end
                                                              local.get 3
                                                              call $__syscall_SYS_uname
                                                              local.set 7
                                                              br 28 (;@1;)
                                                            end
                                                            local.get 2
                                                            local.get 5
                                                            i32.store offset=32
                                                            local.get 3
                                                            local.get 6
                                                            local.get 2
                                                            i32.const 32
                                                            i32.add
                                                            call $__syscall_SYS_fcntl
                                                            local.set 7
                                                            br 27 (;@1;)
                                                          end
                                                          local.get 3
                                                          local.get 6
                                                          call $__syscall_SYS_flock
                                                          local.set 7
                                                          br 26 (;@1;)
                                                        end
                                                        local.get 3
                                                        call $__syscall_SYS_fsync
                                                        local.set 7
                                                        br 25 (;@1;)
                                                      end
                                                      local.get 3
                                                      local.get 6
                                                      call $__syscall_SYS_getcwd
                                                      local.set 7
                                                      br 24 (;@1;)
                                                    end
                                                    local.get 3
                                                    call $__syscall_SYS_chdir
                                                    local.set 7
                                                    br 23 (;@1;)
                                                  end
                                                  local.get 3
                                                  local.get 6
                                                  call $__syscall_SYS_mkdir
                                                  local.set 7
                                                  br 22 (;@1;)
                                                end
                                                local.get 3
                                                call $__syscall_SYS_rmdir
                                                local.set 7
                                                br 21 (;@1;)
                                              end
                                              local.get 3
                                              local.get 6
                                              call $__syscall_SYS_link
                                              local.set 7
                                              br 20 (;@1;)
                                            end
                                            local.get 3
                                            call $__syscall_SYS_unlink
                                            local.set 7
                                            br 19 (;@1;)
                                          end
                                          local.get 3
                                          local.get 6
                                          call $__syscall_SYS_symlink
                                          local.set 7
                                          br 18 (;@1;)
                                        end
                                        local.get 3
                                        local.get 6
                                        local.get 5
                                        call $__syscall_SYS_readlink
                                        local.set 7
                                        br 17 (;@1;)
                                      end
                                      local.get 3
                                      local.get 6
                                      call $__syscall_SYS_chmod
                                      local.set 7
                                      br 16 (;@1;)
                                    end
                                    local.get 3
                                    local.get 6
                                    call $__syscall_SYS_fchmod
                                    local.set 7
                                    br 15 (;@1;)
                                  end
                                  local.get 3
                                  local.get 6
                                  local.get 5
                                  call $__syscall_SYS_chown
                                  local.set 7
                                  br 14 (;@1;)
                                end
                                local.get 3
                                local.get 6
                                local.get 5
                                call $__syscall_SYS_fchown
                                local.set 7
                                br 13 (;@1;)
                              end
                              local.get 3
                              call $__syscall_SYS_umask
                              local.set 7
                              br 12 (;@1;)
                            end
                            local.get 3
                            local.get 6
                            call $__syscall_SYS_utime
                            local.set 7
                            br 11 (;@1;)
                          end
                          local.get 3
                          local.get 6
                          call $__syscall_SYS_statfs
                          local.set 7
                          br 10 (;@1;)
                        end
                        local.get 3
                        local.get 6
                        call $__syscall_SYS_fstatfs
                        local.set 7
                        br 9 (;@1;)
                      end
                      local.get 3
                      local.get 6
                      local.get 5
                      call $__syscall_SYS_getdents64
                      local.set 7
                      br 8 (;@1;)
                    end
                    local.get 3
                    local.get 6
                    local.get 5
                    local.get 4
                    call $__syscall_SYS_fadvise
                    local.set 7
                    br 7 (;@1;)
                  end
                  local.get 2
                  local.get 4
                  i32.store offset=48
                  local.get 3
                  local.get 6
                  local.get 5
                  local.get 2
                  i32.const 48
                  i32.add
                  call $__syscall_SYS_openat
                  local.set 7
                  br 6 (;@1;)
                end
                local.get 0
                i32.const 269
                i32.ne
                br_if 5 (;@1;)
                local.get 2
                local.get 4
                i32.store offset=64
                local.get 3
                local.get 6
                local.get 5
                local.get 2
                i32.const 64
                i32.add
                call $__syscall_SYS_faccessat
                local.set 7
                br 5 (;@1;)
              end
              local.get 3
              local.get 6
              local.get 5
              local.get 4
              call $__syscall_SYS_utimensat
              local.set 7
              br 4 (;@1;)
            end
            local.get 3
            call $__syscall_SYS_eventfd
            local.set 7
            br 3 (;@1;)
          end
          local.get 3
          local.get 6
          call $__syscall_SYS_eventfd2
          local.set 7
          br 2 (;@1;)
        end
        local.get 3
        local.get 6
        call $__syscall_SYS_pipe2
        local.set 7
        br 1 (;@1;)
      end
      call $__syscall_SYS_fork
      local.set 7
    end
    local.get 7
    call $__syscall_ret
    local.set 1
    local.get 2
    i32.const 80
    i32.add
    global.set $__stack_pointer
    local.get 1)
  (func $memcpy (type 2) (param i32 i32 i32) (result i32)
    (local i32 i32 i32 i32 i32 i32)
    block  ;; label = @1
      block  ;; label = @2
        local.get 1
        i32.const 3
        i32.and
        i32.eqz
        br_if 0 (;@2;)
        local.get 2
        i32.eqz
        br_if 0 (;@2;)
        local.get 1
        i32.const 1
        i32.add
        local.set 3
        local.get 0
        local.set 4
        loop  ;; label = @3
          local.get 4
          local.get 1
          i32.load8_u
          i32.store8
          local.get 2
          i32.const -1
          i32.add
          local.set 5
          local.get 4
          i32.const 1
          i32.add
          local.set 4
          local.get 1
          i32.const 1
          i32.add
          local.set 1
          local.get 3
          i32.const 3
          i32.and
          i32.eqz
          br_if 2 (;@1;)
          local.get 3
          i32.const 1
          i32.add
          local.set 3
          local.get 2
          i32.const 1
          i32.ne
          local.set 6
          local.get 5
          local.set 2
          local.get 6
          br_if 0 (;@3;)
          br 2 (;@1;)
        end
      end
      local.get 2
      local.set 5
      local.get 0
      local.set 4
    end
    block  ;; label = @1
      block  ;; label = @2
        block  ;; label = @3
          local.get 4
          i32.const 3
          i32.and
          local.tee 2
          br_if 0 (;@3;)
          block  ;; label = @4
            local.get 5
            i32.const 16
            i32.lt_u
            br_if 0 (;@4;)
            loop  ;; label = @5
              local.get 4
              local.get 1
              i64.load align=4
              i64.store align=4
              local.get 4
              i32.const 8
              i32.add
              local.get 1
              i32.const 8
              i32.add
              i64.load align=4
              i64.store align=4
              local.get 4
              i32.const 16
              i32.add
              local.set 4
              local.get 1
              i32.const 16
              i32.add
              local.set 1
              local.get 5
              i32.const -16
              i32.add
              local.tee 5
              i32.const 15
              i32.gt_u
              br_if 0 (;@5;)
            end
          end
          block  ;; label = @4
            local.get 5
            i32.const 8
            i32.and
            i32.eqz
            br_if 0 (;@4;)
            local.get 4
            local.get 1
            i64.load align=4
            i64.store align=4
            local.get 1
            i32.const 8
            i32.add
            local.set 1
            local.get 4
            i32.const 8
            i32.add
            local.set 4
          end
          block  ;; label = @4
            local.get 5
            i32.const 4
            i32.and
            i32.eqz
            br_if 0 (;@4;)
            local.get 4
            local.get 1
            i32.load
            i32.store
            local.get 1
            i32.const 4
            i32.add
            local.set 1
            local.get 4
            i32.const 4
            i32.add
            local.set 4
          end
          block  ;; label = @4
            local.get 5
            i32.const 2
            i32.and
            i32.eqz
            br_if 0 (;@4;)
            local.get 4
            local.get 1
            i32.load16_u align=1
            i32.store16 align=1
            local.get 4
            i32.const 2
            i32.add
            local.set 4
            local.get 1
            i32.const 2
            i32.add
            local.set 1
          end
          local.get 5
          i32.const 1
          i32.and
          br_if 1 (;@2;)
          br 2 (;@1;)
        end
        block  ;; label = @3
          local.get 5
          i32.const 32
          i32.lt_u
          br_if 0 (;@3;)
          block  ;; label = @4
            block  ;; label = @5
              block  ;; label = @6
                local.get 2
                i32.const -1
                i32.add
                br_table 0 (;@6;) 1 (;@5;) 2 (;@4;) 3 (;@3;)
              end
              local.get 4
              local.get 1
              i32.load
              local.tee 6
              i32.store8
              local.get 4
              local.get 6
              i32.const 16
              i32.shr_u
              i32.store8 offset=2
              local.get 4
              local.get 6
              i32.const 8
              i32.shr_u
              i32.store8 offset=1
              local.get 5
              i32.const -3
              i32.add
              local.set 5
              local.get 4
              i32.const 3
              i32.add
              local.set 7
              i32.const 0
              local.set 2
              loop  ;; label = @6
                local.get 7
                local.get 2
                i32.add
                local.tee 4
                local.get 1
                local.get 2
                i32.add
                local.tee 3
                i32.const 4
                i32.add
                i32.load
                local.tee 8
                i32.const 8
                i32.shl
                local.get 6
                i32.const 24
                i32.shr_u
                i32.or
                i32.store
                local.get 4
                i32.const 4
                i32.add
                local.get 3
                i32.const 8
                i32.add
                i32.load
                local.tee 6
                i32.const 8
                i32.shl
                local.get 8
                i32.const 24
                i32.shr_u
                i32.or
                i32.store
                local.get 4
                i32.const 8
                i32.add
                local.get 3
                i32.const 12
                i32.add
                i32.load
                local.tee 8
                i32.const 8
                i32.shl
                local.get 6
                i32.const 24
                i32.shr_u
                i32.or
                i32.store
                local.get 4
                i32.const 12
                i32.add
                local.get 3
                i32.const 16
                i32.add
                i32.load
                local.tee 6
                i32.const 8
                i32.shl
                local.get 8
                i32.const 24
                i32.shr_u
                i32.or
                i32.store
                local.get 2
                i32.const 16
                i32.add
                local.set 2
                local.get 5
                i32.const -16
                i32.add
                local.tee 5
                i32.const 16
                i32.gt_u
                br_if 0 (;@6;)
              end
              local.get 7
              local.get 2
              i32.add
              local.set 4
              local.get 1
              local.get 2
              i32.add
              i32.const 3
              i32.add
              local.set 1
              br 2 (;@3;)
            end
            local.get 4
            local.get 1
            i32.load
            local.tee 6
            i32.store16 align=1
            local.get 5
            i32.const -2
            i32.add
            local.set 5
            local.get 4
            i32.const 2
            i32.add
            local.set 7
            i32.const 0
            local.set 2
            loop  ;; label = @5
              local.get 7
              local.get 2
              i32.add
              local.tee 4
              local.get 1
              local.get 2
              i32.add
              local.tee 3
              i32.const 4
              i32.add
              i32.load
              local.tee 8
              i32.const 16
              i32.shl
              local.get 6
              i32.const 16
              i32.shr_u
              i32.or
              i32.store
              local.get 4
              i32.const 4
              i32.add
              local.get 3
              i32.const 8
              i32.add
              i32.load
              local.tee 6
              i32.const 16
              i32.shl
              local.get 8
              i32.const 16
              i32.shr_u
              i32.or
              i32.store
              local.get 4
              i32.const 8
              i32.add
              local.get 3
              i32.const 12
              i32.add
              i32.load
              local.tee 8
              i32.const 16
              i32.shl
              local.get 6
              i32.const 16
              i32.shr_u
              i32.or
              i32.store
              local.get 4
              i32.const 12
              i32.add
              local.get 3
              i32.const 16
              i32.add
              i32.load
              local.tee 6
              i32.const 16
              i32.shl
              local.get 8
              i32.const 16
              i32.shr_u
              i32.or
              i32.store
              local.get 2
              i32.const 16
              i32.add
              local.set 2
              local.get 5
              i32.const -16
              i32.add
              local.tee 5
              i32.const 17
              i32.gt_u
              br_if 0 (;@5;)
            end
            local.get 7
            local.get 2
            i32.add
            local.set 4
            local.get 1
            local.get 2
            i32.add
            i32.const 2
            i32.add
            local.set 1
            br 1 (;@3;)
          end
          local.get 4
          local.get 1
          i32.load
          local.tee 6
          i32.store8
          local.get 5
          i32.const -1
          i32.add
          local.set 5
          local.get 4
          i32.const 1
          i32.add
          local.set 7
          i32.const 0
          local.set 2
          loop  ;; label = @4
            local.get 7
            local.get 2
            i32.add
            local.tee 4
            local.get 1
            local.get 2
            i32.add
            local.tee 3
            i32.const 4
            i32.add
            i32.load
            local.tee 8
            i32.const 24
            i32.shl
            local.get 6
            i32.const 8
            i32.shr_u
            i32.or
            i32.store
            local.get 4
            i32.const 4
            i32.add
            local.get 3
            i32.const 8
            i32.add
            i32.load
            local.tee 6
            i32.const 24
            i32.shl
            local.get 8
            i32.const 8
            i32.shr_u
            i32.or
            i32.store
            local.get 4
            i32.const 8
            i32.add
            local.get 3
            i32.const 12
            i32.add
            i32.load
            local.tee 8
            i32.const 24
            i32.shl
            local.get 6
            i32.const 8
            i32.shr_u
            i32.or
            i32.store
            local.get 4
            i32.const 12
            i32.add
            local.get 3
            i32.const 16
            i32.add
            i32.load
            local.tee 6
            i32.const 24
            i32.shl
            local.get 8
            i32.const 8
            i32.shr_u
            i32.or
            i32.store
            local.get 2
            i32.const 16
            i32.add
            local.set 2
            local.get 5
            i32.const -16
            i32.add
            local.tee 5
            i32.const 18
            i32.gt_u
            br_if 0 (;@4;)
          end
          local.get 7
          local.get 2
          i32.add
          local.set 4
          local.get 1
          local.get 2
          i32.add
          i32.const 1
          i32.add
          local.set 1
        end
        block  ;; label = @3
          local.get 5
          i32.const 16
          i32.and
          i32.eqz
          br_if 0 (;@3;)
          local.get 4
          local.get 1
          i32.load8_u
          i32.store8
          local.get 4
          local.get 1
          i32.load offset=1 align=1
          i32.store offset=1 align=1
          local.get 4
          local.get 1
          i64.load offset=5 align=1
          i64.store offset=5 align=1
          local.get 4
          local.get 1
          i32.load16_u offset=13 align=1
          i32.store16 offset=13 align=1
          local.get 4
          local.get 1
          i32.load8_u offset=15
          i32.store8 offset=15
          local.get 4
          i32.const 16
          i32.add
          local.set 4
          local.get 1
          i32.const 16
          i32.add
          local.set 1
        end
        block  ;; label = @3
          local.get 5
          i32.const 8
          i32.and
          i32.eqz
          br_if 0 (;@3;)
          local.get 4
          local.get 1
          i64.load align=1
          i64.store align=1
          local.get 4
          i32.const 8
          i32.add
          local.set 4
          local.get 1
          i32.const 8
          i32.add
          local.set 1
        end
        block  ;; label = @3
          local.get 5
          i32.const 4
          i32.and
          i32.eqz
          br_if 0 (;@3;)
          local.get 4
          local.get 1
          i32.load align=1
          i32.store align=1
          local.get 4
          i32.const 4
          i32.add
          local.set 4
          local.get 1
          i32.const 4
          i32.add
          local.set 1
        end
        block  ;; label = @3
          local.get 5
          i32.const 2
          i32.and
          i32.eqz
          br_if 0 (;@3;)
          local.get 4
          local.get 1
          i32.load16_u align=1
          i32.store16 align=1
          local.get 4
          i32.const 2
          i32.add
          local.set 4
          local.get 1
          i32.const 2
          i32.add
          local.set 1
        end
        local.get 5
        i32.const 1
        i32.and
        i32.eqz
        br_if 1 (;@1;)
      end
      local.get 4
      local.get 1
      i32.load8_u
      i32.store8
    end
    local.get 0)
  (func $strlen (type 3) (param i32) (result i32)
    (local i32 i32 i32)
    local.get 0
    local.set 1
    block  ;; label = @1
      block  ;; label = @2
        local.get 0
        i32.const 3
        i32.and
        i32.eqz
        br_if 0 (;@2;)
        local.get 0
        i32.const 1
        i32.add
        local.set 2
        local.get 0
        local.set 1
        loop  ;; label = @3
          local.get 1
          i32.load8_u
          i32.eqz
          br_if 2 (;@1;)
          local.get 1
          i32.const 1
          i32.add
          local.set 1
          local.get 2
          i32.const 3
          i32.and
          local.set 3
          local.get 2
          i32.const 1
          i32.add
          local.set 2
          local.get 3
          br_if 0 (;@3;)
        end
      end
      local.get 1
      i32.const -4
      i32.add
      local.set 1
      loop  ;; label = @2
        local.get 1
        i32.const 4
        i32.add
        local.tee 1
        i32.load
        local.tee 2
        i32.const -1
        i32.xor
        local.get 2
        i32.const -16843009
        i32.add
        i32.and
        i32.const -2139062144
        i32.and
        i32.eqz
        br_if 0 (;@2;)
      end
      local.get 2
      i32.const 255
      i32.and
      i32.eqz
      br_if 0 (;@1;)
      loop  ;; label = @2
        local.get 1
        i32.const 1
        i32.add
        local.tee 1
        i32.load8_u
        br_if 0 (;@2;)
      end
    end
    local.get 1
    local.get 0
    i32.sub)
  (func $write (type 2) (param i32 i32 i32) (result i32)
    local.get 0
    local.get 1
    local.get 2
    call $__syscall_SYS_write
    call $__syscall_ret)
  (table (;0;) 1 1 funcref)
  (memory (;0;) 2)
  (global $__stack_pointer (mut i32) (i32.const 66672))
  (export "memory" (memory 0))
  (export "_start" (func $_start))
  (data $.rodata (i32.const 1024) "hello world\0a\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00")
  (data $.data (i32.const 1124) "\ef\be\ad\de"))
