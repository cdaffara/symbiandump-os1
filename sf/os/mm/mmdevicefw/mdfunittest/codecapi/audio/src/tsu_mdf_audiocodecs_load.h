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

#ifndef TSU_MDF_AUDIOCODECS_LOAD_H
#define TSU_MDF_AUDIOCODECS_LOAD_H

#include "tsu_mdf_audiocodecs_step.h"
#include <mmf/server/mmfhwdevice.h>

class RTestStepAudioCodecLoad : public RTestStepAudioCodecs,  public MMMFHwDeviceObserver
	{
public:
	virtual TInt FillThisHwBuffer(CMMFBuffer& /*aHwDataBuffer*/) {return 0;}
	virtual TInt EmptyThisHwBuffer(CMMFBuffer& /*aHwDataBuffer*/) {return 0;}
	virtual TInt MsgFromHwDevice(TUid /*aMessageType*/, const TDesC8& /*aMsg*/) {return 0;}
	virtual void Stopped() {}
	virtual void Error(TInt /*aError=KErrNone*/) {}

	RTestStepAudioCodecLoad();
	virtual TVerdict DoTestStepL();
	};

#endif // TSU_MDF_AUDIOCODECS_LOAD_H
