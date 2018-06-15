; RUN: opt -gvn -S < %s | FileCheck %s

define double @test1(double %x, double %y) {
; CHECK: @test1(double %x, double %y)
; CHECK: %add1 = fadd double %x, %y
; CHECK-NOT: fpmath
; CHECK: %foo = fadd double %add1, %add1
  %add1 = fadd double %x, %y, !fpmath !0
  %add2 = fadd double %x, %y
  %foo = fadd double %add1, %add2
  ret double %foo
}

define double @test2(double %x, double %y) {
; CHECK: @test2(double %x, double %y)
; CHECK: %add1 = fadd double %x, %y, !fpmath !0
; CHECK: %foo = fadd double %add1, %add1
  %add1 = fadd double %x, %y, !fpmath !0
  %add2 = fadd double %x, %y, !fpmath !0
  %foo = fadd double %add1, %add2
  ret double %foo
}

define double @test3(double %x, double %y) {
; CHECK: @test3(double %x, double %y)
; CHECK: %add1 = fadd double %x, %y, !fpmath !1
; CHECK: %foo = fadd double %add1, %add1
  %add1 = fadd double %x, %y, !fpmath !1
  %add2 = fadd double %x, %y, !fpmath !0
  %foo = fadd double %add1, %add2
  ret double %foo
}

define i32 @test4(i32 %x, i32 %y) {
; CHECK: @test4(double %x, double %y)
; CHECK: %add1 = fadd double %x, %y, !fpmath !1
; CHECK: %foo = fadd double %add1, %add1
  %add1 = add i32 %x, %y
  %add2 = add i32 %x, %y
  %foo = add i32 %add1, %add2
  ret i32 %foo
}

!0 = !{ float 5.0 }
!1 = !{ float 2.5 }

; Function Attrs: nounwind uwtable
define i32 @main() #0 {
  %1 = alloca i32, align 4
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  %sum = alloca i32, align 4
  store i32 0, i32* %1, align 4
  store i32 5, i32* %a, align 4
  store i32 10, i32* %b, align 4
  %2 = load i32, i32* %a, align 4
  %3 = load i32, i32* %b, align 4
  %4 = call i32 @test4(i32 %2, i32 %3)
  store i32 %4, i32* %sum, align 4
  ret i32 0
}
