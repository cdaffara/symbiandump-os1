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

#ifndef __GSM610FR__
#define __GSM610FR__

//  INCLUDES
#include <e32base.h>

#include "rpeltp.h"

// DEFINES
#define PACKSIZE 65 // Size of one bit packed block (2 frames)

//  CLASS DEFINITIONS
/*
-----------------------------------------------------------------------------

    CGSM610FR_Encoder

    GSM 6.10 full rate (FR) speech encoder.

-----------------------------------------------------------------------------
*/
class CGSM610FR_Encoder
    {
public:
    IMPORT_C void ConstructL();
    IMPORT_C ~CGSM610FR_Encoder();
    IMPORT_C void StartL();
    IMPORT_C void ExecuteL(TUint8* aInBuf, TUint8* OutBuf);
    IMPORT_C void StopL();
private:
    void PackFrame0(struct codes* aCodeBuf, TInt8* pbuf);
    void PackFrame1(struct codes* aCodeBuf, TInt8* pbuf);
private:
    struct codes iCodeBuf;       // buffer for speech codewords, one frame
    TBool iOddFrame;              // is the current frame odd or even frame?
private:
    friend void reset_encoder(CGSM610FR_Encoder* aEncoder);
    friend void prepr(CGSM610FR_Encoder* aEncoder, int2 sof[], int2 so[]);
    friend void preemp(CGSM610FR_Encoder* aEncoder, int2 s[], int2 sof[]);
    friend void ltpcomp(CGSM610FR_Encoder* aEncoder, int2 *Nc, int2 *bc, int2 d[], int k_start);
    friend void ltpfil(CGSM610FR_Encoder* aEncoder, int2 e[], int2 dpp[], int2 d[], int2 bc, int2 Nc, int k_start);
    friend void ltpupd( CGSM610FR_Encoder* aEncoder, int2 dpp[], int2 ep[] );
    friend void LPC_analysis(CGSM610FR_Encoder* aEncoder, int2 ibuf[], struct codes *ecodes);
    friend void invfil(CGSM610FR_Encoder* aEncoder, int2 d[], int2 s[], int2 rp[], int k_start, int k_end);
private:
    int2 z1;            /* delay 1 of offset comp */
    int4 L_z2;          /* delay 2 of offset comp */
    int2 mp;            /* delay of preemphasis */
    int2 u[8];          /* LPC analysis filter delay */
    int2 dp[120];       /* Encoder LTP delay */
    int2 LARpp_prev[8]; /* Encoder LARs of the prev frame */

    };


/*
-----------------------------------------------------------------------------

    CGSM610FR_Decoder

    GSM 6.10 full rate (FR) speech decoder.

-----------------------------------------------------------------------------
*/
class CGSM610FR_Decoder
    {
public:
   IMPORT_C void ConstructL();
   IMPORT_C ~CGSM610FR_Decoder();
   IMPORT_C void StartL();
   IMPORT_C void ExecuteL(TUint8* aSrc, TUint8* aDst);
   IMPORT_C void StopL();
private:
    void UnpackFrame0(struct codes* aCodeBuf, TInt8* pbuf);
    void UnpackFrame1(struct codes* aCodeBuf, TInt8* pbuf);
private:
    struct codes iCodeBuf;       // buffer for speech codewords, one frame
    TBool iOddFrame;             // is the current frame odd or even frame?
private:
    friend void reset_decoder(CGSM610FR_Decoder* aDecoder);
    friend void ltpsyn(CGSM610FR_Decoder* aDecoder, int2 erp[], int2 wt[], int2 bcr, int2 Ncr);
    friend void postpr(CGSM610FR_Decoder* aDecoder, int2 srop[], int2 sr[]);
    friend void LPC_synthesis(CGSM610FR_Decoder* aDecoder, struct codes *dcodes, int2 wt[], int2 obuf[]);
    friend void synfil(CGSM610FR_Decoder* aDecoder, int2 sr[], int2 wt[], int2 rrp[], int k_start, int k_end);
private:
    int2 msr;            /* de emphasis delay */
    int2 v[9];           /* LPC synthesis filter delay */
    int2 drp[120];       /* Decoder LTP delay */
    int2 nrp;            /* LTP delay of the previous frame */
    int2 LARrpp_prev[8]; /* Decoder LARs of the prev frame */
    };


#endif //__GSM610FR__

//-----------------------------------------------------------------------------
//  End of File
//-----------------------------------------------------------------------------
