; ModuleID = 'program/count.c'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: nounwind uwtable
define i32 @foo(i32 %n, i32 %m) #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %sum = alloca i32, align 4
  %c0 = alloca i32, align 4
  %c1 = alloca i32, align 4
  store i32 %n, i32* %1, align 4
  store i32 %m, i32* %2, align 4
  store i32 0, i32* %sum, align 4
  %3 = load i32, i32* %1, align 4
  store i32 %3, i32* %c0, align 4
  br label %4

; <label>:4                                       ; preds = %23, %0
  %5 = load i32, i32* %c0, align 4
  %6 = icmp sgt i32 %5, 0
  br i1 %6, label %7, label %26

; <label>:7                                       ; preds = %4
  %8 = load i32, i32* %2, align 4
  store i32 %8, i32* %c1, align 4
  br label %9

; <label>:9                                       ; preds = %19, %7
  %10 = load i32, i32* %c1, align 4
  %11 = icmp sgt i32 %10, 0
  br i1 %11, label %12, label %22

; <label>:12                                      ; preds = %9
  %13 = load i32, i32* %c0, align 4
  %14 = load i32, i32* %c1, align 4
  %15 = icmp sgt i32 %13, %14
  %16 = select i1 %15, i32 1, i32 0
  %17 = load i32, i32* %sum, align 4
  %18 = add nsw i32 %17, %16
  store i32 %18, i32* %sum, align 4
  br label %19

; <label>:19                                      ; preds = %12
  %20 = load i32, i32* %c1, align 4
  %21 = add nsw i32 %20, -1
  store i32 %21, i32* %c1, align 4
  br label %9

; <label>:22                                      ; preds = %9
  br label %23

; <label>:23                                      ; preds = %22
  %24 = load i32, i32* %c0, align 4
  %25 = add nsw i32 %24, -1
  store i32 %25, i32* %c0, align 4
  br label %4

; <label>:26                                      ; preds = %4
  %27 = load i32, i32* %sum, align 4
  ret i32 %27
}

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)"}
