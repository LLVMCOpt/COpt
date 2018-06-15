; ModuleID = 'my compiler'
target datalayout = "e-m:e-p:32:32-f64:32:64-f80:32-n8:16:32-S128"
define i32 @printstar(i32 %n, i32 %x) {
entry:
br label %loop
loop:
%entry
%i = phi i32 [
%nextvar = add
%cmptmp = icmp
br i1 %cmptmp,
afterloop:
ret i32 0
}
62
; preds = %loop,
1, %entry ], [ %nextvar, %loop ]
i32 %i, 1
ult i32 %i, %n
label %loop, label %afterloop
; preds = %loop
