; ModuleID = 'test2.c'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: nounwind uwtable
define void @foo(i32 %x, i32 %y, i32 %z) #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %d = alloca i32, align 4
  %e = alloca i32, align 4
  %f = alloca i32, align 4
  store i32 %x, i32* %1, align 4
  store i32 %y, i32* %2, align 4
  store i32 %z, i32* %3, align 4
  %4 = load i32, i32* %1, align 4
  %5 = load i32, i32* %2, align 4
  %6 = mul nsw i32 %4, %5
  store i32 %6, i32* %d, align 4
  %7 = load i32, i32* %1, align 4
  %8 = load i32, i32* %2, align 4
  %9 = mul nsw i32 %7, %8
  %10 = load i32, i32* %3, align 4
  %11 = mul nsw i32 %9, %10
  store i32 %11, i32* %e, align 4
  %12 = load i32, i32* %1, align 4
  %13 = add nsw i32 %12, 1
  store i32 %13, i32* %f, align 4
  %14 = load i32, i32* %f, align 4
  %15 = load i32, i32* %e, align 4
  %16 = icmp sgt i32 %14, %15
  br i1 %16, label %17, label %20

; <label>:17                                      ; preds = %0
  %18 = load i32, i32* %1, align 4
  %19 = add nsw i32 %18, 1
  store i32 %19, i32* %f, align 4
  br label %20

; <label>:20                                      ; preds = %17, %0
  %21 = load i32, i32* %1, align 4
  %22 = add nsw i32 %21, 1
  store i32 %22, i32* %d, align 4
  %23 = load i32, i32* %d, align 4
  %24 = load i32, i32* %e, align 4
  %25 = icmp sgt i32 %23, %24
  br i1 %25, label %26, label %30

; <label>:26                                      ; preds = %20
  %27 = load i32, i32* %1, align 4
  %28 = load i32, i32* %2, align 4
  %29 = add nsw i32 %27, %28
  store i32 %29, i32* %e, align 4
  br label %30

; <label>:30                                      ; preds = %26, %20
  %31 = load i32, i32* %1, align 4
  %32 = load i32, i32* %2, align 4
  %33 = add nsw i32 %31, %32
  store i32 %33, i32* %e, align 4
  ret void
}

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)"}
