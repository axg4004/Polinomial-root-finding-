/***********************************************************************
 * Student C framework to calculate the roots and evaluate polynomials
 * static functions are not REQURED, you are free to implement they way
 * you wish
 * Course: Applied Programming 
 * Revised: 2015
 *
 * 10/16/2015 R. Repka  - Removed evalPoly, extern
 * 04/03/2015 R. Repka  - Added cpow() and Horner comments, 
 * 10/24/2016 R. Repka  - Updated evalDerivs comments
 * 10/26/2016 R. Repka  - Changed createPoly to initPoly
 * 10/09/2017 R, Repka  - Minor comment changes
 *
 * Modified: Abhimanyu Gupta
 ***********************************************************************/ 
#include <complex.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "ClassErrors.h"
#include "poly.h"

#define ZERO 0.00000001

/*---------------------------------------------------------------------------
  Define local functions
---------------------------------------------------------------------------*/
static double complex* quadraticRoots( polynomial *p);
static double complex laguerre(polynomial *p, double tol, int verb);
static polynomial* deflPoly(polynomial *p, double complex root);
static double complex* evalDerivs(polynomial *p, double complex point);
static void printComplex(double complex x);
int compare(const void *p1, const void *p2);

/*---------------------------------------------------------------------------
  Initializes a polynomial data structure with nterms.  This allocates storage
  for the actual polynomial.
  
  Where: polynomial *p       - Pointer to polynomial data structure to create
         unsigned int nterms - The number of elements to create
  Returns: nothing
  Errors:  prints an error and exits
---------------------------------------------------------------------------*/
void initPoly(polynomial *p, unsigned int nterms){

	p -> nterms = nterms;
	p -> polyCoef = malloc((nterms)*sizeof(double complex));

	if ( p -> polyCoef == NULL)
	{
	  fprintf(stderr, "No ROOM in MALLOC, \n"); 	
	  exit(-99);
	}
}


/*---------------------------------------------------------------------------
  Destroys/frees a polynomial
  
  Where: polynomial *p - Pointer to polynomial data structure to destroy
  Returns: nothing
  Errors:  none
---------------------------------------------------------------------------*/
void freePoly(polynomial *p){
	
	free(p -> polyCoef);
}


/*---------------------------------------------------------------------------
  This OPTIONAL function evaluates a polynomial at a complex point z.  
  Don't use the cpow() function, it generates round off errors.  You are 
  required to use Horner's factorization 
   
  Were: polynomial *p    - Pointer to the polynomial to evaluate
        double complex z - Point to evaluate 
  Returns: double complex- Complex solution
  Errors:  none
---------------------------------------------------------------------------*/
double complex cevalPoly(polynomial *p, double complex z){

	double complex x = p -> polyCoef[p -> nterms - 1]; 
	int i;

	for (i = p -> nterms - 2; i>=0 ; i--)
	{
	  x = (x*z) + p -> polyCoef[i];
	}

	return(x);

}


