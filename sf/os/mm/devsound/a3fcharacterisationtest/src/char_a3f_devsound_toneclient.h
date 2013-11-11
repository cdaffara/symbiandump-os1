// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CHAR_A3F_DEVSOUND_TONECLIENT_H
#define CHAR_A3F_DEVSOUND_TONECLIENT_H

#include <mmf/server/sounddevice.h>
#include "char_a3f_devsound_compliancesuiteconstants.h"
#include <testframework.h>
/*
An interface to a set of DevSound callback functions.
*/
class MA3FDevsoundToneClientObserver
	{
public:
	virtual void ClientInitializeCompleteCallback(TInt aError) = 0;
	virtual void ClientToneFinishedCallback(TInt aError) = 0;
	};

class CA3FDevSoundToneClient : public CBase, public MDevSoundObserver
	{
public:
	static CA3FDevSoundToneClient* NewL(MA3FDevsoundToneClientObserver &aObserver);
	~CA3FDevSoundToneClient();

	void SetPriority(TInt priority);
	TInt InitTonePlay(TInt aFrequency,TInt aDuration);
	TInt InitDualTonePlay(TInt aFrequency, TInt aFrequencyTwo, TInt aDuration);
	TInt InitDTMFStringPlay(const TDesC &aDTMFString);
	TInt InitToneSequencePlay();
	TInt PlayTone();
	TInt PlayDualTone();
	TInt PlayDTMFString();
	TInt PlayToneSequence();
	
private:
	CA3FDevSoundToneClient(MA3FDevsoundToneClientObserver &aObserver);
	void ConstructL();
	// from MDevSoundObserver
	void BufferToBeFilled(CMMFBuffer *aBuffer);
	void BufferToBeEmptied(CMMFBuffer *aBuffer);
	void DeviceMessage(TUid aMessageType, const TDesC8& aMsg);
	void ConvertError(TInt aError);
	void PlayError(TInt aError);
	void RecordError(TInt aError);

	void InitializeComplete(TInt aError);
	void ToneFinished(TInt aError);
private:
	CMMFDevSound* iDevSound;
	MA3FDevsoundToneClientObserver& iObserver;
	TInt iDuration;
	TInt iFrequency;
	TInt iFrequencyTwo;
	TBuf16<KMaxStringLength> iDTMFString;
	};

#endif //CHAR_A3F_DEVSOUND_TONECLIENT_H
	

