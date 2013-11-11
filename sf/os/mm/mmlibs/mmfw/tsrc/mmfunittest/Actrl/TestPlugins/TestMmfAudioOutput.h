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

#ifndef __TEST_MMF_AUDIOOUTPUT_H__
#define __TEST_MMF_AUDIOOUTPUT_H__

#include "MMFAudioOutput.h"

class CTestMMFAudioOutput: public CMMFAudioOutput 
	{
public:
	static MDataSink* NewSinkL() ;
	virtual ~CTestMMFAudioOutput() {};

	virtual void SinkPrimeL();
	virtual void SinkPlayL();
	
protected:
	inline CTestMMFAudioOutput() {}

	};

#endif
