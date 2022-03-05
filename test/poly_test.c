#ifdef NDEBUG
#undef NDEBUG
#endif

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "poly_data.h"

#define CHECK_PTR(p)  \
  do {                \
    if (p == NULL) {  \
      exit(1);        \
    }                 \
  } while (0)

#define C PolyFromCoeff

static Mono M(Poly p, poly_exp_t n) {
    return MonoFromPoly(&p, n);
}

static Poly MakePolyHelper(poly_exp_t dummy, ...) {
    va_list list;
    va_start(list, dummy);
    size_t count = 0;
    while (true) {
        va_arg(list, Poly);
        if (va_arg(list, poly_exp_t) < 0)
            break;
        count++;
    }
    va_start(list, dummy);
    Mono *arr = calloc(count, sizeof (Mono));
    CHECK_PTR(arr);
    for (size_t i = 0; i < count; ++i) {
        Poly p = va_arg(list, Poly);
        arr[i] = M(p, va_arg(list, poly_exp_t));
        assert(i == 0 || MonoGetExp(&arr[i]) > MonoGetExp(&arr[i - 1]));
    }
    va_end(list);
    Poly res = PolyAddMonos(count, arr);
    free(arr);
    return res;
}

#define P(...) MakePolyHelper(0, __VA_ARGS__, PolyZero(), -1)

static bool TestOpCopy(Poly a, Poly b, Poly res,
                       Poly (*op)(const Poly *, const Poly *)) {
    Poly c = op(&a, &b);
    bool is_eq = PolyIsEq(&c, &res);
    PolyDestroy(&a);
    PolyDestroy(&b);
    PolyDestroy(&c);
    PolyDestroy(&res);
    return is_eq;
}

static bool TestOpPtr(Poly *a, Poly *b, Poly res,
                      Poly (*op)(const Poly *, const Poly *)) {
    Poly c = op(a, b);
    bool is_eq = PolyIsEq(&c, &res);
    PolyDestroy(&c);
    PolyDestroy(&res);
    return is_eq;
}

static bool TestAdd(Poly a, Poly b, Poly res) {
    return TestOpCopy(a, b, res, PolyAdd);
}

static bool TestAddMonos(size_t count, Mono monos[], Poly res) {
    Poly b = PolyAddMonos(count, monos);
    bool is_eq = PolyIsEq(&b, &res);
    PolyDestroy(&b);
    PolyDestroy(&res);
    return is_eq;
}

static bool TestMul(Poly a, Poly b, Poly res) {
    return TestOpCopy(a, b, res, PolyMul);
}

static bool TestSub(Poly a, Poly b, Poly res) {
    return TestOpCopy(a, b, res, PolySub);
}

static bool TestDegBy(Poly a, size_t var_idx, poly_exp_t res) {
    bool is_eq = PolyDegBy(&a, var_idx) == res;
    PolyDestroy(&a);
    return is_eq;
}

static bool TestDeg(Poly a, poly_exp_t res) {
    bool is_eq = PolyDeg(&a) == res;
    PolyDestroy(&a);
    return is_eq;
}

static bool TestEq(Poly a, Poly b, bool res) {
    bool is_eq = PolyIsEq(&a, &b) == res;
    PolyDestroy(&a);
    PolyDestroy(&b);
    return is_eq;
}

static bool TestAt(Poly a, poly_coeff_t x, Poly res) {
    Poly b = PolyAt(&a, x);
    bool is_eq = PolyIsEq(&b, &res);
    PolyDestroy(&a);
    PolyDestroy(&b);
    PolyDestroy(&res);
    return is_eq;
}

/** WŁAŚCIWE TESTY UDOSTĘPNIONE W PRZYKŁADZIE **/

static bool SimpleAddTest(void) {
    bool res = true;
    res &= TestAdd(C(1),
                   C(2),
                   C(3));
    res &= TestAdd(P(C(1), 1),
                   C(2),
                   P(C(2), 0, C(1), 1));
    res &= TestAdd(C(1),
                   P(C(2), 2),
                   P(C(1), 0, C(2), 2));
    res &= TestAdd(P(C(1), 1),
                   P(C(2), 2),
                   P(C(1), 1, C(2), 2));
    res &= TestAdd(C(0),
                   P(C(1), 1),
                   P(C(1), 1));
    res &= TestAdd(P(C(1), 1),
                   P(C(-1), 1),
                   C(0));
    res &= TestAdd(P(C(1), 1, C(2), 2),
                   P(C(-1), 1),
                   P(C(2), 2));
    res &= TestAdd(P(C(2), 0, C(1), 1),
                   P(C(-1), 1),
                   C(2));
    res &= TestAdd(C(1),
                   P(C(-1), 0, C(1), 1),
                   P(C(1), 1));
    res &= TestAdd(C(1),
                   P(P(C(-1), 0, C(1), 1), 0),
                   P(P(C(1), 1), 0));
    res &= TestAdd(C(1),
                   P(C(1), 0, C(2), 2),
                   P(C(2), 0, C(2), 2));
    res &= TestAdd(C(1),
                   P(P(C(1), 0, C(1), 1), 0, C(2), 2),
                   P(P(C(2), 0, C(1), 1), 0, C(2), 2));
    res &= TestAdd(C(1),
                   P(P(C(-1), 0, C(1), 1), 0, C(2), 2),
                   P(P(C(1), 1), 0, C(2), 2));

    res &= TestAdd(P(P(C(1), 2), 0, P(C(2), 1), 1, C(1), 2),
                   P(P(C(1), 2), 0, P(C(-2), 1), 1, C(1), 2),
                   P(P(C(2), 2), 0, C(2), 2));
    res &= TestAdd(P(P(C(1), 2), 0, P(C(2), 1), 1, C(1), 2),
                   P(P(C(-1), 2), 0, P(C(1), 0, C(2), 1, C(1), 2), 1, C(-1), 2),
                   P(P(C(1), 0, C(4), 1, C(1), 2), 1));

    res &= TestAdd(P(P(C(1), 0, C(1), 1), 0, C(1), 1),
                   P(P(C(1), 0, C(-1), 1), 0, C(-1), 1),
                   C(2));
    res &= TestAdd(P(P(P(C(1), 0, C(1), 1), 0, C(1), 1), 1),
                   P(P(P(C(1), 0, C(-1), 1), 0, C(-1), 1), 1),
                   P(C(2), 1));

    Poly a = P(C(1), 1);
    Poly b = PolyAdd(&a, &a);
    Poly c = P(C(2), 1);
    res &= PolyIsEq(&b, &c);
    PolyDestroy(&a);
    PolyDestroy(&b);
    PolyDestroy(&c);
    return res;
}

