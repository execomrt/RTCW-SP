/*
===========================================================================

Return to Castle Wolfenstein single player GPL Source Code
Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company. 

This file is part of the Return to Castle Wolfenstein single player GPL Source Code (RTCW SP Source Code).  

RTCW SP Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

RTCW SP Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RTCW SP Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the RTCW SP Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the RTCW SP Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

// bg_lib,c -- standard C library replacement routines used by code
// compiled for the virtual machine

#include "q_shared.h"

/*-
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#if defined( LIBC_SCCS ) && !defined( lint )
#if 0
static char sccsid[] = "@(#)qsort.c	8.1 (Berkeley) 6/4/93";
#endif
static const char rcsid[] =
#endif /* LIBC_SCCS and not lint */

#include <stdlib.h>

typedef int cmp_t ( const void *, const void * );
static char *med3( char *, char *, char *, cmp_t * );
static void  swapfunc( char *, char *, int, int );

#ifndef min
#define min( a, b )   ( a ) < ( b ) ? a : b
#endif

/*
 * Qsort routine from Bentley & McIlroy's "Engineering a Sort Function".
 */
#define swapcode( TYPE, parmi, parmj, n ) {		  \
		long i = ( n ) / sizeof( TYPE );		   \
		register TYPE *pi = (TYPE *) ( parmi );		  \
		register TYPE *pj = (TYPE *) ( parmj );		  \
		do {						\
			register TYPE t = *pi;		  \
			*pi++ = *pj;				\
			*pj++ = t;				\
		} while ( --i > 0 );			  \
}

#define SWAPINIT( a, es ) swaptype = ( (char *)a - (char *)0 ) % sizeof( long ) || \
									 es % sizeof( long ) ? 2 : es == sizeof( long ) ? 0 : 1;

static void
swapfunc( a, b, n, swaptype )
char *a, *b;
int n, swaptype;
{
	if ( swaptype <= 1 ) {
		swapcode( long, a, b, n )
	} else {
		swapcode( char, a, b, n )
	}
}

#define swap( a, b )				  \
	if ( swaptype == 0 ) {				  \
		long t = *(long *)( a );		  \
		*(long *)( a ) = *(long *)( b );		\
		*(long *)( b ) = t;			  \
	} else \
		swapfunc( a, b, es, swaptype )

#define vecswap( a, b, n )    if ( ( n ) > 0 ) swapfunc( a, b, n, swaptype )

static char *
med3( a, b, c, cmp )
char *a, *b, *c;
cmp_t *cmp;
{
	return cmp( a, b ) < 0 ?
		   ( cmp( b, c ) < 0 ? b : ( cmp( a, c ) < 0 ? c : a ) )
		   : ( cmp( b, c ) > 0 ? b : ( cmp( a, c ) < 0 ? a : c ) );
}

void
qsort_function( a, n, es, cmp )
void *a;
size_t n, es;
cmp_t *cmp;
{
	char *pa, *pb, *pc, *pd, *pl, *pm, *pn;
	int d, r, swaptype, swap_cnt;

loop:   SWAPINIT( a, es );
	swap_cnt = 0;
	if ( n < 7 ) {
		for ( pm = (char *)a + es; pm < (char *)a + n * es; pm += es )
			for ( pl = pm; pl > (char *)a && cmp( pl - es, pl ) > 0;
				  pl -= es )
				swap( pl, pl - es );
		return;
	}
	pm = (char *)a + ( n / 2 ) * es;
	if ( n > 7 ) {
		pl = a;
		pn = (char *)a + ( n - 1 ) * es;
		if ( n > 40 ) {
			d = ( n / 8 ) * es;
			pl = med3( pl, pl + d, pl + 2 * d, cmp );
			pm = med3( pm - d, pm, pm + d, cmp );
			pn = med3( pn - 2 * d, pn - d, pn, cmp );
		}
		pm = med3( pl, pm, pn, cmp );
	}
	swap( a, pm );
	pa = pb = (char *)a + es;

	pc = pd = (char *)a + ( n - 1 ) * es;
	for (;; ) {
		while ( pb <= pc && ( r = cmp( pb, a ) ) <= 0 ) {
			if ( r == 0 ) {
				swap_cnt = 1;
				swap( pa, pb );
				pa += es;
			}
			pb += es;
		}
		while ( pb <= pc && ( r = cmp( pc, a ) ) >= 0 ) {
			if ( r == 0 ) {
				swap_cnt = 1;
				swap( pc, pd );
				pd -= es;
			}
			pc -= es;
		}
		if ( pb > pc ) {
			break;
		}
		swap( pb, pc );
		swap_cnt = 1;
		pb += es;
		pc -= es;
	}
	if ( swap_cnt == 0 ) {  /* Switch to insertion sort */
		for ( pm = (char *)a + es; pm < (char *)a + n * es; pm += es )
			for ( pl = pm; pl > (char *)a && cmp( pl - es, pl ) > 0;
				  pl -= es )
				swap( pl, pl - es );
		return;
	}

	pn = (char *)a + n * es;
	r = min( pa - (char *)a, pb - pa );
	vecswap( a, pb - r, r );
	r = min( pd - pc, pn - pd - es );
	vecswap( pb, pn - r, r );
	if ( ( r = pb - pa ) > es ) {
		qsort_function( a, r / es, es, cmp );
	}
	if ( ( r = pd - pc ) > es ) {
		/* Iterate rather than recurse to save stack space */
		a = pn - r;
		n = r / es;
		goto loop;
	}
/*		qsort_function(pn - r, r / es, es, cmp);*/
}

