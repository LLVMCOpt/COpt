; ModuleID = '<stdin>'
source_filename = "<stdin>"
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64"
target triple = "x86_64-apple-macosx10.7.2"

; Function Attrs: nounwind optsize readonly ssp uwtable
define void @CMSColorWorldCreateParametricData(i8* dereferenceable(1) %a0, i8* dereferenceable(1) %a1, i8* dereferenceable(1) %a2, i8* dereferenceable(1) %a3, i64 %count) #0 {
entry:
  br label %for.body.i

for.body.i:                                       ; preds = %for.inc.i, %entry
  %i = phi i64 [ 0, %entry ], [ %i.inc, %for.inc.i ]
  %0 = load i8, i8* %a0, !invariant.load !0
  %cond0 = icmp eq i8 %0, 0
  br i1 %cond0, label %for.inc.i, label %if.then26.i

if.then26.i:                                      ; preds = %for.body.i
  %1 = load i8, i8* %a1, !invariant.load !0
  %cond1 = icmp eq i8 %1, 1
  br i1 %cond1, label %if.else.i.i, label %lor.lhs.false.i.i

if.else.i.i:                                      ; preds = %if.then26.i
  %2 = load i8, i8* %a2, !invariant.load !0
  %cond2 = icmp eq i8 %2, 2
  br i1 %cond2, label %lor.lhs.false.i.i, label %for.inc.i

lor.lhs.false.i.i:                                ; preds = %if.else.i.i, %if.then26.i
  %3 = load i8, i8* %a3, !invariant.load !0
  %cond3 = icmp eq i8 %3, 3
  br i1 %cond3, label %for.inc.i, label %if.end28.i

for.inc.i:                                        ; preds = %lor.lhs.false.i.i, %if.else.i.i, %for.body.i
  %i.inc = add nsw i64 %i, 1
  %cmp17.i = icmp ult i64 %i.inc, %count
  br i1 %cmp17.i, label %for.body.i, label %if.end28.i

if.end28.i:                                       ; preds = %for.inc.i, %lor.lhs.false.i.i
  ret void
}

attributes #0 = { nounwind optsize readonly ssp uwtable }

!0 = !{}