static bool SimpleAddMonosTest(void) {
    bool res = true;
    {
        Mono m[] = {M(C(-1), 0), M(C(1), 0)};
        res &= TestAddMonos(2, m, C(0));
    }
    {
        Mono m[] = {M(C(-1), 1), M(C(1), 1)};
        res &= TestAddMonos(2, m, C(0));
    }
    {
        Mono m[] = {M(C(1), 0), M(C(1), 0)};
        res &= TestAddMonos(2, m, C(2));
    }
    {
        Mono m[] = {M(C(1), 1), M(C(1), 1)};
        res &= TestAddMonos(2, m, P(C(2), 1));
    }
    {
        Mono m[] = {M(P(C(-1), 1), 0), M(P(C(1), 1), 0)};
        res &= TestAddMonos(2, m, C(0));
    }
    {
        Mono m[] = {M(P(C(-1), 0), 1),
                    M(P(C(1), 0), 1),
                    M(C(2), 0),
                    M(C(1), 1),
                    M(P(C(2), 1), 2),
                    M(P(C(2), 2), 2)};
        res &= TestAddMonos(6, m, P(C(2), 0, C(1), 1, P(C(2), 1, C(2), 2), 2));
    }
    return res;
}

static bool SimpleMulTest(void) {
    bool res = true;
    res &= TestMul(C(2),
                   C(3),
                   C(6));
    res &= TestMul(P(C(1), 1),
                   C(2),
                   P(C(2), 1));
    res &= TestMul(C(3),
                   P(C(2), 2),
                   P(C(6), 2));
    res &= TestMul(P(C(1), 1),
                   P(C(2), 2),
                   P(C(2), 3));
    res &= TestMul(P(C(-1), 0, C(1), 1),
                   P(C(1), 0, C(1), 1),
                   P(C(-1), 0, C(1), 2));
    res &= TestMul(P(P(C(1), 2), 0, P(C(1), 1), 1, C(1), 2),
                   P(P(C(1), 2), 0, P(C(-1), 1), 1, C(1), 2),
                   P(P(C(1), 4), 0, P(C(1), 2), 2, C(1), 4));
    return res;
}

static bool SimpleNegTest(void) {
    Poly a = P(P(C(1), 0, C(2), 2), 0, P(C(1), 1), 1, C(1), 2);
    Poly b = PolyNeg(&a);
    Poly c = P(P(C(-1), 0, C(-2), 2), 0, P(C(-1), 1), 1, C(-1), 2);
    bool is_eq = PolyIsEq(&b, &c);
    PolyDestroy(&a);
    PolyDestroy(&b);
    PolyDestroy(&c);
    return is_eq;
}

static bool SimpleSubTest(void) {
    return TestSub(P(P(C(1), 2), 0, P(C(2), 1), 1, C(1), 2),
                   P(P(C(1), 2), 0, P(C(-1), 0, C(-2), 1, C(-1), 2), 1, C(1), 2),
                   P(P(C(1), 0, C(4), 1, C(1), 2), 1));
}

#define POLY_P P(P(C(1), 3), 0, P(C(1), 2), 2, C(1), 3)

static bool SimpleDegByTest(void) {
    bool res = true;
    res &= TestDegBy(C(0), 1, -1);
    res &= TestDegBy(C(1), 0, 0);
    res &= TestDegBy(P(C(1), 1), 1, 0);
    res &= TestDegBy(POLY_P, 0, 3);
    res &= TestDegBy(POLY_P, 1, 3);
    return res;
}

static bool SimpleDegTest(void) {
    bool res = true;
    res &= TestDeg(C(0), -1);
    res &= TestDeg(C(1), 0);
    res &= TestDeg(P(C(1), 1), 1);
    res &= TestDeg(POLY_P, 4);
    return res;
}

static bool SimpleIsEqTest(void) {
    bool res = true;
    res &= TestEq(C(0), C(0), true);
    res &= TestEq(C(0), C(1), false);
    res &= TestEq(C(1), C(1), true);
    res &= TestEq(P(C(1), 1), C(1), false);
    res &= TestEq(P(C(1), 1), P(C(1), 1), true);
    res &= TestEq(P(C(1), 1), P(C(1), 2), false);
    res &= TestEq(P(C(1), 1), P(C(2), 1), false);
    res &= TestEq(POLY_P, POLY_P, true);
    Poly a = C(1);
    Poly b = C(2);
    Poly p = POLY_P;
    res &= TestEq(PolyAdd(&p, &a), PolyAdd(&p, &b), false);
    PolyDestroy(&a);
    PolyDestroy(&b);
    PolyDestroy(&p);
    return res;
}

static bool SimpleAtTest(void) {
    bool res = true;
    res &= TestAt(C(2), 1, C(2));
    res &= TestAt(P(C(1), 0, C(1), 18), 10, C(1000000000000000001L));
    res &= TestAt(P(C(3), 1, C(2), 3, C(1), 5), 10, C(102030));
    res &= TestAt(P(P(C(1), 4), 0, P(C(1), 2), 2, C(1), 3), 2,
                  P(C(8), 0, C(4), 2, C(1), 4));
    return res;
}

static bool OverflowTest(void) {
    bool res = true;
    res &= TestMul(P(C(1L << 32), 1), C(1L << 32), C(0));
    res &= TestAt(P(C(1), 64), 2, C(0));
    res &= TestAt(P(C(1), 0, C(1), 64), 2, C(1));
    res &= TestAt(P(P(C(1), 1), 64), 2, C(0));
    return res;
}

