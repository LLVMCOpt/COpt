; ModuleID = 'tests/test1.ll'
source_filename = "tests/test1.ll"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: alwaysinline nounwind uwtable
define i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  %sum = alloca i32, align 4
  store i32 0, i32* %3, align 4
  store i32 5, i32* %a, align 4
  store i32 10, i32* %b, align 4
  %4 = load i32, i32* %a, align 4
  %5 = load i32, i32* %b, align 4
  %6 = bitcast i32* %1 to i8*
  call void @llvm.lifetime.start(i64 4, i8* %6)
  %7 = bitcast i32* %2 to i8*
  call void @llvm.lifetime.start(i64 4, i8* %7)
  store i32 %4, i32* %1, align 4
  store i32 %5, i32* %2, align 4
  %8 = load i32, i32* %1, align 4
  %9 = load i32, i32* %2, align 4
  %10 = add nsw i32 %8, %9
  %11 = bitcast i32* %1 to i8*
  call void @llvm.lifetime.end(i64 4, i8* %11)
  %12 = bitcast i32* %2 to i8*
  call void @llvm.lifetime.end(i64 4, i8* %12)
  store i32 %10, i32* %sum, align 4
  ret i32 0
}

; Function Attrs: alwaysinline argmemonly nounwind
declare void @llvm.lifetime.start(i64, i8* nocapture) #1

; Function Attrs: alwaysinline argmemonly nounwind
declare void @llvm.lifetime.end(i64, i8* nocapture) #1

attributes #0 = { alwaysinline nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { alwaysinline argmemonly nounwind }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)"}
