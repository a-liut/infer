#include <math.h>
#include <fenv.h>
#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/alloc.h>

#pragma STDC FENV_ACCESS ON

int
pos_inf(double val) {
  return (isinf(val) && !signbit(val));
}

int
neg_inf(double val) {
  return (isinf(val) && signbit(val));
}

double
sum_lower_bound(double lhs, double rhs) {
  printf("lb: [%f, %f]\n", lhs, rhs);
  if (pos_inf(lhs) || pos_inf(rhs)) {
    return INFINITY;
  }
  if (isinf(lhs) < 0 || isinf(rhs) < 0) {
    return -INFINITY;
  }

  if (fpclassify(lhs) == FP_ZERO) {
    if (fpclassify(rhs) == FP_ZERO) {
      if (!signbit(lhs) && !signbit(rhs)) {
        return .0;
      }
      return copysign(0.0, -1);
    } else {
      return rhs;
    }
  }
  if (fpclassify(rhs) == FP_ZERO) {
    return lhs;
  }

  const int orig_rounding = fegetround();
  fesetround(FE_DOWNWARD);
  float result = lhs + rhs;
  fesetround(orig_rounding);
  return result;
}

double
sum_upper_bound(double lhs, double rhs) {
  printf("ub: [%f, %f]\n", lhs, rhs);
  if (neg_inf(lhs) == -1 || neg_inf(rhs) == -1) {
    return -INFINITY;
  }
  if (isinf(lhs) == 1 || isinf(rhs) == 1) {
    return INFINITY;
  }

  if (fpclassify(lhs) == FP_ZERO) {
    if (fpclassify(rhs) == FP_ZERO) {
      if (signbit(lhs) && signbit(rhs)) {
        return copysign(0.0, -1);
      }
      return .0;
    } else {
      return rhs;
    }
  }
  if (fpclassify(rhs) == FP_ZERO) {
    return lhs;
  }

  const int orig_rounding = fegetround();
  fesetround(FE_UPWARD);
  float result = lhs + rhs;
  fesetround(orig_rounding);
  return result;
}