/**
 * Tests addition, subtraction and multiplication
 * of numbers using polynomials.
 */
static bool SimpleArithmeticTest(void) {
    for (poly_coeff_t i = -100; i < 100; ++i) {
        Poly p1 = PolyFromCoeff(i);
        for (poly_coeff_t j = -100; j < 100; ++j) {
            Poly p2 = PolyFromCoeff(j);
            if (!TestOpPtr(&p1, &p2, PolyFromCoeff(i + j), PolyAdd))
                return false;
            if (!TestOpPtr(&p1, &p2, PolyFromCoeff(i - j), PolySub))
                return false;
            if (!TestOpPtr(&p1, &p2, PolyFromCoeff(i * j), PolyMul))
                return false;
            if (TestOpPtr(&p1, &p2, PolyFromCoeff(i + j + 1), PolyAdd))
                return false;
            if (TestOpPtr(&p1, &p2, PolyFromCoeff(i - j - 1), PolySub))
                return false;
            if (TestOpPtr(&p1, &p2, PolyFromCoeff(i * j + 1), PolyMul))
                return false;
            PolyDestroy(&p2);
        }
        PolyDestroy(&p1);
    }
    return true;
}

/**
 *  Tests building of long polynomials.
 */
static bool LongPolynomialTest(void) {
    bool res = true;
    Poly p = PolyFromCoeff(1);
    for (poly_exp_t poly_deg = 10; poly_deg < 90011 && res; poly_deg += 1000) {
        Mono *m = calloc((size_t)poly_deg + 1, sizeof (Mono));
        for (poly_exp_t i = 0; i <= poly_deg; ++i) {
            Poly tmp = PolyClone(&p);
            m[i] = MonoFromPoly(&tmp, i);
        }
        Poly long_p = PolyAddMonos((unsigned)poly_deg + 1, m); //  1 + x + x^2 + ...
        free(m);
        if (PolyDeg(&long_p) != poly_deg)
            res = false;
        Poly mono_sum = PolyAt(&long_p, 1);
        if (!PolyIsCoeff(&mono_sum))
            res = false;
        Poly mono_sum_poly = PolyFromCoeff(poly_deg + 1);
        if (!PolyIsEq(&mono_sum, &mono_sum_poly))
            res = false;
        PolyDestroy(&mono_sum_poly);
        PolyDestroy(&mono_sum);
        mono_sum = PolyAt(&long_p, -1);
        if (!PolyIsEq(&mono_sum, &p))
            res = false;
        PolyDestroy(&mono_sum);
        PolyDestroy(&long_p);
    }
    PolyDestroy(&p);
    return res;
}

/**
 *  Tests primitive cases of "PolyAt" command.
 */
static bool AtTest1(void) {
    Poly p_one = PolyFromCoeff(1);
    Poly p_two = PolyFromCoeff(2);
    Poly p, p_res, p_expected_res;
    p_expected_res = PolyFromCoeff(LONG_MAX);
    const size_t bits_num = sizeof (poly_coeff_t) * CHAR_BIT - 1;
    Mono m[bits_num];
    for (size_t i = 0; i < bits_num; ++i) {
        p = PolyClone(&p_one);
        m[i] = MonoFromPoly(&p, (poly_exp_t) i);
    }
    p = PolyAddMonos(bits_num, m);
    p_res = PolyAt(&p, 2);
    if (!PolyIsEq(&p_res, &p_expected_res))
        return false;
    PolyDestroy(&p);
    PolyDestroy(&p_res);
    PolyDestroy(&p_expected_res);
    p_expected_res = PolyFromCoeff(LONG_MAX - 1);
    for (size_t i = 0; i < bits_num - 1; ++i) {
        p = PolyClone(&p_two);
        m[i] = MonoFromPoly(&p, (poly_exp_t) i);
    }
    p = PolyAddMonos(bits_num - 1, m);
    p_res = PolyAt(&p, 2);
    if (!PolyIsEq(&p_res, &p_expected_res))
        return false;
    PolyDestroy(&p);
    PolyDestroy(&p_res);
    PolyDestroy(&p_expected_res);
    PolyDestroy(&p_one);
    PolyDestroy(&p_two);
    return true;
}

/**
 * Tests "PolyAt" at recursively created polynomials.
 */
static bool AtTest2(void) {
    const size_t poly_size = 6;
    const size_t poly_depth = 3;
    const size_t upper_size = 5;
    bool result = true;
    Poly p = PolyFromCoeff(1);
    Mono *m = calloc(poly_size, sizeof (Mono));
    for (size_t j = 0; j < poly_depth; ++j) {
        for (size_t i = 0; i < poly_size; ++i) {
            Poly tmp = PolyClone(&p);
            m[i] = MonoFromPoly(&tmp, (poly_exp_t) i);
        }
        PolyDestroy(&p);
        p = PolyAddMonos(poly_size, m);
        // p = p + px + ... + px^n
    }
    Poly p_upper_size = PolyFromCoeff((poly_coeff_t) upper_size);
    Poly p2 = PolyMul(&p_upper_size, &p);
    free(m);
    m = calloc(upper_size, sizeof (Mono));
    for (size_t i = 0; i < upper_size; ++i) {
        Poly tmp = PolyClone(&p);
        m[i] = MonoFromPoly(&tmp, (poly_exp_t) i);
    }
    PolyDestroy(&p);
    p = PolyAddMonos(upper_size, m);
    Poly p3 = PolyAt(&p, 1);
    if (!PolyIsEq(&p3, &p2))
        result = false;
    if (PolyDeg(&p) != (upper_size - 1) + (poly_depth * ((poly_exp_t)poly_size - 1)))
        result = false;
    if (PolyDegBy(&p, 0) != upper_size - 1)
        result = false;
    if (PolyDegBy(&p, 1) != (poly_exp_t)poly_size - 1)
        result = false;
    PolyDestroy(&p);
    PolyDestroy(&p2);
    PolyDestroy(&p3);
    PolyDestroy(&p_upper_size);
    free(m);
    return result;
}

