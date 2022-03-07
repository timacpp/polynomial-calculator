## About the project

This is a user-friendly calculator for [multi-variable polynomials](https://en.wikipedia.org/wiki/Polynomial#Number_of_variables), based on [Reverse Polish Notation](https://en.wikipedia.org/wiki/Reverse_Polish_notation). 
It provides a simple way of inputting the polynomials as well as a variety of operations to perform on them.

## Polynomial representation
The polynomials are represented by either of the following forms:

 - ```coeff``` - constant polynomial
 - ```(coeff,exp)``` - non-constant polynomial of one monomial
 - ```(coeff1,exp1)+(coeff2,exp2)``` - non-constant polynomial of two monomials 

where each ```coeff``` is either a polynomial, forming a *nested* polynomial, or
a number of ```poly_coeff_t``` type, and ```exp``` is ```poly_exp_t``` (see custom type definitions in ```poly.h```).

Furthermore, one can inductively expand the last case for polynomials of arbitrary number of monomials.

From the definition above one polynomial can be represented in many ways, eg. ```1``` and ```(1, 0)```.
In order to solve this problem, the polynomials are automatically reduced to their *minimal form*.
In addition, the monomials are always being sorted in ascending order of the exponents.

Let's take a look at some examples (more can be found in ```examples``` directory):
```
# Constant polynomial: f(x) = 42
42

# One primitive monomial: f(x) = 4x^5
(4,5)

# One nested monomial: f(x, y) = (3x^2)(y^5)
((3,2),5)

# Many different monomials: f(x, y, z) = 1 + (2x^3) + (4y^5 + (7z^10)y^9)x^6
((4,5)+((7,10),9),6)+1+(2,3)
```

In order to add a polynomial on stack, one has to enter it manually to standard input.

## Calculator operations
The calculator provides the following operations, which are called by entering their name and required parameters:

 - ```ZERO``` - puts a zero polynomial on stack
 - ```ADD```, ```SUB```, ```MUL``` - pops 2 polynomials from stack and puts sum/difference/product on stack
 - ```IS_ZERO```, ```IS_COEFF```, ```IS_EQ``` - checks whether a top polynomial is zero/constant/equal to the next top polynomial and prints result
 - ```NEG```, ```POP```, ```PRINT```, ```CLONE``` - negates/removes/prints/clones the top polynomial
 - ```DEG```, ```DEG_BY var```, ```AT x``` - prints degree/degree by variable/value at point of a top polynomial
 - ```COMPOSE k``` - pops k polynomials from stack and puts their composition on stack