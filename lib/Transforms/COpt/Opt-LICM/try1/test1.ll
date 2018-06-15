; ModuleID = 'test1.c'
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
  br label %2

; <label>:2                                       ; preds = %9, %0
  %3 = load i32, i32* %i, align 4
  %4 = icmp slt i32 %3, 3
  br i1 %4, label %5, label %12

; <label>:5                                       ; preds = %2
  store i32 5, i32* %x, align 4
  %6 = load i32, i32* %i, align 4
  %7 = load i32, i32* %y, align 4
  %8 = mul nsw i32 %7, %6
  store i32 %8, i32* %y, align 4
  br label %9

; <label>:9                                       ; preds = %5
  %10 = load i32, i32* %i, align 4
  %11 = add nsw i32 %10, 1
  store i32 %11, i32* %i, align 4
  br label %2

; <label>:12                                      ; preds = %2
  store i32 10, i32* %r, align 4
  store i32 0, i32* %i1, align 4
  br label %13

; <label>:13                                      ; preds = %22, %12
  %14 = load i32, i32* %i1, align 4
  %15 = load i32, i32* %1, align 4
  %16 = mul nsw i32 42, %15
  %17 = icmp slt i32 %14, %16
  br i1 %17, label %18, label %25

; <label>:18                                      ; preds = %13
  store i32 5, i32* %y, align 4
  store i32 6, i32* %x, align 4
  %19 = load i32, i32* %y, align 4
  %20 = load i32, i32* %r, align 4
  %21 = add nsw i32 %20, %19
  store i32 %21, i32* %r, align 4
  br label %22

; <label>:22                                      ; preds = %18
  %23 = load i32, i32* %i1, align 4
  %24 = add nsw i32 %23, 1
  store i32 %24, i32* %i1, align 4
  br label %13

; <label>:25                                      ; preds = %13
  %26 = load i32, i32* %r, align 4
  ret i32 %26
}

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)"}
