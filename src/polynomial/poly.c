/** @file
  Implementacja klasy wielomianów rzadkich wielu zmiennych.
  Przyjmiemy pewne założenia o wielomianach:

  (a) Wielomian ma posortowane jednomiany w porządku rosnącym według wykładników.

  (b) Wielomian jest stały wtedy i tylko wtedy, gdy nie ma jednomianów.

  (c) Wielomian niestały nie może mieć zerowego jednomianu.

  @author Tymofii Vedmedenko <tv433559@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include <stdlib.h>
#include <string.h>

#include "poly.h"

/**
 * Sprawdza, czy jednomian jest zerem. Zerowym jednomianem
 * nazywamy jednomian, przy którym stoi zerowy wielomian.
 * @param[in] m : jednomian
 * @return Czy jednomian jest zerem?
 */
static inline bool MonoIsZero(const Mono *m) {
    return PolyIsZero(&m->p);
}

/**
 * Daje wskaźnik na wielomian przy jednomianie
 * @param[in] m : jednomian
 * @return wskaźnik na wielomian
 */
static inline Poly* MonoGetPoly(const Mono *m) {
    return (Poly*) &m->p;
}

/**
 * Sprawdza, czy wielomian przy jednomianie jest stały.
 * @param[in] m : jednomian
 * @return Czy wielomian przy jednomianie jest stały?
 */
static inline bool MonoHasConstPoly(const Mono *m) {
    return PolyIsCoeff(&m->p);
}

/**
 * Tworzy pusty niestały wielomian pewnego rozmiaru.
 * @param[in] polySize : rozmiar wielomianu
 * @return wielomian ustalonego rozmiaru z alakowaną pamięcią
 */
static Poly PolyAllocate(size_t polySize) {
    Poly resPoly = {
            .size = polySize,
            .arr = malloc(polySize * sizeof(Mono))
    };

    if (!resPoly.arr) // W razie błędu alokacji pamięci
        exit(1); // kończymy program awaryjnym kodem 1.

    return resPoly;
}

void PolyDestroy(Poly *p) {
    if (PolyIsCoeff(p))
        return;

    for (size_t curMonoID = 0; curMonoID < p->size; curMonoID++)
        MonoDestroy(&p->arr[curMonoID]);

    if (p->arr)
        free(p->arr);
}

/**
 * Tworzy głęboką kopię wielomianu p, zapisując ją do pCopy.
 * @param[in] p : wielomian do kopiowania (source)
 * @param[in] pCopy : wielomian do zapisywania (destination)
 */
static void PolyDeepCopy(const Poly* p, Poly* pCopy) {
    if (PolyIsCoeff(p)) {
        pCopy->coeff = p->coeff;
        return;
    }

    *pCopy = PolyAllocate(p->size);

    for (size_t curMonoID = 0; curMonoID < p->size; curMonoID++)
        pCopy->arr[curMonoID] = MonoClone(&p->arr[curMonoID]);
}

Poly PolyClone(const Poly *p) {
    Poly pCopy = PolyZero();
    PolyDeepCopy(p, &pCopy);

    return pCopy;
}

/**
 * Zmniejsza rozmiar wielomianu do newSize oraz
 * redukuje wielomian niestały do stałego, jeżeli on
 * się składa tylko z jednego jednomianu postaci @f$c * x^0@f$
 * @param[in] p : wielomian
 * @param[in] newSize : nowy rozmiar
 * @return Zredukowany wielomian
 */
