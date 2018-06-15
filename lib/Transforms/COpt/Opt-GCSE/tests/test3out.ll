; ModuleID = '<stdin>'
source_filename = "<stdin>"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: nounwind uwtable
define i32 @foo(i32 %argc, i8** %argv) #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i8**, align 8
  %n = alloca i32, align 4
  %m = alloca i32, align 4
  %t = alloca i32, align 4
  %x = alloca i32, align 4
  %y = alloca i32, align 4
  store i32 %argc, i32* %2, align 4
  store i8** %argv, i8*** %3, align 8
  %4 = load i32, i32* %2, align 4
  %5 = icmp slt i32 %4, 2
  br i1 %5, label %9, label %6

; <label>:6:                                      ; preds = %0
  %7 = load i32, i32* %n, align 4
  %8 = icmp sle i32 %7, 0
  br i1 %8, label %9, label %10

; <label>:9:                                      ; preds = %6, %0
  store i32 -1, i32* %1, align 4
  br label %39

; <label>:10:                                     ; preds = %6
  %11 = load i32, i32* %2, align 4
  store i32 %11, i32* %n, align 4
  %12 = load i32, i32* %n, align 4
  %13 = add nsw i32 %12, 1
  store i32 %13, i32* %m, align 4
  %14 = load i32, i32* %n, align 4
  %15 = mul nsw i32 %14, 5
  store i32 %15, i32* %t, align 4
  %16 = load i32, i32* %m, align 4
  %17 = sdiv i32 %16, 2
  store i32 %17, i32* %t, align 4
  %18 = load i32, i32* %t, align 4
  %19 = load i32, i32* %n, align 4
  %20 = icmp eq i32 %18, %19
  br i1 %20, label %21, label %28

; <label>:21:                                     ; preds = %10
  %22 = load i32, i32* %n, align 4
  %23 = srem i32 %22, 3
  store i32 %23, i32* %m, align 4
  store i32 %23, i32* %n, align 4
  store i32 %23, i32* %m, align 4
  %24 = load i32, i32* %n, align 4
  %25 = mul nsw i32 %24, 2
  store i32 %25, i32* %t, align 4
  %26 = load i32, i32* %m, align 4
  %27 = sdiv i32 %26, 5
  store i32 %27, i32* %m, align 4
  br label %33

; <label>:28:                                     ; preds = %10
  %29 = load i32, i32* %m, align 4
  %30 = sdiv i32 %29, 5
  store i32 %30, i32* %m, align 4
  %31 = load i32, i32* %n, align 4
  %32 = srem i32 %31, 3
  store i32 %32, i32* %m, align 4
  store i32 30, i32* %t, align 4
  br label %33

; <label>:33:                                     ; preds = %28, %21
  %whichExp = phi i32 [ %30, %28 ], [ %27, %21 ]
  %34 = load i32, i32* %n, align 4
  %35 = mul nsw i32 %34, 2
  store i32 %35, i32* %t, align 4
  store i32 %whichExp, i32* %x, align 4
  %36 = load i32, i32* %n, align 4
  %37 = srem i32 %36, 3
  store i32 %37, i32* %n, align 4
  store i32 30, i32* %y, align 4
  store i32 %17, i32* %n, align 4
  %38 = load i32, i32* %2, align 4
  store i32 %38, i32* %1, align 4
  br label %39

; <label>:39:                                     ; preds = %33, %9
  %40 = load i32, i32* %1, align 4
  ret i32 %40
}

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)"}
