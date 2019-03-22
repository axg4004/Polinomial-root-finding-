/***********************************************************************
 * Tests three types of root finding, secant, newton, and bisection,
 * based on user input and prints out the timing results.
 * Course: Applied Programming 
 *
 * Author: Abhimanyu Gupta
 ***********************************************************************/ 

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "ClassErrors.h"
#include "poly.h"

#define BUFFER (256)


/*
  Main function to test root finding methods based on user input 
*/
int main(int argc, char* argv[])
{
   /*------------------------------------------------------------------------
      UI variables with sentential values
   ------------------------------------------------------------------------*/
   int verbose = 0;
   FILE *Polyfile = NULL;

   /*------------------------------------------------------------------------
     These variables are used to control the getopt_long_only command line 
     parsing utility.  
   ------------------------------------------------------------------------*/
   int rc;
   /* getopt_long stores the option index here. */
   int option_index = 0;

   /* This contains the short command line parameters list */
   //char *getoptOptions = "vbsnt:g:u:";    /* add lots of stuff here */
   char *getoptOptions = "vi:"; //for hw6

   /* This contains the long command line parameter list, it should mostly 
     match the short list                                                  */
   struct option long_options[] = {
      /* These options don’t set a flag.
         We distinguish them by their indices. */
      {"verbose",   no_argument,       0, 'v'},
      {"verb",      no_argument,       0, 'v'},
      {"v",         no_argument,       0, 'v'},      
      {"input",     required_argument,       0, 'i'},
      {"in",        required_argument,       0, 'i'},

      {0, 0, 0, 0}
   };
  
   opterr = 1;           /* Enable automatic error reporting */
   while ((rc = getopt_long_only(argc, argv, getoptOptions, long_options, 
                                                    &option_index)) != -1) 
   {
      /* Detect the end of the options. */
      switch (rc)
        {
          case 'v':                    /* Verbose */
          verbose = 1;
          break;
	  
	  case 'i':                    /* Input */
          Polyfile = fopen(optarg,"r"); 
          break;


        case '?':  /* Handled by the default error handler */
          break;

       default:
          printf ("Internal error: undefined option %0xX\n", rc);
          exit(PGM_INTERNAL_ERROR);
       } // End switch
   } /* end while */

   /*------------------------------------------------------------------------
     Check for command line syntax errors
   ------------------------------------------------------------------------*/
   if ((optind < argc)){
      fprintf(stderr, "Tests root finding methods\n");
      fprintf(stderr, "usage: hw5 -b[isection] | -s[ecant] | -n[ewton]   -t[olerance] number\n");
      fprintf(stderr, "          -g[uess1] number   <-g[u]ess2 number   <-verb[ose]> \n");
      fprintf(stderr, " e.g:   hw5 -bisection -tol 1E-6 -g1 -2 -g2 3 -verb\n"); 

      fflush(stderr);
      return(PGM_INTERNAL_ERROR);

   } /* End if error */

/*---------------------------------------INPUT FILE READ--------------------------------------- */


char String[BUFFER];
char String2[BUFFER];
char str[] = " ";
char *p;
polynomial x;
int count=0;


if (Polyfile != NULL){
	
   while (fgets(String, BUFFER, Polyfile) != NULL)
     {
	
	strcpy(String2 , String);
	p = strtok( String2 , str);
	while(p != NULL){
		++count;
		p = strtok(NULL, str);
	}
	initPoly(&x, count);
	p = strtok( String , str);
	while (p != NULL){
		count--;
		x.polyCoef[count] = atof(p);
		p = strtok(NULL, str);		
	}
	printRoots(&x, 1E-6, verbose);
	freePoly(&x);
	}
	fclose(Polyfile);
}
	else 
	{
	 fprintf(stderr, " file not found, not working\n");
	}

}