static Poly PolyReduce(Poly *p, size_t newSize) {
    assert(!PolyIsCoeff(p) && newSize <= p->size);

    // Zmiejszenie rozmiaru wielomianu
    p->size = newSize;

    bool freeTermOnly = p->size == 1 &&
                        MonoGetExp(&p->arr[0]) == 0 &&
                        MonoHasConstPoly(&p->arr[0]);

    // Wielomian, który ma niezerowy rozmiar oraz nie składa się z jednego
    // jednomianu postaci c * x^0 nie da się zredukować do wielomianu stalego.
    if (newSize != 0 && !freeTermOnly)
        return *p;

    // Redukujemy wielomian do zerowego, gdy jego rozmiar jest zerowy,
    // a w p.p. do współczynnika 'c' we wzorze na wielomian c * x^0.
    poly_coeff_t newCoeff = (p->size > 0 ? MonoGetPoly(&p->arr[0])->coeff : 0);
    PolyDestroy(p);

    return PolyFromCoeff(newCoeff);
}


/**
 * Dodaje dwa jednomiany jednego stopnia.
 * @param[in] m1 : jednomian @f$m_1@f$
 * @param[in] m2 : jednomian @f$m_2@f$
 * @return @f$m_1 + m_2@f$
 */
static inline Mono MonoAdd(const Mono *m1, const Mono *m2) {
    assert(MonoGetExp(m1) == MonoGetExp(m2));
    return (Mono) {
        .exp = MonoGetExp(m1),
        .p = PolyAdd(MonoGetPoly(m1), MonoGetPoly(m2))
    };
}

/**
 * Dodaje dwa wielomiany niestale.
 * @param[in] p : niestały wielomian @f$p@f$
 * @param[in] q : niestały wielomian @f$q@f$
 * @return @f$p + q@f$
 */
static Poly PolyAddNoConst(const Poly *p, const Poly *q) {
    assert(!PolyIsCoeff(p) && !PolyIsCoeff(q));

    size_t resMonoID = 0, pMonoID = 0, qMonoID = 0;
    Poly resPoly = PolyAllocate(p->size + q->size);

    // Wstawiamy do wielomianu resPoly jednomian o mniejszej potędze
    // wsród jednomianów o indeksach pMonoID, qMonoID z wielomianów p, q odpowiednio.
    while (pMonoID < p->size && qMonoID < q->size) {
        if (MonoGetExp(&p->arr[pMonoID]) < MonoGetExp(&q->arr[qMonoID])) {
            resPoly.arr[resMonoID++] = MonoClone(&p->arr[pMonoID++]);
        } else if (MonoGetExp(&p->arr[pMonoID]) > MonoGetExp(&q->arr[qMonoID])) {
            resPoly.arr[resMonoID++] = MonoClone(&q->arr[qMonoID++]);
        } else {
            Mono midResult = MonoAdd(&p->arr[pMonoID++], &q->arr[qMonoID++]);
            if (!MonoIsZero(&midResult)) // Jeżeli wynik dodawania jest zerem
                resPoly.arr[resMonoID++] = midResult; // to nie wstawiamy go do wielomianu.
        }
    }

    // Jeżeli wielomiany p, q są różnych rozmiarów,
    // to doklejamy pozostale jednomiany większego do końca.
    while (pMonoID < p->size)
        resPoly.arr[resMonoID++] = MonoClone(&p->arr[pMonoID++]);
    while (qMonoID < q->size)
        resPoly.arr[resMonoID++] = MonoClone(&q->arr[qMonoID++]);

    return PolyReduce(&resPoly, resMonoID);
}

/**
 * Dodaje stały wielomian p i niestały wielomian q.
 * @param[in] p : stały wielomian @f$p@f$
 * @param[in] q : niestały wielomian @f$q@f$
 * @return @f$p + q@f$
 */
static Poly PolyAddOneConst(const Poly* p, const Poly* q) {
    assert(PolyIsCoeff(p) && !PolyIsCoeff(q));

    size_t resMonoID = 0, qMonoID = 0;
    Mono constTermMono = MonoFromPoly(p, 0);
    Poly resPoly = PolyAllocate(q->size + 1);

    // Pierwszy jednomian w wielomianie resPoly
    Mono newFreeTerm = MonoGetExp(&q->arr[qMonoID]) == 0 ?
                       MonoAdd(&constTermMono, &q->arr[qMonoID++]) :
                       constTermMono;

    if (!MonoIsZero(&newFreeTerm)) // Jeżeli wyraz wolny nie jest zerem,
        resPoly.arr[resMonoID++] = newFreeTerm; // to dodajemy go do resPoly.

    // Doklejamy pozostale jednomiany wielomianu q do końca resPoly.
    while (qMonoID < q->size)
        resPoly.arr[resMonoID++] = MonoClone(&q->arr[qMonoID++]);

    return PolyReduce(&resPoly, resMonoID);
}

