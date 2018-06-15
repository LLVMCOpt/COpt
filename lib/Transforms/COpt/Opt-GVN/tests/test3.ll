; RUN: opt -basicaa -gvn -disable-output < %s


define i64 @main(i64** %arrayidx) {
entry:
  %p = load i64*, i64** %arrayidx, align 8
  %cmpnull = icmp eq i64* %p, null
  br label %BB2

entry2:                                           ; No predecessors!
  br label %BB2

BB2:                                              ; preds = %entry2, %entry
  %bc = bitcast i64** %arrayidx to i64*
  %load = load i64, i64* %bc, align 8
  ret i64 %load
}
