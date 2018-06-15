; RUN: opt < %s -S -loop-unroll -pass-remarks=loop-unroll -unroll-count=16 2>&1 | FileCheck -check-prefix=COMPLETE-UNROLL %s
; RUN: opt < %s -S -loop-unroll -pass-remarks=loop-unroll -unroll-count=4 2>&1 | FileCheck -check-prefix=PARTIAL-UNROLL %s

; COMPLETE-UNROLL: remark: {{.*}}: completely unrolled loop with 16 iterations
; PARTIAL-UNROLL: remark: {{.*}}: unrolled loop by a factor of 4
define i32 @baz(i32) {
  ret i32 50
}

define i32 @sum() {
entry:
  br label %for.body

for.body:                                         ; preds = %for.body, %entry
  %s.06 = phi i32 [ 0, %entry ], [ %add1, %for.body ]
  %i.05 = phi i32 [ 0, %entry ], [ %inc, %for.body ]
  %add = add nsw i32 %i.05, 4
  %call = tail call i32 @baz(i32 %add) #2
  %add1 = add nsw i32 %call, %s.06
  %inc = add nsw i32 %i.05, 1
  %exitcond = icmp eq i32 %inc, 16
  br i1 %exitcond, label %for.end, label %for.body

for.end:                                          ; preds = %for.body
  ret i32 %add1
}

define i32 @main() #0 {
  %1 = call i32 @sum()
  ret i32 0
}
