// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "types.h"
#include "rpeltp.h"
#include "basicop.h"
#include "tables.h"
#include "gsm610fr.h"

/*
** Static variables are allocated as globals in order to make it
** possible to clear them in run time (reset codec). This might be
** useful e.g. in possible EC code
*/


/*
** void reset_encoder(CGSM610FR_Encoder* aEncoder)
**
** Function clears encoder variables.
** Input:
**   None
** Output:
**   Clear z1, L_z2, mp, LARpp_Prev[0..7], u[0..7], dp[0..119]
** Return value:
**   None
*/
void reset_encoder(CGSM610FR_Encoder* aEncoder)
{
  int i;

  aEncoder->z1 = 0;
  aEncoder->L_z2 = 0;
  aEncoder->mp = 0;

  for ( i = 0; i <= 7; i++ )
    aEncoder->LARpp_prev[i] = 0;
  for ( i = 0; i <= 7; i++ )
    aEncoder->u[i] = 0;
  for ( i = 0; i <= 119; i++ )
    aEncoder->dp[i] = 0;
}


/*
** void reset_decoder(CGSM610FR_Encoder* aDecoder)
**
** Function clears decoder variables.
** Input:
**   None
** Output:
**   Clear LARpp_Prev[0..7], v[0..8], drp[0..119], nrp
** Return value:
**   None
*/
void reset_decoder(CGSM610FR_Decoder* aDecoder)
{
  int i;

  for ( i = 0; i <= 7; i++ )
    aDecoder->LARrpp_prev[i] = 0;
  for ( i = 0; i <= 8; i++ )
    aDecoder->v[i] = 0;
  aDecoder->msr = 0;
  for ( i = 0; i <= 119; i++ )
    aDecoder->drp[i] = 0;
  aDecoder->nrp = 40;
}

/*
#  4.2.1. Downscaling of the input signal
#
#  4.2.2. Offset compensation
#
#  This part implements a high-pass filter and requires extended
#  arithmetic precision for the recursive part of this filter.
#
#  The input of this procedure is the array so[0..159] and the output
#  array sof[0..159].
#
#  Keep z1 and L_z2 in memory for the next frame.
#  Initial value: z1=0; L_z2=0;

@  Downscaling and offset compensation are combined in order to spare
@  unnecessary data moves.
*/

void prepr( CGSM610FR_Encoder* aEncoder, int2 sof[], int2 so[] )
{
  int k;

  int2 msp;
  int2 temp;
  int4 L_s2;
/*
# 4.2.1. Downscaling of the input signal
# |== FOR k=0 to 159:
# | so[k] = sop[k] >> 3;
# | so[k] = so[k] << 2;
# |== NEXT k:
*/

/*
# |== FOR k = 0 to 159:
# |Compute the non-recursive part.
# | s1 = sub( so[k], z1 );
# | z1 = so[k];
# |Compute the recursive part.
# | L_s2 = s1;
# | L_s2 = L_s2 << 15;
# |Execution of a 31 by 16 bits multiplication.
# | msp = L_z2 >> 15;
# | lsp = L_sub( L_z2, ( msp << 15 ) );
# | temp = mult_r( lsp, 32735 );
# | L_s2 = L_add( L_s2, temp );
# | L_z2 = L_add( L_mult( msp, 32735 ) >> 1, L_s2 );
# |Compute sof[k] with rounding.
# | sof[k] = L_add( L_z2, 16384 ) >> 15;
# |== NEXT k:
*/
  for (k=0; k <= 159; k++) {

    /* Downscaling */
    temp = shl( shr( so[k], 3 ), 2 );

    /* Compute the non-recursive part. */
    /* Compute the recursive part. */

    L_s2 = L_deposit_l( sub( temp, aEncoder->z1 ) );
    aEncoder->z1 = temp;


    L_s2 = L_shl( L_s2, 15 );
    /* Execution of a 31 by 16 bits multiplication. */
    msp = extract_l( L_shr( aEncoder->L_z2, 15 ) );
    temp = extract_l( L_sub( aEncoder->L_z2, L_shl( L_deposit_l( msp ), 15 ) ) );
    temp = mult_r( temp, 32735 );
    L_s2 = L_add( L_s2, L_deposit_l( temp ) );
    aEncoder->L_z2 = L_add( L_shr( L_mult( msp, 32735 ), 1 ), L_s2 );
    /* Compute sof[k] with rounding. */
    sof[k] = extract_l( L_shr( L_add( aEncoder->L_z2, (int4) 16384 ), 15 ) );
  }
}

/*
#  4.2.3. Preemphasis
#
#  Keep mp in memory for the next frame.
#  Initial value: mp=0;
*/
void preemp( CGSM610FR_Encoder* aEncoder, int2 s[], int2 sof[] )
{
  int k;
  int2 temp;
/*
# |== FOR k=0 to 159:
# | s[k] = add( sof[k], mult_r( mp, -28180 ) );
# | mp = sof[k];
# |== NEXT k:
*/

/*
@ Reverse looping in order to make it possible to
@ update filter delay mp only at the end of the loop
*/
  temp = sof[159]; /* make overwrite possible */

  for ( k = 159; k >= 1; k-- )
    s[k] = add( sof[k], mult_r( sof[k-1], -28180 ) );

  s[0] = add( sof[0], mult_r( aEncoder->mp, -28180 ) );

  aEncoder->mp = temp;
}

