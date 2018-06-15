; RUN: opt < %s -loop-unroll -unroll-runtime -unroll-allow-partial -S | FileCheck %s

declare void @f() convergent

define i32 @full_unroll() {
entry:
  br label %l3

l3:
  %x.0 = phi i32 [ 0, %entry ], [ %inc, %l3 ]
  call void @f() ;convergent
  %inc = add nsw i32 %x.0, 1
  %exitcond = icmp eq i32 %inc, 3
  br i1 %exitcond, label %exit, label %l3

exit:
  ret i32 0
}

define i32 @pragma_unroll(i32 %n) {
entry:
  %loop_ctl = mul nsw i32 %n, 24
  br label %l3, !llvm.loop !0

l3:
  %x.0 = phi i32 [ 0, %entry ], [ %inc, %l3 ]

  call void @f() convergent
  %inc = add nsw i32 %x.0, 1
  %exitcond = icmp eq i32 %inc, %loop_ctl
  br i1 %exitcond, label %exit, label %l3, !llvm.loop !0

exit:
  ret i32 0
}

!0 = !{!0, !{!"llvm.loop.unroll.count", i32 16}}
