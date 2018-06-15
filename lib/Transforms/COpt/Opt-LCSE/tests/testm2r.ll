; ModuleID = 'test3.ll'
source_filename = "test3.ll"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: nounwind uwtable
define i32 @main(i32 %a, i32 %b, i32 %c, i32 %d, i32 %e) #0 {
  %1 = add nsw i32 %b, %c
  %2 = sub nsw i32 %1, %d
  %3 = add nsw i32 %2, %c
  %4 = sub nsw i32 %e, %d
  %5 = add nsw i32 %e, %2
  %6 = mul nsw i32 %3, %5
  %7 = add nsw i32 %e, %2
  %8 = mul nsw i32 %7, %5
  %9 = add nsw i32 %e, %8
  %10 = mul nsw i32 %7, %5
  %11 = add nsw i32 %10, %8
  %12 = sub nsw i32 %11, %7
  %13 = add nsw i32 %12, %8
  %14 = sub nsw i32 %5, %7
  %15 = add nsw i32 %5, %12
  %16 = mul nsw i32 %13, %15
  %17 = add nsw i32 %5, %12
  %18 = mul nsw i32 %17, %15
  %19 = add nsw i32 %5, %18
  %20 = mul nsw i32 %17, %15
  %21 = add nsw i32 %20, %18
  %22 = sub nsw i32 %21, %17
  %23 = add nsw i32 %22, %18
  %24 = sub nsw i32 %15, %17
  %25 = add nsw i32 %15, %22
  %26 = mul nsw i32 %23, %25
  %27 = add nsw i32 %15, %22
  %28 = mul nsw i32 %27, %25
  %29 = add nsw i32 %15, %28
  %30 = mul nsw i32 %27, %25
  %31 = add nsw i32 %30, %28
  %32 = sub nsw i32 %31, %27
  %33 = add nsw i32 %32, %28
  %34 = sub nsw i32 %25, %27
  %35 = add nsw i32 %25, %32
  %36 = mul nsw i32 %33, %35
  %37 = add nsw i32 %25, %32
  %38 = mul nsw i32 %37, %35
  %39 = add nsw i32 %25, %38
  %40 = mul nsw i32 %37, %35
  %41 = add nsw i32 %40, %38
  %42 = sub nsw i32 %41, %37
  %43 = add nsw i32 %42, %38
  %44 = sub nsw i32 %35, %37
  %45 = add nsw i32 %35, %42
  %46 = mul nsw i32 %43, %45
  %47 = add nsw i32 %35, %42
  %48 = mul nsw i32 %47, %45
  %49 = add nsw i32 %35, %48
  %50 = mul nsw i32 %47, %45
  %51 = add nsw i32 %50, %48
  %52 = sub nsw i32 %51, %47
  %53 = add nsw i32 %52, %48
  %54 = sub nsw i32 %45, %47
  %55 = add nsw i32 %45, %52
  %56 = mul nsw i32 %53, %55
  %57 = add nsw i32 %45, %52
  %58 = mul nsw i32 %57, %55
  %59 = add nsw i32 %45, %58
  %60 = mul nsw i32 %57, %55
  %61 = add nsw i32 %60, %58
  %62 = sub nsw i32 %61, %57
  %63 = add nsw i32 %62, %58
  %64 = sub nsw i32 %55, %57
  %65 = add nsw i32 %55, %62
  %66 = mul nsw i32 %63, %65
  %67 = add nsw i32 %55, %62
  %68 = mul nsw i32 %67, %65
  %69 = add nsw i32 %55, %68
  %70 = mul nsw i32 %67, %65
  %71 = add nsw i32 %70, %68
  %72 = sub nsw i32 %71, %67
  %73 = add nsw i32 %72, %68
  %74 = sub nsw i32 %65, %67
  %75 = add nsw i32 %65, %72
  %76 = mul nsw i32 %73, %75
  %77 = add nsw i32 %65, %72
  %78 = mul nsw i32 %77, %75
  %79 = add nsw i32 %65, %78
  %80 = mul nsw i32 %77, %75
  ret i32 %77
}

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)"}