/*
#  4.2.4. Autocorrelation
#
#  The goal is to compute the array L_ACF[k]. The signal s[i] must be
#  scaled in order to avoid an overflow situation.
*
* output:
*       scalauto (return value)
*
*/
int2 autoc( int4 L_ACF[], int2 s[] )
{
  int k, i;

  int2 smax;
  int2 temp;
  int4 L_temp2;
  int2 scalauto;
/*
# Dynamic scaling of the array s[0..159].
#
# Search for the maximum.
#
# smax=0;
# |== FOR k = 0 to 159:
# | temp = abs( s[k] );
# | IF ( temp > smax ) THEN smax = temp;
# |== NEXT k;
*/
  smax = 0;
  for ( k = 0; k <= 159; k++ ) {
    temp = abs_s( s[k] );
    if ( sub( temp, smax ) > 0 )
      smax = temp;
  }
/*
# Computation of the scaling factor.
#
# IF ( smax == 0 ) THEN scalauto = 0;
#    ELSE scalauto = sub( 4, norm( smax << 16 ) );
*/
  if ( smax == 0 )
    scalauto = 0;
  else
    scalauto = sub( 4, norm_l( L_deposit_h( smax ) ) );
/* 
# Scaling of the array s[0..159].
# IF ( scalauto > 0 ) THEN
#    | temp = 16384 >> sub( scalauto, 1 );
#    |== FOR k=0 to 159:
#    |    s[k] = mult_r( s[k], temp );
#    |== NEXT k:
*/
  if ( scalauto > 0 ) {
    temp = shr( 16384, sub( scalauto, 1 ) );
    for ( k = 0; k <= 159; k++ ) 
      s[k] = mult_r( s[k], temp );
  }
/*
# Compute the L_ACF[..].
# |== FOR k=0 to 8:
# |     L_ACF[k] = 0;
# |==== FOR i=k to 159:
# |         L_temp = L_mult( s[i], s[i-k] );
# |         L_ACF[k] = L_add( L_ACF[k], L_temp );
# |==== NEXT i:
# |== NEXT k:
*/
  for ( k = 0; k <= 8; k++ ) {
    L_temp2 = 0;
    for ( i = k; i <= 159; i++ )
	L_temp2 = L_mac( L_temp2, s[i], s[i-k] );

    L_ACF[k] = L_temp2;
  }
/*
# Rescaling of the array s[0..159].
#
# IF ( scalauto > 0 ) THEN
#   |== FOR k = 0 to 159:
#   |    s[k] = s[k] << scalauto;
#   |== NEXT k:
*/
  if ( scalauto > 0 ) {
    for ( k = 0; k <= 159; k++ )
	 s[k] = shl( s[k], scalauto );
  }
  return(scalauto); /* scalauto is retuned to be used also in vad */
  
}


/*
#  4.2.5. Computation of the reflection coefficients
*/
void schur( int2 r[], int4 L_ACF[] )
{
  int k, i, n, m;

  int2 P[9];
  int2 K[7];
  int2 ACF[9];
  int2 normacf;

/*
# Schur recursion with 16 bits arithmetic
#
#    IF ( L_ACF[0] == 0 ) THEN
#                   |== FOR i=1 to 8:
#                   |    r[i] = 0;
#                   |== NEXT i:
#                   |    EXIT; / continue with section 4.2.6/
#    normacf = norm( L_ACF[0] ); / temp is spec replaced with normacf /
#    |== FOR k=0 to 8:
#    |    ACF[k] = ( L_ACF[k] << normacf ) >> 16;
#    |== NEXT k:
*/
  if ( L_ACF[0] == 0 ) {
    for ( i = 0; i <= 7; i++)
	 r[i] = 0;
    return; /* continue with section 4.2.6 */
  }
  normacf = norm_l( L_ACF[0] );

  for ( k = 0; k <= 8; k++ )
    ACF[k] = extract_h( L_shl( L_ACF[k], normacf ) );
/*
# Initialize array P[..] and K[..] for the recursion.
#
#    |== FOR i=1 to 7:
#    |    K[9-i] = ACF[i];
#    |== NEXT i:
#
#    |== FOR i=0 to 8:
#    |    P[i] = ACF[i];
#    |== NEXT i:
*/
  for ( i = 1; i <= 7; i++ )
    K[7-i] = ACF[i];

  for ( i = 0; i <= 8; i++ )
    P[i] = ACF[i];
/*
# Compute reflection coefficients
#    |== FOR n=1 to 8:
#    |    IF ( P[0] < abs( P[1] ) ) THEN
#    |                        |== FOR i=n to 8:
#    |                        |    r[i] = 0;
#    |                        |== NEXT i:
#    |                        |    EXIT; /continue with
#    |                        |    section 4.2.6./
#    |    r[n] = div( abs( P[1] ), P[0] );
#    |    IF ( P[1] > 0 ) THEN r[n] = sub( 0, r[n] );
#    |
#    |    IF ( n == 8 ) THEN EXIT; /continue with section 4.2.6/
#    |    Schur recursion
#    |    P[0] = add( P[0], mult_r( P[1], r[n] ) );
#    |==== FOR m=1 to 8-n:
#    |         P[m] = add( P[m+1], mult_r( K[9-m], r[n] ) );
#    |         K[9-m] = add( K[9-m], mult_r( P[m+1], r[n] ) );
#    |==== NEXT m:
#    |
#    |== NEXT n:
*/

  for ( n = 0; n <= 7; n++ )  {
    if ( sub( P[0], abs_s( P[1] ) ) < 0 ) {
	 for ( i = n; i <= 7; i++ )
	   r[i] = 0;
	 return; /* continue with section 4.2.6. */
    }

    if ( P[1] > 0 )
	 r[n] = negate( div_s( P[1], P[0] ) );
    else
	 r[n] = div_s( negate( P[1] ), P[0] );

    if ( sub(int2 (n), 7) == 0 )
	 return; /* continue with section 4.2.6 */

    /* Schur recursion */
    P[0] = add( P[0], mult_r( P[1], r[n] ) );
    for ( m = 1; m <= 7-n; m++ ) {
/*
 *    mac_r cannot be used because it rounds the result after
 *    addition when add( xx, mult_r ) rounds first the result
 *    of the product. That is why the following two lines cannot
 *    be used instead of the curently used lines.
 *
 *    P[m] = mac_r( L_deposit_l( P[m+1] ), K[7-m], r[n] );
 *    K[7-m] = mac_r( L_deposit_l( K[7-m] ), P[m+1], r[n] );
*/
      P[m] = add( P[m+1], mult_r( K[7-m], r[n] ) );
      K[7-m] = add( K[7-m], mult_r( P[m+1], r[n] ) );
    }
  }
}

/*
#  4.2.6. Transformation of reflection coefficients to Log.-Area Ratios -----
#
#  The following scaling for r[..] and LAR[..] has been used:
#
#  r[..] = integer( real_r[..]*32768. ); -1. <= real_r < 1.
#  LAR[..] = integer( real_LAR[..]*16384. );
#  with -1.625 <= real_LAR <= 1.625
*/

