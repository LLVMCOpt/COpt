; ModuleID = '<stdin>'
source_filename = "<stdin>"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: nounwind uwtable
define i32 @foo(i32 %a, i32 %b, i32 %c) #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %d = alloca i32, align 4
  store i32 %a, i32* %1, align 4
  store i32 %b, i32* %2, align 4
  store i32 %c, i32* %3, align 4
  %4 = load i32, i32* %3, align 4
  switch i32 %4, label %25 [
    i32 1, label %5
    i32 2, label %9
    i32 3, label %13
    i32 4, label %17
    i32 5, label %21
  ]

; <label>:5:                                      ; preds = %0
  %6 = load i32, i32* %1, align 4
  %7 = load i32, i32* %2, align 4
  %8 = add nsw i32 %6, %7
  store i32 %8, i32* %d, align 4
  br label %29

; <label>:9:                                      ; preds = %0
  %10 = load i32, i32* %1, align 4
  %11 = load i32, i32* %2, align 4
  %12 = add nsw i32 %10, %11
  store i32 %12, i32* %d, align 4
  br label %29

; <label>:13:                                     ; preds = %0
  %14 = load i32, i32* %1, align 4
  %15 = load i32, i32* %2, align 4
  %16 = add nsw i32 %14, %15
  store i32 %16, i32* %d, align 4
  br label %29

; <label>:17:                                     ; preds = %0
  %18 = load i32, i32* %1, align 4
  %19 = load i32, i32* %2, align 4
  %20 = add nsw i32 %18, %19
  store i32 %20, i32* %d, align 4
  br label %29

; <label>:21:                                     ; preds = %0
  %22 = load i32, i32* %1, align 4
  %23 = load i32, i32* %2, align 4
  %24 = add nsw i32 %22, %23
  store i32 %24, i32* %d, align 4
  br label %29

; <label>:25:                                     ; preds = %0
  %26 = load i32, i32* %1, align 4
  %27 = load i32, i32* %2, align 4
  %28 = add nsw i32 %26, %27
  store i32 %28, i32* %d, align 4
  br label %29

; <label>:29:                                     ; preds = %25, %21, %17, %13, %9, %5
  %whichExp = phi i32 [ %28, %25 ], [ %24, %21 ], [ %20, %17 ], [ %16, %13 ], [ %12, %9 ], [ %8, %5 ]
  store i32 %whichExp, i32* %d, align 4
  %30 = load i32, i32* %d, align 4
  ret i32 %30
}

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)"}
