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

#include "rpeltp.h"
#include "codec.h"
#include "gsm610fr.h"

/*
** LPC analysis part of the RPE-LTP-coder
**
** Input:
**      sop[0..159]
**              unprocessed sample frame 
** Output:
**      d[0..159]
**              residual 
**      LARc[0..7]
**              coded reflection coefficients 
**      *SP_flag:
**              decision of tx DTX (boolean)
**      *VAD_flag:
**              decision of VAD (boolean)
**
** Return value:
**      None
*/
void LPC_analysis(CGSM610FR_Encoder* aEncoder, int2 ibuf[], struct codes *ecodes)
{

  int4 L_ACF[9];
  int2 LAR[8]; /* used for r[], LAR[], LARpp */
  int2 rp[8]; /* used for LARp[], rp[] */
//  int2 scalauto; /* returned from autoc to be used by vad */


  prepr( aEncoder, ibuf, ibuf ); /* sof <- so */
  preemp( aEncoder, ibuf, ibuf ); /* s <- sof */
//  scalauto = autoc( L_ACF, ibuf ); /* L_ACF <- , s <- s */
  autoc( L_ACF, ibuf ); /* L_ACF <- , s <- s */

  /*
  ** VAD decision could be computed here when L_ACF and scalauto are available.
  ** In current version vad is executed after LAR computation
  */

  schur( LAR, L_ACF ); /* r <- L_ACF */

  larcomp( LAR, LAR ); /* LAR <- r */

  codlar( ecodes->LARc, LAR ); /* LARc <- LAR */
  declar( LAR, ecodes->LARc ); /* LARpp <- LARc */

  cparc1( rp, aEncoder->LARpp_prev, LAR ); /* LARp <- LARpp_prev, LARpp */
  crp( rp, rp ); /* rp <- LARp */
  invfil( aEncoder, ibuf, ibuf, rp, 0, 12 ); /* d <- s */

  cparc2( rp, aEncoder->LARpp_prev, LAR ); /* LARp <- LARpp_prev, LARpp */
  crp( rp, rp ); /* rp <- LARp */
  invfil( aEncoder, ibuf, ibuf, rp, 13, 26 ); /* d <- s */

  cparc3( rp, aEncoder->LARpp_prev, LAR ); /* LARp <- LARpp_prev, LARpp */
  crp( rp, rp ); /* rp <- LARp */
  invfil( aEncoder, ibuf, ibuf, rp, 27, 39 ); /* d <- s*/

  cparc4( rp, aEncoder->LARpp_prev, LAR ); /* LARp <- LARpp_prev, LARpp */
  crp( rp, rp ); /* rp <- LARp */
  invfil( aEncoder, ibuf, ibuf, rp, 40, 159 ); /* d <- s */
  return;
}

/*
** Encoding of the residual signal of the LPC analysis filter
** Input:
**
** d[k_start..k_start+39]
**      LPC residual (output of LPC analysis filter)
**
** Output:
**
** bc, Nc
**      encoded LTP parameters (gain and lag)
** xmaxc
**      block maximum of the encoded subframe.
** xMc[0..12]
**              coded normalized RPE pulses
**
** return xmax for SID computation
*/
int2 residual_encoder( CGSM610FR_Encoder* aEncoder, int sf_nro, int2 d[], struct sfcodes *sfc )
{
  int k_start;
  int2 xmax; /* return value */

  /* Note: d[] is used also for for x[] and e[] */

  int2 xM[13]; /* used for xM[], xMp[] */
  /* xM[] is required simultaneously with xMc because LTP
   * also decodes xMc
   */
  int2 dpp[40]; /* required simultaneously with ep[] that is
		 * stored into d[]
		 */
  int2 Exp;
  int2 mant;

  k_start = sf_nro * 40;

  ltpcomp( aEncoder, &(sfc->Nc), &(sfc->bc), d, k_start );
  ltpfil( aEncoder, &d[k_start], dpp, d, sfc->bc, sfc->Nc, k_start ); /* e, dpp <- d */
  weight( &d[k_start], &d[k_start] ); /* x <- e */
  sfc->Mc = gridsel( xM, &d[k_start] ); /* xM <- x */

  /*
  ** quatize residual and store unquantized xmax for SID
  ** computation
  */
  xmax = apcm( &(sfc->xmaxc), xM, sfc->xMc, &Exp, &mant);
  /* EXP, mant computed int APCM */

  iapcm( xM, sfc->xMc, Exp, mant ); /* xMp <- xMc */
  gridpos( &d[k_start], xM, sfc->Mc ); /* ep <- xMc,Mc */
  ltpupd( aEncoder, dpp, &d[k_start] ); /* dp <- dpp, x */

  return( xmax );
}


