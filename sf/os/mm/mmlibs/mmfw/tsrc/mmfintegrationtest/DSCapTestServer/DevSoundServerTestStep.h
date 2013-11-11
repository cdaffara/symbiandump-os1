// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __DEVSOUNDSERVERTESTSTEP_H__
#define __DEVSOUNDSERVERTESTSTEP_H__


#include <simulprocserver.h>
#include <mmf/server/sounddevice.h>


class CSDevSoundTestStep1 : public CSimulProcTestStep, public MDevSoundObserver
	{
public:
	static CSDevSoundTestStep1* NewL();
	void ConstructL();
	virtual void StartProcessing(TRequestStatus& aStatus);
	virtual TVerdict EndProcessingAndReturnResult(TDes8& aMessage);		
	virtual ~CSDevSoundTestStep1();


protected:
	void InitializeComplete (TInt aError);
	void ToneFinished (TInt aError);
	void BufferToBeFilled (CMMFBuffer* aBuffer);
	void PlayError (TInt aError);
	void BufferToBeEmptied (CMMFBuffer* aBuffer);
	void RecordError (TInt aError);
	void ConvertError (TInt aError);
	void DeviceMessage (TUid aMessageType, const TDesC8& aMsg);

	TVerdict TestInitialize(TMMFState aMode);
	TVerdict TestPlayTone(TInt aFreq, TTimeIntervalMicroSeconds aDur);
	TVerdict DoPlaySimpleTone();

protected:
	enum TCallback 
		{
		EInitComplete,
		EToneFinished,
		EBuffToFill,
		EPlayError,
		EBuffToEmpty,
		ERecError,
		EConvertMsg,
		EDeviceMsg
		};

	
protected:
	//CMMFDevSound* iMMFDevSound;

	TRequestStatus* iStatus;
	TVerdict iVerdict;

	TFixedArray<TInt, 7> iCallbackArray;
	void ResetCallbacks();
	TInt GetCallbackTotal();

	CMMFBuffer* iBuffer;

	CMMFDevSound* iMMFDevSound;
	TInt iExpectedValue;
	TInt iCallbackError;
	RChunk iChunk;
	};
	
	
#endif

