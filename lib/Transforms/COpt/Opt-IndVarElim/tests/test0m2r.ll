; ModuleID = 'tests/test1.ll'
source_filename = "tests/test1.ll"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@.str = private unnamed_addr constant [19 x i8] c"%d %d %d %d %d %d\0A\00", align 1
@.str.1 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

; Function Attrs: nounwind uwtable
define i32 @foo(i32 %a, i32 %b, i32 %c, i32 %d, i32 %e) #0 {
  br label %1

; <label>:1:                                      ; preds = %7, %0
  %.03 = phi i32 [ %e, %0 ], [ %6, %7 ]
  %i.0 = phi i32 [ 0, %0 ], [ %8, %7 ]
  %.02 = phi i32 [ %c, %0 ], [ %4, %7 ]
  %.01 = phi i32 [ %b, %0 ], [ %.0, %7 ]
  %.0 = phi i32 [ %a, %0 ], [ %5, %7 ]
  %2 = icmp slt i32 %i.0, 5
  br i1 %2, label %3, label %9

; <label>:3:                                      ; preds = %1
  %4 = add nsw i32 %.0, %d
  %5 = mul nsw i32 %i.0, %i.0
  %6 = sdiv i32 %.0, %i.0
  br label %7

; <label>:7:                                      ; preds = %3
  %8 = add nsw i32 %i.0, 1
  br label %1

; <label>:9:                                      ; preds = %1
  %10 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([19 x i8], [19 x i8]* @.str, i32 0, i32 0), i32 %.0, i32 %.01, i32 %.02, i32 %d, i32 %.03, i32 %i.0)
  ret i32 %.02
}

declare i32 @printf(i8*, ...) #1

; Function Attrs: nounwind uwtable
define i32 @main() #0 {
  %1 = call i32 @foo(i32 1, i32 2, i32 3, i32 4, i32 5)
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.1, i32 0, i32 0), i32 %1)
  ret i32 0
}

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)"}