//==================================================================================

#ifndef _MSC_VER

// this file is excluded from release builds because of intrinsics

size_t strlen( const char *string ) {
	const char  *s;

	s = string;
	while ( *s ) {
		s++;
	}
	return s - string;
}


char *strcat( char *strDestination, const char *strSource ) {
	char    *s;

	s = strDestination;
	while ( *s ) {
		s++;
	}
	while ( *strSource ) {
		*s++ = *strSource++;
	}
	*s = 0;
	return strDestination;
}

char *strcpy( char *strDestination, const char *strSource ) {
	char *s;

	s = strDestination;
	while ( *strSource ) {
		*s++ = *strSource++;
	}
	*s = 0;
	return strDestination;
}


int strcmp( const char *string1, const char *string2 ) {
	while ( *string1 == *string2 && *string1 && *string2 ) {
		string1++;
		string2++;
	}
	return *string1 - *string2;
}


char *strchr( const char *string, int c ) {
	while ( *string ) {
		if ( *string == c ) {
			return ( char * )string;
		}
		string++;
	}
	return (char *)0;
}

char *strstr( const char *string, const char *strCharSet ) {
	while ( *string ) {
		int i;

		for ( i = 0 ; strCharSet[i] ; i++ ) {
			if ( string[i] != strCharSet[i] ) {
				break;
			}
		}
		if ( !strCharSet[i] ) {
			return (char *)string;
		}
		string++;
	}
	return (char *)0;
}
#endif 

// NOTE TTimo: hey, given the number of !defined, I wonder what platform uses this
#if !defined ( _MSC_VER ) && !defined ( __linux__ ) && !defined ( MAC_WOLF2_MP ) && !defined ( MAC_WOLF2_SP )

int tolower( int c ) {
	if ( Q_isupper( c ) ) {
		c += 'a' - 'A';
	}
	return c;
}


int toupper( int c ) {
	if ( c >= 'a' && c <= 'z' ) {
		c += 'A' - 'a';
	}
	return c;
}

#endif
//#ifndef _MSC_VER
#ifndef _MSC_VER
void *memmove( void *dest, const void *src, size_t count ) {
	int i;

	if ( dest > src ) {
		for ( i = count - 1 ; i >= 0 ; i-- ) {
			( (char *)dest )[i] = ( (char *)src )[i];
		}
	} else {
		for ( i = 0 ; i < count ; i++ ) {
			( (char *)dest )[i] = ( (char *)src )[i];
		}
	}
	return dest;
}
#endif

#if 0

double floor( double x ) {
	return (int)( x + 0x40000000 ) - 0x40000000;
}

void *memset( void *dest, int c, size_t count ) {
	while ( count-- ) {
		( (char *)dest )[count] = c;
	}
	return dest;
}

void *memcpy( void *dest, const void *src, size_t count ) {
	while ( count-- ) {
		( (char *)dest )[count] = ( (char *)src )[count];
	}
	return dest;
}

char *strncpy( char *strDest, const char *strSource, size_t count ) {
	char    *s;

	s = strDest;
	while ( *strSource && count ) {
		*s++ = *strSource++;
		count--;
	}
	while ( count-- ) {
		*s++ = 0;
	}
	return strDest;
}

