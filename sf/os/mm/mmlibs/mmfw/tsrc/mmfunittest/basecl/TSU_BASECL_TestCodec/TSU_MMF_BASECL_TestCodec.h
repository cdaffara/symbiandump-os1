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

#ifndef __TSU_MMF_MMFBASECL_TESTCODEC_H__
#define __TSU_MMF_MMFBASECL_TESTCODEC_H__

#include <mmf/server/mmfcodec.h>

/**
*
* CMMF_TSU_BASECL_TestCodec
*
* Test codec used by the TSU_MMF_BASECL unit test harness
* Codec converts upper case ascii to lower case ascii
*
*/
class CMMF_TSU_BASECL_TestCodec : public CMMFCodec
	{
public:
	static CMMFCodec* NewL(TAny* aInitParams);
	virtual ~CMMF_TSU_BASECL_TestCodec();
	TCodecProcessResult ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst);
private:
	CMMF_TSU_BASECL_TestCodec();
	void ConstructL(TAny* aInitParams);
private:
	const CMMFDataBuffer* iSrc;
	CMMFDataBuffer* iDst;
	};

#endif 