/**
 *  Tests degree change after arithmetic operations.
 */
static bool DegreeOpChangeTest(void) {
    Poly p_one = PolyFromCoeff(1);
    Poly p_res = PolySub(&p_one, &p_one); // 1 - 1
    if (!PolyIsZero(&p_res))
        return false;
    PolyDestroy(&p_res);
    Poly p2 = PolyNeg(&p_one);
    p_res = PolyAdd(&p_one, &p2); // 1 + -1
    if (!PolyIsZero(&p_res))
        return false;
    PolyDestroy(&p_res);
    PolyDestroy(&p2);
    {
        const poly_exp_t poly_len = 5;
        Mono m[poly_len];
        for (poly_exp_t i = 0; i < poly_len; ++i) {
            p2 = PolyClone(&p_one);
            m[i] = MonoFromPoly(&p2, i);
        }
        Mono m2 = MonoClone(&m[poly_len - 1]);
        p2 = PolyAddMonos(poly_len, m);
        Poly p3 = PolyAddMonos(1, &m2);
        p_res = PolySub(&p2, &p3); // (1 + x + ... + x^n) - x^n
        if (PolyDeg(&p_res) != poly_len - 2)
            return false;
        PolyDestroy(&p2);
        PolyDestroy(&p3);
        PolyDestroy(&p_res);
    }
    {
        const poly_exp_t poly_len = 5;
        Mono m[poly_len];
        for (poly_exp_t i = 0; i < poly_len - 1; i++) {
            p2 = PolyClone(&p_one);
            m[i] = MonoFromPoly(&p2, i);
        }
        p2 = PolyClone(&p_one);
        m[poly_len - 1] = MonoFromPoly(&p2, poly_len);
        Mono m2[2];
        m2[0] = MonoClone(&m[poly_len - 1]);

        p2 = PolyAddMonos(poly_len, m);
        Poly p3;
        p3 = PolyClone(&p_one);
        m2[1] = MonoFromPoly(&p3, poly_len - 1);

        p3 = PolyAddMonos(2, m2);
        p_res = PolySub(&p2, &p3);
        // (1 + x + ... x^(n - 2) + x^n) - (x^(n-1) + x^n)
        if (PolyDeg(&p_res) != poly_len - 1)
            return false;
        PolyDestroy(&p2);
        PolyDestroy(&p3);
        PolyDestroy(&p_res);
    }
    PolyDestroy(&p_one);
    return true;
}

/**
 *  Tests correctness of degree evaluation.
 */
static bool DegTest(void) {
    Poly p = P(P(C(1), 1, C(1), 10), 1, P(C(1), 1, C(1), 2), 2);
    bool res = PolyDeg(&p) == 11;
    PolyDestroy(&p);
    return res;
}

/**
 * Creates poly with one variable from coefficients and exponents.
 * @param count number of monomials
 * @param val coefficients
 * @param exp exponents
 */
static Poly MakePoly(size_t count, const poly_coeff_t *val, poly_exp_t *exp) {
    Mono *tmp = calloc(count, sizeof (Mono));
    size_t shift = 0;
    for (size_t i = 0; i < count; i++) {
        Poly p = PolyFromCoeff(val[i]);
        if (val[i] == 0) {
            shift--;
            PolyDestroy(&p);
        }
        else {
            tmp[i + shift] = MonoFromPoly(&p, exp[i]);
        }
    }
    Poly res = PolyAddMonos(count + shift, tmp);
    free(tmp);
    return res;
}

/**
 * Creates multi-variable poly from coefficients and exponents.
 * @param count number of monomials
 * @param val coefficients
 * @param exp exponents
 */
static Poly MakePolyFromPolynomials(size_t count, const Poly *val,
                                    poly_exp_t *exp) {
    Mono *tmp = calloc(count, sizeof (struct Mono));
    for (size_t i = 0; i < count; ++i)
        tmp[i] = MonoFromPoly(&val[i], exp[i]);
    Poly res = PolyAddMonos(count, tmp);
    free(tmp);
    return res;
}

/**
 *  Creates poly with of an arbitrary depth
 *  using first exponent and coefficient dataset.
 */
static Poly RecursiveBuild(int depth, int *exp_shift, int *coef_shift) {
    if (depth == 0) {
        return PolyFromCoeff(coef_arr1[(*coef_shift)++]);
    }
    else {
        size_t size = exp_arr1[*exp_shift];
        *exp_shift += 1;
        Mono m[size];
        for (size_t i = 0; i < size; ++i) {
            Poly p = RecursiveBuild(depth - 1, exp_shift, coef_shift);
            if (PolyIsZero(&p)) {
                PolyDestroy(&p);
                Poly p2 = PolyFromCoeff(1);
                m[i] = MonoFromPoly(&p2, exp_arr2[*exp_shift]);
            }
            else {
                m[i] = MonoFromPoly(&p, exp_arr2[*exp_shift]);
            }
            *exp_shift += 1;
        }
        return PolyAddMonos(size, m);
    }
}

/**
 *  Creates poly with of an arbitrary depth
 *  using second exponent and coefficient dataset.
 */
static Poly RecursiveBuild2(int depth, int *exp_shift, int *coef_shift,
                            const poly_coeff_t *coef_arr,
                            const poly_exp_t *exp_arr) {
    if (depth == 0) {
        return PolyFromCoeff(coef_arr[(*coef_shift)++]);
    }
    else {
        size_t size = exp_arr[*exp_shift];
        *exp_shift += 1;
        Mono m[size];
        for (size_t i = 0; i < size; ++i) {
            Poly p = RecursiveBuild2(depth - 1, exp_shift, coef_shift, coef_arr, exp_arr);
            if (PolyIsZero(&p)) {
                PolyDestroy(&p);
                Poly p2 = PolyFromCoeff(1);
                m[i] = MonoFromPoly(&p2, exp_arr[*exp_shift]);
            }
            else {
                m[i] = MonoFromPoly(&p, exp_arr[*exp_shift]);
            }
            *exp_shift += 1;
        }
        return PolyAddMonos(size, m);
    }
}

