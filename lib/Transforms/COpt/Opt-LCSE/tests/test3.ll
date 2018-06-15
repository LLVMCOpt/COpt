; ModuleID = 'test3.c'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: nounwind uwtable
define i32 @main(i32 %a, i32 %b, i32 %c, i32 %d, i32 %e) #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  store i32 %a, i32* %1, align 4
  store i32 %b, i32* %2, align 4
  store i32 %c, i32* %3, align 4
  store i32 %d, i32* %4, align 4
  store i32 %e, i32* %5, align 4
  %6 = load i32, i32* %2, align 4
  %7 = load i32, i32* %3, align 4
  %8 = add nsw i32 %6, %7
  store i32 %8, i32* %1, align 4
  %9 = load i32, i32* %1, align 4
  %10 = load i32, i32* %4, align 4
  %11 = sub nsw i32 %9, %10
  store i32 %11, i32* %2, align 4
  %12 = load i32, i32* %2, align 4
  %13 = load i32, i32* %3, align 4
  %14 = add nsw i32 %12, %13
  store i32 %14, i32* %3, align 4
  %15 = load i32, i32* %5, align 4
  store i32 %15, i32* %1, align 4
  %16 = load i32, i32* %1, align 4
  %17 = load i32, i32* %4, align 4
  %18 = sub nsw i32 %16, %17
  store i32 %18, i32* %4, align 4
  %19 = load i32, i32* %1, align 4
  %20 = load i32, i32* %2, align 4
  %21 = add nsw i32 %19, %20
  store i32 %21, i32* %5, align 4
  %22 = load i32, i32* %5, align 4
  %23 = load i32, i32* %3, align 4
  %24 = mul nsw i32 %23, %22
  store i32 %24, i32* %3, align 4
  %25 = load i32, i32* %1, align 4
  %26 = load i32, i32* %2, align 4
  %27 = add nsw i32 %25, %26
  store i32 %27, i32* %4, align 4
  %28 = load i32, i32* %4, align 4
  %29 = load i32, i32* %5, align 4
  %30 = mul nsw i32 %28, %29
  store i32 %30, i32* %3, align 4
  %31 = load i32, i32* %3, align 4
  %32 = load i32, i32* %1, align 4
  %33 = add nsw i32 %32, %31
  store i32 %33, i32* %1, align 4
  %34 = load i32, i32* %4, align 4
  %35 = load i32, i32* %5, align 4
  %36 = mul nsw i32 %34, %35
  store i32 %36, i32* %2, align 4
  %37 = load i32, i32* %2, align 4
  %38 = load i32, i32* %3, align 4
  %39 = add nsw i32 %37, %38
  store i32 %39, i32* %1, align 4
  %40 = load i32, i32* %1, align 4
  %41 = load i32, i32* %4, align 4
  %42 = sub nsw i32 %40, %41
  store i32 %42, i32* %2, align 4
  %43 = load i32, i32* %2, align 4
  %44 = load i32, i32* %3, align 4
  %45 = add nsw i32 %43, %44
  store i32 %45, i32* %3, align 4
  %46 = load i32, i32* %5, align 4
  store i32 %46, i32* %1, align 4
  %47 = load i32, i32* %1, align 4
  %48 = load i32, i32* %4, align 4
  %49 = sub nsw i32 %47, %48
  store i32 %49, i32* %4, align 4
  %50 = load i32, i32* %1, align 4
  %51 = load i32, i32* %2, align 4
  %52 = add nsw i32 %50, %51
  store i32 %52, i32* %5, align 4
  %53 = load i32, i32* %5, align 4
  %54 = load i32, i32* %3, align 4
  %55 = mul nsw i32 %54, %53
  store i32 %55, i32* %3, align 4
  %56 = load i32, i32* %1, align 4
  %57 = load i32, i32* %2, align 4
  %58 = add nsw i32 %56, %57
  store i32 %58, i32* %4, align 4
  %59 = load i32, i32* %4, align 4
  %60 = load i32, i32* %5, align 4
  %61 = mul nsw i32 %59, %60
  store i32 %61, i32* %3, align 4
  %62 = load i32, i32* %3, align 4
  %63 = load i32, i32* %1, align 4
  %64 = add nsw i32 %63, %62
  store i32 %64, i32* %1, align 4
  %65 = load i32, i32* %4, align 4
  %66 = load i32, i32* %5, align 4
  %67 = mul nsw i32 %65, %66
  store i32 %67, i32* %2, align 4
  %68 = load i32, i32* %2, align 4
  %69 = load i32, i32* %3, align 4
  %70 = add nsw i32 %68, %69
  store i32 %70, i32* %1, align 4
  %71 = load i32, i32* %1, align 4
  %72 = load i32, i32* %4, align 4
  %73 = sub nsw i32 %71, %72
  store i32 %73, i32* %2, align 4
  %74 = load i32, i32* %2, align 4
  %75 = load i32, i32* %3, align 4
  %76 = add nsw i32 %74, %75
  store i32 %76, i32* %3, align 4
  %77 = load i32, i32* %5, align 4
  store i32 %77, i32* %1, align 4
  %78 = load i32, i32* %1, align 4
  %79 = load i32, i32* %4, align 4
  %80 = sub nsw i32 %78, %79
  store i32 %80, i32* %4, align 4
  %81 = load i32, i32* %1, align 4
  %82 = load i32, i32* %2, align 4
  %83 = add nsw i32 %81, %82
  store i32 %83, i32* %5, align 4
  %84 = load i32, i32* %5, align 4
  %85 = load i32, i32* %3, align 4
  %86 = mul nsw i32 %85, %84
  store i32 %86, i32* %3, align 4
  %87 = load i32, i32* %1, align 4
  %88 = load i32, i32* %2, align 4
  %89 = add nsw i32 %87, %88
  store i32 %89, i32* %4, align 4
  %90 = load i32, i32* %4, align 4
  %91 = load i32, i32* %5, align 4
  %92 = mul nsw i32 %90, %91
  store i32 %92, i32* %3, align 4
  %93 = load i32, i32* %3, align 4
  %94 = load i32, i32* %1, align 4
  %95 = add nsw i32 %94, %93
  store i32 %95, i32* %1, align 4
  %96 = load i32, i32* %4, align 4
  %97 = load i32, i32* %5, align 4
  %98 = mul nsw i32 %96, %97
  store i32 %98, i32* %2, align 4
  %99 = load i32, i32* %2, align 4
  %100 = load i32, i32* %3, align 4
  %101 = add nsw i32 %99, %100
  store i32 %101, i32* %1, align 4
  %102 = load i32, i32* %1, align 4
  %103 = load i32, i32* %4, align 4
  %104 = sub nsw i32 %102, %103
  store i32 %104, i32* %2, align 4
  %105 = load i32, i32* %2, align 4
  %106 = load i32, i32* %3, align 4
  %107 = add nsw i32 %105, %106
  store i32 %107, i32* %3, align 4
  %108 = load i32, i32* %5, align 4
  store i32 %108, i32* %1, align 4
  %109 = load i32, i32* %1, align 4
  %110 = load i32, i32* %4, align 4
  %111 = sub nsw i32 %109, %110
  store i32 %111, i32* %4, align 4
  %112 = load i32, i32* %1, align 4
  %113 = load i32, i32* %2, align 4
  %114 = add nsw i32 %112, %113
  store i32 %114, i32* %5, align 4
  %115 = load i32, i32* %5, align 4
  %116 = load i32, i32* %3, align 4
  %117 = mul nsw i32 %116, %115
  store i32 %117, i32* %3, align 4
  %118 = load i32, i32* %1, align 4
  %119 = load i32, i32* %2, align 4
  %120 = add nsw i32 %118, %119
  store i32 %120, i32* %4, align 4
  %121 = load i32, i32* %4, align 4
  %122 = load i32, i32* %5, align 4
  %123 = mul nsw i32 %121, %122
  store i32 %123, i32* %3, align 4
  %124 = load i32, i32* %3, align 4
  %125 = load i32, i32* %1, align 4
  %126 = add nsw i32 %125, %124
  store i32 %126, i32* %1, align 4
  %127 = load i32, i32* %4, align 4
  %128 = load i32, i32* %5, align 4
  %129 = mul nsw i32 %127, %128
  store i32 %129, i32* %2, align 4
  %130 = load i32, i32* %2, align 4
  %131 = load i32, i32* %3, align 4
  %132 = add nsw i32 %130, %131
  store i32 %132, i32* %1, align 4
  %133 = load i32, i32* %1, align 4
  %134 = load i32, i32* %4, align 4
  %135 = sub nsw i32 %133, %134
  store i32 %135, i32* %2, align 4
  %136 = load i32, i32* %2, align 4
  %137 = load i32, i32* %3, align 4
  %138 = add nsw i32 %136, %137
  store i32 %138, i32* %3, align 4
  %139 = load i32, i32* %5, align 4
  store i32 %139, i32* %1, align 4
  %140 = load i32, i32* %1, align 4
  %141 = load i32, i32* %4, align 4
  %142 = sub nsw i32 %140, %141
  store i32 %142, i32* %4, align 4
  %143 = load i32, i32* %1, align 4
  %144 = load i32, i32* %2, align 4
  %145 = add nsw i32 %143, %144
  store i32 %145, i32* %5, align 4
  %146 = load i32, i32* %5, align 4
  %147 = load i32, i32* %3, align 4
  %148 = mul nsw i32 %147, %146
  store i32 %148, i32* %3, align 4
  %149 = load i32, i32* %1, align 4
  %150 = load i32, i32* %2, align 4
  %151 = add nsw i32 %149, %150
  store i32 %151, i32* %4, align 4
  %152 = load i32, i32* %4, align 4
  %153 = load i32, i32* %5, align 4
  %154 = mul nsw i32 %152, %153
  store i32 %154, i32* %3, align 4
  %155 = load i32, i32* %3, align 4
  %156 = load i32, i32* %1, align 4
  %157 = add nsw i32 %156, %155
  store i32 %157, i32* %1, align 4
  %158 = load i32, i32* %4, align 4
  %159 = load i32, i32* %5, align 4
  %160 = mul nsw i32 %158, %159
  store i32 %160, i32* %2, align 4
  %161 = load i32, i32* %2, align 4
  %162 = load i32, i32* %3, align 4
  %163 = add nsw i32 %161, %162
  store i32 %163, i32* %1, align 4
  %164 = load i32, i32* %1, align 4
  %165 = load i32, i32* %4, align 4
  %166 = sub nsw i32 %164, %165
  store i32 %166, i32* %2, align 4
  %167 = load i32, i32* %2, align 4
  %168 = load i32, i32* %3, align 4
  %169 = add nsw i32 %167, %168
  store i32 %169, i32* %3, align 4
  %170 = load i32, i32* %5, align 4
  store i32 %170, i32* %1, align 4
  %171 = load i32, i32* %1, align 4
  %172 = load i32, i32* %4, align 4
  %173 = sub nsw i32 %171, %172
  store i32 %173, i32* %4, align 4
  %174 = load i32, i32* %1, align 4
  %175 = load i32, i32* %2, align 4
  %176 = add nsw i32 %174, %175
  store i32 %176, i32* %5, align 4
  %177 = load i32, i32* %5, align 4
  %178 = load i32, i32* %3, align 4
  %179 = mul nsw i32 %178, %177
  store i32 %179, i32* %3, align 4
  %180 = load i32, i32* %1, align 4
  %181 = load i32, i32* %2, align 4
  %182 = add nsw i32 %180, %181
  store i32 %182, i32* %4, align 4
  %183 = load i32, i32* %4, align 4
  %184 = load i32, i32* %5, align 4
  %185 = mul nsw i32 %183, %184
  store i32 %185, i32* %3, align 4
  %186 = load i32, i32* %3, align 4
  %187 = load i32, i32* %1, align 4
  %188 = add nsw i32 %187, %186
  store i32 %188, i32* %1, align 4
  %189 = load i32, i32* %4, align 4
  %190 = load i32, i32* %5, align 4
  %191 = mul nsw i32 %189, %190
  store i32 %191, i32* %2, align 4
  %192 = load i32, i32* %2, align 4
  %193 = load i32, i32* %3, align 4
  %194 = add nsw i32 %192, %193
  store i32 %194, i32* %1, align 4
  %195 = load i32, i32* %1, align 4
  %196 = load i32, i32* %4, align 4
  %197 = sub nsw i32 %195, %196
  store i32 %197, i32* %2, align 4
  %198 = load i32, i32* %2, align 4
  %199 = load i32, i32* %3, align 4
  %200 = add nsw i32 %198, %199
  store i32 %200, i32* %3, align 4
  %201 = load i32, i32* %5, align 4
  store i32 %201, i32* %1, align 4
  %202 = load i32, i32* %1, align 4
  %203 = load i32, i32* %4, align 4
  %204 = sub nsw i32 %202, %203
  store i32 %204, i32* %4, align 4
  %205 = load i32, i32* %1, align 4
  %206 = load i32, i32* %2, align 4
  %207 = add nsw i32 %205, %206
  store i32 %207, i32* %5, align 4
  %208 = load i32, i32* %5, align 4
  %209 = load i32, i32* %3, align 4
  %210 = mul nsw i32 %209, %208
  store i32 %210, i32* %3, align 4
  %211 = load i32, i32* %1, align 4
  %212 = load i32, i32* %2, align 4
  %213 = add nsw i32 %211, %212
  store i32 %213, i32* %4, align 4
  %214 = load i32, i32* %4, align 4
  %215 = load i32, i32* %5, align 4
  %216 = mul nsw i32 %214, %215
  store i32 %216, i32* %3, align 4
  %217 = load i32, i32* %3, align 4
  %218 = load i32, i32* %1, align 4
  %219 = add nsw i32 %218, %217
  store i32 %219, i32* %1, align 4
  %220 = load i32, i32* %4, align 4
  %221 = load i32, i32* %5, align 4
  %222 = mul nsw i32 %220, %221
  store i32 %222, i32* %2, align 4
  %223 = load i32, i32* %2, align 4
  %224 = load i32, i32* %3, align 4
  %225 = add nsw i32 %223, %224
  store i32 %225, i32* %1, align 4
  %226 = load i32, i32* %1, align 4
  %227 = load i32, i32* %4, align 4
  %228 = sub nsw i32 %226, %227
  store i32 %228, i32* %2, align 4
  %229 = load i32, i32* %2, align 4
  %230 = load i32, i32* %3, align 4
  %231 = add nsw i32 %229, %230
  store i32 %231, i32* %3, align 4
  %232 = load i32, i32* %5, align 4
  store i32 %232, i32* %1, align 4
  %233 = load i32, i32* %1, align 4
  %234 = load i32, i32* %4, align 4
  %235 = sub nsw i32 %233, %234
  store i32 %235, i32* %4, align 4
  %236 = load i32, i32* %1, align 4
  %237 = load i32, i32* %2, align 4
  %238 = add nsw i32 %236, %237
  store i32 %238, i32* %5, align 4
  %239 = load i32, i32* %5, align 4
  %240 = load i32, i32* %3, align 4
  %241 = mul nsw i32 %240, %239
  store i32 %241, i32* %3, align 4
  %242 = load i32, i32* %1, align 4
  %243 = load i32, i32* %2, align 4
  %244 = add nsw i32 %242, %243
  store i32 %244, i32* %4, align 4
  %245 = load i32, i32* %4, align 4
  %246 = load i32, i32* %5, align 4
  %247 = mul nsw i32 %245, %246
  store i32 %247, i32* %3, align 4
  %248 = load i32, i32* %3, align 4
  %249 = load i32, i32* %1, align 4
  %250 = add nsw i32 %249, %248
  store i32 %250, i32* %1, align 4
  %251 = load i32, i32* %4, align 4
  %252 = load i32, i32* %5, align 4
  %253 = mul nsw i32 %251, %252
  store i32 %253, i32* %2, align 4
  %254 = load i32, i32* %4, align 4
  ret i32 %254
}

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)"}