void larcomp( int2 LAR[], int2 r[] )
{
  int i;

  int2 temp;
/*
# Computation of the LAR[1..8] from the r[1..8]
#    |== FOR i=1 to 8:
#    |    temp = abs( r[i] );
#    |    IF ( temp < 22118 ) THEN temp = temp >> 1;
#    |         else if ( temp < 31130 ) THEN temp = sub( temp, 11059 );
#    |              else temp = sub( temp, 26112 ) << 2;
#    |    LAR[i] = temp;
#    |    IF ( r[i] < 0 ) THEN LAR[i] = sub( 0, LAR[i] );
#    |== NEXT i:
*/
  for ( i = 1; i <= 8; i++ ) {
	int j = i-1;
    temp = abs_s( r[j] );

    if ( sub( temp, 22118 ) < 0 )
      temp = shr( temp, 1 );
    else if ( sub( temp, 31130 ) < 0 )
      temp = sub( temp, 11059 );
    else
      temp = shl( sub( temp, 26112 ), 2 );

    if ( r[j] < 0 )
      LAR[j] = negate( temp );
    else
      LAR[j] = temp;
  }
}


/*
#  4.2.7. Quantization and coding of the Log.-Area Ratios
#
#  This procedure needs fpur tables; following equations give the
#  optimum scaling for the constants:
#
#  A[1..8]=integer( real_A[1..8]*1024 ); 8 values (see table 4.1)
#  B[1..8]=integer( real_B[1..8]*512 );  8 values (see table 4.1)
#  MAC[1..8]=maximum of the LARc[1..8];  8 values (see table 4.1)
#  MAC[1..8]=minimum of the LARc[1..8];  8 values (see table 4.1)
*/

void codlar( int2 LARc[], int2 LAR[] )
{

  int i;

  int2 temp;
/*
# Computation for quantizing and coding the LAR[1..8]
#
#    |== FOR i=1 to 8:
#    |    temp = mult( A[i], LAR[i] );
#    |    temp = add( temp, B[i] );
#    |    temp = add( temp, 256 );      for rounding
#    |    LARc[i] = temp >> 9;
#    |
#    | Check if LARc[i] lies between MIN and MAX
#    |    IF ( LARc[i] > MAC[i] ) THEN LARc[i] = MAC[i];
#    |    IF ( LARc[i] < MIC[i] ) THEN LARc[i] = MIC[i];
#    |    LARc[i] = sub( LARc[i], MIC[i] ); / See note below /
#    |== NEXT i:
#
# NOTE: The equation is used to make all the LARc[i] positive.
*/
  for ( i = 1; i <= 8; i++ ) {
	int j = i-1;
    temp = mult( A[j], LAR[j] );
    temp = add( temp, B[j] );
    temp = add( temp, 256 ); /* for rounding */
    temp = shr( temp, 9 );
    /* Check if LARc[i] lies between MIN and MAX */
    if ( sub( temp, MAC[j] ) > 0 )
      LARc[j] = sub( MAC[j], MIC[j] );
    else if ( sub( temp, MIC[j] ) < 0 )
      LARc[j] = 0;
    else
      LARc[j] = sub( temp, MIC[j] );
  }
}


/*
#  4.2.8 Decoding of the coded Log.-Area Ratios
#
#  This procedure requires for efficient implementation two variables.
#
#  INVA[1..8]=integer((32768*8)/(real_A[1..8]);    8 values (table 4.2)
#  MIC[1..8]=minimum value of the LARc[1..8];      8 values (table 4.2)
*/

void declar( int2 LARpp[], int2 LARc[] )
{
  int i;

  int2 temp1;
  int2 temp2;
/*
# Compute the LARpp[1..8].
#
#    |== FOR i=1 to 8:
#    |    temp1 = add( LARc[i], MIC[i] ) << 10; /See note below/
#    |    temp2 = B[i] << 1;
#    |    temp1 = sub( temp1, temp2 );
#    |    temp1 = mult_r( INVA[i], temp1 );
#    |    LARpp[i] = add( temp1, temp1 );
#    |== NEXT i:
#
# NOTE: The addition of MIC[i] is used to restore the sign of LARc[i].
*/
  for ( i = 1; i <= 8; i++ ) {
	int j = i-1;
    temp1 = shl( add( LARc[j], MIC[j] ), 10 );
    temp2 = shl( B[j], 1 );
    temp1 = sub( temp1, temp2 );
    temp1 = mult_r( INVA[j], temp1 );
    LARpp[j] = add( temp1, temp1 );
  }
}


/*
#  4.2.9. Computation of the quantized reflection coefficients
#
#  Within each frame of 160 anallyzed speech samples the short term
#  analysissss and synthesis filters operate with four different sets of
#  coefficients, derived from the previous set of decoded 
#  LARs(LARpp(j-1)) and the actual set of decoded LARs (LARpp(j)).
#
# 4.2.9.1 Interpolation of the LARpp[1..8] to get LARp[1..8]
*/

void cparc1( int2 LARp[], int2 LARpp_prev[], int2 LARpp[] )
{
  int i;

  int2 temp;
/*
#    FOR k_start=0 to k_end = 12.
#         
#    |==== FOR i=1 to 8:
#    |         LARp[i] = add( ( LARpp(j-1)[i] >> 2 ) ,( LARpp[i] >> 2 ) );
#    |         LARp[i] = add( LARp[i], ( LARpp(j-1)[i] >> 1 ) );
#    |==== NEXT i:
*/
  /* k_start=0 to k_end=12 */

  for ( i = 1; i <= 8; i++ ) {
	int j = i-1;
    temp = add( shr( LARpp_prev[j], 2 ), shr( LARpp[j], 2 ) );
    LARp[j] = add( temp, shr( LARpp_prev[j], 1 ) );
  }
}
 