/**
 *  Tests whether "PolyDegBy" and "PolyDeg" checks all monomials.
 */
static bool DegByTest(void) {
    bool result = true;
    int exp_shift = 0;
    int coef_shift = 0;
    const unsigned depth = 5;
    Poly p = RecursiveBuild((int) depth, &exp_shift, &coef_shift);
    int deg_arr[] = {355, 368, 384, 399, 399};
    for (unsigned i = 0; i < depth; ++i)
        if (deg_arr[i] != PolyDegBy(&p, i))
            result = false;
    if (1639 != PolyDeg(&p))
        result = false;
    PolyDestroy(&p);
    return result;
}

/**
 *  Tests multiplication using the short formulas.
 */
static bool MulTest1(void) {
    {
        poly_coeff_t val[] = {1, 1};
        poly_exp_t exp[] = {0, 1};
        Poly p1 = MakePoly(2, val, exp);
        Poly p2 = PolyMul(&p1, &p1);
        poly_coeff_t res_val[] = {1, 2, 1};
        poly_exp_t res_exp[] = {0, 1, 2};
        Poly p_res1 = MakePoly(3, res_val, res_exp);
        if (!PolyIsEq(&p2, &p_res1))
            return false;
        PolyDestroy(&p2);
        PolyDestroy(&p_res1);
        PolyDestroy(&p1);
    }
    {
        poly_coeff_t val1[] = {1, 1};
        poly_coeff_t val2[] = {1, -1};
        poly_exp_t exp[] = {0, 1};
        poly_exp_t exp_res[] = {0, 2};
        Poly p1 = MakePoly(2, val1, exp);
        Poly p2 = MakePoly(2, val2, exp);
        Poly p3 = PolyMul(&p1, &p2);
        Poly p_res1 = MakePoly(2, val2, exp_res);
        if (!PolyIsEq(&p3, &p_res1))
            return false;
        PolyDestroy(&p3);
        PolyDestroy(&p_res1);
        PolyDestroy(&p1);
        PolyDestroy(&p2);
    }
    {
        Poly p_two = PolyFromCoeff(2);
        poly_coeff_t val[] = {1, 1};
        poly_exp_t exp[] = {0, 1};
        Poly p1 = MakePoly(2, val, exp);
        Poly p_arr[] = {PolyClone(&p1), PolyClone(&p1)};
        Poly p2 = MakePolyFromPolynomials(2, p_arr, exp);
        Poly p3 = PolyMul(&p2, &p2);
        poly_coeff_t res_val[] = {1, 2, 1};
        poly_exp_t res_exp[] = {0, 1, 2};
        Poly p_res1 = MakePoly(3, res_val, res_exp);
        Poly p_arr2[] = {PolyClone(&p_res1),
                         PolyMul(&p_res1, &p_two),
                         PolyClone(&p_res1)};
        Poly p_res2 = MakePolyFromPolynomials(3, p_arr2, res_exp);
        if (!PolyIsEq(&p3, &p_res2))
            return false;
        PolyDestroy(&p_two);
        PolyDestroy(&p3);
        PolyDestroy(&p_res1);
        PolyDestroy(&p_res2);
        PolyDestroy(&p1);
        PolyDestroy(&p2);
    }
    return true;
}

/**
 *  Creates poly with @p rec variables
 *  where each variable is bind to one monomial.
 */
static Poly BuildRecursivePoly(const poly_coeff_t *coef_arr,
                               const poly_exp_t *exp_arr, size_t rec) {
    Poly res = PolyZero();
    for (size_t i = rec; i > 0; --i) {
        poly_coeff_t coef = coef_arr[i - 1];
        poly_exp_t exp = exp_arr[i - 1];
        if (coef == 0) {
            if (!PolyIsZero(&res)) {
                Mono m = MonoFromPoly(&res, 0);
                res = PolyAddMonos(1, &m);
            }
            continue;
        }
        Poly p = PolyFromCoeff(coef);
        if (PolyIsZero(&res)) {
            Mono m = MonoFromPoly(&p, exp);
            PolyDestroy(&res);
            res = PolyAddMonos(1, &m);
        }
        else {
            Mono m[2] = {MonoFromPoly(&p, exp), MonoFromPoly(&res, 0)};
            res = PolyAddMonos(2, m);
        }
    }
    return res;
}

/**
 *  Tests addition of long single-variable polynomials.
 */
static bool AddTest1(void) {
    const poly_exp_t step = 10;
    bool good = true;
    size_t current_max_exp = 10;
    size_t previous_current_max_exp = 0;
    poly_exp_t *exp_list = calloc(conf_size, sizeof (poly_exp_t));
    poly_coeff_t *poly_coef_res_arr = calloc(conf_size, sizeof (poly_coeff_t));
    for (size_t i = 0; i < conf_size; ++i) {
        exp_list[i] = (poly_exp_t)i;
        poly_coef_res_arr[i] = coef_arr2[i];
    }
    while (current_max_exp <= conf_size && good) {
        for (size_t i = previous_current_max_exp; i < current_max_exp; ++i)
            poly_coef_res_arr[i] = coef_arr1[i] + coef_arr2[i];
        previous_current_max_exp = current_max_exp;
        Poly p1 = MakePoly(current_max_exp, coef_arr1, exp_list);
        size_t local_max_exp = current_max_exp;
        while (local_max_exp <= conf_size && good) {
            Poly p2 = MakePoly(local_max_exp, coef_arr2, exp_list);
            Poly p3 = PolyAdd(&p1, &p2);
            PolyDestroy(&p2);
            Poly p_expected_res = MakePoly(local_max_exp, poly_coef_res_arr, exp_list);
            if (!PolyIsEq(&p3, &p_expected_res))
                good = false;
            PolyDestroy(&p3);
            PolyDestroy(&p_expected_res);
            local_max_exp *= step;
        }
        Poly p2 = PolyNeg(&p1);
        Poly p3 = PolyAdd(&p2, &p1);
        if (!PolyIsZero(&p3))
            good = false;
        PolyDestroy(&p1);
        PolyDestroy(&p2);
        PolyDestroy(&p3);
        current_max_exp *= step;
    }
    free(exp_list);
    free(poly_coef_res_arr);
    return good;
}