double sqrt( double x ) {
	float y;
	float delta;
	float maxError;

	if ( x <= 0 ) {
		return 0;
	}

	// initial guess
	y = x / 2;

	// refine
	maxError = x * 0.001;

	do {
		delta = ( y * y ) - x;
		y -= delta / ( 2 * y );
	} while ( delta > maxError || delta < -maxError );

	return y;
}


float sintable[1024] = {
	0.000000,0.001534,0.003068,0.004602,0.006136,0.007670,0.009204,0.010738,
	0.012272,0.013805,0.015339,0.016873,0.018407,0.019940,0.021474,0.023008,
	0.024541,0.026075,0.027608,0.029142,0.030675,0.032208,0.033741,0.035274,
	0.036807,0.038340,0.039873,0.041406,0.042938,0.044471,0.046003,0.047535,
	0.049068,0.050600,0.052132,0.053664,0.055195,0.056727,0.058258,0.059790,
	0.061321,0.062852,0.064383,0.065913,0.067444,0.068974,0.070505,0.072035,
	0.073565,0.075094,0.076624,0.078153,0.079682,0.081211,0.082740,0.084269,
	0.085797,0.087326,0.088854,0.090381,0.091909,0.093436,0.094963,0.096490,
	0.098017,0.099544,0.101070,0.102596,0.104122,0.105647,0.107172,0.108697,
	0.110222,0.111747,0.113271,0.114795,0.116319,0.117842,0.119365,0.120888,
	0.122411,0.123933,0.125455,0.126977,0.128498,0.130019,0.131540,0.133061,
	0.134581,0.136101,0.137620,0.139139,0.140658,0.142177,0.143695,0.145213,
	0.146730,0.148248,0.149765,0.151281,0.152797,0.154313,0.155828,0.157343,
	0.158858,0.160372,0.161886,0.163400,0.164913,0.166426,0.167938,0.169450,
	0.170962,0.172473,0.173984,0.175494,0.177004,0.178514,0.180023,0.181532,
	0.183040,0.184548,0.186055,0.187562,0.189069,0.190575,0.192080,0.193586,
	0.195090,0.196595,0.198098,0.199602,0.201105,0.202607,0.204109,0.205610,
	0.207111,0.208612,0.210112,0.211611,0.213110,0.214609,0.216107,0.217604,
	0.219101,0.220598,0.222094,0.223589,0.225084,0.226578,0.228072,0.229565,
	0.231058,0.232550,0.234042,0.235533,0.237024,0.238514,0.240003,0.241492,
	0.242980,0.244468,0.245955,0.247442,0.248928,0.250413,0.251898,0.253382,
	0.254866,0.256349,0.257831,0.259313,0.260794,0.262275,0.263755,0.265234,
	0.266713,0.268191,0.269668,0.271145,0.272621,0.274097,0.275572,0.277046,
	0.278520,0.279993,0.281465,0.282937,0.284408,0.285878,0.287347,0.288816,
	0.290285,0.291752,0.293219,0.294685,0.296151,0.297616,0.299080,0.300543,
	0.302006,0.303468,0.304929,0.306390,0.307850,0.309309,0.310767,0.312225,
	0.313682,0.315138,0.316593,0.318048,0.319502,0.320955,0.322408,0.323859,
	0.325310,0.326760,0.328210,0.329658,0.331106,0.332553,0.334000,0.335445,
	0.336890,0.338334,0.339777,0.341219,0.342661,0.344101,0.345541,0.346980,
	0.348419,0.349856,0.351293,0.352729,0.354164,0.355598,0.357031,0.358463,
	0.359895,0.361326,0.362756,0.364185,0.365613,0.367040,0.368467,0.369892,
	0.371317,0.372741,0.374164,0.375586,0.377007,0.378428,0.379847,0.381266,
	0.382683,0.384100,0.385516,0.386931,0.388345,0.389758,0.391170,0.392582,
	0.393992,0.395401,0.396810,0.398218,0.399624,0.401030,0.402435,0.403838,
	0.405241,0.406643,0.408044,0.409444,0.410843,0.412241,0.413638,0.415034,
	0.416430,0.417824,0.419217,0.420609,0.422000,0.423390,0.424780,0.426168,
	0.427555,0.428941,0.430326,0.431711,0.433094,0.434476,0.435857,0.437237,
	0.438616,0.439994,0.441371,0.442747,0.444122,0.445496,0.446869,0.448241,
	0.449611,0.450981,0.452350,0.453717,0.455084,0.456449,0.457813,0.459177,
	0.460539,0.461900,0.463260,0.464619,0.465976,0.467333,0.468689,0.470043,
	0.471397,0.472749,0.474100,0.475450,0.476799,0.478147,0.479494,0.480839,
	0.482184,0.483527,0.484869,0.486210,0.487550,0.488889,0.490226,0.491563,
	0.492898,0.494232,0.495565,0.496897,0.498228,0.499557,0.500885,0.502212,
	0.503538,0.504863,0.506187,0.507509,0.508830,0.510150,0.511469,0.512786,
	0.514103,0.515418,0.516732,0.518045,0.519356,0.520666,0.521975,0.523283,
	0.524590,0.525895,0.527199,0.528502,0.529804,0.531104,0.532403,0.533701,
	0.534998,0.536293,0.537587,0.538880,0.540171,0.541462,0.542751,0.544039,
	0.545325,0.546610,0.547894,0.549177,0.550458,0.551738,0.553017,0.554294,
	0.555570,0.556845,0.558119,0.559391,0.560662,0.561931,0.563199,0.564466,
	0.565732,0.566996,0.568259,0.569521,0.570781,0.572040,0.573297,0.574553,
	0.575808,0.577062,0.578314,0.579565,0.580814,0.582062,0.583309,0.584554,
	0.585798,0.587040,0.588282,0.589521,0.590760,0.591997,0.593232,0.594466,
	0.595699,0.596931,0.598161,0.599389,0.600616,0.601842,0.603067,0.604290,
	0.605511,0.606731,0.607950,0.609167,0.610383,0.611597,0.612810,0.614022,
	0.615232,0.616440,0.617647,0.618853,0.620057,0.621260,0.622461,0.623661,
	0.624859,0.626056,0.627252,0.628446,0.629638,0.630829,0.632019,0.633207,
	0.634393,0.635578,0.636762,0.637944,0.639124,0.640303,0.641481,0.642657,
	0.643832,0.645005,0.646176,0.647346,0.648514,0.649681,0.650847,0.652011,
	0.653173,0.654334,0.655493,0.656651,0.657807,0.658961,0.660114,0.661266,
	0.662416,0.663564,0.664711,0.665856,0.667000,0.668142,0.669283,0.670422,
	0.671559,0.672695,0.673829,0.674962,0.676093,0.677222,0.678350,0.679476,
	0.680601,0.681724,0.682846,0.683965,0.685084,0.686200,0.687315,0.688429,
	0.689541,0.690651,0.691759,0.692866,0.693971,0.695075,0.696177,0.697278,
	0.698376,0.699473,0.700569,0.701663,0.702755,0.703845,0.704934,0.706021,
	0.707107,0.708191,0.709273,0.710353,0.711432,0.712509,0.713585,0.714659,
	0.715731,0.716801,0.717870,0.718937,0.720003,0.721066,0.722128,0.723188,
	0.724247,0.725304,0.726359,0.727413,0.728464,0.729514,0.730563,0.731609,
	0.732654,0.733697,0.734739,0.735779,0.736817,0.737853,0.738887,0.739920,
	0.740951,0.741980,0.743008,0.744034,0.745058,0.746080,0.747101,0.748119,
	0.749136,0.750152,0.751165,0.752177,0.753187,0.754195,0.755201,0.756206,
	0.757209,0.758210,0.759209,0.760207,0.761202,0.762196,0.763188,0.764179,
	0.765167,0.766154,0.767139,0.768122,0.769103,0.770083,0.771061,0.772036,
	0.773010,0.773983,0.774953,0.775922,0.776888,0.777853,0.778817,0.779778,
	0.780737,0.781695,0.782651,0.783605,0.784557,0.785507,0.786455,0.787402,
	0.788346,0.789289,0.790230,0.791169,0.792107,0.793042,0.793975,0.794907,
	0.795837,0.796765,0.797691,0.798615,0.799537,0.800458,0.801376,0.802293,
	0.803208,0.804120,0.805031,0.805940,0.806848,0.807753,0.808656,0.809558,
	0.810457,0.811355,0.812251,0.813144,0.814036,0.814926,0.815814,0.816701,
	0.817585,0.818467,0.819348,0.820226,0.821103,0.821977,0.822850,0.823721,
	0.824589,0.825456,0.826321,0.827184,0.828045,0.828904,0.829761,0.830616,
	0.831470,0.832321,0.833170,0.834018,0.834863,0.835706,0.836548,0.837387,
	0.838225,0.839060,0.839894,0.840725,0.841555,0.842383,0.843208,0.844032,
	0.844854,0.845673,0.846491,0.847307,0.848120,0.848932,0.849742,0.850549,
	0.851355,0.852159,0.852961,0.853760,0.854558,0.855354,0.856147,0.856939,
	0.857729,0.858516,0.859302,0.860085,0.860867,0.861646,0.862424,0.863199,
	0.863973,0.864744,0.865514,0.866281,0.867046,0.867809,0.868571,0.869330,
	0.870087,0.870842,0.871595,0.872346,0.873095,0.873842,0.874587,0.875329,
	0.876070,0.876809,0.877545,0.878280,0.879012,0.879743,0.880471,0.881197,
	0.881921,0.882643,0.883363,0.884081,0.884797,0.885511,0.886223,0.886932,
	0.887640,0.888345,0.889048,0.889750,0.890449,0.891146,0.891841,0.892534,
	0.893224,0.893913,0.894599,0.895284,0.895966,0.896646,0.897325,0.898001,
	0.898674,0.899346,0.900016,0.900683,0.901349,0.902012,0.902673,0.903332,
	0.903989,0.904644,0.905297,0.905947,0.906596,0.907242,0.907886,0.908528,
	0.909168,0.909806,0.910441,0.911075,0.911706,0.912335,0.912962,0.913587,
	0.914210,0.914830,0.915449,0.916065,0.916679,0.917291,0.917901,0.918508,
	0.919114,0.919717,0.920318,0.920917,0.921514,0.922109,0.922701,0.923291,
	0.923880,0.924465,0.925049,0.925631,0.926210,0.926787,0.927363,0.927935,
	0.928506,0.929075,0.929641,0.930205,0.930767,0.931327,0.931884,0.932440,
	0.932993,0.933544,0.934093,0.934639,0.935184,0.935726,0.936266,0.936803,
	0.937339,0.937872,0.938404,0.938932,0.939459,0.939984,0.940506,0.941026,
	0.941544,0.942060,0.942573,0.943084,0.943593,0.944100,0.944605,0.945107,
	0.945607,0.946105,0.946601,0.947094,0.947586,0.948075,0.948561,0.949046,
	0.949528,0.950008,0.950486,0.950962,0.951435,0.951906,0.952375,0.952842,
	0.953306,0.953768,0.954228,0.954686,0.955141,0.955594,0.956045,0.956494,
	0.956940,0.957385,0.957826,0.958266,0.958703,0.959139,0.959572,0.960002,
	0.960431,0.960857,0.961280,0.961702,0.962121,0.962538,0.962953,0.963366,
	0.963776,0.964184,0.964590,0.964993,0.965394,0.965793,0.966190,0.966584,
	0.966976,0.967366,0.967754,0.968139,0.968522,0.968903,0.969281,0.969657,
	0.970031,0.970403,0.970772,0.971139,0.971504,0.971866,0.972226,0.972584,
	0.972940,0.973293,0.973644,0.973993,0.974339,0.974684,0.975025,0.975365,
	0.975702,0.976037,0.976370,0.976700,0.977028,0.977354,0.977677,0.977999,
	0.978317,0.978634,0.978948,0.979260,0.979570,0.979877,0.980182,0.980485,
	0.980785,0.981083,0.981379,0.981673,0.981964,0.982253,0.982539,0.982824,
	0.983105,0.983385,0.983662,0.983937,0.984210,0.984480,0.984749,0.985014,
	0.985278,0.985539,0.985798,0.986054,0.986308,0.986560,0.986809,0.987057,
	0.987301,0.987544,0.987784,0.988022,0.988258,0.988491,0.988722,0.988950,
	0.989177,0.989400,0.989622,0.989841,0.990058,0.990273,0.990485,0.990695,
	0.990903,0.991108,0.991311,0.991511,0.991710,0.991906,0.992099,0.992291,
	0.992480,0.992666,0.992850,0.993032,0.993212,0.993389,0.993564,0.993737,
	0.993907,0.994075,0.994240,0.994404,0.994565,0.994723,0.994879,0.995033,
	0.995185,0.995334,0.995481,0.995625,0.995767,0.995907,0.996045,0.996180,
	0.996313,0.996443,0.996571,0.996697,0.996820,0.996941,0.997060,0.997176,
	0.997290,0.997402,0.997511,0.997618,0.997723,0.997825,0.997925,0.998023,
	0.998118,0.998211,0.998302,0.998390,0.998476,0.998559,0.998640,0.998719,
	0.998795,0.998870,0.998941,0.999011,0.999078,0.999142,0.999205,0.999265,
	0.999322,0.999378,0.999431,0.999481,0.999529,0.999575,0.999619,0.999660,
	0.999699,0.999735,0.999769,0.999801,0.999831,0.999858,0.999882,0.999905,
	0.999925,0.999942,0.999958,0.999971,0.999981,0.999989,0.999995,0.999999
};

