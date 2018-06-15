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

define double @test4(double %x, double %y) {
; CHECK: @test4(double %x, double %y)
; CHECK: %add1 = fadd double %x, %y, !fpmath !1
; CHECK: %foo = fadd double %add1, %add1
  %add1 = fadd double %x, %y
  %add2 = fadd double %x, %y
  %foo = fadd double %add1, %add2
  ret double %foo
}

!0 = !{ float 5.0 }
!1 = !{ float 2.5 }

; Function Attrs: nounwind uwtable
define i32 @main() #0 {
  %1 = alloca double, align 4
  %a = alloca double, align 4
  %b = alloca double, align 4
  %sum = alloca double, align 4
  store double 0.0, double* %1, align 4
  store double 5.0, double* %a, align 4
  store double 10.0, double* %b, align 4
  %2 = load double, double* %a, align 4
  %3 = load double, double* %b, align 4
  %4 = call double @test4(double %2, double %3)
  %5 = call double @test1(double 2500.0, double %3)
  %6 = call double @test2(double 3450.0, double %3)
  %7 = call double @test3(double %2, double %3)
  %8 = call double @test4(double %2, double %3)
  %9 = call double @test1(double %2, double %3)
  %10 = call double @test2(double 100.0, double %3)
  %11 = call double @test3(double %2, double 5432.0)
  %12 = call double @test4(double 2890.0, double %3)
  %13 = call double @test1(double 250.0, double %3)
  %14 = call double @test2(double %2, double 1234.0)
  %15 = call double @test3(double %2, double %3)
  %16 = call double @test4(double %2, double 520.0)
  %17 = call double @test1(double %2, double 6000.0)
  %18 = call double @test2(double 7070.0, double 2340.0)
  %19 = call double @test3(double 3500.0, double %3)
  %20 = call double @test4(double %2, double %3)
  %21 = call double @test1(double 2500.0, double %3)
  %22 = call double @test2(double %2, double %3)
  %23 = call double @test2(double %2, double 1000.0)
  store double %4, double* %sum, align 4
  ret i32 0
}
