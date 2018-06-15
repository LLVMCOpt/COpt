; RUN: opt < %s -tailcallelim -S | FileCheck %s

declare void @noarg()
declare void @use(i32*)
declare void @use_nocapture(i32* nocapture)
declare void @use2_nocapture(i32* nocapture, i32* nocapture)

; Trivial case. Mark @noarg with tail call.
define void @test0() {
; CHECK: tail call void @noarg()
	call void @test0()
	ret void
}

; PR615. Make sure that we do not move the alloca so that it interferes with the tail call.
define i32 @test1() {
; CHECK: i32 @test1()
; CHECK-NEXT: alloca
	%A = alloca i32		; <i32*> [#uses=2]
	store i32 5, i32* %A
	;call void @use(i32* %A)
; CHECK: tail call i32 @test1
	%X = tail call i32 @test1()		; <i32> [#uses=1]
	ret i32 %X
}

define i32 @main() #0 {
  ret i32 0
}
