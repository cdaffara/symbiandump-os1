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

#ifndef __GSMAUDIO_H__
#define __GSMAUDIO_H__


//*******************************************************************
//* INCLUDE FILES:
//*******************************************************************

// Standard EPOC32 includes
#include <e32base.h>
// Public Media Server includes
#include <mda/common/base.h>
#include <mda/common/resource.h>
#include <mda/common/controller.h>
#include <mda/common/audio.hrh>
#include <mda/common/gsmaudio.hrh>


//*******************************************************************
//* CONSTANTS:
//*******************************************************************

// Wav Codec Uids
/**
@publishedAll
@released
Uid value for GSM610 codec
*/
const TUint KUidMdaGsmWavCodecValue = KUidMdaGsmWavCodecDefine;
/**
@publishedAll
@released
Uid for GSM610 codec
*/
const TUid  KUidMdaGsmWavCodec      = { KUidMdaGsmWavCodecValue };

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mda/common/mdagsmwavcodec.h>
#endif
#endif	// __GSMAUDIO_H__
