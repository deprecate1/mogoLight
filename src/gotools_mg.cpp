//
// C++ Implementation: gotools_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "gotools_mg.h"
#include <stdlib.h>
#include <math.h>
#include "innermcgoplayer_mg.h"
#include <sys/timeb.h>

using namespace std;

int MoGo::GoTools::outputMode = STDOUT_MODE;
int MoGo::GoTools::savedOutputMode = STDERR_MODE;
bool MoGo::GoTools::appendToFile = false;
bool MoGo::GoTools::savedAppendToFile = false;
bool MoGo::GoTools::useColor=true;
double MoGo::GoTools::startMemoryUsed=0;

Vector<Vector<int> >MoGo::GoTools::all8Permutations;

FILE *MoGo::GoTools::outputFile = 0;
std::string MoGo::GoTools::fileName = "outputMoGo";
std::string MoGo::GoTools::savedFileName = "outputMoGo";
#define COEF_CACHE_MISS_RANDOM ((1024))


// #define USE_NEW_RANDOM




#ifdef USE_NEW_RANDOM
/*
------------------------------------------------------------------------------
Standard definitions and types, Bob Jenkins
------------------------------------------------------------------------------
*/
#ifndef STANDARD
# define STANDARD
# ifndef STDIO
#  include <stdio.h>
#  define STDIO
# endif
# ifndef STDDEF
#  include <stddef.h>
#  define STDDEF
# endif
typedef  unsigned long long  ub8;
#define UB8MAXVAL 0xffffffffffffffffLL
#define UB8BITS 64
typedef    signed long long  sb8;
#define SB8MAXVAL 0x7fffffffffffffffLL
typedef  unsigned long  int  ub4;   /* unsigned 4-byte quantities */
#define UB4MAXVAL 0xffffffff
typedef    signed long  int  sb4;
#define UB4BITS 32
#define SB4MAXVAL 0x7fffffff
typedef  unsigned short int  ub2;
#define UB2MAXVAL 0xffff
#define UB2BITS 16
typedef    signed short int  sb2;
#define SB2MAXVAL 0x7fff
typedef  unsigned       char ub1;
#define UB1MAXVAL 0xff
#define UB1BITS 8
typedef    signed       char sb1;   /* signed 1-byte quantities */
#define SB1MAXVAL 0x7f
typedef                 int  word;  /* fastest type available */

#define bis(target,mask)  ((target) |=  (mask))
#define bic(target,mask)  ((target) &= ~(mask))
#define bit(target,mask)  ((target) &   (mask))
#ifndef _min
# define _min(a,b) (((a)<(b)) ? (a) : (b))
#endif /* min */
#ifndef _max
# define _max(a,b) (((a)<(b)) ? (b) : (a))
#endif /* max */
#ifndef align
# define align(a) (((ub4)a+(sizeof(void *)-1))&(~(sizeof(void *)-1)))
#endif /* align */
#ifndef abs
# define abs(a)   (((a)>0) ? (a) : -(a))
#endif
#define TRUE  1
#define FALSE 0
#define SUCCESS 0  /* 1 on VAX */

#endif /* STANDARD */











/*
------------------------------------------------------------------------------
rand.h: definitions for a random number generator
By Bob Jenkins, 1996, Public Domain
MODIFIED:
  960327: Creation (addition of randinit, really)
  970719: use context, not global variables, for internal state
  980324: renamed seed to flag
  980605: recommend RANDSIZL=4 for noncryptography.
  010626: note this is public domain
------------------------------------------------------------------------------
*/
#ifndef STANDARD
#include "standard.h"
#endif

#ifndef RAND
#define RAND
#define RANDSIZL   (8)  /* I recommend 8 for crypto, 4 for simulations */
#define RANDSIZ    (1<<RANDSIZL)

