// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef _RPELTP_H
#define _RPELTP_H

#include "types.h"

const TInt KFrameSize = 160;
const TInt KSubFrameSize = 40;

#define FRAMESIZE KFrameSize   /* samples in speech frame */
#define SUBFRAMESIZE KSubFrameSize /* samples in subframe */

class CGSM610FR_Encoder;
class CGSM610FR_Decoder;

void reset_encoder( CGSM610FR_Encoder* aEncoder );
void reset_decoder( CGSM610FR_Decoder* aDecoder );

void prepr( CGSM610FR_Encoder* aEncoder, int2 sof[], int2 so[] );
void preemp( CGSM610FR_Encoder* aEncoder, int2 s[], int2 sof[] );
int2 autoc( int4 L_ACF[], int2 s[] ); /* scalauto is returned */
void schur( int2 r[], int4 L_ACF[] );
void larcomp( int2 LAR[], int2 r[] );
void codlar( int2 LARc[], int2 LAR[] );
void declar( int2 LARpp[], int2 LARc[] );
void cparc1( int2 LARp[], int2 LARpp_prev[], int2 LARpp[] );
void cparc2( int2 LARp[], int2 LARpp_prev[], int2 LARpp[] );
void cparc3( int2 LARp[], int2 LARpp_prev[], int2 LARpp[] );
void cparc4( int2 LARp[], int2 LARpp_prev[], int2 LARpp[] );
void crp( int2 rp[], int2 LARp[] );
void invfil( CGSM610FR_Encoder* aEncoder, int2 d[], int2 s[], int2 rp[], int k_start, int k_end );
void ltpcomp( CGSM610FR_Encoder* aEncoder, int2 *Nc, int2 *bc, int2 d[], int k_start );
void ltpfil( CGSM610FR_Encoder* aEncoder, int2 e[], int2 dpp[], int2 d[], int2 bc, int2 Nc, int k_start );
void weight( int2 x[], int2 e[] );
int2 gridsel( int2 xM[], int2 x[] );
void expman( int2 *EXP, int2 *mant, int2 xmaxc );
int2 quantize_xmax( int2 xmax );
int2 apcm( int2 *xmaxc, int2 xMc[], int2 xM[], int2 *EXP, int2 *mant );
void iapcm( int2 xMp[], int2 xMc[], int2 EXP, int2 mant );
void gridpos( int2 ep[], int2 xMp[], int2 Mc );
void ltpupd( CGSM610FR_Encoder* aEncoder, int2 dpp[], int2 ep[] );
void ltpsyn( CGSM610FR_Decoder* aDecoder, int2 erp[], int2 wt[], int2 bcr, int2 Ncr );
void synfil( CGSM610FR_Decoder* aDecoder, int2 sr[], int2 wt[], int2 rrp[], int k_start, int k_end );

void postpr( CGSM610FR_Decoder* aDecoder, int2 srop[], int2 sr[] );

#endif