/**
 * Dodaje dwa stałe wielomiany.
 * @param[in] p : stały wielomian @f$p@f$
 * @param[in] q : stały wielomian @f$q@f$
 * @return @f$p + q@f$
 */
static Poly PolyAddBothConst(const Poly *p, const Poly *q) {
    assert(PolyIsCoeff(p) && PolyIsCoeff(q));
    return PolyFromCoeff(p->coeff + q->coeff);
}

Poly PolyAdd(const Poly *p, const Poly *q) {
    if (PolyIsCoeff(p) && PolyIsCoeff(q))
        return PolyAddBothConst(p, q);
    else if (!PolyIsCoeff(p) && !PolyIsCoeff(q))
        return PolyAddNoConst(p, q);
    return (PolyIsCoeff(p) ? PolyAddOneConst(p, q) : PolyAddOneConst(q, p));
}

/**
 * Dodaje wielomian @f$q@f$ do wielomiana @f$p@f$
 * Wielomian @f$p@f$ jest przyjmowany na własność.
 * @param[in] p : wielomian do aktualizacji @f$p@f$
 * @param[in] q : wielomian do dodawania @f$q@f$
 */
static void PolyAddTo(Poly *p, const Poly *q) {
    Poly midResult = PolyAdd(p, q);
    PolyDestroy(p);
    *p = midResult;
}


/**
 * Mnoży przez wielomian @f$q@f$ wielomian @f$p@f$
 * Wielomian @f$p@f$ jest przyjmowany na własność.
 * @param[in] p : wielomian do aktualizacji @f$p@f$
 * @param[in] q : wielomian do dodawania @f$q@f$
 */
static void PolyMulBy(Poly *p, const Poly *q) {
    Poly midResult = PolyMul(p, q);
    PolyDestroy(p);
    *p = midResult;
}

/**
 * Dodaje jednomian @f$m_2@f$ do jednomiana @f$m_1@f$
 * Jednomian @f$m_1@f$ jest przyjmowany na własność.
 * @param[in] m1 : jednomian do aktualizacji @f$m_1@f$
 * @param[in] m2 : jednomian do dodawania @f$m_2@f$
 */
static void MonoAddTo(Mono *m1, const Mono *m2) {
    Mono midResult = MonoAdd(m1, m2);
    MonoDestroy(m1);
    *m1 = midResult;
}

/**
 * Komparator dla sortowania tablicy jednomianów
 * w porządku rosnącym według wartości potęg.
 * @param[in] m1 : jednomian @f$m_1@f$
 * @param[in] m2 : jednomian @f$m_2@f$
 * @return róznica wykładników
 */
static inline int MonoComparator(const void* m1, const void* m2) {
     return (MonoGetExp(m1) - MonoGetExp(m2));
}

Poly PolyOwnMonos(size_t count, Mono *monos) {
    if (count == 0 || monos == NULL)
        return PolyZero();

    size_t resMonoID = 0;
    Poly resPoly = PolyAllocate(count);
    qsort(monos, count, sizeof(Mono), MonoComparator);

    for (size_t curMonoID = 0; curMonoID < count; curMonoID++) {
        poly_exp_t curExp = MonoGetExp(&monos[curMonoID]);
        Mono midResult = monos[curMonoID];

        // Sumujemy wszystkie jednomiany o tej samej potędze.
        for (; curMonoID + 1 < count; curMonoID++) {
            if (MonoGetExp(&monos[curMonoID + 1]) != curExp)
                break;
            MonoAddTo(&midResult, &monos[curMonoID + 1]);
            MonoDestroy(&monos[curMonoID + 1]);
        }

        if (!MonoIsZero(&midResult)) // Jeżeli wynik sumowania nie jest zerem,
            resPoly.arr[resMonoID++] = midResult; // to dodajemy go do końca resPoly.
    }

    free(monos);
    return PolyReduce(&resPoly, resMonoID);
}