double sin( double x ) {
	int index;
	int quad;

	index = 1024 * x / ( M_PI * 0.5 );
	quad = ( index >> 10 ) & 3;
	index &= 1023;
	switch ( quad ) {
	case 0:
		return sintable[index];
	case 1:
		return sintable[1023 - index];
	case 2:
		return -sintable[index];
	case 3:
		return -sintable[1023 - index];
	}
	return 0;
}


double cos( double x ) {
	int index;
	int quad;

	index = 1024 * x / ( M_PI * 0.5 );
	quad = ( index >> 10 ) & 3;
	index &= 1023;
	switch ( quad ) {
	case 3:
		return sintable[index];
	case 0:
		return sintable[1023 - index];
	case 1:
		return -sintable[index];
	case 2:
		return -sintable[1023 - index];
	}
	return 0;
}


double atan2( double y, double x ) {
	float base;
	float temp;
	float dir;
	float test;
	int i;

	if ( x < 0 ) {
		if ( y >= 0 ) {
			// quad 1
			base = M_PI / 2;
			temp = x;
			x = y;
			y = -temp;
		} else {
			// quad 2
			base = M_PI;
			x = -x;
			y = -y;
		}
	} else {
		if ( y < 0 ) {
			// quad 3
			base = 3 * M_PI / 2;
			temp = x;
			x = -y;
			y = temp;
		}
	}

	if ( y > x ) {
		base += M_PI / 2;
		temp = x;
		x = y;
		y = temp;
		dir = -1;
	} else {
		dir = 1;
	}

	// calcualte angle in octant 0
	if ( x == 0 ) {
		return base;
	}
	y /= x;

	for ( i = 0 ; i < 512 ; i++ ) {
		test = sintable[i] / sintable[1023 - i];
		if ( test > y ) {
			break;
		}
	}

	return base + dir * i * ( M_PI / 2048 );
}