/* context of random number generator */
struct randctx {/**assert(0);**/
  ub4 randcnt;
  ub4 randrsl[RANDSIZ];
  ub4 randmem[RANDSIZ];
  ub4 randa;
  ub4 randb;
  ub4 randc;
};
typedef  struct randctx  randctx;

/*
------------------------------------------------------------------------------
 If (flag==TRUE), then use the contents of randrsl[0..RANDSIZ-1] as the seed.
------------------------------------------------------------------------------
*/
void randinit(/*_ randctx *r, word flag _*/);

void isaac(/*_ randctx *r _*/);


/*
------------------------------------------------------------------------------
 Call rand(/o_ randctx *r _o/) to retrieve a single 32-bit random value
------------------------------------------------------------------------------
*/
#define rand(r) \
   (!(r)->randcnt-- ? \
     (isaac(r), (r)->randcnt=RANDSIZ-1, (r)->randrsl[(r)->randcnt]) : \
     (r)->randrsl[(r)->randcnt])

#endif  /* RAND */











/*
------------------------------------------------------------------------------
rand.c: By Bob Jenkins.  My random number generator, ISAAC.  Public Domain.
MODIFIED:
  960327: Creation (addition of randinit, really)
  970719: use context, not global variables, for internal state
  980324: added main (ifdef'ed out), also rearranged randinit()
  010626: Note that this is public domain
------------------------------------------------------------------------------
*/
#ifndef STANDARD
#include "standard.h"
#endif
#ifndef RAND
#include "rand.h"
#endif


#define ind(mm,x)  (*(ub4 *)((ub1 *)(mm) + ((x) & ((RANDSIZ-1)<<2))))
#define rngstep(mix,a,b,mm,m,m2,r,x) \
{/**assert(0);**/ \
  x = *m;  \
  a = (a^(mix)) + *(m2++); \
  *(m++) = y = ind(mm,x) + a + b; \
  *(r++) = b = ind(mm,y>>RANDSIZL) + x; \
}

void     isaac(randctx *ctx) {/**assert(0);**/
  register ub4 a,b,x,y,*m,*mm,*m2,*r,*mend;
  mm=ctx->randmem; r=ctx->randrsl;
  a = ctx->randa; b = ctx->randb + (++ctx->randc);
  for (m = mm, mend = m2 = m+(RANDSIZ/2); m<mend; ) {/**assert(0);**/
    rngstep( a<<13, a, b, mm, m, m2, r, x);
    rngstep( a>>6 , a, b, mm, m, m2, r, x);
    rngstep( a<<2 , a, b, mm, m, m2, r, x);
    rngstep( a>>16, a, b, mm, m, m2, r, x);
  }
  for (m2 = mm; m2<mend; ) {/**assert(0);**/
    rngstep( a<<13, a, b, mm, m, m2, r, x);
    rngstep( a>>6 , a, b, mm, m, m2, r, x);
    rngstep( a<<2 , a, b, mm, m, m2, r, x);
    rngstep( a>>16, a, b, mm, m, m2, r, x);
  }
  ctx->randb = b; ctx->randa = a;
}


#define mix(a,b,c,d,e,f,g,h) \
{/**assert(0);**/ \
   a^=b<<11; d+=a; b+=c; \
   b^=c>>2;  e+=b; c+=d; \
   c^=d<<8;  f+=c; d+=e; \
   d^=e>>16; g+=d; e+=f; \
   e^=f<<10; h+=e; f+=g; \
   f^=g>>4;  a+=f; g+=h; \
   g^=h<<8;  b+=g; h+=a; \
   h^=a>>9;  c+=h; a+=b; \
}

