; Ensure constant propagation of logical instructions is working correctly.

; RUN: opt < %s -constprop -die -S | FileCheck %s
; CHECK-NOT:     {{and|or|xor}}

define i32 @foo(i32 %a, i32 %b, i32 %c, i32 %d) #0 {
  %1 = add nsw i32 %b, %c
  %2 = sub nsw i32 %a, %d
  %3 = add nsw i32 %b, %c
  %4 = sub nsw i32 %a, %d
  
  %5 = add nsw i32 %4, 1
  
        %R = and i32 4, 1234            ; <i32> [#uses=1]
        ret i32 %R
}

define i1 @test1.upgrd.1() {
        %R = and i1 true, false         ; <i1> [#uses=1]
        ret i1 %R
}

define i32 @test2() {
        %R = or i32 4, 1234             ; <i32> [#uses=1]
        ret i32 %R
}

define i1 @test2.upgrd.2() {
        %R = or i1 true, false          ; <i1> [#uses=1]
        ret i1 %R
}

define i32 @test3() {
        %R = xor i32 4, 1234            ; <i32> [#uses=1]
        ret i32 %R
}

define i1 @test3.upgrd.3() {
        %R = xor i1 true, false         ; <i1> [#uses=1]
        ret i1 %R
}