Poly PolyAddMonos(size_t count, const Mono monos[]) {
    Mono* monosCopy = malloc(count * sizeof(Mono));
    memcpy(monosCopy, monos, count * sizeof(Mono));

    return PolyOwnMonos(count, monosCopy);
}

Poly PolyCloneMonos(size_t count, const Mono monos[]) {
    Mono* monosCopy = malloc(count * sizeof(Mono));

    for (size_t curMonoID = 0; curMonoID < count; curMonoID++)
        monosCopy[curMonoID] = MonoClone(&monos[curMonoID]);

    return PolyOwnMonos(count, monosCopy);
}

/**
 * Mnoży dwa jednomiany.
 * @param[in] m1 : jednomian @f$m_1@f$
 * @param[in] m2 : jednomian @f$m_2@f$
 * @return @f$m1 * m2@f$
 */
static inline Mono MonoMul(const Mono *m1, const Mono *m2) {
    return (Mono) {
        .exp = MonoGetExp(m1) + MonoGetExp(m2),
        .p = PolyMul(MonoGetPoly(m1), MonoGetPoly(m2))
    };
}

/**
 * Mnoży dwa wielomiany niestale.
 * @param[in] p : niestały wielomian @f$p@f$
 * @param[in] q : niestały wielomian @f$q@f$
 * @return @f$p * q@f$
 */
static Poly PolyMulNoConst(const Poly *p, const Poly *q) {
    assert(!PolyIsCoeff(p) && !PolyIsCoeff(q));

    size_t resMonoID = 0;
    Poly resPoly = PolyAllocate(p->size * q->size);

    // Dodajemy do resPoly wyniki mnożenia parami jednomianów z wielomianów p, q.
    for (size_t pMonoID = 0; pMonoID < p->size; pMonoID++) {
        for (size_t qMonoID = 0; qMonoID < q->size; qMonoID++) {
            Mono midResult = MonoMul(&p->arr[pMonoID], &q->arr[qMonoID]);
            if (!MonoIsZero(&midResult)) // Jeżeli nie zdarzył overflow typu,
                resPoly.arr[resMonoID++] = midResult; // to dodajemy do końca resPoly.
        }
    }

    PolyReduce(&resPoly, resMonoID);

    // Tworzymy kopię wielomianu resPoly, sortując i grupując wyrazy.
    Poly resPolySorted = PolyAddMonos(resPoly.size, resPoly.arr);

    // Zwolniamy pamięć do tablicy jednomianów w nieposortowanym wielomianie
    free(resPoly.arr);

    return resPolySorted;
}

/**
 * Mnoży stały wielomian p i niestały q.
 * @param[in] p : stały wielomian @f$p@f$
 * @param[in] q : niestały wielomian @f$q@f$
 * @return @f$p * q@f$
 */
static Poly PolyMulOneConst(const Poly *p, const Poly *q) {
    assert(PolyIsCoeff(p) && !PolyIsCoeff(q));

    if (PolyIsZero(p))
        return PolyZero();

    size_t resMonoID = 0;
    Poly resPoly = PolyAllocate(q->size);
    Mono constTermMono = MonoFromPoly(p, 0); // Jednomian p * x^0.

    // Dodajemy do resPoly wynik mnożenia każdego jednomianu z q przez stały jednomian.
    for (size_t qMonoID = 0; qMonoID < q->size; qMonoID++) {
        Mono midResult = MonoMul(&constTermMono, &q->arr[qMonoID]);
        if (!MonoIsZero(&midResult)) // Jeżeli nie zdarzył overflow typu,
            resPoly.arr[resMonoID++] = midResult; // to dodajemy do końca resPoly.
    }

    return PolyReduce(&resPoly, resMonoID);
}

