; ModuleID = 'source/test1.c'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: nounwind uwtable
define void @foo(i32 %a, i32 %b) #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %x = alloca i32, align 4
  %y = alloca i32, align 4
  %z = alloca i32, align 4
  %q = alloca i32, align 4
  store i32 %a, i32* %1, align 4
  store i32 %b, i32* %2, align 4
  %3 = load i32, i32* %1, align 4
  %4 = load i32, i32* %2, align 4
  %5 = add nsw i32 %3, %4
  store i32 %5, i32* %y, align 4
  %6 = load i32, i32* %1, align 4
  %7 = load i32, i32* %2, align 4
  %8 = mul nsw i32 %6, %7
  store i32 %8, i32* %q, align 4
  %9 = load i32, i32* %y, align 4
  %10 = add nsw i32 %9, 5
  store i32 %10, i32* %y, align 4
  %11 = load i32, i32* %y, align 4
  %12 = load i32, i32* %y, align 4
  %13 = mul nsw i32 %11, %12
  store i32 %13, i32* %x, align 4
  %14 = load i32, i32* %x, align 4
  store i32 %14, i32* %z, align 4
  %15 = load i32, i32* %z, align 4
  %16 = add nsw i32 %15, 2
  store i32 %16, i32* %2, align 4
  ret void
}

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)"}