/**
 *  Tests addition of long multi-variable polynomials.
 */
static bool AddTest2(void) {
    bool good = true;
    const poly_exp_t step = 2;
    poly_coeff_t *pol_arr_expected_res = calloc(conf_size, sizeof (poly_coeff_t));
    for (size_t j = 0; j < conf_size; ++j)
        pol_arr_expected_res[j] = coef_arr2[j];
    size_t prev_poly_len = 0;
    size_t first_poly_len = 2;
    size_t second_poly_len;
    while (first_poly_len <= conf_size && good) {
        for (size_t i = prev_poly_len; i < first_poly_len; ++i)
            pol_arr_expected_res[i] += coef_arr1[i];
        prev_poly_len = first_poly_len;
        Poly p1 = BuildRecursivePoly(coef_arr1, exp_arr1, first_poly_len);
        second_poly_len = first_poly_len;
        while (second_poly_len <= conf_size && good) {
            Poly p2 = BuildRecursivePoly(coef_arr2, exp_arr1, second_poly_len);
            Poly p_res = PolyAdd(&p1, &p2);
            PolyDestroy(&p2);
            Poly p_expected_res = BuildRecursivePoly(pol_arr_expected_res, exp_arr1,
                                                     second_poly_len);
            if (!PolyIsEq(&p_res, &p_expected_res))
                good = false;
            PolyDestroy(&p_res);
            PolyDestroy(&p_expected_res);
            second_poly_len *= step;
        }
        Poly p2 = PolyNeg(&p1);
        Poly p3 = PolyAdd(&p2, &p1);
        if (!PolyIsZero(&p3))
            good = false;
        PolyDestroy(&p1);
        PolyDestroy(&p2);
        PolyDestroy(&p3);
        first_poly_len *= step;
    }
    free(pol_arr_expected_res);
    return good;
}

/**
 *  Tests subtraction of long single-variable polynomials.
 */
static bool SubTest1(void) {
    const poly_exp_t step = 10;
    bool good = true;
    size_t current_max_exp = 10;
    size_t previous_current_max_exp = 0;
    poly_exp_t *exp_list = calloc(conf_size, sizeof (poly_exp_t));
    poly_coeff_t *poly_coef_res_arr = calloc(conf_size, sizeof (poly_coeff_t));
    for (size_t i = 0; i < conf_size; ++i) {
        exp_list[i] = (poly_exp_t)i;
        poly_coef_res_arr[i] = coef_arr2[i];
    }
    while (current_max_exp <= conf_size && good) {
        for (size_t i = previous_current_max_exp; i < current_max_exp; ++i)
            poly_coef_res_arr[i] = coef_arr2[i] - coef_arr1[i];
        previous_current_max_exp = current_max_exp;
        Poly p1 = MakePoly(current_max_exp, coef_arr1, exp_list);
        size_t local_max_exp = current_max_exp;
        while (local_max_exp <= conf_size && good) {
            Poly p2 = MakePoly(local_max_exp, coef_arr2, exp_list);
            Poly p3 = PolySub(&p2, &p1);
            Poly p_expected_res = MakePoly(local_max_exp, poly_coef_res_arr, exp_list);
            if (!PolyIsEq(&p3, &p_expected_res))
                good = false;
            PolyDestroy(&p3);
            PolyDestroy(&p2);
            PolyDestroy(&p_expected_res);
            local_max_exp *= step;
        }
        PolyDestroy(&p1);
        current_max_exp *= step;
    }
    free(exp_list);
    free(poly_coef_res_arr);
    return good;
}

/**
 *  Tests subtraction of long multi-variable polynomials.
 */
static bool SubTest2(void) {
    bool good = true;
    const poly_exp_t step = 2;
    poly_coeff_t *pol_arr_expected_res = calloc(conf_size, sizeof (poly_coeff_t));
    for (size_t j = 0; j < conf_size; ++j)
        pol_arr_expected_res[j] = coef_arr2[j];
    size_t prev_poly_len = 0;
    size_t first_poly_len = 2;
    size_t second_poly_len;
    while (first_poly_len <= conf_size && good) {
        for (size_t i = prev_poly_len; i < first_poly_len; ++i)
            pol_arr_expected_res[i] -= coef_arr1[i];
        prev_poly_len = first_poly_len;
        Poly p1 = BuildRecursivePoly(coef_arr1, exp_arr1, first_poly_len);
        second_poly_len = first_poly_len;
        while (second_poly_len <= conf_size && good) {
            Poly p2 = BuildRecursivePoly(coef_arr2, exp_arr1, second_poly_len);
            Poly p_res = PolySub(&p2, &p1);
            Poly p_expected_res = BuildRecursivePoly(pol_arr_expected_res, exp_arr1,
                                                     second_poly_len);
            if (!PolyIsEq(&p_res, &p_expected_res))
                good = false;
            PolyDestroy(&p2);
            PolyDestroy(&p_res);
            PolyDestroy(&p_expected_res);
            second_poly_len *= step;
        }
        Poly p2 = PolyNeg(&p1);
        Poly p3 = PolyAdd(&p2, &p1);
        if (!PolyIsZero(&p3))
            good = false;
        PolyDestroy(&p1);
        PolyDestroy(&p2);
        PolyDestroy(&p3);
        first_poly_len *= step;
    }
    free(pol_arr_expected_res);

    return good;
}

