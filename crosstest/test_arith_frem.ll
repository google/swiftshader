target triple = "i686-pc-linux-gnu"

define float @_Z6myFremff(float %a, float %b) {
  %rem = frem float %a, %b
  ret float %rem
}

define double @_Z6myFremdd(double %a, double %b) {
  %rem = frem double %a, %b
  ret double %rem
}