#endif

#ifndef _MSC_VER
double tan( double x ) {
	return sin( x ) / cos( x );
}
#endif


static int randSeed = 0;
#ifndef _MSC_VER
void    srand( unsigned seed ) {
	randSeed = seed;
}

int     rand( void ) {
	randSeed = ( 69069 * randSeed + 1 );
	return randSeed & 0x7fff;
}

double atof( const char *string ) {
	float sign;
	float value;
	int c;


	// skip whitespace
	while ( *string <= ' ' ) {
		if ( !*string ) {
			return 0;
		}
		string++;
	}

	// check sign
	switch ( *string ) {
	case '+':
		string++;
		sign = 1;
		break;
	case '-':
		string++;
		sign = -1;
		break;
	default:
		sign = 1;
		break;
	}

	// read digits
	value = 0;
	c = string[0];
	if ( c != '.' ) {
		do {
			c = *string++;
			if ( c < '0' || c > '9' ) {
				break;
			}
			c -= '0';
			value = value * 10 + c;
		} while ( 1 );
	} else {
		string++;
	}

	// check for decimal point
	if ( c == '.' ) {
		double fraction;

		fraction = 0.1;
		do {
			c = *string++;
			if ( c < '0' || c > '9' ) {
				break;
			}
			c -= '0';
			value += c * fraction;
			fraction *= 0.1;
		} while ( 1 );

	}

	// not handling 10e10 notation...

	return value * sign;
}

