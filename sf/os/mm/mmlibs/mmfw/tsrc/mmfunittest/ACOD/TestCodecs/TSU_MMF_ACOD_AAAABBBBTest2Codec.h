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

#ifndef __TSU_MMF_ACOD_AAAABBBBTEST2CODEC_H__
#define __TSU_MMF_ACOD_AAAABBBBTEST2CODEC_H__

#include <mmf/server/mmfcodec.h>

/** @xxxx
 * @class CMMFAAAABBBBTest2Codec TSU_MMF_ACOD_AAAABBBBTest2Codec.h "TestCodecs/TSU_MMF_ACOD_AAAABBBBTest2Codec.h"
 * @brief Test codec for use in preferred supplier test. 
 * This codec takes the same fourCC codes as CMMFAAAABBBBTestCodec.
 */
class CMMFAAAABBBBTest2Codec : public CMMFCodec
	{
public:
	static CMMFCodec* NewL(TAny* aInitParams);
	virtual ~CMMFAAAABBBBTest2Codec();
	TCodecProcessResult ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst);
private:
	CMMFAAAABBBBTest2Codec();
	void ConstructL(TAny* aInitParams);
private:
	//owned by the Datapath
	const CMMFDataBuffer* iSrc;
	CMMFDataBuffer* iDst;
	};

#endif 