/*---------------------------------------------------------------------------
 This finds all the roots (real and complex) of a real polynomial.  If there
 is a single root, it solves for it.  If there are two roots then it uses
 the quadratic formula.  If there are more than two roots, it uses Laguerre.
 If a complex root is found then BOTH the + and - roots are deflated from the
 polynomial.
 
 Where: polynomial *poly - Pointer to an array of polynomial coefficients
        double tolerance - Termination tolerance
        int verb         - Verbose flag indicating whether to print intermediate
                           results or not ( 1 = TRUE, 0 = FALSE 
 
 Returns: double complex *roots - A pointer to a complex root array
 Errors:  prints an error and exits
---------------------------------------------------------------------------*/
double complex* roots(polynomial *poly, double tolerance, int verb){


	double complex *root;

	double real; // Should they be double complex????
	double imag;
	double complex Conj;
	polynomial *temp;
	
	int i = poly -> nterms -2;
		if (verb == 1){
			printf(" \n P(x) =  ");
			printPoly(poly);
		}
 if( poly -> polyCoef[poly -> nterms - 1] == 0){
	root = malloc((poly -> nterms -2)*sizeof(double complex));
	poly -> nterms--;
	i--;
}
else {
	root = malloc((poly -> nterms -1)*sizeof(double complex));
}
// Root 1 
	if ((poly -> nterms) == 2){
		root[i] = -(poly -> polyCoef[0])/(poly -> polyCoef[1]);

		
		i--;	
		return(root);
	}
// Root 2
	if ((poly -> nterms ) == 3){
		double complex *temp1 = quadraticRoots(poly);
		root[i] = temp1[0];
		root[i-1] = temp1[1];
		free(temp1);
		i -=2;
	return(root);
	}

	while ((poly -> nterms) >= 4){	
		if (poly -> polyCoef[0] == 0){
			root[i] = 0;
			i--;
			temp = deflPoly(poly, 0); // Deflate
			poly -> nterms = temp -> nterms; 
			memcpy(poly -> polyCoef , temp -> polyCoef, sizeof(double complex)*(temp -> nterms));
			freePoly(temp);
			free(temp);
			
		}
		root[i]   =  laguerre(poly, tolerance, verb);
		real = creal(root[i]);
		imag = cimag(root[i]);	
		if (cabs(imag) >  ZERO)  
		{
			Conj = real+ I*(-imag);
			
			
			temp = deflPoly(poly, root[i]);
			poly -> nterms = temp -> nterms; 
			memcpy(poly -> polyCoef , temp -> polyCoef, sizeof(double complex)*(temp -> nterms));
			freePoly(temp);
			free(temp);
	
			temp = deflPoly(poly, Conj);
			poly -> nterms = temp -> nterms; 
			memcpy(poly -> polyCoef , temp -> polyCoef, sizeof(double complex)*(temp -> nterms));
			freePoly(temp);
			free(temp);

			root[i-1] = Conj;
			i -=2;
		}		
		else {
			temp = deflPoly(poly, root[i]);
			poly -> nterms = temp -> nterms; 
			memcpy(poly -> polyCoef , temp -> polyCoef, sizeof(double complex)*(temp -> nterms));
			freePoly(temp);
			free(temp);

			if(verb  == 1){
				printf("\n Deflated: P(x)  = ");
				printPoly(poly);
				printf("\n Found final two roots through quadratic formula ");
			}
					
			i--;
		}
		
// Root 1
			if ((poly -> nterms) == 2){
				root[i] = -(poly -> polyCoef[0])/(poly -> polyCoef[1]);

				if(verb  == 1){
				printf("\n Deflated: P(x)  = ");
				printPoly(poly);
				printf("\n Found final two roots through quadratic formula ");
			}
				i--;	
				return(root);
			}
// Root 2

			if ((poly -> nterms ) == 3)
			{
				if(poly -> polyCoef[poly -> nterms - 1] == 0){
					root[i] = -(poly -> polyCoef[0])/(poly -> polyCoef[1]);
					if(verb  == 1){
				printf("\n Deflated: P(x)  = ");
				printPoly(poly);
				printf("\n Found final two roots through quadratic formula ");
			}
				i--;	
				return(root);

			}
			else {
				double complex *temp1 = quadraticRoots(poly);
				root[i] = temp1[0];
				root[i-1] = temp1[1];
				free(temp1);
				i -=2;
			}
		}
	}

	return(root);
}

/*---------------------------------------------------------------------------
  This function evaluates the polynomial derivatives at a point using the
  compact method or another equivalent method.
     
  If you decide not use the compact all-in-one p,p',p'' evaluation code, 
  you can implement this function:
        As separate p, p' and p'' derivatives
        Then evaluate each function separately
        Still returning 3 values
  OR
  You can choose to create functions of your own and not implement this function
  at all
  
  Where: polynomial *p        - Pointer to a polynomial data to evaluate
         double complex point - The point to evaluate
  Returns: double complex*    - Pointer to an array of evaluated 
                                derivatives d, d' and d'' 
  Errors:  prints an error and exits
---------------------------------------------------------------------------*/
static double complex* evalDerivs(polynomial *p, double complex point)
{

	int i;
	double complex *d = malloc(3*sizeof(double complex));
	d[0] = cevalPoly(p, point);
	polynomial df;

	df.nterms = p-> nterms - 1;
	df.polyCoef = malloc(df.nterms*sizeof(double complex));
	
	for (i = 1; i < p -> nterms ; i++)
	{
	  df.polyCoef[i-1] = i*(p -> polyCoef[i]);
	}

	d[1] = cevalPoly(&df, point); 
	polynomial dff;
	dff.nterms = p-> nterms - 2;
	dff.polyCoef = malloc(dff.nterms*sizeof(double complex));
	
	for (i = 1; i < p -> nterms - 1; i++)
	{
	  dff.polyCoef[i-1] = i*(df.polyCoef[i]);
	}

	d[2] = cevalPoly(&dff, point);
	free(df.polyCoef);
	free(dff.polyCoef);
	return(d);

}

/*---------------------------------------------------------------------------
  Returns the two roots from a quadratic polynomial
  
  Where: polynomial *p - Pointer to a 2nd order polynomial
  Returns: double complex* - A pointer to a complex root pair.
  Errors:  prints an error and exits
---------------------------------------------------------------------------*/
static double complex* quadraticRoots( polynomial *p){

	//double complex *quad = malloc(2*sizeof(double complex));
	double complex *quad = malloc(2*sizeof(double complex));
	double complex A = p -> polyCoef[2];
	double complex B = p -> polyCoef[1];
	double complex C = p -> polyCoef[0];
	double complex SQT = csqrt((B*B) - (4*A*C));
	
	quad[0] = ((-B + SQT)/(2*A));
	quad[1] = ((-B - SQT)/(2*A));

	return(quad);

}

