// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This is a test codec which takes MP3 data & gives it back to the datapath
// pretending it has converted it to PCM16 data thus producing lots of noise
// 
//

#ifndef __MMF_MP3_CODEC_H__
#define __MMF_MP3_CODEC_H__

#include <mmf/server/mmfcodec.h>
#include <mmf/server/mmfdatabuffer.h>

class CMMFMP3To16PcmCodec : public CMMFCodec
	{
public:
	static CMMFCodec* NewL(TAny* aInitParams);
	virtual ~CMMFMP3To16PcmCodec();
	TCodecProcessResult ProcessL(const CMMFBuffer& aSrc, CMMFBuffer& aDst);
	virtual void ResetL();

private:
	CMMFMP3To16PcmCodec();
	void ConstructL(TAny* aInitParams);
	};

#endif

