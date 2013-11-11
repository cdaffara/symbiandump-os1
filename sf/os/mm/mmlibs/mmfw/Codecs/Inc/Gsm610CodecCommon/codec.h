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

#ifndef _CODEC_H
#define _CODEC_H

#include "types.h"

class CGSM610FR_Encoder;
class CGSM610FR_Decoder;

void LPC_analysis( CGSM610FR_Encoder* aEncoder, int2 sop[], struct codes *ecodes );
int2 residual_encoder( CGSM610FR_Encoder* aEncoder, int sf_nro, int2 d[], struct sfcodes *sfc );
void residual_decoder( CGSM610FR_Decoder* aDecoder, int sf_nro, struct sfcodes *sfc, int2 obuf[] );
void LPC_synthesis( CGSM610FR_Decoder* aDecoder, struct codes *dcodes, int2 wt[], int2 obuf[] );
void RPELTP_encoder( CGSM610FR_Encoder* aEncoder, int2 ibuf[], struct codes *ecodes );
void RPELTP_decoder( CGSM610FR_Decoder* aDecoder, struct codes *dcodes, int2 obuf[] );

#endif