double _atof( const char **stringPtr ) {
	const char  *string;
	float sign;
	float value;
	int c;

	string = *stringPtr;

	// skip whitespace
	while ( *string <= ' ' ) {
		if ( !*string ) {
			*stringPtr = string;
			return 0;
		}
		string++;
	}

	// check sign
	switch ( *string ) {
	case '+':
		string++;
		sign = 1;
		break;
	case '-':
		string++;
		sign = -1;
		break;
	default:
		sign = 1;
		break;
	}

	// read digits
	value = 0;
	if ( string[0] != '.' ) {
		do {
			c = *string++;
			if ( c < '0' || c > '9' ) {
				break;
			}
			c -= '0';
			value = value * 10 + c;
		} while ( 1 );
	}

	// check for decimal point
	if ( c == '.' ) {
		double fraction;

		fraction = 0.1;
		do {
			c = *string++;
			if ( c < '0' || c > '9' ) {
				break;
			}
			c -= '0';
			value += c * fraction;
			fraction *= 0.1;
		} while ( 1 );

	}

	// not handling 10e10 notation...
	*stringPtr = string;

	return value * sign;
}

#endif 

#if !defined( _MSC_VER ) && !defined( __linux__ )

int atoi( const char *string ) {
	int sign;
	int value;
	int c;


	// skip whitespace
	while ( *string <= ' ' ) {
		if ( !*string ) {
			return 0;
		}
		string++;
	}

	// check sign
	switch ( *string ) {
	case '+':
		string++;
		sign = 1;
		break;
	case '-':
		string++;
		sign = -1;
		break;
	default:
		sign = 1;
		break;
	}

	// read digits
	value = 0;
	do {
		c = *string++;
		if ( c < '0' || c > '9' ) {
			break;
		}
		c -= '0';
		value = value * 10 + c;
	} while ( 1 );

	// not handling 10e10 notation...

	return value * sign;
}