void cparc2( int2 LARp[], int2 LARpp_prev[], int2 LARpp[] )
{
  int i;
  
/*
#    FOR k_start=13 to k_end = 26.
#    |==== FOR i=1 to 8:
#    |         LARp[i] = add( ( LARpp(j-1)[i] >> 1 ), ( LARpp[i] >> 1 ) );
#    |==== NEXT i:
*/
  /* k_start=13 to k_end=26 */
	
  for (i=1; i <= 8; i++) {
	int j = i-1;
    LARp[j] = add( shr( LARpp_prev[j], 1 ), shr( LARpp[j], 1 ) );
  }
}


void cparc3( int2 LARp[], int2 LARpp_prev[], int2 LARpp[] )
{
  int i;

  int2 temp;
  
/*
#    FOR k_start=27 to k_end = 39.
#    |==== FOR i=1 to 8:
#    |         LARp[i] = add( ( LARpp(j-1)[i] >> 2 ), ( LARpp[i] >> 2 ) );
#    |         LARp[i] = add( LARp[i], ( LARpp[i] >> 1 ) );
#    |==== NEXT i:
*/
  /* k_start=27 to k_end=39 */
	
  for ( i = 1; i <= 8; i++ ) {
	int j = i-1;
    temp = add( shr( LARpp_prev[j], 2 ), shr( LARpp[j], 2 ) );
    LARp[j] = add( temp, shr( LARpp[j], 1 ) );
  }
}


void cparc4( int2 LARp[], int2 LARpp_prev[], int2 LARpp[] )
{
  int i;
  
/*
#    FOR k_start=40 to k_end = 159.
#    |==== FOR i=1 to 8:
#    |         LARp[i] = LARpp[i];
#    |==== NEXT i:
*/
  /* k_start=40 to k_end=159 */
	
  for ( i = 1; i <= 8; i++ ) {
	int j = i-1;
    LARp[j] = LARpp[j];
    /* note new LARs saved here for next frame */
    LARpp_prev[j] = LARpp[j];
  }

}


/*
#  4.2.9.2 Computation of the rp[] from the interpolated LARp[]
#
#  The input of this procedure is the interpolated LARp[1..8] array. The
#  reflection coefficients, rp[i], are used in the analysis filter and in
#  the synthesis filter.
*/

void crp( int2 rp[], int2 LARp[] )
{
  int i;

  int2 temp;

/*
#    |== FOR i=1 to 8:
#    |    temp = abs( LARp[i] );
#    |    IF ( temp < 11059 ) THEN temp = temp << 1;
#    |         ELSE IF ( temp < 20070 ) THEN temp = add( temp, 11059 );
#    |              ELSE temp = add( ( temp >> 2 ), 26112 );
#    |    rp[i] = temp;
#    |    IF ( LARp[i] < 0 ) THEN rp[i] = sub( 0, rp[i] );
#    |== NEXT i:
*/
  for (i=1; i <= 8; i++) {
	int j = i-1;
    temp = abs_s( LARp[j] );
    if ( sub( temp, 11059 ) < 0 )
      temp = shl( temp, 1 );
    else if ( sub( temp, 20070 ) < 0 )
      temp = add( temp, 11059 );
    else
      temp = add( shr( temp, 2 ), 26112 );

    if ( LARp[j] < 0 )
      rp[j] = negate( temp );
    else
      rp[j] = temp;
  }
}


/*
#  4.2.10. Short term analysis filtering
#
#  This procedure computes the short term residual d[..] to be fed
#  to the RPE-LTP loop from s[..] signal and from the local rp[..]
#  array (quantized reflection coefficients). As the call of this
#  procedure can be done in many ways (see the interpolation of the LAR
#  coefficients), it is assumed that the computation begins with index
#  k_start (for arrays d[..] and s[..]) and stops with index k_end
#  (k_start and k_end are defined in 4.2.9.1): This procedure also need
#  to keep the array u[0..7] in memory for each call.
#
#  Keep the array u[0..7] in memory.
#  Initial value: u[0..7]=0;
*/

void invfil( CGSM610FR_Encoder* aEncoder, int2 d[], int2 s[], int2 rp[], int k_start, int k_end )
{
  //ALEX//extern int2 u[];

  int k, i;

  int2 temp;
  int2 sav;
  int2 di;
/*
#    |== FOR k=k_start to k_end:
#    |    di = s[k];
#    |    sav = di;
#    |==== FOR i=1 to 8:
#    |         temp = add( u[i], mult_r( rp[i], di ) );
#    |         di = add( di, mult_r( rp[i], u[i] ) );
#    |         u[i] = sav;
#    |         sav = temp;
#    |==== NEXT i:
#    |    d[k] = di;
#    |== NEXT k:
*/
  for ( k = k_start; k <= k_end; k++ ) {
    di = s[k];
    sav = di;
    for ( i = 1; i <= 8; i++ ) {
	  int j = i-1;
      temp = add( aEncoder->u[j], mult_r( rp[j], di ) );
      di = add( di, mult_r( rp[j], aEncoder->u[j] ) );
      aEncoder->u[j] = sav;
      sav = temp;
    }
    d[k] = di;
  }

}


/*
#  4.2.11. Calculation of the LTP parameters
#
#  This procedure computes the LTP gain (bc) and the LTP lag (Nc) for
#  the long term analysis filter. This is deone by calculating a maximum
#  of the cross-correlation function between the current sub-segment
#  short term residual signal d[0..39] (output of the short term 
#  analysis filter; for each sub-segment of the RPE-LTP analysis) and the
#  previous reconstructed short term residual signal dp[-120..-1]. A
#  dynamic scaling must be performed to avoid overflow.
# 
#  Initial value: dp[-120..-1]=0;
*/