/**
 * Mnoży dwa stale wielomiany.
 * @param[in] p : stały wielomian @f$p@f$
 * @param[in] q : stały wielomian @f$q@f$
 * @return @f$p * q@f$
 */
static Poly PolyMulBothConst(const Poly *p, const Poly *q) {
    assert(PolyIsCoeff(p) && PolyIsCoeff(q));
    return PolyFromCoeff(p->coeff * q->coeff);
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
    Poly qNegated = PolyNeg(q);
    Poly resPoly = PolyAdd(p, &qNegated);

    PolyDestroy(&qNegated);
    return resPoly;
}

/**
 * Podniesienie do potęgi w czasie logarytmicznym.
 * @param[in] base : podstawa @f$x@f$
 * @param[in] exp : wykładnik @f$n@f$
 * @return @f$x^n@f$
 */
static poly_coeff_t NumberToPower(poly_coeff_t base, poly_exp_t exp) {
    if (exp == 0)
        return 1;

    poly_coeff_t sqrtResult = NumberToPower(base, exp / 2);
    return sqrtResult * sqrtResult * (exp % 2 == 1 ? base : 1);
}

Poly PolyAt(const Poly *p, poly_coeff_t x) {
    if (PolyIsCoeff(p))
        return PolyClone(p);

    Poly resPoly = PolyZero();

    for (size_t pMonoID = 0; pMonoID < p->size; pMonoID++) {
        // Obliczenie wyrazu x^n, gdzie n - potęga obecnego jednomianu z 'p'
        poly_exp_t curExp = MonoGetExp(&p->arr[pMonoID]);
        poly_coeff_t newCoeff = NumberToPower(x, curExp);

        // Obiczenie wartości jednomiana w punkcie 'x'
        Poly coeffPoly = PolyFromCoeff(newCoeff);
        Poly midResult = PolyMul(MonoGetPoly(&p->arr[pMonoID]),
                                 &coeffPoly);

        // Aktualizacja wyniku drogą dodawania do niego jednomiana.
        if (!PolyIsZero(&midResult))
            PolyAddTo(&resPoly, &midResult);

        PolyDestroy(&midResult);
    }

    return resPoly;
}

static inline poly_exp_t MonoDegBy(const Mono *m, size_t var_idx) {
    return PolyDegBy(&m->p, var_idx - 1);
}

poly_exp_t PolyDegBy(const Poly *p, size_t var_idx) {
    if (PolyIsCoeff(p))
        return (PolyIsZero(p) ? -1 : 0);
    else if (var_idx == 0) // Jeżeli obecny poziom rekurencji odpowiada var_idx,
        return MonoGetExp(&p->arr[p->size - 1]); // to zwracamy największą potęgę.

    // Szukamy największy stopień jednomianu ze wzgłędu na var_idx.
    poly_exp_t maxMonoDegByIdx = 0;

    for (size_t pMonoID = 0; pMonoID < p->size; pMonoID++) {
        poly_exp_t curDeg = MonoDegBy(&p->arr[pMonoID], var_idx);

        if (maxMonoDegByIdx < curDeg)
            maxMonoDegByIdx = curDeg;
    }

    return maxMonoDegByIdx;
}

static inline poly_exp_t MonoDeg(const Mono *m) {
    return MonoGetExp(m) + PolyDeg(&m->p);
}

