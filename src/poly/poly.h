/** @file
  Interface for a class of rare multi-variable polynomials

  @authors Jakub Pawlewicz <pan@mimuw.edu.pl>,
           Marcin Peczarski <marpe@mimuw.edu.pl>,
           Tymofii Vedmedenko
  @copyright University of Warsaw
  @date 2021
*/

#ifndef __POLY_H__
#define __POLY_H__

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

/** Type representing monomial coefficient. */
typedef long poly_coeff_t;

/** Type representing monomial exponent. */
typedef int poly_exp_t;

struct Mono;

/**
 * Type representing a multi-variable poly.
 */
typedef struct Poly {
    /*
     * Union representing either a value of a constant
     * poly (arr == NULL), or a count of monomials
     */
    union {
        poly_coeff_t coeff;
        size_t size;
    };

    /** Array of multi-variable monomials **/
    struct Mono *arr;
} Poly;

/**
 * Structure representing a monomial.
 * Monomial has a form @f$px_i^n@f$.
 * Coefficient @f$p@f$ can also be a
 * poly of multiple variables @f$x_{i+1}@f$.
 */
typedef struct Mono {
  Poly       p;   ///< coefficient
  poly_exp_t exp; ///< exponent
} Mono;


static inline poly_exp_t MonoGetExp(const Mono *m) {
    return m->exp;
}

static inline Poly* MonoGetPoly(const Mono *m) {
    return (Poly*) &m->p;
}

/**
 * Creates a constant poly.
 * @param[in] c : poly value
 * @return created poly
 */
static inline Poly PolyFromCoeff(poly_coeff_t c) {
    return (Poly) {.coeff = c, .arr = NULL};
}

/**
 * Creates a zero poly.
 * @return zero poly
 */
static inline Poly PolyZero(void) {
    return PolyFromCoeff(0);
}

static inline bool PolyIsZero(const Poly *p);

/**
 * Creates monomial @f$px_i^n@f$.
 * @param[in] p : poly - monomial coefficient
 * @param[in] n : exponent
 * @return monomial @f$px_i^n@f$
 */
static inline Mono MonoFromPoly(const Poly *p, poly_exp_t n) {
    assert(n == 0 || !PolyIsZero(p));
    return (Mono) {.p = *p, .exp = n};
}

/**
 * Checks if a poly is constant.
 * @param[in] p : poly
 * @return Is poly constant?
 */
static inline bool PolyIsCoeff(const Poly *p) {
    return p->arr == NULL;
}

/**
 * Checks if a poly is equivalent to zero.
 * @return Is poly equivalent to zero?
 */
static inline bool PolyIsZero(const Poly *p) {
    return PolyIsCoeff(p) && p->coeff == 0;
}

/**
 * Checks if a monomial is equivalent to zero.
 * @param[in] m : monomial
 * @return Is monomial equivalent to zero?
 */
static inline bool MonoIsZero(const Mono *m) {
    return PolyIsZero(&m->p);
}

/**
 * Clears an allocated memory for a poly.
 * @param[in] p : poly
 */
void PolyDestroy(Poly *p);

/**
 * Clears an allocated memory for a monomial.
 * @param[in] m : monomial
 */
static inline void MonoDestroy(Mono *m) {
    PolyDestroy(&m->p);
}

/**
 * Deep copies a poly.
 * @param[in] p : poly to copy
 * @return copied poly
 */
Poly PolyClone(const Poly *p);

/**
 * Deep copies a monomial.
 * @param[in] m : monomial to copy
 * @return copied monomial
 */
static inline Mono MonoClone(const Mono *m) {
    return (Mono) {
            .p = PolyClone(&m->p),
            .exp = m->exp
    };
}

/**
 * Performs an addition of two polynomials.
 * @param[in] p : poly @f$p@f$
 * @param[in] q : poly @f$q@f$
 * @return @f$p + q@f$
 */
Poly PolyAdd(const Poly *p, const Poly *q);

/**
 * Sums up a list of monomials. Takes
 * content of a @p monos on property.
 * @param[in] count : number of monomials
 * @param[in] monos : array of monomials
 * @return poly as a sum
 */
Poly PolyAddMonos(size_t count, const Mono monos[]);

/**
 * Sums up a list of monomials. Takes
 * the array @p monos on property.
 * @param[in] count : number of monomials
 * @param[in] monos : array of monomials
 * @return poly as a sum
 */
Poly PolyOwnMonos(size_t count, Mono *monos);

/**
 * Sums up a list of monomials. Clones
 * the content of @p monos.
 * @param[in] count : number of monomials
 * @param[in] monos : array of monomials
 * @return poly as a sum
 */
Poly PolyCloneMonos(size_t count, const Mono monos[]);

/**
 * Returns a multiplication of two polynomials.
 * @param[in] p : wielomian @f$p@f$
 * @param[in] q : wielomian @f$q@f$
 * @return @f$p * q@f$
 */
Poly PolyMul(const Poly *p, const Poly *q);

/**
 * Returns a negation of a poly.
 * @param[in] p : wielomian @f$p@f$
 * @return @f$-p@f$
 */
Poly PolyNeg(const Poly *p);

/**
 * Returns a subtraction result of two polynomials.
 * @param[in] p : poly @f$p@f$
 * @param[in] q : poly @f$q@f$
 * @return @f$p - q@f$
 */
Poly PolySub(const Poly *p, const Poly *q);

/**
 * Returns a degree of a poly of a specific variable.
 * Result is -1 for constant polynomials.
 * @param[in] p : wielomian
 * @param[in] var_idx : indeks zmiennej
 * @return stopień wielomianu @p p z względu na zmienną o indeksie @p var_idx
 */
poly_exp_t PolyDegBy(const Poly *p, size_t var_idx);

/**
 * Counts the degree of a poly.
 * Result is -1 for constant polynomials.
 * @param[in] p : poly
 * @return degree of @p p
 */
poly_exp_t PolyDeg(const Poly *p);

/**
 * Checks equality of two polynomials.
 * @param[in] p : poly @f$p@f$
 * @param[in] q : poly @f$q@f$
 * @return @f$p = q@f$
 */
bool PolyIsEq(const Poly *p, const Poly *q);

/**
 * Evaluates the poly at the point @p x.
 * @param[in] p : poly @f$p@f$
 * @param[in] x : point of evaluation @f$x@f$
 * @return @f$p(x, x_0, x_1, \ldots)@f$
 */
Poly PolyAt(const Poly *p, poly_coeff_t x);

/**
 * Composes poly @p with @p k polynomials from
 * an array @p q. If the length of @p is less than than @p k,
 * the rest of monomials are set to zero.
 * @param[in] p : outer poly @f$p@f$
 * @param[in] q : inner polynomials
 * @param[in] k : number of the inner
 * @return @f$p(q_0, q_1, q_2, \ldots)@f$
 */
Poly PolyCompose(const Poly *p, size_t k, const Poly q[]);

#endif /* __POLY_H__ */

