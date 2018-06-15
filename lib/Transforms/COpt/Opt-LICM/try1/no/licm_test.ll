; ModuleID = 'licm_test.c'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%struct._IO_FILE = type { i32, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, %struct._IO_marker*, %struct._IO_FILE*, i32, i32, i64, i16, i8, [1 x i8], i8*, i64, i8*, i8*, i8*, i8*, i64, i32, [20 x i8] }
%struct._IO_marker = type { %struct._IO_marker*, %struct._IO_FILE*, i32 }
%struct.twoInt = type { i32, i32 }

@stderr = external global %struct._IO_FILE*, align 8
@.str = private unnamed_addr constant [21 x i8] c"USAGE: licm a b c d\0A\00", align 1
@i2 = common global %struct.twoInt zeroinitializer, align 4
@.str.1 = private unnamed_addr constant [22 x i8] c"%d %d %d %d %d %d %d\0A\00", align 1

; Function Attrs: nounwind uwtable
define i32 @main(i32 %argc, i8** %argv) #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i8**, align 8
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  %c = alloca i32, align 4
  %d = alloca i32, align 4
  %x = alloca i32, align 4
  %i = alloca i32, align 4
  %j = alloca i32, align 4
  store i32 0, i32* %1, align 4
  store i32 %argc, i32* %2, align 4
  store i8** %argv, i8*** %3, align 8
  %4 = load i32, i32* %2, align 4
  %5 = icmp slt i32 %4, 5
  br i1 %5, label %6, label %9

; <label>:6                                       ; preds = %0
  %7 = load %struct._IO_FILE*, %struct._IO_FILE** @stderr, align 8
  %8 = call i32 (%struct._IO_FILE*, i8*, ...) @fprintf(%struct._IO_FILE* %7, i8* getelementptr inbounds ([21 x i8], [21 x i8]* @.str, i32 0, i32 0))
  call void @exit(i32 1) #4
  unreachable

; <label>:9                                       ; preds = %0
  %10 = load i8**, i8*** %3, align 8
  %11 = getelementptr inbounds i8*, i8** %10, i64 1
  %12 = load i8*, i8** %11, align 8
  %13 = call i32 @atoi(i8* %12) #5
  store i32 %13, i32* %a, align 4
  %14 = load i8**, i8*** %3, align 8
  %15 = getelementptr inbounds i8*, i8** %14, i64 2
  %16 = load i8*, i8** %15, align 8
  %17 = call i32 @atoi(i8* %16) #5
  store i32 %17, i32* %b, align 4
  %18 = load i8**, i8*** %3, align 8
  %19 = getelementptr inbounds i8*, i8** %18, i64 3
  %20 = load i8*, i8** %19, align 8
  %21 = call i32 @atoi(i8* %20) #5
  store i32 %21, i32* %c, align 4
  %22 = load i8**, i8*** %3, align 8
  %23 = getelementptr inbounds i8*, i8** %22, i64 4
  %24 = load i8*, i8** %23, align 8
  %25 = call i32 @atoi(i8* %24) #5
  store i32 %25, i32* %d, align 4
  %26 = load i32, i32* %a, align 4
  store i32 %26, i32* getelementptr inbounds (%struct.twoInt, %struct.twoInt* @i2, i32 0, i32 0), align 4
  %27 = load i32, i32* %b, align 4
  store i32 %27, i32* getelementptr inbounds (%struct.twoInt, %struct.twoInt* @i2, i32 0, i32 1), align 4
  store i32 0, i32* %j, align 4
  br label %28

; <label>:28                                      ; preds = %68, %9
  %29 = load i32, i32* %j, align 4
  %30 = load i32, i32* %b, align 4
  %31 = icmp slt i32 %29, %30
  br i1 %31, label %32, label %71

; <label>:32                                      ; preds = %28
  store i32 0, i32* %i, align 4
  br label %33

; <label>:33                                      ; preds = %61, %32
  %34 = load i32, i32* %i, align 4
  %35 = load i32, i32* %c, align 4
  %36 = icmp slt i32 %34, %35
  br i1 %36, label %37, label %64

; <label>:37                                      ; preds = %33
  %38 = load i32, i32* %c, align 4
  %39 = load i32, i32* %b, align 4
  %40 = sdiv i32 %38, %39
  %41 = load i32, i32* %b, align 4
  %42 = load i32, i32* %j, align 4
  %43 = mul nsw i32 %41, %42
  %44 = add nsw i32 %40, %43
  store i32 %44, i32* %a, align 4
  %45 = load i32, i32* %b, align 4
  %46 = load i32, i32* %j, align 4
  %47 = mul nsw i32 %45, %46
  %48 = load i32, i32* getelementptr inbounds (%struct.twoInt, %struct.twoInt* @i2, i32 0, i32 0), align 4
  %49 = add nsw i32 %47, %48
  store i32 %49, i32* %x, align 4
  %50 = load i32, i32* %j, align 4
  %51 = load i32, i32* %i, align 4
  %52 = icmp eq i32 %50, %51
  br i1 %52, label %53, label %57

; <label>:53                                      ; preds = %37
  %54 = load i32, i32* %a, align 4
  %55 = load i32, i32* %c, align 4
  %56 = mul nsw i32 %54, %55
  store i32 %56, i32* %d, align 4
  br label %57

; <label>:57                                      ; preds = %53, %37
  %58 = load i32, i32* %a, align 4
  %59 = load i32, i32* %c, align 4
  %60 = sub nsw i32 %58, %59
  store i32 %60, i32* %a, align 4
  br label %61

; <label>:61                                      ; preds = %57
  %62 = load i32, i32* %i, align 4
  %63 = add nsw i32 %62, 1
  store i32 %63, i32* %i, align 4
  br label %33

; <label>:64                                      ; preds = %33
  %65 = load i32, i32* %b, align 4
  %66 = load i32, i32* %c, align 4
  %67 = add nsw i32 %65, %66
  store i32 %67, i32* getelementptr inbounds (%struct.twoInt, %struct.twoInt* @i2, i32 0, i32 1), align 4
  br label %68

; <label>:68                                      ; preds = %64
  %69 = load i32, i32* %j, align 4
  %70 = add nsw i32 %69, 1
  store i32 %70, i32* %j, align 4
  br label %28

; <label>:71                                      ; preds = %28
  %72 = load i32, i32* %a, align 4
  %73 = load i32, i32* %b, align 4
  %74 = load i32, i32* %c, align 4
  %75 = load i32, i32* %d, align 4
  %76 = load i32, i32* %x, align 4
  %77 = load i32, i32* getelementptr inbounds (%struct.twoInt, %struct.twoInt* @i2, i32 0, i32 0), align 4
  %78 = load i32, i32* getelementptr inbounds (%struct.twoInt, %struct.twoInt* @i2, i32 0, i32 1), align 4
  %79 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([22 x i8], [22 x i8]* @.str.1, i32 0, i32 0), i32 %72, i32 %73, i32 %74, i32 %75, i32 %76, i32 %77, i32 %78)
  %80 = load i32, i32* %1, align 4
  ret i32 %80
}

declare i32 @fprintf(%struct._IO_FILE*, i8*, ...) #1

; Function Attrs: noreturn nounwind
declare void @exit(i32) #2

; Function Attrs: nounwind readonly
declare i32 @atoi(i8*) #3

declare i32 @printf(i8*, ...) #1

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { noreturn nounwind "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind readonly "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { noreturn nounwind }
attributes #5 = { nounwind readonly }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)"}