static poly_coeff_t *MullArray(size_t size1, const poly_coeff_t *arr_1,
                               size_t size2, const poly_coeff_t *arr_2) {
    poly_coeff_t *res = calloc(size1 + size2, sizeof (poly_coeff_t));
    for (size_t i = 0; i < size1; ++i)
        for (size_t j = 0; j < size2; ++j)
            res[i + j] += arr_1[i] * arr_2[j];
    return res;
}

/**
 *  Tests multiplication of long polynomials.
 */
static bool MulTest2(void) {
    bool good = true;
    size_t step = 10;
    size_t poly_one_len = 5;
    size_t poly_two_len;
    size_t current_conf_size = conf_size / 20 + 1;
    poly_exp_t *exp_list = calloc(2 * current_conf_size, sizeof (poly_exp_t));
    for (size_t i = 0; i < current_conf_size * 2; ++i)
        exp_list[i] = (poly_exp_t)i;
    while (poly_one_len < current_conf_size && good) {
        poly_two_len = poly_one_len;
        Poly p1 = MakePoly(poly_one_len, coef_arr1, exp_list);
        while (poly_two_len < current_conf_size && good) {
            Poly p2 = MakePoly(poly_two_len, coef_arr2, exp_list);
            poly_coeff_t *expected_res_coef = MullArray(poly_one_len, coef_arr1,
                                                        poly_two_len, coef_arr2);
            Poly p_expected_res = MakePoly(poly_one_len + poly_two_len,
                                           expected_res_coef, exp_list);
            Poly p_res = PolyMul(&p1, &p2);
            if (!PolyIsEq(&p_expected_res, &p_res))
                good = false;
            PolyDestroy(&p2);
            PolyDestroy(&p_expected_res);
            PolyDestroy(&p_res);
            free(expected_res_coef);
            poly_two_len *= step;
        }
        PolyDestroy(&p1);
        poly_one_len *= step;
    }
    free(exp_list);
    return good;
}

/**
 *  Tests "IsEq" on long polynomials.
 */
static bool IsEqTest(void) {
    bool result = true;
    const size_t copy_size = 1000;
    poly_coeff_t *coef_copy = calloc(copy_size, sizeof (poly_coeff_t));
    memcpy(coef_copy, coef_arr1, copy_size * sizeof (poly_coeff_t));
    coef_copy[90]++;
    {
        Poly p1 = PolyFromCoeff(1);
        Poly p2 = PolyFromCoeff(1);
        Poly p3 = PolyFromCoeff(2);
        if (!PolyIsEq(&p1, &p2))
            result = false;
        if (PolyIsEq(&p1, &p3))
            result = false;
        PolyDestroy(&p1);
        PolyDestroy(&p2);
        PolyDestroy(&p3);
    }
    if (result) {
        Poly p1 = MakePoly(100, coef_arr1, exp_arr1);
        Poly p2 = MakePoly(100, coef_arr1, exp_arr1);
        Poly p3 = MakePoly(100, coef_copy, exp_arr1);
        if (!PolyIsEq(&p1, &p2))
            result = false;
        if (PolyIsEq(&p1, &p3))
            result = false;
        PolyDestroy(&p1);
        PolyDestroy(&p2);
        PolyDestroy(&p3);
    }
    if (result) {
        Poly p1 = BuildRecursivePoly(coef_arr1, exp_arr1, 100);
        Poly p2 = BuildRecursivePoly(coef_arr1, exp_arr1, 100);
        Poly p3 = BuildRecursivePoly(coef_copy, exp_arr1, 100);
        if (!PolyIsEq(&p1, &p2))
            result = false;
        if (PolyIsEq(&p1, &p3))
            result = false;
        PolyDestroy(&p1);
        PolyDestroy(&p2);
        PolyDestroy(&p3);
    }
    if (result) {
        int exp_shift_base = 0;
        int coef_shift_base = 5;
        int exp_shift = exp_shift_base;
        int coef_shift = coef_shift_base;
        Poly p1 = RecursiveBuild2(3, &exp_shift, &coef_shift, coef_arr1, exp_arr1);
        exp_shift = exp_shift_base;
        coef_shift = coef_shift_base;
        Poly p2 = RecursiveBuild2(3, &exp_shift, &coef_shift, coef_arr1, exp_arr1);
        exp_shift = exp_shift_base;
        coef_shift = coef_shift_base;
        Poly p3 = RecursiveBuild2(3, &exp_shift, &coef_shift, coef_copy, exp_arr1);
        if (!PolyIsEq(&p1, &p2))
            result = false;
        if (PolyIsEq(&p1, &p3))
            result = false;
        PolyDestroy(&p1);
        PolyDestroy(&p2);
        PolyDestroy(&p3);
    }
    free(coef_copy);
    return result;
}

/**
 *  Tests memory usage for rare polynomials.
 */
static bool RarePolynomialTest(void) {
    bool result = true;
    const size_t size = 4000;
    poly_exp_t rare_exp_arr[size];
    rare_exp_arr[0] = exp_arr2[0];
    poly_coeff_t sum = coef_arr1[0];
    for (size_t i = 1; i < size; ++i) {
        rare_exp_arr[i] = rare_exp_arr[i - 1] + exp_arr2[i];
        sum += coef_arr1[i];
    }
    Poly p = MakePoly(size, coef_arr1, rare_exp_arr);
    Poly expected_res = PolyFromCoeff(sum);
    Poly res = PolyAt(&p, 1);
    if (!PolyIsEq(&expected_res, &res))
        result = false;
    if (PolyDeg(&p) != rare_exp_arr[size - 1])
        result = false;
    PolyDestroy(&p);
    PolyDestroy(&res);
    PolyDestroy(&expected_res);
    return result;
}

/**
 *  Tests whether PolyAddMonos and MonoFromPoly performs a plain copy of input.
 */