/*---------------------------------------------------------------------------
  Uses Laguerre's method to compute a root of a real polynomial
  Note: In addition to verifying that the update value alpha is greater than 
        to, also be sure to verify that polynomial evaluated at the 
        "current guess" is not less than the tolerance.  If p(x) is less than
        tol, you have found a root.
  
  Where: polynomial *p - Pointer to a polynomial structure to evaluate
         double tol    - Termination tolerance 
         int verb      - Verbose flag indicating whether to print intermediate
                         results or not ( 1 = TRUE, 0 = FALSE 
 Returns: double complex - The complex root or (NAN + 0.0*I) if  none is found
 Errors:  none
---------------------------------------------------------------------------*/
static double complex laguerre(polynomial *p, double tol, int verb){
    	
	if(verb == 1)
	{
	  printf("\n Laguerre's Algorithm(tol = 1e-09)\n ");
		
	}

	double complex x = 0;
	double complex n = p -> nterms - 1;
	double complex final = 0.0;
	int count = 0;
	double complex*  Dev;
	double complex G;
	double complex H;
	double complex SQT;
	double complex G2;
	double complex temp;
	double complex temp2;

	do
	{ 	

	  Dev = evalDerivs( p, x);
	  G = Dev[1]/Dev[0];
	  G2 = G*G;
	  H = G2 - (Dev[2]/Dev[0]);
	  SQT = csqrt((n-1)*((n*H)-G2));
	  temp = (G + SQT);
	  temp2 = (G - SQT);

	if(cabs(temp) < cabs(temp2)) 
 	{
	  final = n/temp2;
	}
	
	else 
	{
	  final = n/temp;
	}
		
	if(verb == 1)
	{		
	  printf("\n it %d X:", count);
	  printComplex(x);

	  printf(" \n G(x): ");
   	  printComplex(G);

	  printf("\n H(x): ");
	  printComplex(H);

       	  printf("\n Alpha:  ");
	  printComplex(final);
				
	}
	
	count ++;
	x = x - final;		
			
	free(Dev);
		
	}

	while ( cabs(final) >= tol);
	
	return(x);
}

/*---------------------------------------------------------------------------
  Deflates a root from the polynomial, returning the new polynomial 
  
  Where: polynomial *p       - Pointer to the polynomial to deflate
         double complex root - The root to use 
  Returns: polynomial *      - Pointer to the deflated polynomial
  Errors:  none
---------------------------------------------------------------------------*/
static polynomial* deflPoly(polynomial *p, double complex root){

	polynomial *b = malloc(sizeof(polynomial));
	initPoly(b, p -> nterms -1);
	
	b -> polyCoef[b -> nterms -1] = p -> polyCoef[p -> nterms - 1];

	for (int i = (b -> nterms -2) ;  i >= 0; i--)
	{
	  b-> polyCoef[i] = (b -> polyCoef[i+1])*root + p -> polyCoef[i+1];		
	}

	return(b);

}

/*---------------------------------------------------------------------------
  The function prints out complex data
  
  Where: double complex x - the complex data to print out
  returns:  nothing
  errors:   none
---------------------------------------------------------------------------*/
static void printComplex(double complex x){

	if(cabs(cimag(x)) <= ZERO)
	{
	  printf("%f" ,creal(x));	 
	}

	else
	{
	  printf("%f + I*%f " ,creal(x), cimag(x)); // check conditions
	}
  
}

/*---------------------------------------------------------------------------
  Prints a polynomial
  Where: polynomial *p - Pointer to polynomial data structure to print
  Errors: none
  Returns: nothing
---------------------------------------------------------------------------*/
void printPoly (polynomial *p){

	for (int temp = ((p -> nterms)-1) ;temp >= 0; temp--)
	{
	  printComplex(p -> polyCoef[temp]);
	  
	  if (temp != 0)
	  {
	    printf("x^%d +",temp);
	  }

	}
}

/*---------------------------------------------------------------------------
  Prints the roots of a polynomial from largest (in magnitude) to smallest
  
  Where: polynomial *p - Pointer to polynomial data structure to print
  Returns: nothing
  Errors:  none
---------------------------------------------------------------------------*/
void printRoots (polynomial *p, double tolerance, int verb)
{
	int temp; 

	if( p -> polyCoef[p -> nterms - 1] == 0)
	{
	  temp = p -> nterms - 1;	
	}

	else 
	{
	  temp = p -> nterms;
	}

	double complex *z = roots(p, tolerance, verb);
	qsort(z ,temp-1, sizeof(double complex), compare); 
	printf("\n Roots: \n" );
	
	for (int i = 0; i < (temp-1); i++)
	{
	  printComplex(z[i]);
	  printf( " \n");
		
	}

	free(z);
}

/*---------------------------------------------------------------------------*/
int compare(const void *p1, const void *p2){

	double complex *p = (double complex *)p1;
	double complex *q = (double complex *)p2;
	
	double complex diff = (*p - *q);
	
	if (ZERO >= cabs(diff)) 
	{ 
	  return 0;
	}
	
	else if (cabs(*p) < cabs(*q)) 
	{
	  return -1;
	}

	else 
	{
	  return 1;
	}

}