int _atoi( const char **stringPtr ) {
	int sign;
	int value;
	int c;
	const char  *string;

	string = *stringPtr;

	// skip whitespace
	while ( *string <= ' ' ) {
		if ( !*string ) {
			return 0;
		}
		string++;
	}

	// check sign
	switch ( *string ) {
	case '+':
		string++;
		sign = 1;
		break;
	case '-':
		string++;
		sign = -1;
		break;
	default:
		sign = 1;
		break;
	}

	// read digits
	value = 0;
	do {
		c = *string++;
		if ( c < '0' || c > '9' ) {
			break;
		}
		c -= '0';
		value = value * 10 + c;
	} while ( 1 );

	// not handling 10e10 notation...

	*stringPtr = string;

	return value * sign;
}

int abs( int n ) {
	return n < 0 ? -n : n;
}

double fabs( double x ) {
	return x < 0 ? -x : x;
}



//=========================================================


#define ALT         0x00000001      /* alternate form */
#define HEXPREFIX   0x00000002      /* add 0x or 0X prefix */
#define LADJUST     0x00000004      /* left adjustment */
#define LONGDBL     0x00000008      /* long double */
#define LONGINT     0x00000010      /* long integer */
#define QUADINT     0x00000020      /* quad integer */
#define SHORTINT    0x00000040      /* short integer */
#define ZEROPAD     0x00000080      /* zero (as opposed to blank) pad */
#define FPT         0x00000100      /* floating point number */

#define to_digit( c )     ( ( c ) - '0' )
#define is_digit( c )     ( (unsigned)to_digit( c ) <= 9 )
#define to_char( n )      ( ( n ) + '0' )

void AddInt( char **buf_p, int val, int width, int flags ) {
	char text[32];
	int digits;
	int signedVal;
	char    *buf;

	digits = 0;
	signedVal = val;
	if ( val < 0 ) {
		val = -val;
	}
	do {
		text[digits++] = '0' + val % 10;
		val /= 10;
	} while ( val );

	if ( signedVal < 0 ) {
		text[digits++] = '-';
	}

	buf = *buf_p;

	if ( !( flags & LADJUST ) ) {
		while ( digits < width ) {
			*buf++ = ( flags & ZEROPAD ) ? '0' : ' ';
			width--;
		}
	}

	while ( digits-- ) {
		*buf++ = text[digits];
		width--;
	}

	if ( flags & LADJUST ) {
		while ( width-- ) {
			*buf++ = ( flags & ZEROPAD ) ? '0' : ' ';
		}
	}

	*buf_p = buf;
}