void ltpcomp( CGSM610FR_Encoder* aEncoder, int2 *Nc, int2 *bc, int2 d[], int k_start )
{
  int k, i;

  int2 lambda;
  int2 temp;
  int2 scal;
  int2 dmax;
  int4 L_max;
  int2 wt[40]; /* scaled residual, original cannot be destroyed */
  int4 L_result;
  int4 L_power;
  int2 R;
  int2 S;
/*
# Search of optimum scaling of d[kstart+0..39]
#    dmax = 0;
#    |== FOR k=0 to 39:
#    |    temp = abs( d[k] );
#    |    IF ( temp > dmax ) THEN dmax = temp;
#    |== NEXT k:
*/
  dmax = 0;
  for (k=0; k <= 39; k++) {
    temp = abs_s( d[k+k_start] );
    if ( sub( temp, dmax ) > 0 )
      dmax = temp;
  }
/*
#    temp = 0;
#    IF ( dmax == 0 ) THEN scal = 0;
#         ELSE temp = norm( (long)dmax << 16 );
#    IF ( temp > 6 ) THEN scal = 0;
#         ELSE scal = sub( 6, temp );
*/  
  temp = 0;
  if ( dmax == 0 )
    scal = 0;
  else
    temp = norm_s( dmax );

  if ( sub( temp, 6 ) > 0 )
    scal = 0;
  else
    scal = sub( 6, temp );  /* 0 <= scal <= 6 */
/*
# Initialization of a working array wt[0..39]
#    |== FOR k=0 to 39:
#    |    wt[k] = d[k] >> scal;
#    |== NEXT k:
*/
  for (k=0; k <= 39; k++)
    wt[k] = shr( d[k+k_start], scal ); /* scal >= 0 */
/*
# Search for the maximum of crosscorrelation and coding of the LTP lag.
#    L_max = 0;
#    Nc = 40;
# 
#    |== FOR lambda=40 to 120:
#    |    L_result = 0;
#    |==== FOR k=0 to 39:
#    |         L_temp = L_mult( wt[k], dp[k-lambda] );
#    |         L_result = L_add( L_temp, L_result );
#    |==== NEXT k:
#    |    IF ( L_result > L_max ) THEN
#    |                                  |    Nc = lambda;
#    |                                  |    L_max = L_result;
#    |== NEXT lambda:
*/
  L_max = 0; /* 32 bits maximum */
  *Nc = 40; /* index for the maximum of cross correlation */
  for ( lambda = 40; lambda <= 120; lambda++ ) {
    L_result = 0;
    for (k = 0; k <= 39; k++)
	  L_result = L_mac( L_result, wt[k], aEncoder->dp[k-lambda+120] );
  /* Borland C++ 3.1 error if -3 (386-instructions) are used.
  ** The code makes error (compared to (L_result > L_max)
  ** comparison. The problem disapears if the result of L_sub
  ** is stored to variable, e.g.
  **   if ( ( L_debug = L_sub( L_result, L_max ) ) > 0 ) {
  **
  ** Problem does not occur when -2 option (only 286
  ** instructions are used)
  **
  ** The problem exist e.g. with GSM full rate test seq01.ib
  */
    if ( L_sub( L_result, L_max ) > 0 ) {
	 *Nc = lambda;
	 L_max = L_result;
    }
  }

/*
# Re-scaling of L-max
#    L_max = L_max >> sub( 6, scal );
*/
  L_max = L_shr( L_max, sub( 6, scal ) );
/*
# Initialization of a working array wt[0..39]
#    |== FOR k=0 to 39:
#    |    wt[k] = dp[k-Nc] >> 3;
#    |== NEXT k:
*/
  for (k = 0; k <= 39; k++)
    wt[k] = shr( aEncoder->dp[k - *Nc + 120], 3 );
/*
# Compute the power of the reconstructed short term residual signal dp[..]
#    L_power = 0;
#    |== FOR k=0 to 39:
#    |    L_temp = L_mult( wt[k], wt[k] );
#    |    L_power = L_add( L_temp, L_power );
#    |== NEXT k:
*/
  L_power = 0;
  for ( k = 0; k <= 39; k++ )
    L_power = L_mac( L_power, wt[k], wt[k] );
/*
# Normalization of L_max  and L_power
#    IF ( L_max <= 0 ) THEN
#                             | bc = 0;
#                             | EXIT; /cont. with 4.2.12/
*/
  if ( L_max <= 0 ) {
    *bc = 0;
    return;
  }
/*
#    IF ( L_max >= L_power ) THEN
#                             | bc = 3;
#                             | EXIT; /cont. with 4.2.12/
*/
  if ( L_sub( L_max, L_power ) >= 0 ) {
    *bc = 3;
    return;
  }
/*
#    temp = norm( L_power );
#    R = ( L_max << temp ) >> 16 );
#    S = ( L_power << temp ) >> 16 );
*/
  temp = norm_l( L_power );
  R = extract_h( L_shl( L_max, temp ) );
  S = extract_h( L_shl( L_power, temp ) );
/*
# Coding of the LTP gain
#
# Table 4.3a must be used to obtain the level DLB[i] for the
# quantization of the LTP gain b to get the coded version bc.
#
#    |== FOR bc=0 to 2:
#    |    IF ( R <= mult( S, DLB[bc] ) ) THEN EXIT; /cont. with 4.2.12/
#    |== NEXT bc:
#
#    bc = 3;
*/
  for ( i = 0; i <= 2; i++ ) {
    if ( sub( R, mult( S, DLB[i] ) ) <= 0 ) {
      *bc = int2 (i);
      return;
    }
  }

  *bc = 3;

}


/*
#  4.2.12. Long term analysis filtering
#
#  In this part, we have to decode the bc parameter to compute the
#  samples of the estimate dpp[0..39]. The decoding of bc needs the use
#  of table 4.3b. The long term residual signal e[0..39] is then
#  calculated to be fed to the RPE encoding section.
*/

void ltpfil( CGSM610FR_Encoder* aEncoder, int2 e[], int2 dpp[], int2 d[], int2 bc, int2 Nc, int k_start )
{
  int2 bp;
  int k;

/*
#    Decoding of the coded LTP gain.
#       bp = QLB[bc];
*/
	bp = QLB[bc];
/*
# Calculating the array e[0..39] and the array dpp[0..39]
#
#    |== FOR k=0 to 39:
#    |         dpp[k] = mult_r( bp, dp[k-Nc] );
#    |         e[k] = sub( d[k], dpp[k] );
#    |== NEXT k:
*/
  for ( k = 0; k <= 39; k++ ) {
    dpp[k] = mult_r( bp, aEncoder->dp[k - Nc + 120] );
    e[k] = sub( d[k+k_start], dpp[k] );
  }
}