/* if (flag==TRUE), then use the contents of randrsl[] to initialize mm[]. */
void randinit(randctx *ctx, word     flag) {/**assert(0);**/
  word i;
  ub4 a,b,c,d,e,f,g,h;
  ub4 *m,*r;
  ctx->randa = ctx->randb = ctx->randc = 0;
  m=ctx->randmem;
  r=ctx->randrsl;
  a=b=c=d=e=f=g=h=0x9e3779b9;  /* the golden ratio */

  for (i=0; i<4; ++i)          /* scramble it */
  {/**assert(0);**/
    mix(a,b,c,d,e,f,g,h);
  }

  if (flag) {/**assert(0);**/
    /* initialize using the contents of r[] as the seed */
    for (i=0; i<RANDSIZ; i+=8) {/**assert(0);**/
      a+=r[i  ]; b+=r[i+1]; c+=r[i+2]; d+=r[i+3];
      e+=r[i+4]; f+=r[i+5]; g+=r[i+6]; h+=r[i+7];
      mix(a,b,c,d,e,f,g,h);
      m[i  ]=a; m[i+1]=b; m[i+2]=c; m[i+3]=d;
      m[i+4]=e; m[i+5]=f; m[i+6]=g; m[i+7]=h;
    }
    /* do a second pass to make all of the seed affect all of m */
    for (i=0; i<RANDSIZ; i+=8) {/**assert(0);**/
      a+=m[i  ]; b+=m[i+1]; c+=m[i+2]; d+=m[i+3];
      e+=m[i+4]; f+=m[i+5]; g+=m[i+6]; h+=m[i+7];
      mix(a,b,c,d,e,f,g,h);
      m[i  ]=a; m[i+1]=b; m[i+2]=c; m[i+3]=d;
      m[i+4]=e; m[i+5]=f; m[i+6]=g; m[i+7]=h;
    }
  } else {/**assert(0);**/
    /* fill in m[] with messy stuff */
    for (i=0; i<RANDSIZ; i+=8) {/**assert(0);**/
      mix(a,b,c,d,e,f,g,h);
      m[i  ]=a; m[i+1]=b; m[i+2]=c; m[i+3]=d;
      m[i+4]=e; m[i+5]=f; m[i+6]=g; m[i+7]=h;
    }
  }

  isaac(ctx);            /* fill in the first set of results */
  ctx->randcnt=RANDSIZ;  /* prepare to use the first set of results */
}



randctx ctx;

#endif













extern int MPI_NUMERO;
extern int MPI_NUMBER;



namespace MoGo {

  GoTools::GoTools() {/**assert(0);**/}



  GoTools::~GoTools() {/**assert(0);**/}}

void MoGo::GoTools::print( const char * msg, ... ) {
#ifdef MOGOMPI

  //PATCH : GoTools::print is always called by the proc 0.   
 /* if(MPI_NUMERO!=0)
    assert(0);*/
  //another proc never calls GoTools::print
  
  //if (MPI_NUMERO==0)
#endif
    {
      char a[10000];
      va_list args;
      va_start(args,msg);
      vsnprintf(a, 10000, msg, args);
      if (outputMode == STDERR_MODE) {
	if (!InnerMCGoPlayer::dontDisplay) { // it should clearly not be in InnerMCGoPlayer...
	  fprintf(stderr, "%s", a);
	  fflush(stderr);
	}
      } else if (outputMode == STDOUT_MODE) {
	fprintf(stdout, "%s", a);
	fflush(stdout);
      } else if (outputMode == FILE_MODE) {/**assert(0);**/
	if (outputFile == 0) {/**assert(0);**/
	  if (appendToFile)
	    outputFile = fopen(fileName.c_str(), "a");
	  else
	    outputFile = fopen(fileName.c_str(), "w");
	}
	if (outputFile != 0) {/**assert(0);**/
	  fprintf(outputFile, "%s", a);
	  fflush(outputFile);
	}
      } else if (outputMode == LOG_MODE) {
	if (!InnerMCGoPlayer::dontDisplay) { // it should clearly not be in InnerMCGoPlayer...
	  fprintf(stderr, "%s", a);
	  fflush(stderr);
	}
	
	if (outputFile == 0) {
	  if (appendToFile)
	    outputFile = fopen(fileName.c_str(), "a");
	  else
	    outputFile = fopen(fileName.c_str(), "w");
	}
	if (outputFile != 0) {
	  fprintf(outputFile, "%s", a);
	  fflush(outputFile);
	}
      } else if (outputMode == IGNORE_MODE)
	;
      va_end(args);
    }
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GoTools::setOutputMode( int mode ) {
  outputMode = mode;
}

 // TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GoTools::changeFileName( const std::string &name, bool appendToFile_) {
  if (outputFile)
    fclose(outputFile);
  fileName = name;
  appendToFile=appendToFile_;
  outputFile=0;
}

