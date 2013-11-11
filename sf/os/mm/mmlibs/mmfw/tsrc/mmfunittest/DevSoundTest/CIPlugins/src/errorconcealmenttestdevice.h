// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef ERRORCONCEALMENTTESTDEVICE_H
#define ERRORCONCEALMENTTESTDEVICE_H

#include <mmf/server/mmfswcodecwrapper.h>
#include <mmf/server/devsoundstandardcustominterfaces.h>


class CMMFErrorConcealment : public CBase,
							 public MMMFErrorConcealmentIntfc
	{
public:
	//return Pointer to the CMMFErrorConcealment class
	static CMMFErrorConcealment* NewL();

	//Destructor
	~CMMFErrorConcealment();

	// from mirror MErrorConcealment method.
	TInt ConcealErrorForNextBuffer();
	TInt SetFrameMode(TBool aFrameModeOn);
	TInt FrameModeRqrdForEC(TBool& aFrameModeRqrd);
	};


/*
CErrorConcealmentTestDevice

Test hw device used by the
TSU_MMF_DEVSOUND_CIU_SUITE unit test harness.
*/
class CErrorConcealmentTestDevice : public CMMFSwCodecWrapper,
									public MMMFErrorConcealmentIntfc
	{
public:
	static CMMFHwDevice* NewL();
	virtual ~CErrorConcealmentTestDevice();
	virtual TInt Start(TDeviceFunc aFuncCmd, TDeviceFlow aFlowCmd);
	virtual TInt Stop();
	virtual TInt Pause();
	virtual TInt Init(THwDeviceInitParams& aDevInfo);
	virtual TAny* CustomInterface(TUid aInterfaceId);
	virtual TInt ThisHwBufferFilled(CMMFBuffer& aFillBufferPtr);
	virtual TInt ThisHwBufferEmptied(CMMFBuffer& aEmptyBufferPtr);
	virtual TInt SetConfig(TTaskConfig& aConfig);
	virtual TInt StopAndDeleteCodec();
	virtual TInt DeleteCodec();

	// from CMMFSwCodecWrapper
	virtual CMMFSwCodec& Codec();

	// from mirror MErrorConcealment method.
	TInt ConcealErrorForNextBuffer();
	TInt SetFrameMode(TBool aFrameModeOn);
	TInt FrameModeRqrdForEC(TBool& aFrameModeRqrd);

private:
	CErrorConcealmentTestDevice();
	void ConstructL();

private:
	CMMFErrorConcealment* iErrorConcealment;
	};

#endif 