/*
#  4.2.13. Weighting filter
#
#  The coefficients of teh weighting filter are stored in tables (see
#  table 4.4). The following scaling is used:
#
#  H[0..10] = integer( real_H[0..10]*8192 );
*/

void weight( int2 x[], int2 e[] )
{
  int k, i;

  int2 wt[50];
  int4 L_result;
/*
# Initialization of a temporary working array wt[0..49]
#    |== FOR k=0 to 4:
#    |    wt[k] = 0;
#    |== NEXT k:
#
#    |== FOR k=5 to 44:
#    |    wt[k] = e[k-5];
#    |== NEXT k:
#
#    |== FOR k=45 to 49:
#    |    wt[k] = 0;
#    |== NEXT k:
*/
  for ( k = 0; k <= 4; k++ ) 
    wt[k] = 0;
     
  for ( k = 5; k <= 44; k++ ) 
    wt[k] = e[k-5];
     
  for ( k = 45; k <= 49; k++ ) 
    wt[k] = 0;
/*
# Compute the signal x[0..39]
#    |== FOR k=0 to 39:
#    |    L_result = 8192;
#    |==== FOR i=0 to 10:
#    |         L_temp = L_mult( wt[k+i], H[i] );
#    |         L_result = L_add( L_result, L_temp );
#    |==== NEXT i:
#    |    L_result = L_add( L_result, L_result ); /scaling L_result (x2)/
#    |    L_result = L_add( L_result, L_result ); /scaling L_result (x4)/
#    |    x[k] = (int)( L_result >> 16 );
#    |== NEXT k:
*/
  for ( k = 0; k <= 39; k++ ) {
    L_result = L_deposit_l( 8192 );
    for ( i = 0; i <= 10; i++ )
      L_result = L_mac( L_result, wt[k+i], H[i] );

    /* scaling L_result (x4) and extract: scaling possible with new shift
     * because saturation is added L_shl
     *
     * L_result = L_add( L_result, L_result );
     * L_result = L_add( L_result, L_result );
     * x[k] = extract_h( L_result ); 
     @ Scaling can be done with L_shift because now shift has saturation
     */

    x[k] = extract_h( L_shl( L_result, 2 ) );
  }
}


/*
#  4.2.14. RPE grid selection
#
#  The signal x[0..39] is used to select the RPE grid which is
#  represented by Mc.
*/

int2 gridsel( int2 xM[], int2 x[] )
{
  int i, k;

  int2 temp1;
  int4 L_EM;
  int4 L_result;
  int2 Mc;
/*
#    EM = 0;
#    Mc = 0;
*/
  L_EM = 0;
  Mc = 0;
/*
#    |== FOR m=0 to 3:
#    |    L_result = 0;
#    |==== FOR k=0 to 12:
#    |         temp1 = x[i+(3*k)] >> 2;
#    |         L_temp = L_mult( temp1, temp1 );
#    |         L_result = L_add( L_temp, L_result );
#    |==== NEXT i:
#    |    IF ( L_result > L_max ) THEN
#    |                                  |    Mc = m;
#    |                                  |    EM = L_result;
#    |== NEXT m:
*/  
  for ( i = 0; i <= 3; i++ ) {
    L_result = 0;
    for ( k = 0; k <= 12; k++ ) {
      temp1 = shr( x[i+(3*k)], 2 );
      L_result = L_mac( L_result, temp1, temp1 );
    }
    if ( L_sub( L_result, L_EM ) > 0 ) {
      Mc = int2 (i);
      L_EM = L_result;
    }
  }
/*
# Down-sampling by factor 3 to get the selected xM[0..12] RPE sequence
#    |== FOR i=0 to 12:
#    |    xM[k] = x[Mc+(3*i)];
#    |== NEXT i:
*/
  for ( k = 0; k <= 12; k++ )
    xM[k] = x[Mc+(3*k)];

  return Mc;
}


/*
# Compute exponent and mantissa of the decoded version of xmaxc
#
# Part of APCM and (subrogram apcm() InvAPCM (iapcm())
*/

void expman( int2 *Exp, int2 *mant, int2 xmaxc )
{
  int i;
/*
# Compute exponent and mantissa of the decoded version of xmaxc.
#
#    exp = 0;
#    IF ( xmaxc > 15 ) THEN exp = sub( ( xmaxc >> 3 ), 1 );
#    mant = sub( xmaxc, ( exp << 3 ) );
*/
  *Exp = 0;
  if ( sub( xmaxc, 15 ) > 0 )
    *Exp = sub( shr( xmaxc, 3 ), 1 );
  
  *mant = sub( xmaxc, shl( *Exp, 3 ) );
/*
# Normalize mantissa 0 <= mant <= 7.
#    IF ( mant == 0 ) THEN    |    exp = -4;
#                             |    mant = 15 ;
#    ELSE | itest = 0;
#         |== FOR i=0 to 2:
#         |    IF ( mant > 7 ) THEN itest = 1;
#         |    IF ( itest == 0 ) THEN mant = add( ( mant << 1 ), 1 );
#         |    IF ( itest == 0 ) THEN exp = sub( exp, 1 );
#         |== NEXT i:
*/
  if ( *mant == 0 ) {
    *Exp = -4;
    *mant = 15 ;
  }
  else {
    for ( i = 0; i <= 2; i++ ) {
      if ( sub( *mant, 7 ) > 0 )
	break;
      else {
	*mant = add( shl( *mant, 1 ), 1 );
	*Exp = sub( *Exp, 1 );
      }
    }
  }
/*
#    mant = sub( mant, 8 );
*/
  *mant = sub( *mant, 8 );
}


int2 quantize_xmax( int2 xmax )
{
  int i;

  int2 Exp;
  int2 temp;
  int2 itest;
/*
# Quantizing and coding of xmax to get xmaxc.
#    exp = 0;
#    temp = xmax >> 9;
#    itest = 0;
#    |== FOR i=0 to 5:
#    |    IF ( temp <= 0 ) THEN itest = 1;
#    |    temp = temp >> 1;
#    |    IF ( itest == 0 ) THEN exp = add( exp, 1 )  ;
#    |== NEXT i:
*/
  Exp = 0;
  temp = shr( xmax, 9 );
  itest = 0;
  for ( i = 0; i <= 5; i++ ) {
    if ( temp <= 0 )
      itest = 1;
    temp = shr( temp, 1 );
    if ( itest == 0 )
      Exp = add( Exp, 1 )  ;
  }

/*
#    temp = add( exp, 5 );
#    xmaxc = add( ( xmax >> temp ), ( exp << 3 ) );
*/
  temp = add( Exp, 5 );

  return ( add( shr( xmax, temp ), shl( Exp, 3 ) ) ); /* xmaxc */

}


