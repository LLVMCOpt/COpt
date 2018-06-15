; ModuleID = 'test7.ll'
source_filename = "test7.ll"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: nounwind uwtable
define i32 @main(i32 %e, i32 %f, i32 %g) #0 {
  %1 = icmp sgt i32 %e, 10
  br i1 %1, label %2, label %5

; <label>:2:                                      ; preds = %0
  %3 = add nsw i32 1, 2
  %4 = add nsw i32 1, 2
  br label %5

; <label>:5:                                      ; preds = %2, %0
  %.0 = phi i32 [ %4, %2 ], [ %g, %0 ]
  ret i32 %.0
}

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)"}