void AddFloat( char **buf_p, float fval, int width, int prec ) {
	char text[32];
	int digits;
	float signedVal;
	char    *buf;
	int val;

	// FIXME!!!! handle fractions

	digits = 0;
	signedVal = fval;
	if ( fval < 0 ) {
		fval = -fval;
	}

	val = (int)fval;
	do {
		text[digits++] = '0' + val % 10;
		val /= 10;
	} while ( val );

	if ( signedVal < 0 ) {
		text[digits++] = '-';
	}

	buf = *buf_p;

	while ( digits < width ) {
		*buf++ = ' ';
		width--;
	}

	while ( digits-- ) {
		*buf++ = text[digits];
	}

	*buf_p = buf;
}


void AddString( char **buf_p, char *string, int width, int prec ) {
	int size;
	char    *buf;

	buf = *buf_p;

	if ( string == NULL ) {
		string = "(null)";
		prec = -1;
	}

	if ( prec >= 0 ) {
		for ( size = 0; size < prec; size++ ) {
			if ( string[size] == '\0' ) {
				break;
			}
		}
	} else {
		size = strlen( string );
	}

	width -= size;

	while ( size-- ) {
		*buf++ = *string++;
	}

	while ( width-- > 0 ) {
		*buf++ = ' ';
	}

	*buf_p = buf;
}

/*
vsprintf

I'm not going to support a bunch of the more arcane stuff in here
just to keep it simpler.  For example, the '*' and '$' are not
currently supported.  I've tried to make it so that it will just
parse and ignore formats we don't support.
*/
int vsprintf( char *buffer, const char *fmt, va_list argptr ) {
	int     *arg;
	char    *buf_p;
	char ch;
	int flags;
	int width;
	int prec;
	int n;
	char sign;

	buf_p = buffer;
	arg = (int *)argptr;

	while ( qtrue ) {
		// run through the format string until we hit a '%' or '\0'
		for ( ch = *fmt; ( ch = *fmt ) != '\0' && ch != '%'; fmt++ ) {
			*buf_p++ = ch;
		}
		if ( ch == '\0' ) {
			goto done;
		}

		// skip over the '%'
		fmt++;

		// reset formatting state
		flags = 0;
		width = 0;
		prec = -1;
		sign = '\0';

rflag:
		ch = *fmt++;
reswitch:
		switch ( ch ) {
		case '-':
			flags |= LADJUST;
			goto rflag;
		case '.':
			n = 0;
			while ( is_digit( ( ch = *fmt++ ) ) ) {
				n = 10 * n + ( ch - '0' );
			}
			prec = n < 0 ? -1 : n;
			goto reswitch;
		case '0':
			flags |= ZEROPAD;
			goto rflag;
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			n = 0;
			do {
				n = 10 * n + ( ch - '0' );
				ch = *fmt++;
			} while ( is_digit( ch ) );
			width = n;
			goto reswitch;
		case 'c':
			*buf_p++ = (char)*arg;
			arg++;
			break;
		case 'd':
		case 'i':
			AddInt( &buf_p, *arg, width, flags );
			arg++;
			break;
		case 'f':
			AddFloat( &buf_p, *(double *)arg, width, prec );
#ifdef __LCC__
			arg += 1;   // everything is 32 bit in my compiler
#else
			arg += 2;
#endif
			break;
		case 's':
			AddString( &buf_p, (char *)*arg, width, prec );
			arg++;
			break;
		case '%':
			*buf_p++ = ch;
			break;
		default:
			*buf_p++ = (char)*arg;
			arg++;
			break;
		}
	}

done:
	*buf_p = 0;
	return buf_p - buffer;
}


/* this is really crappy */
int sscanf( const char *buffer, const char *fmt, ... ) {
	int cmd;
	int     **arg;
	int count;

	arg = (int **)&fmt + 1;
	count = 0;

	while ( *fmt ) {
		if ( fmt[0] != '%' ) {
			fmt++;
			continue;
		}

		cmd = fmt[1];
		fmt += 2;

		switch ( cmd ) {
		case 'i':
		case 'd':
		case 'u':
			**arg = _atoi( &buffer );
			break;
		case 'f':
			*(float *)*arg = _atof( &buffer );
			break;
		}
		arg++;
	}

	return count;
}

#endif