/*
#  4.2.15. APCM quantization of the selected RPE sequence
#
#  Keep in memory exp and mant for the following inverse APCM quantizer.
*
* return unquantzed xmax for SID computation
*/

int2 apcm( int2 *xmaxc, int2 xM[], int2 xMc[], int2 *Exp, int2 *mant )
{
  int k;

  int2 temp;
  int2 temp1;
  int2 temp2;
  int2 temp3;
  int2 xmax;
/*
# Find the maximum absolute value of xM[0..12].
#    xmax = 0;
#    |== FOR k=0 to 12:
#    |    temp = abs( xM[k] );
#    |    IF ( temp > xmax ) THEN xmax = temp;
#    |== NEXT i:
*/
  xmax = 0;
  for ( k = 0; k <= 12; k++ ) {
    temp = abs_s( xM[k] );
     if ( sub( temp, xmax ) > 0 )
       xmax = temp;
  }

  /*
   * quantization of xmax moved to function because it is used
   * also in comfort noise generation
   */
  *xmaxc = quantize_xmax( xmax );

  expman( Exp, mant, *xmaxc ); /* compute exp. and mant. */
/*
# Quantizing and coding of the xM[0..12] RPE sequence to get the xMc[0..12]
#
# This computation uses the fact that the decoded version of xmaxc can
# be calculated by using the exponent and mantissa part of xmaxc
# (logarithmic table).
#
# So, this method avoids any division and uses only scaling of the RPE
# samples by a function of the exponent. A direct multiplication by the
# inverse of the mantissa (NRFAC[0..7] found in table 4.5) gives the 3
# bit coded version xMc[0..12} of the RPE samples.
#
# Direct computation of xMc[0..12] using table 4.5.
#    temp1 = sub( 6, exp );   /normalization by the exponent/
#    temp2 = NRFAC[mant];     /see table 4.5 (inverse mantissa)/
#    |== FOR k=0 to 12:
#    |    xM[k] = xM[k] << temp1;
#    |    xM[k] = mult( xM[k], temp2 );
#    |    xMc[k] = add( ( xM[k] >> 12 ), 4 );     / See note below/
#    |== NEXT i:
#
# NOTE: This equation is used to make all the xMx[i] positive.
*/
  temp1 = sub( 6, *Exp );
  temp2 = NRFAC[*mant];

  for ( k = 0; k <= 12; k++ )  {
    temp3 = shl( xM[k], temp1 );
    temp3 = mult( temp3, temp2 );
    xMc[k] = add( shr( temp3, 12 ), 4 );
  }

  return xmax;
}

/*
#  4.2.16. APCM inverse quantization
#
#  This part is for decoding the RPE sequence of coded xMc[0..12] samples
#  to obtain the xMp[0..12] array. Table 4.6 is used to get the mantissa
#  of xmaxc (FAC[0..7]).
*/

void iapcm( int2 xMp[], int2 xMc[], int2 Exp, int2 mant )
{
  //ALEX//extern int2 FAC[];

  int k;

  int2 temp;
  int2 temp1;
  int2 temp2;
  int2 temp3;
/*
#    temp1 = FAC[mant];       /See 4.2.15 for mant/
#    temp2 = sub( 6, exp );   /See 4.2.15 for exp/
#    temp3 = 1 << sub( temp2, 1 );
*/
  temp1 = FAC[mant];
  temp2 = sub( 6, Exp );
  temp3 = shl( 1, sub( temp2, 1 ) );
/*
#    |== FOR k=0 to 12:
#    |    temp = sub( ( xMc[k] << 1 ), 7 );  /See note below/
#    |    temp = temp << 12;
#    |    temp = mult_r( temp1, temp );
#    |    temp = add( temp, temp3 );
#    |    xMp[k] = temp >> temp2;
#    |== NEXT i:
#
# NOTE: This subtraction is used to restore the sign of xMc[i].
*/
  for ( k = 0; k <= 12; k++ ) {
    temp = sub( shl( xMc[k], 1 ), 7 );
    temp = shl( temp, 12 );
    temp = mult_r( temp1, temp );
    temp = add( temp, temp3 );
    xMp[k] = shr( temp, temp2 );
  }
}

/*
#  4.2.17. RPE grid positioning
#
#  This procedure computes the reconstructed long term residual signal
#  ep[0..39] for the LTP analysis filter. The inputs are the Mc which is
#  the grid position selection and the xMp[0..12] decoded RPE samples
#  which are upsampled by factor of 3 by inserting zero values.
*/

void gridpos( int2 ep[], int2 xMp[], int2 Mc )
{
  int k;
/*
#    |== FOR k=0 to 39:
#    |    ep[k] = 0;
#    |== NEXT k:
*/
  for ( k = 0; k <= 39; k++ ) 
    ep[k] = 0;
/*
#    |== FOR i=0 to 12:
#    |    ep[Mc + (3*k)] = xMp[k];
#    |== NEXT i:
*/
  for ( k = 0; k <= 12; k++ ) 
    ep[Mc + (3*k)] = xMp[k];
}


/*
#  4.2.18. Update of the reconstructed short term residual signal dp[]
#
#  Keep the array dp[-120..-1] in memory for the next sub-segment.
#  Initial value: dp[-120..-1]=0;
*/

void ltpupd( CGSM610FR_Encoder* aEncoder, int2 dpp[], int2 ep[] )
{
  int i;
/*
#    |== FOR k=0 to 79:
#    |    dp[-120+k] = dp[-80+k];
#    |== NEXT k:
*/
  for (i = 0; i <= 79; i++) 
    aEncoder->dp[-120+i+120] = aEncoder->dp[-80+i+120];
/*
#    |== FOR k=0 to 39:
#    |    dp[-40+k] = add( ep[k], dpp[k] );
#    |== NEXT k:
*/
  for (i = 0; i <= 39; i++) 
    aEncoder->dp[-40+i+120] = add( ep[i], dpp[i] );
}