/*
** Decoding of the coded LPC-residual
**
** Input:
** xmaxcr
**      coded block maxmimum
** xMcr[0..12]
**      coded normalized RPE pulses
**
** Output:
** drp[k_start..k_start+39]
**      decoded LPC residual (input signal for LPC-synthesis filter)
*/
void residual_decoder(CGSM610FR_Decoder* aDecoder, int sf_nro, struct sfcodes *sfc, int2 wt[])
{
  int k_start;

  int2 EXP;
  int2 mant;
  int2 xMrp[13];
  int2 erp[40];

  k_start = sf_nro * 40;

  /* in decoder EXP ja mant must be computed from xmaxcr */
  expman( &EXP, &mant, sfc->xmaxc ); /* EXP, mant <- xmaxc */
  iapcm( xMrp, sfc->xMc, EXP, mant ); /* xMrp <- xMc */
  gridpos( erp, xMrp, sfc->Mc ); /* erp <- xMc,Mc */
	
  ltpsyn( aDecoder, erp, &wt[k_start], sfc->bc, sfc->Nc );
}

/*
** LPC synthesis part of the RPE-LTP-coder
**
** Input:
**      LARcr[0..7]
**              coded reflection coefficients
**      wt[0..159]
**              decoded residual
**
** Output:
**      srop[0..159]
**              decoded speech
*/
void LPC_synthesis(CGSM610FR_Decoder* aDecoder, struct codes *dcodes, int2 wt[], int2 obuf[])
{
  int2 LARr[8]; /* used for LARr[], LARpp */
  int2 rrp[8]; /* used for LARp[], rrp[] */

  declar(LARr, dcodes->LARc); /* LARrpp <- LARc */

  cparc1( rrp, aDecoder->LARrpp_prev, LARr ); /* LARp <- LARrpp_prev, LARr */
  crp( rrp, rrp ); /* rrp <- LARp */
  synfil( aDecoder, obuf, wt, rrp, 0, 12 ); /* sr <- wt */

  cparc2( rrp, aDecoder->LARrpp_prev, LARr ); /* LARp <- LARrpp_prev, LARr */
  crp( rrp, rrp ); /* rrp <- LARp */
  synfil( aDecoder, obuf, wt, rrp, 13, 26 ); /* sr <- wt */

  cparc3( rrp, aDecoder->LARrpp_prev, LARr ); /* LARp <- LARrpp_prev, LARr */
  crp( rrp, rrp ); /* rrp <- LARp */
  synfil( aDecoder, obuf, wt, rrp, 27, 39 ); /* sr <- wt */

  cparc4( rrp, aDecoder->LARrpp_prev, LARr ); /* LARp <- LARrpp_prev, LARr */
  crp( rrp, rrp ); /* rrp <- LARp */
  synfil( aDecoder, obuf, wt, rrp, 40, 159 ); /* sr <- wt */
  postpr( aDecoder, obuf, obuf );
  /* combines deemphasis, upscaling and truncation */

}


/*
** RPE-LTP Encoder
**
** void RPELTP_encoder(CGSM610FR_Encoder* aEncoder, int2 ibuf[], struct codes *ecodes)
**
** Input:
**      ibuf[0..159]
**              Original speech to be coded
**
** Output:
**      ecodes
**              encoded speech stored as codewords
*/
void RPELTP_encoder(CGSM610FR_Encoder* aEncoder, int2 ibuf[], struct codes *ecodes)
{
  int i;
//  int2 xmax[4]; /* collect unquantized xmax data for dtx */	

  LPC_analysis( aEncoder, ibuf, ecodes );
  for (i = 0; i < 4; i++)
//    xmax[i] = residual_encoder( aEncoder, i, ibuf, &(ecodes->sfc[i]) );	
    residual_encoder( aEncoder, i, ibuf, &(ecodes->sfc[i]) );
}


/*
** RPE-LTP Decoder
**
** void RPELTP_decoder(struct codes *dcodes, int2 obuf[])
** Input:
**      dcodes
**              encoded speech stored as codewords to be decoded
**
** Output:
**      obuf[0..159]
**              Decoded speech
*/
void RPELTP_decoder(CGSM610FR_Decoder* aDecoder, struct codes *dcodes, int2 obuf[])
{
  int i;

  dcodes->LARc[0] &= 0x7fff; /* VAD flag in sequences */
  dcodes->LARc[1] &= 0x7fff; /* SP flag in sequences */

  for (i = 0; i < 4; i++)
    residual_decoder(aDecoder, i, &(dcodes->sfc[i]), obuf); /* -> wt */

  LPC_synthesis(aDecoder, dcodes, obuf, obuf); /* wt -> srop */
}

