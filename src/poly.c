#include "poly.h"
#include <stdlib.h>

// TO-DO:
// 1) What does it mean "na własność" (valgrind)
// 3) Doxygen add maths (
// 2) Reduce
// 4) CMake + Debug version
// 5) Way to long and big functions
// 6) Should it be in the interface?
// 7) pow ???
// 8) Just git and push
// 10) add static

bool PolyZeroMonos(const Poly *p);

bool MonoIsZero(const Mono *m) {
    return ((HasConstPoly(m) && PolyIsZero(&m->p)) ||
            (!HasConstPoly(m) && PolyZeroMonos(&m->p)));
}

bool PolyZeroMonos(const Poly *p) {
    assert(!PolyIsCoeff(p));

    for (size_t curMonoID = 0; curMonoID < p->size; curMonoID++) {
        if (!MonoIsZero(&p->arr[curMonoID]))
            return false;
    }

    return true;
}

bool DestroyMonoIfZero(Mono *m) {
    bool isZero = MonoIsZero(m);

    if (isZero)
        MonoDestroy(m);

    return isZero;
}

bool PolyOnlyFreeTerm(const Poly *p) {
    assert(!PolyIsCoeff(p));

    for (size_t curMonoID = 0; curMonoID < p->size; curMonoID++) {
        if (curMonoID == 0 && MonoIsFreeTerm(&p->arr[0]))
            continue;

        else if (!MonoIsZero(&p->arr[curMonoID]))
            return false;
    }

    return true;
}

void PolyReduceToCoeff(Poly *p) {
    assert(!PolyIsCoeff(p));

    if (!PolyOnlyFreeTerm(p))
        return;

    Poly coeffPoly = PolyFromCoeff(PolyGetFreeTerm(p));

    PolyDestroy(p);
    *p = coeffPoly;
}

void PolyDestroy(Poly *p) {
    if (PolyIsCoeff(p))
        return;

    for (size_t curMonoID = 0; curMonoID < p->size; curMonoID++)
        MonoDestroy(&p->arr[curMonoID]);

    if (p->arr)
        free(p->arr);
}

void PolyDeepCopy(const Poly* p, Poly* pCopy) {
    if (PolyIsCoeff(p)) {
        pCopy->coeff = p->coeff;
        return;
    }

    *pCopy = PolyFromSize(p->size);

    for (size_t curMonoID = 0; curMonoID < p->size; curMonoID++)
        pCopy->arr[curMonoID] = MonoClone(&p->arr[curMonoID]);
}

Poly PolyClone(const Poly *p) {
    Poly pCopy = PolyZero();

    PolyDeepCopy(p, &pCopy);

    return pCopy;
}

Poly PolyAddNoConst(const Poly *p, const Poly *q) {
    assert(!PolyIsCoeff(p) && !PolyIsCoeff(q));

    Poly sumPoly = PolyFromSize(p->size + q->size);
    size_t sumMonoID = 0, pMonoID = 0, qMonoID = 0;

    while (pMonoID < p->size && qMonoID < q->size) {
        if (MonoGetExp(&p->arr[pMonoID]) < MonoGetExp(&q->arr[qMonoID])) {
            sumPoly.arr[sumMonoID++] = MonoClone(&p->arr[pMonoID++]);
        } else if (MonoGetExp(&p->arr[pMonoID]) > MonoGetExp(&q->arr[qMonoID])) {
            sumPoly.arr[sumMonoID++] = MonoClone(&q->arr[qMonoID++]);
        } else {
            sumPoly.arr[sumMonoID] = MonoAdd(&p->arr[pMonoID++], &q->arr[qMonoID++]);
            if (!DestroyMonoIfZero(&sumPoly.arr[sumMonoID]))
                sumMonoID++;
        }
    }

    while (pMonoID < p->size)
        sumPoly.arr[sumMonoID++] = MonoClone(&p->arr[pMonoID++]);
    while (qMonoID < q->size)
        sumPoly.arr[sumMonoID++] = MonoClone(&q->arr[qMonoID++]);

    PolySetSize(&sumPoly, sumMonoID);
    PolyReduceToCoeff(&sumPoly);

    return sumPoly;
}

