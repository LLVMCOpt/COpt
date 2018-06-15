; ModuleID = 'tests/test2.ll'
source_filename = "tests/test2.ll"

%struct.X = type { i8* }

declare void @noarg()

declare void @use(i32*)

declare void @use_nocapture(i32* nocapture)

declare void @use2_nocapture(i32* nocapture, i32* nocapture)

define void @test0() {
  call void @noarg()
  ret void
}

define i32 @test1() {
  %A = alloca i32
  br label %tailrecurse

tailrecurse:                                      ; preds = %tailrecurse, %0
  store i32 5, i32* %A
  call void @use(i32* %A)
  br label %tailrecurse
}

define i32 @test2(i32 %X) {
entry:
  %tmp.1 = icmp eq i32 %X, 0
  br i1 %tmp.1, label %then.0, label %endif.0

then.0:                                           ; preds = %entry
  %tmp.4 = add i32 %X, 1
  ret i32 %tmp.4

endif.0:                                          ; preds = %entry
  %tmp.10 = add i32 %X, -1
  %tmp.8 = call i32 @test2(i32 %tmp.10)
  %DUMMY = add i32 %X, 1
  ret i32 %tmp.8
}

define i32 @test3(i32 %c) {
entry:
  br label %tailrecurse

tailrecurse:                                      ; preds = %else, %entry
  %c.tr = phi i32 [ %c, %entry ], [ %tmp.5, %else ]
  %tmp.1 = icmp eq i32 %c.tr, 0
  br i1 %tmp.1, label %return, label %else

else:                                             ; preds = %tailrecurse
  %tmp.5 = add i32 %c.tr, -1
  br label %tailrecurse

return:                                           ; preds = %tailrecurse
  ret i32 0
}

define void @test4() {
  %a = alloca i32
  call void @use_nocapture(i32* %a)
  call void @noarg()
  ret void
}

define i32* @test5(i32* nocapture %A, i1 %cond) {
  %B = alloca i32
  br label %tailrecurse

tailrecurse:                                      ; preds = %cond_true, %0
  %A.tr = phi i32* [ %A, %0 ], [ %B, %cond_true ]
  %cond.tr = phi i1 [ %cond, %0 ], [ false, %cond_true ]
  br i1 %cond.tr, label %cond_true, label %cond_false

cond_true:                                        ; preds = %tailrecurse
  br label %tailrecurse

cond_false:                                       ; preds = %tailrecurse
  call void @use2_nocapture(i32* %A.tr, i32* %B)
  call void @noarg()
  ret i32* null
}

define void @test6(i32* %a, i32* %b) {
  %c = alloca [100 x i8], align 16
  %tmp = bitcast [100 x i8]* %c to i32*
  call void @use2_nocapture(i32* %b, i32* %tmp)
  ret void
}

; Function Attrs: nounwind uwtable
define void @test7(i32* %a, i32* %b) #0 {
entry:
  %c = alloca [100 x i8], align 16
  %0 = bitcast [100 x i8]* %c to i32*
  call void @use2_nocapture(i32* %0, i32* %a)
  call void @use2_nocapture(i32* %b, i32* %0)
  ret void
}

define i32* @test8(i32* nocapture %A, i1 %cond) {
  %B = alloca i32
  %B2 = alloca i32
  br label %tailrecurse

tailrecurse:                                      ; preds = %cond_true, %0
  %A.tr = phi i32* [ %A, %0 ], [ %B, %cond_true ]
  %cond.tr = phi i1 [ %cond, %0 ], [ false, %cond_true ]
  br i1 %cond.tr, label %cond_true, label %cond_false

cond_true:                                        ; preds = %tailrecurse
  call void @use(i32* %B2)
  br label %tailrecurse

cond_false:                                       ; preds = %tailrecurse
  call void @use2_nocapture(i32* %A.tr, i32* %B)
  call void @noarg()
  ret i32* null
}

define void @test9(i32* byval %a) {
  call void @use(i32* %a)
  ret void
}

declare void @ctor(%struct.X*)

define void @test10(%struct.X* noalias sret %agg.result, i1 zeroext %b) {
entry:
  %x = alloca %struct.X, align 8
  br i1 %b, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  call void @ctor(%struct.X* %agg.result)
  br label %return

if.end:                                           ; preds = %entry
  call void @ctor(%struct.X* %x)
  br label %return

return:                                           ; preds = %if.end, %if.then
  ret void
}

declare void @test11_helper1(i8** nocapture, i8*)

declare void @test11_helper2(i8*)

define void @test11() {
  %a = alloca i8*
  %b = alloca i8
  call void @test11_helper1(i8** %a, i8* %b)
  %c = load i8*, i8** %a
  call void @test11_helper2(i8* %c)
  ret void
}

define void @test12() {
entry:
  %e = alloca i8
  call void undef(i8* undef) [ "foo"(i8* %e) ]
  unreachable
}

attributes #0 = { nounwind uwtable }