static bool MemoryThiefTest(void) {
    const size_t poly_size = 10;
    const size_t poly_depth = 3;
    Poly p = PolyFromCoeff(1);
    Mono *m = calloc(poly_size, sizeof (Mono));
    for (size_t j = 0; j < poly_depth; ++j) {
        for (size_t i = 0; i < poly_size; ++i) {
            Poly tmp = PolyClone(&p);
            m[i] = MonoFromPoly(&tmp, (poly_exp_t) i);
        }
        PolyDestroy(&p);
        p = PolyAddMonos(poly_size, m);
        // p = p + px + px^2
    }
    Poly p2 = PolyClone(&p);
    Mono m2 = MonoFromPoly(&p2, 5);
    MonoDestroy(&m2);
    PolyDestroy(&p);
    free(m);
    return true;
}

static bool MemoryFreeTest(void) {
    Poly *p = malloc(sizeof (struct Poly));
    *p = PolyFromCoeff(5);
    Mono m = MonoFromPoly(p, 4);
    *p = PolyFromCoeff(3);
    PolyDestroy(p);
    free(p);
    Poly p2 = PolyAddMonos(1, &m);
    Poly p3 = PolyAt(&p2, 2);
    Poly p4 = PolyFromCoeff(80);
    bool res = PolyIsEq(&p4, &p3);
    PolyDestroy(&p2);
    PolyDestroy(&p3);
    PolyDestroy(&p4);
    return res;
}

/**
 * Tests correctness of "PolyCompose".
 */
static bool ComposeTest(void) {
    Poly q[] = {
            P(C(1), 4),
            P(P(C(1), 0, C(1), 1), 1)
    };

    size_t size = sizeof(q) / sizeof(q[0]);

    Poly p = P(P(P(C(1), 6), 5), 2, P(C(1), 0, C(1), 2), 3, C(5), 7);
    Poly received = PolyCompose(&p, size, q);
    Poly expected = P(C(1), 12, P(C(1), 0, C(2), 1, C(1), 2), 14, C(5), 28);

    bool res = PolyIsEq(&expected, &received);

    PolyDestroy(&received);
    PolyDestroy(&expected);
    PolyDestroy(&p);

    for (size_t i = 0; i < size; i++)
        PolyDestroy(&q[i]);

    return res;
}

/**
 *  Tests whether "PolyOwnMonos" doesn't copy monomials from input.
 */
static bool PolyOwnTest(void) {
    Mono* monos = malloc(2 * sizeof(Mono));
    CHECK_PTR(monos);

    monos[0] = M(P(C(-1), 1), 1);
    monos[1] = M(P(C(1), 1), 2);

    Poly *p1 = &(monos + 0)->p;
    Poly *p2 = &(monos + 1)->p;

    Poly received = PolyOwnMonos(2, monos);
    Poly expected = P(P(C(-1), 1), 1, P(C(1), 1), 2);

    bool res = PolyIsEq(&received, &expected);

    PolyDestroy(&received);
    PolyDestroy(&expected);

    res &= (monos && p1 && p2);

    return res;
}

/**
 *  Tests whether "PolyClone" deep copies monomials form input.
 */
static bool PolyCloneTest(void) {
    Mono* monos = malloc(2 * sizeof(Mono));
    CHECK_PTR(monos);

    monos[0] = M(P(C(-1), 1), 1);
    monos[1] = M(P(C(1), 1), 2);

    Poly *p1 = &(monos + 0)->p;
    Poly *p2 = &(monos + 1)->p;

    Poly received = PolyCloneMonos(2, monos);
    Poly expected = P(P(C(-1), 1), 1, P(C(1), 1), 2);

    bool res = PolyIsEq(&received, &expected);

    PolyDestroy(&received);
    PolyDestroy(&expected);

    if (!monos || !p1 || !p2)
        return false;

    PolyDestroy(p1);
    PolyDestroy(p2);
    free(monos);

    return res;
}

/** Test groups **/

static bool SimpleNegGroup(void) {
    return SimpleNegTest() && SimpleSubTest();
}

static bool SimpleDegGroup(void) {
    return SimpleDegByTest() && SimpleDegTest();
}

static bool AtGroup(void) {
    return AtTest1() && AtTest2();
}

static bool DegGroup(void) {
    return DegTest() && DegByTest();
}

static bool ArithmeticGroup(void) {
    return MulTest1() && MulTest2() &&
           AddTest1() && AddTest2() &&
           SubTest1() && SubTest2();
}

static bool MemoryGroup(void) {
    return RarePolynomialTest() &&
           MemoryThiefTest() &&
           MemoryFreeTest();
}

/** Running the tests */

#define SIZE(x) (sizeof (x) / sizeof (x)[0])

typedef struct {
    char const *name;
    bool (*function)(void);
} test_list_t;

#define TEST(t) {#t, t}

static const test_list_t test_list[] = {
        TEST(SimpleAddTest),
        TEST(SimpleAddMonosTest),
        TEST(SimpleMulTest),
        TEST(SimpleNegTest),
        TEST(SimpleSubTest),
        TEST(SimpleNegGroup),
        TEST(SimpleDegByTest),
        TEST(SimpleDegTest),
        TEST(SimpleDegGroup),
        TEST(SimpleIsEqTest),
        TEST(SimpleAtTest),
        TEST(OverflowTest),
        TEST(SimpleArithmeticTest),
        TEST(LongPolynomialTest),
        TEST(AtTest1),
        TEST(AtTest2),
        TEST(AtGroup),
        TEST(DegreeOpChangeTest),
        TEST(DegTest),
        TEST(DegByTest),
        TEST(DegGroup),
        TEST(MulTest1),
        TEST(MulTest2),
        TEST(AddTest1),
        TEST(AddTest2),
        TEST(SubTest1),
        TEST(SubTest2),
        TEST(ArithmeticGroup),
        TEST(IsEqTest),
        TEST(RarePolynomialTest),
        TEST(MemoryThiefTest),
        TEST(MemoryFreeTest),
        TEST(MemoryGroup),
        TEST(ComposeTest),
        TEST(PolyOwnTest),
        TEST(PolyCloneTest)
};

int main() {
    for (size_t i = 0; i < SIZE(test_list); ++i) {
        if (!test_list[i].function()) {
            fprintf(stderr, "%s: WRONG\n", test_list[i].name);
        }
    }

    return EXIT_SUCCESS;
}