Poly PolyAddOneConst(const Poly* p, const Poly* q) {
    assert(PolyIsCoeff(p) && !PolyIsCoeff(q));

    Poly sumPoly = PolyFromSize(q->size + 1);
    Mono constantMono = (Mono) {.exp = 0, .p = PolyClone(p)};
    size_t sumMonoID = 0, qMonoID = 0;

    if (MonoGetExp(&q->arr[qMonoID]) != 0)
        sumPoly.arr[sumMonoID] = constantMono;
    else
        sumPoly.arr[sumMonoID] = MonoAdd(&constantMono, &q->arr[qMonoID++]);

    if (!DestroyMonoIfZero(&sumPoly.arr[sumMonoID]))
        sumMonoID++;
    while (qMonoID < q->size)
        sumPoly.arr[sumMonoID++] = MonoClone(&q->arr[qMonoID++]);

    PolySetSize(&sumPoly, sumMonoID);
    PolyReduceToCoeff(&sumPoly);

    return sumPoly;
}

static Poly PolyAddBothConst(const Poly *p, const Poly *q) {
    assert(PolyIsCoeff(p) && PolyIsCoeff(q));
    return PolyFromCoeff(p->coeff + q->coeff);
}

Poly PolyAdd(const Poly *p, const Poly *q) {
    if (PolyIsCoeff(p) && PolyIsCoeff(q))
        return PolyAddBothConst(p, q);
    else if (!PolyIsCoeff(p) && !PolyIsCoeff(q))
        return PolyAddNoConst(p, q);
    return(PolyIsCoeff(p) ? PolyAddOneConst(p, q) : PolyAddOneConst(q, p));
}

int MonoComparator(const void* m1, const void* m2) {
     return (MonoGetExp(m1) - MonoGetExp(m2));
}

void RecursiveMonoArraySort(size_t count, Mono* monos) {
    qsort(monos, count, sizeof(Mono), MonoComparator);

    for (size_t curMonoID = 0; curMonoID < count; curMonoID++) {
        if (!HasConstPoly(&monos[curMonoID]))
            RecursiveMonoArraySort(monos[curMonoID].p.size, monos[curMonoID].p.arr);
    }
}

size_t countDifferentExps(size_t count, Mono* monos) {
    size_t expCounter = (count > 0);

    for (size_t curMonoID = 0; curMonoID + 1 < count; curMonoID++) {
        if (MonoGetExp(&monos[curMonoID]) != MonoGetExp(&monos[curMonoID + 1]))
            expCounter++;
    }

    return expCounter;
}

Poly PolyAddMonos(size_t count, const Mono monos[]) {
    if (count == 0)
        return PolyZero();

    RecursiveMonoArraySort(count, (Mono*) monos);
    Poly res = PolyFromSize(countDifferentExps(count, (Mono*) monos));
    size_t resMonoID = 0;

    for (size_t curMonoID = 0; curMonoID < count; curMonoID++) {
        poly_exp_t curExp = MonoGetExp(&monos[curMonoID]);
        res.arr[resMonoID] = monos[curMonoID]; // ok

        while (curMonoID + 1 < count && MonoGetExp(&monos[curMonoID + 1]) == curExp) {
            res.arr[resMonoID] = MonoAdd(&res.arr[resMonoID], &monos[curMonoID + 1]);
            curMonoID++;
        }

       if (!DestroyMonoIfZero(&res.arr[resMonoID]))
           resMonoID++;
    }

    PolySetSize(&res, resMonoID);
    PolyReduceToCoeff(&res);

    return res;
}

Poly PolyMulBothConst(const Poly *p, const Poly *q) {
    assert(PolyIsCoeff(p) && PolyIsCoeff(q));
    return PolyFromCoeff(p->coeff * q->coeff);
}

Poly PolyMulOneConst(const Poly *p, const Poly *q) {
    assert(PolyIsCoeff(p) && !PolyIsCoeff(q));

    if (PolyIsZero(p))
        return PolyZero();

    Mono constMono = {.exp = 0, .p = PolyFromCoeff(p->coeff)};
    Poly multiPoly = PolyFromSize(q->size);
    size_t multiMonoID = 0;

    for (size_t qMonoID = 0; qMonoID < q->size; qMonoID++)
        multiPoly.arr[multiMonoID++] = MonoMul(&constMono, &q->arr[qMonoID]);

    PolySetSize(&multiPoly, multiMonoID);
    PolyReduceToCoeff(&multiPoly);

    return multiPoly;
}

