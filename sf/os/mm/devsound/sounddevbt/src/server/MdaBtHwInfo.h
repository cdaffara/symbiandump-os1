// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __MDABTHWINFO_H__
#define __MDABTHWINFO_H__

#include <e32base.h>
#include "mdasound.h"

class CMdaHwInfo : public CBase 
	{

public:
	static CMdaHwInfo* NewL();
	~CMdaHwInfo();
	inline RMdaDevSound::TSoundFormatsSupportedBuf& GetPlayFormatsSupported() {return iPlayFormatsSupported;}
	inline RMdaDevSound::TCurrentSoundFormatBuf& GetPlayFormat() {return iPlayFormat;}
	inline RMdaDevSound::TSoundFormatsSupportedBuf& GetRecordFormatsSupported() { return iRecordFormatsSupported;}
	inline RMdaDevSound::TCurrentSoundFormatBuf& GetRecordFormat() { return iRecordFormat;}

protected:
	CMdaHwInfo();
private:
	void ConstructL();
	void GetHwInfoL();

	RMdaDevSound::TSoundFormatsSupportedBuf iPlayFormatsSupported;
	RMdaDevSound::TCurrentSoundFormatBuf	iPlayFormat;
	RMdaDevSound::TSoundFormatsSupportedBuf iRecordFormatsSupported;
	RMdaDevSound::TCurrentSoundFormatBuf	iRecordFormat;
	RMdaDevSound iDevice;

	};

#endif