void MoGo::GoTools::saveOutputMode( ) {/**assert(0);**/
  savedOutputMode = outputMode;
  savedFileName = fileName;
  savedAppendToFile = appendToFile;
  if (outputFile) {/**assert(0);**/
    fclose(outputFile);
    outputFile = 0;
  }
}

void MoGo::GoTools::restoreOutputMode( ) {/**assert(0);**/
  outputMode=savedOutputMode;
  changeFileName(savedFileName,savedAppendToFile);
  /*if (outputFile) fclose(outputFile);
  fileName = savedFileName;
  appendToFile = savedAppendToFile;*/
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
std::string MoGo::GoTools::toString( int a ) {
  std::string tmp;
  return sprint(tmp, "%i", a);
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
std::string MoGo::GoTools::toString( double a, bool smallPrecision ) {
  std::string tmp;
  if (smallPrecision)
    return sprint(tmp, "%4.2f", a);
  else
    return sprint(tmp, "%f", a);
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
std::string MoGo::GoTools::sprint( std::string & s, const char * msg, ... ) {
  char a[10000];
  va_list args;
  va_start(args,msg);
  vsnprintf(a, 10000, msg, args);
  s = std::string(a);
  va_end(args);
  return s;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::GoTools::toDouble( const char *s, double & v ) {
  char *p;
  v=strtod(s, &p);
  return p!=s;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::GoTools::toInt( const char * s, int & v ) {
  char *p;
  v=(int)strtol(s, &p,10);
  return p!=s;
}

void MoGo::GoTools::print( const Vector< std::string > & v ) {/**assert(0);**/
  for (int i = 0 ; i < (int)v.size() ; i++)
    print("%s\n",v[i].c_str());
}
void MoGo::GoTools::print( const Vector< Vector < int > > & v ) {/**assert(0);**/
  for (int i = 0 ; i < (int)v.size() ; i++)
    print(v[i]);
}

double MoGo::GoTools::max( const Vector< double > & v, int * index ) {/**assert(0);**/
  if (index)
    *index = -1;
  double m=-1e100;
  for (int i = 0 ; i < (int)v.size() ; i++)
    if (v[i] > m) {/**assert(0);**/
      m=v[i];
      if (index)
        *index = i;
    }
  return m;
}

Vector<int> allRands;
// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool initAllRands() {
  allRands.resize(100000);
  for (int i=0;i<(int)allRands.size();i++)
#ifdef USE_NEW_RANDOM
    allRands[i]=rand(&ctx);
#else
    allRands[i]=rand();
#endif
  return true;
}
bool toInitAllRands=initAllRands();
int MoGo::GoTools::randDec[NUM_THREADS_MAX];
bool MoGo::GoTools::fastRand=false;
int MoGo::GoTools::iInRandArray[NUM_THREADS_MAX*1024];

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GoTools::mogoSRand( int seed ) {
#ifdef USE_NEW_RANDOM
  ctx.randa=ctx.randb=ctx.randc=(ub4)0;
  for (int i=0; i<256; ++i) ctx.randrsl[i]=(ub4)0;
  randinit(&ctx, false);
  for (int i=0;i<NUM_THREADS_MAX;i++) randDec[i]=rand(&ctx);
#else
  srand(seed);
  for (int i=0;i<NUM_THREADS_MAX;i++) randDec[i]=rand();
  for (int i=0;i<NUM_THREADS_MAX*COEF_CACHE_MISS_RANDOM;i++) iInRandArray[i]=0;
#endif
}






// TEMPORARY COMMENT: THIS FUNCTION IS USED
int MoGo::GoTools::mogoRand2( const int & max ) {
  /*#ifdef PARALLELIZED
    int numThread=ThreadsManagement::getNumThread();
  #else
    int numThread=0;
  #endif
    unsigned int hi, lo;
  int seed=randDec[numThread];
    lo = 16807 * (seed & 0xffff);
    hi = 16807 * (seed >> 16);
    lo += (hi & 0x7fff) << 16;
    lo += hi >> 15;
    randDec[numThread] = (lo & 0x7FFFFFFF) + (lo >> 31);
    return ((randDec[numThread] & 0xffff) * max) >> 16;
  */
  return ((mogoRand2 () & 0xffff) * max) >> 16;
}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
// TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
int MoGo::GoTools::mogoRand( const int & max ) {
#ifdef PARALLELIZED
  int numThread=ThreadsManagement::getNumThread();
#else
  int numThread=0;
#endif
  if (fastRand) {
    // return rand_int(max);
    if (iInRandArray[numThread*COEF_CACHE_MISS_RANDOM]>=(int)allRands.size()-10) {
#ifdef USE_NEW_RANDOM
      randDec[numThread]=rand(&ctx);
#else
      randDec[numThread]=rand();
#endif
      iInRandArray[numThread*COEF_CACHE_MISS_RANDOM]=0;
    }
    // assert(iInRandArray<(int)allRands.size());
    int tmp=/*abs(*/allRands[iInRandArray[numThread*COEF_CACHE_MISS_RANDOM]]+randDec[numThread]/*)*/;
    if (tmp == int(-2147483648)) tmp=0; else if (tmp < 0) tmp=-tmp;
    iInRandArray[numThread*COEF_CACHE_MISS_RANDOM]++;
    assert(tmp>=0); assert(max>0);
    tmp=tmp%max;
    assert(tmp >=0); assert(tmp<max);
    return tmp;
  } else {/**assert(0);**/

#ifdef USE_NEW_RANDOM
    return rand(&ctx)%max;
#else
    return rand()%max;
#endif

  }
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
int MoGo::GoTools::mogoRand2( ) {
#ifdef PARALLELIZED
  int numThread=ThreadsManagement::getNumThread();
#else
  int numThread=0;
#endif
  if (fastRand) {
    if (iInRandArray[numThread*COEF_CACHE_MISS_RANDOM]>=(int)allRands.size()-10) {
#ifdef USE_NEW_RANDOM
      randDec[numThread]=rand(&ctx);
#else
      randDec[numThread]=rand();
#endif
      iInRandArray[numThread*COEF_CACHE_MISS_RANDOM]=0;
    }
    // assert(iInRandArray<(int)allRands.size());
    int tmp=/*abs(*/allRands[iInRandArray[numThread*COEF_CACHE_MISS_RANDOM]]+randDec[numThread]/*)*/;
    if (tmp == int(-2147483648)) tmp=0; else if (tmp < 0) tmp=-tmp;
    iInRandArray[numThread*COEF_CACHE_MISS_RANDOM]++;
    return tmp;
  } else {/**assert(0);**/

#ifdef USE_NEW_RANDOM
    return rand(&ctx);
#else
    return rand();
#endif

  }
}

int MoGo::GoTools::mogoRand( ) {/**assert(0);**/
#ifdef USE_NEW_RANDOM
  return rand(&ctx);
#else
  return rand();
#endif
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GoTools::setFastRandMode( bool _fastRand ) {
  fastRand = _fastRand;
}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GoTools::printWithColorScaled( double nv, const char * msg, ... ) {
  if ((!useColor) || (outputMode == FILE_MODE) || (outputMode == LOG_MODE)) {
    va_list args;
    va_start(args,msg);
    char a[10000];
    vsnprintf(a, 10000, msg, args);
    print(a);
  } else {/**assert(0);**/
    std::string color;
    if (nv < -10.)
      color = "30";
    else if (nv < -1.)
      color="34";
    else if (nv < -0.4)
      color = "36";
    else if (nv < 0.)
      color = "32";
    else if (nv < 0.4)
      color = "35";
    else if (nv < 1.)
      color = "33";
    else
      color = "31";
    char a[10000];
    va_list args;
    va_start(args,msg);
    std::string message="\033["+color+"m";
    strcpy(a,message.c_str());
    vsnprintf(a+message.length(), 10000, msg, args);
    message=a; message+="\033[0m";
    print(message.c_str());
    va_end(args);
  }

}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GoTools::displayWithColor( bool useColor ) {
  MoGo::GoTools::useColor=useColor;
}
void MoGo::GoTools::print( const Vector< int > & v ) {/**assert(0);**/
  for (int i=0;i<(int)v.size();i++)
    print("%i ", v[i]);
  print("\n");
}

void MoGo::GoTools::print( const Vector< unsigned int > & v ) {/**assert(0);**/
  for (int i=0;i<(int)v.size();i++)
    print("%i ", v[i]);
  print("\n");
}

const Vector< int > & MoGo::GoTools::oneRandom8Permutations( ) {/**assert(0);**/
  if (all8Permutations.size()==0)
    initializePermutations();

  return all8Permutations[mogoRand(all8Permutations.size())];
}

void MoGo::GoTools::initializePermutations( ) {/**assert(0);**/
  all8Permutations.resize(40320);
  Vector<int> count(8);
  int i=0;
  while (add1(count,8)) {/**assert(0);**/
    if (isPermutation(count)) {/**assert(0);**/
      all8Permutations[i]=count;
      i++;
    }
  }
  assert(i==40320);
}

bool MoGo::GoTools::add1( Vector< int > & v, int base ) {/**assert(0);**/
  int i=0;
  while (i<(int)v.size()) {/**assert(0);**/
    v[i]++;
    if (v[i]>=base)
      v[i]=0;
    else
      break;
    i++;
  }
  return i<(int)v.size();
}

bool MoGo::GoTools::isPermutation( const Vector< int > & v ) {/**assert(0);**/
  Vector<bool> used(v.size(), false);
  for (int i=0;i<(int)v.size();i++)
    if (used[v[i]])
      return false;
    else
      used[v[i]]=true;

  return true;
}



double MoGo::GoTools::std( const Vector< double > & x ) {/**assert(0);**/
  if (x.size() < 2)
    return 0. ;
  else
    return(sqrt(sum(product(centered(x),centered(x)))/((double)x.size()-1))+1e-15);
}
double MoGo::GoTools::sum( const Vector< double > & x ) {/**assert(0);**/
  double s=0.0;
  for (int i=0;i<(int)x.size();i++)
    s+=x[i];
  return(s);

}
Vector< double > MoGo::GoTools::product( const Vector< double > & x, const Vector< double > & y ) {/**assert(0);**/
  Vector<double> prod;
  assert(x.size()==y.size());
  prod.resize((int)x.size());
  for (int i=0;i<(int)prod.size();i++)
    prod[i]=x[i]*y[i];
  return(prod);

}
Vector< double > MoGo::GoTools::centered( const Vector< double > & x ) {/**assert(0);**/
  Vector<double>y(x);
  double m(mean(x));

  for (int i=0;i<(int)y.size();i++)
    y[i]=y[i]-m;

  return(y);

}
double MoGo::GoTools::mean( const Vector< double > & x ) {/**assert(0);**/
  if (x.size()==0)
    return 0.;
  return(sum(x)/(double)x.size());
}
double MoGo::GoTools::correlation( const Vector< double > & x, const Vector< double > & y ) {/**assert(0);**/
  assert(x.size() == y.size()) ;
  assert(x.size()>1);
  /*  GoTools::print("correlation\n");
    GoTools::print("a=[");
    GoTools::print(x);
    GoTools::print("];");
    GoTools::print("b=[");
    GoTools::print(y);
    GoTools::print("];");
    GoTools::print("\n");*/
  double v=(sum(product(centered(x),centered(y)))/((double)x.size()-1)/(std(x)*std(y)));
  /* for (int i=0;i<(int)x.size();i++)
     print("%i ", x[i]);
   print("\n");
   for (int i=0;i<(int)y.size();i++)
     print("%i ", y[i]);
   print("\n");
   print("v=%f\n",v);*/
  return v;
}

double MoGo::GoTools::correlationDifferentSizes( const Vector< double > & x, const Vector< double > & y ) {/**assert(0);**/
  return(mean(productDifferentSizes(centered(x),centered(y)))/(std(x)*std(y)));
}
Vector< double > MoGo::GoTools::productDifferentSizes( const Vector< double > & x, const Vector< double > & y ) {/**assert(0);**/
  Vector<double> prod;
  int size=(int)min((int)x.size(),(int)y.size());
  prod.resize(size);
  for (int i=0;i<(int)prod.size();i++)
    prod[i]=x[i]*y[i];
  return(prod);

}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GoTools::mogoSRand( ) {
  struct timeb tp;
  ftime(&tp);
  //     GoTools::print("clock=%i/%i\n", tp.millitm, tp.time);
  MoGo::GoTools::mogoSRand(tp.millitm+1000*tp.time);
}

void MoGo::GoTools::print( const Vector< double > & v ) {/**assert(0);**/
  for (int i=0;i<(int)v.size();i++)
    print("%g ", v[i]);
  print("\n");
}

void MoGo::GoTools::print( const Vector< float > & v ) {/**assert(0);**/
  for (int i=0;i<(int)v.size();i++)
    print("%g ", v[i]);
  print("\n");
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
Vector< std::string > MoGo::GoTools::toVector( const std::string & s ) {
  Vector< std::string > res;
  for (int i=0;i<(int)s.length(); ) {
    string tmp;
    for ( ;i<(int)s.length();i++)
      if (s[i]==' ')
        break;
      else
        tmp+=s[i];
    if (tmp.length()>0)
      res.push_back(tmp);
    i++;
    for ( ;i<(int)s.length();i++)
      if (s[i]!=' ')
        break;
  }
  return res;
}

Vector< std::string > MoGo::GoTools::split( const char * s, char sep ) {/**assert(0);**/
  Vector<std::string> res;
  if (s==0)
    return res;
  int i=0;
  while (s[i]!=0) {/**assert(0);**/
    std::string tmp;
    for (;s[i]!=0 && s[i]!=sep;i++)
      tmp+=s[i];
    if (tmp.length()>0)
      res.push_back(tmp);
    while (s[i]==sep) i++;
  }
  return res;
}

void printOnFile(const int &v, FILE *f) {/**assert(0);**/
  fprintf(f, "%i ", v);
}
void printOnFile(const double &v, FILE *f) {/**assert(0);**/
  fprintf(f, "%f ", v);
}
void readOnFile(int &v, FILE *f) {/**assert(0);**/
  fscanf(f, "%i ", &v);
}
void readOnFile(double &v, FILE *f) {/**assert(0);**/
  float vf;
  fscanf(f, "%f ", &vf);
  v=vf;
}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
double MoGo::GoTools::memoryUsed( ) {
  return double((long long)sbrk(0))/1000000.;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GoTools::initMemoryUsed( ) {
  startMemoryUsed=memoryUsed();
}

double MoGo::GoTools::additionalMemoryUsed( ) {/**assert(0);**/
  return memoryUsed()-startMemoryUsed;
}
