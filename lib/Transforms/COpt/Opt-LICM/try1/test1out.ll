; ModuleID = '<stdin>'
source_filename = "<stdin>"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: nounwind uwtable
define i32 @foo(i32 %a) #0 {
  %1 = alloca i32, align 4
  %x = alloca i32, align 4
  %y = alloca i32, align 4
  %i = alloca i32, align 4
  %r = alloca i32, align 4
  %i1 = alloca i32, align 4
  store i32 %a, i32* %1, align 4
  store i32 0, i32* %y, align 4
  store i32 0, i32* %i, align 4
  %i.promoted = load i32, i32* %i, align 1
  %x.promoted5 = load i32, i32* %x, align 1
  %y.promoted7 = load i32, i32* %y, align 1
  br label %2

; <label>:2:                                      ; preds = %9, %0
  %3 = phi i32 [ %8, %9 ], [ %y.promoted7, %0 ]
  %4 = phi i32 [ 5, %9 ], [ %x.promoted5, %0 ]
  %5 = phi i32 [ %10, %9 ], [ %i.promoted, %0 ]
  %6 = icmp slt i32 %5, 3
  br i1 %6, label %7, label %11

; <label>:7:                                      ; preds = %2
  %8 = mul nsw i32 %3, %5
  br label %9

; <label>:9:                                      ; preds = %7
  %10 = add nsw i32 %5, 1
  br label %2

; <label>:11:                                     ; preds = %2
  %.lcssa8 = phi i32 [ %3, %2 ]
  %.lcssa6 = phi i32 [ %4, %2 ]
  %.lcssa4 = phi i32 [ %5, %2 ]
  store i32 %.lcssa4, i32* %i, align 1
  store i32 %.lcssa6, i32* %x, align 1
  store i32 %.lcssa8, i32* %y, align 1
  store i32 10, i32* %r, align 4
  store i32 0, i32* %i1, align 4
  %12 = load i32, i32* %1, align 4
  %13 = mul nsw i32 42, %12
  %i1.promoted = load i32, i32* %i1, align 1
  %y.promoted = load i32, i32* %y, align 1
  %x.promoted = load i32, i32* %x, align 1
  %r.promoted = load i32, i32* %r, align 1
  br label %14

; <label>:14:                                     ; preds = %22, %11
  %15 = phi i32 [ %21, %22 ], [ %r.promoted, %11 ]
  %16 = phi i32 [ 6, %22 ], [ %x.promoted, %11 ]
  %17 = phi i32 [ 5, %22 ], [ %y.promoted, %11 ]
  %18 = phi i32 [ %23, %22 ], [ %i1.promoted, %11 ]
  %19 = icmp slt i32 %18, %13
  br i1 %19, label %20, label %24

; <label>:20:                                     ; preds = %14
  %21 = add nsw i32 %15, 5
  br label %22

; <label>:22:                                     ; preds = %20
  %23 = add nsw i32 %18, 1
  br label %14

; <label>:24:                                     ; preds = %14
  %.lcssa3 = phi i32 [ %15, %14 ]
  %.lcssa2 = phi i32 [ %16, %14 ]
  %.lcssa1 = phi i32 [ %17, %14 ]
  %.lcssa = phi i32 [ %18, %14 ]
  store i32 %.lcssa, i32* %i1, align 1
  store i32 %.lcssa1, i32* %y, align 1
  store i32 %.lcssa2, i32* %x, align 1
  store i32 %.lcssa3, i32* %r, align 1
  %25 = load i32, i32* %r, align 4
  ret i32 %25
}

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)"}