/*
#  4.3.2. Long term synthesis filtering
#
#  Keep the nrp value for the next sub-segment.
#  Initial value: nrp=40;
#
#  Keep the array drp[-120..-1] for the next sub-segment.
#  Initial value: drp[-120..-1]=0;
*/

void ltpsyn( CGSM610FR_Decoder* aDecoder, int2 erp[], int2 wt[], int2 bcr, int2 Ncr )
{
  int k, i;

  int2 drpp;
  int2 Nr;
  int2 brp;
/*
# Check the limits of Nr
#    Nr = Ncr;
#    IF ( Ncr < 40 ) THEN Nr = nrp;
#    IF ( Ncr > 120 ) THEN Nr = nrp;
#    nrp = Nr;
*/
  if ( sub( Ncr, 40 ) < 0 )
    Nr = aDecoder->nrp;
  else if ( sub( Ncr, 120 ) > 0 )
    Nr = aDecoder->nrp;
  else
    Nr = Ncr;

  aDecoder->nrp = Nr;

/*
# Decoding of the LTP gain bcr.
#    brp = QLB[bcr];
*/
  brp = QLB[bcr];
/*
# Computation of the reconstructed short term residual signal drp[0..39].
#    |== FOR k=0 to 39:
#    |    drpp = mult_r( brp, drp[k-Nr] );
#    |    drp[k+120] = add( erp[k], drpp );
#    |== NEXT k:
*/
  for ( k = 0; k <= 39; k++ ) { 
    drpp = mult_r( brp, aDecoder->drp[k-Nr+120] );
    wt[k] = add( erp[k], drpp );
  }
/*
# Update of the reconstructed short term residual signal drp[-1..-120]
#    |== FOR k=0 to 119:
#    |    drp[-120+k] = drp[-80+k];
#    |== NEXT k:
*/

  for ( i = 0; i < 80; i++ )
    aDecoder->drp[i] = aDecoder->drp[40+i];

  for ( i = 0; i < 40; i++ )
    aDecoder->drp[i+80] = wt[i];
}


/*
#  4.3.4. Short term synthesis filtering section
#
#  This procedure uses the drp[0..39] signal and produces the sr[0..159]
#  signal which is the output of the short term synthesis filter. For
#  ease of explanation, a temporary array wt[0..159] is used.
#
#  Initialization of the array wt[0..159].
#
#  For the first sub-segment in a frame:
#    |== FOR k=0 to 39:
#    |    wt[k] = drp[k];
#    |== NEXT k:
#
#  For the second sub-segment in a frame:
#    |== FOR k=0 to 39:
#    |    wt[40+k] = drp[k];
#    |== NEXT k:
#
#  For the third sub-segment in a frame:
#    |== FOR k=0 to 39:
#    |    wt[80+k] = drp[k];
#    |== NEXT k:
#
#  For the fourth sub-segment in a frame:
#    |== FOR k=0 to 39:
#    |    wt[120+k] = drp[k];
#    |== NEXT k:
#
#  As the call of the short term synthesis filter procedure can be done
#  in many ways (see the interpolation of the LAR coefficient), it is
#  assumed that the computation begins with index k_start (for arrays
#  wt[..] and sr[..]) and stops with index k_end (k_start and k_end are
#  defined in 4.2.9.1). The procedure also needs to keep the array
#  v[0..8] in memory between calls.
#
#  Keep the array v[0..8] in memory for the next call.
#  Initial value: v[0..8]=0;
*/

void synfil( CGSM610FR_Decoder* aDecoder, int2 sr[], int2 wt[], int2 rrp[], int k_start, int k_end )
{
  int k;
  int i;

  int2 sri;
/*
#    |== FOR k=k_start to k_end:
#    |    sri = wt[k];
#    |==== FOR i=1 to 8:
#    |         sri = sub( sri, mult_r( rrp[9-i], v[8-i] ) );
#    |         v[9-i] = add( v[8-i], mult_r( rrp[9-i], sri ) ) ;
#    |==== NEXT i:
#    |    sr[k] = sri;
#    |    v[0] = sri;
#    |== NEXT k:
*/
  for ( k = k_start; k <= k_end; k++ ) {
    sri = wt[k];
    for ( i = 1; i <= 8; i++ ) {
	  int j = i+1;
      sri = sub( sri, mult_r( rrp[9-j], aDecoder->v[8-i] ) );
      aDecoder->v[9-i] = add( aDecoder->v[8-i], mult_r( rrp[9-j], sri ) ) ;
    }
    sr[k] = sri;
    aDecoder->v[0] = sri;
  }

}


/*
** 4.3.5., 4.3.6., 4.3.7. Postprocessing
**
** Combined deemphasis, upscaling and truncation
*/
void postpr( CGSM610FR_Decoder* aDecoder, int2 srop[], int2 sr[] )
{
  int k;
/*
# 4.3.5. Deemphasis filtering
#
# Keep msr in memory for the next frame.
# Initial value: msr=0;
*/
/*
#    |== FOR k=0 to 159:
#    |    temp = add( sr[k], mult_r( msr, 28180 ) );
#    |    msr = temp;
#    |    sro[k] = msr;
#    |== NEXT k:
*/
/*
# 4.3.6 Upscaling of the output signal
*/
/*
#    |== FOR k=0 to 159:
#    |    srop[k] = add( sro[k], sro[k] );
#    |== NEXT k:
*/
/*
# 4.3.7. Truncation of the output variable
*/
/*
#    |== FOR k=0 to 159:
#    |    srop[k] = srop[k] >> 3;
#    |    srop[k] = srop[k] << 3;
#    |== NEXT k:
*/

  for ( k = 0; k <= 159; k++ ) {
    aDecoder->msr = add( sr[k], mult_r( aDecoder->msr, 28180 ) );
    srop[k] = int2 (shl( aDecoder->msr, 1 ) & 0xfff8);
  }
}

