; ModuleID = 'tests/testm2r.ll'
source_filename = "test3.ll"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: nounwind uwtable
define i32 @main(i32 %a, i32 %b, i32 %c, i32 %d, i32 %e) #0 {
  %1 = add nsw i32 %b, %c
  %2 = sub nsw i32 %1, %d
  %3 = add nsw i32 %e, %2
  %4 = mul nsw i32 %3, %3
  %5 = add nsw i32 %4, %4
  %6 = sub nsw i32 %5, %3
  %7 = add nsw i32 %3, %6
  %8 = mul nsw i32 %7, %7
  %9 = add nsw i32 %8, %8
  %10 = sub nsw i32 %9, %7
  %11 = add nsw i32 %7, %10
  %12 = mul nsw i32 %11, %11
  %13 = add nsw i32 %12, %12
  %14 = sub nsw i32 %13, %11
  %15 = add nsw i32 %11, %14
  %16 = mul nsw i32 %15, %15
  %17 = add nsw i32 %16, %16
  %18 = sub nsw i32 %17, %15
  %19 = add nsw i32 %15, %18
  %20 = mul nsw i32 %19, %19
  %21 = add nsw i32 %20, %20
  %22 = sub nsw i32 %21, %19
  %23 = add nsw i32 %19, %22
  %24 = mul nsw i32 %23, %23
  %25 = add nsw i32 %24, %24
  %26 = sub nsw i32 %25, %23
  %27 = add nsw i32 %23, %26
  %28 = mul nsw i32 %27, %27
  %29 = add nsw i32 %28, %28
  %30 = sub nsw i32 %29, %27
  %31 = add nsw i32 %27, %30
  ret i32 %31
}

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)"}