poly_exp_t PolyDeg(const Poly *p) {
    if (PolyIsCoeff(p))
        return (PolyIsZero(p) ? -1 : 0);

    // Szukamy największy stopień jednomianu.
    poly_exp_t maxMonoDeg = 0;

    for (size_t pMonoID = 0; pMonoID < p->size; pMonoID++) {
        poly_exp_t curDeg = MonoDeg(&p->arr[pMonoID]);

        if (curDeg > maxMonoDeg)
            maxMonoDeg = curDeg;
    }

    return maxMonoDeg;
}

/**
 * Sprawdza równość dwóch jednomianów.
 * @param[in] m1 : jednomian @f$m_1@f$
 * @param[in] m2 : jednomian @f$m_2@f$
 * @return @f$m_1 = m_2@f$
 */
static inline bool MonoIsEq(const Mono *m1, const Mono *m2) {
    return (MonoGetExp(m1) == MonoGetExp(m2) &&
            PolyIsEq(MonoGetPoly(m1), MonoGetPoly(m2)));
}

bool PolyIsEq(const Poly *p, const Poly *q) {
    if (PolyIsCoeff(p) && PolyIsCoeff(q))
        return (p->coeff == q->coeff);
    else if (PolyIsCoeff(p) || PolyIsCoeff(q) || p->size != q->size)
        return false;

    for (size_t curMonoID = 0; curMonoID < p->size; curMonoID++) {
        if (!MonoIsEq(&p->arr[curMonoID], &q->arr[curMonoID]))
            return false;
    }

    return true;
}

/**
 * Podnosi wielomian @p p do potęgi @p exp.
 * @param[in] p : wielomian @f$p@f$
 * @param[in] exp: wielomian
 * @return @f$p^exp@f$
 */
static Poly PolyToPower(const Poly *p, poly_exp_t exp) {
    if (exp == 0)
        return PolyFromCoeff(1);
    else if (PolyIsCoeff(p))
        return PolyFromCoeff(NumberToPower(p->coeff, exp));

    Poly sqrtResult = PolyToPower(p, exp / 2);
    Poly resPoly = PolyMul(&sqrtResult, &sqrtResult);

    if (exp % 2 == 1)
        PolyMulBy(&resPoly, p);

    PolyDestroy(&sqrtResult);

    return resPoly;
}

static Poly PolyComposeFrom(const Poly *p, size_t idx, size_t k, const Poly q[]);

static Poly MonoComposeFrom(const Mono *m, size_t idx, size_t k, const Poly q[]) {
    Poly toCompose = (idx < k ? q[idx] : PolyZero());
    Poly multinomial = PolyToPower(&toCompose, MonoGetExp(m));

    Poly nextComposition = PolyComposeFrom(MonoGetPoly(m), idx + 1, k, q);
    Poly resPoly = PolyMul(&multinomial, &nextComposition);

    PolyDestroy(&multinomial);
    PolyDestroy(&nextComposition);

    return resPoly;
}

/**
 * Składa wielomian @p p z @p k wielomianami tablicy @p q
 * rozpoczynając od wielomianu z tablicy @p p o indeksie @p idx.
 * @param[in] p : wielomian @f$p@f$
 * @param[in] q : tablica wielomianów
 * @param[in] k : liczba wielomianów
 * @param[in] idx : indeks początku
 * @return @f$p(p_0, p_1, \ldots, q_{idx}, q_{idx + 1}, \ldots)@f$
 */
static Poly PolyComposeFrom(const Poly *p, size_t idx, size_t k, const Poly q[]) {
    if (PolyIsCoeff(p))
        return PolyClone(p);

    Poly resPoly = PolyZero();

    for (size_t curMonoID = 0; curMonoID < p->size; curMonoID++) {
        // Dodajemy do wyniku złożenie jednomianu z wielomianami tablicy q.
        Poly composition = MonoComposeFrom(&p->arr[curMonoID], idx, k, q);
        PolyAddTo(&resPoly, &composition);

        PolyDestroy(&composition);
    }

    return resPoly;
}

Poly PolyCompose(const Poly *p, size_t k, const Poly q[]) {
   return PolyComposeFrom(p, 0, k, q);
}