Poly PolyMulNoConst(const Poly *p, const Poly *q) {
    assert(!PolyIsCoeff(p) && !PolyIsCoeff(q));

    Poly multiPoly = PolyFromSize(p->size * q->size);
    size_t multiMonoID = 0;

    for (size_t pMonoID = 0; pMonoID < p->size; pMonoID++) {
        for (size_t qMonoID = 0; qMonoID < q->size; qMonoID++) {
            multiPoly.arr[multiMonoID++] = MonoMul(&p->arr[pMonoID], &q->arr[qMonoID]);
        }
    }

    PolySetSize(&multiPoly, multiMonoID);

    Poly sortedMultiPoly = PolyClone(&multiPoly);
    PolyDestroy(&multiPoly);

    sortedMultiPoly = PolyAddMonos(sortedMultiPoly.size, sortedMultiPoly.arr);

    return sortedMultiPoly;
}

Poly PolyMul(const Poly *p, const Poly *q) {
    if (PolyIsCoeff(p) && PolyIsCoeff(q))
        return PolyMulBothConst(p, q);
    else if (!PolyIsCoeff(p) && !PolyIsCoeff(q))
        return PolyMulNoConst(p, q);
    return (PolyIsCoeff(p) ? PolyMulOneConst(p, q) : PolyMulOneConst(q, p));
}

Poly PolyNeg(const Poly *p) {
    const Poly minusOne = PolyFromCoeff(-1);
    return PolyMul(p, &minusOne);
}

Poly PolySub(const Poly *p, const Poly *q) {
    const Poly qNegated = PolyNeg(q);
    return PolyAdd(p, &qNegated);
}

poly_coeff_t power(poly_coeff_t base, poly_exp_t power) {
    if (power == 0)
        return 1;

    poly_coeff_t res = 1;
    for (int i = 0; i < power; i++)
        res *= base;

    return res;
}

Poly PolyAt(const Poly *p, poly_coeff_t x) {
//    if (PolyIsCoeff(p))
//        return PolyClone(p);
//
//    size_t curMonoID = 0;
//    Mono* newMonos = malloc(p->size * sizeof(Mono));
//
//    while (curMonoID < p->size) {
//        poly_coeff_t newCoeff = power(x, MonoGetExp(&newMonos[curMonoID]));
//        Poly coeffPoly = PolyFromCoeff(newCoeff);
//
//        newMonos[curMonoID] = (Mono) {
//            .p = PolyMul(&coeffPoly, MonoGetPoly(&p->arr[curMonoID])),
//            .exp = MonoDegBy()
//        };
//
//        curMonoID++;
//    }
//
//    return PolyAddMonos(curMonoID, newMonos);
}

poly_exp_t PolyDegBy(const Poly *p, size_t var_idx) {
    if (PolyIsCoeff(p))
        return (PolyIsZero(p) ? -1 : 0);
    else if (var_idx == 0)
        return MonoGetExp(&p->arr[p->size - 1]);

    poly_exp_t maxMonoDegByIdx = 0;

    for (size_t curMonoID = 0; curMonoID < p->size; curMonoID++) {
        poly_exp_t curDeg = MonoDegBy(&p->arr[curMonoID], var_idx);

        if (maxMonoDegByIdx < curDeg)
            maxMonoDegByIdx = curDeg;
    }

    return maxMonoDegByIdx;
}

poly_exp_t PolyDeg(const Poly *p) {
    if (PolyIsCoeff(p))
        return (PolyIsZero(p) ? -1 : 0);

    poly_exp_t maxMonoDeg = 0;

    for (size_t curMonoID = 0; curMonoID < p->size; curMonoID++) {
        poly_exp_t curDeg = MonoDeg(&p->arr[curMonoID]);

        if (curDeg > maxMonoDeg)
            maxMonoDeg = curDeg;
    }

    return maxMonoDeg;
}

bool MonoIsEq(const Mono *m1, const Mono *m2) {
    return (MonoGetExp(m1) == MonoGetExp(m2) && PolyIsEq(MonoGetPoly(m1), MonoGetPoly(m2)));
}

bool PolyIsEqConst(const Poly *p, const Poly *q) {
    assert(PolyIsCoeff(p) && !PolyIsCoeff(q));
    return (PolyOnlyFreeTerm(q) && PolyGetFreeTerm(q) == p->coeff);
}

bool PolyIsEq(const Poly *p, const Poly *q) {
    if (PolyIsCoeff(p) && PolyIsCoeff(q))
        return (p->coeff == q->coeff);
    else if (PolyIsCoeff(p) || PolyIsCoeff(q))
        return (PolyIsCoeff(p) ? PolyIsEqConst(p, q) : PolyIsEqConst(q, p));
    else if (p->size != q->size)
        return false;

    for (size_t curMonoID = 0; curMonoID < p->size; curMonoID++) {
        if (!MonoIsEq(&p->arr[curMonoID], &q->arr[curMonoID]))
            return false;
    }

    return true;
}



