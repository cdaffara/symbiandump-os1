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

#ifndef __TEST_MMF_CUSTOM_MMF_AUDIO_CONTROLLER_H__
#define __TEST_MMF_CUSTOM_MMF_AUDIO_CONTROLLER_H__

#include "MmfAudioController.h"
#include <mmf/server/mmfclip.h>


class CCustomMMFMessageHolder : public CBase
	{
public:
	static CCustomMMFMessageHolder* NewL(TMMFMessage& aMessage)
		{return new CCustomMMFMessageHolder(aMessage);}
	TMMFMessage iMessage;
private:
	CCustomMMFMessageHolder(TMMFMessage& aMessage): CBase(), iMessage(aMessage){}
	};



class CCustomMmfAudioController: public CMMFAudioController 
	{
public:
	static CMMFController* NewL() ;
    virtual ~CCustomMmfAudioController();
	virtual void PrimeL();
	virtual void PlayL();
	virtual void PauseL();
	virtual void StopL();
	virtual void PrimeL(TMMFMessage& aMessage);
	virtual void PlayL(TMMFMessage& aMessage);
	virtual void PauseL(TMMFMessage& aMessage);
	
	virtual void ResetL();
	virtual void RemoveDataSourceL(MDataSource& aDataSource);
	virtual void RemoveDataSinkL(MDataSink& aDataSink);
	virtual void MapdSetVolumeL(TInt aVolume);
	virtual void MapdGetMaxVolumeL(TInt& aMaxVolume);
	virtual void MapdGetVolumeL(TInt& aVolume);
	virtual void CustomCommand(TMMFMessage& aMessage);
	virtual void MarcSetMaxFileSizeL(TInt aFileSize);
	virtual void MarcGetRecordTimeAvailableL(TTimeIntervalMicroSeconds& aTime);

private:
	 TBool IsMemoryAllocCmd(TMMFMessage& aMessage );
	 TInt  AllocMemoryTestL();
	 CCustomMMFMessageHolder* iStoppingMessage;
	 
protected:
	CCustomMmfAudioController();
	virtual TBool IsUnderTest(){return iIsTest;}
	TBool iIsTest;
	};

//The controller is meant for testing. Other than the overridden functions, other functions
//inherited from CMMFAudioController should not be used.
class CMemoryFailAudioController : public CMMFAudioController
	{
public:
	static CMMFController* NewL() ;
	~CMemoryFailAudioController();
    virtual void AddDataSourceL(MDataSource& aSource);
    virtual void AddDataSinkL(MDataSink& aSink);
	virtual void SetPrioritySettings(const TMMFPrioritySettings& aSettings);
	virtual TTimeIntervalMicroSeconds DurationL() const;
protected:
	CMemoryFailAudioController();
    //Source which is not owned by the controller
    CMMFClip* iClip;
    MDataSink* iSink;
    //Extracted from MDataSink passed by the MMF. Not owned by the controller
    CMMFDevSound* iMMFDevSound;
	TMMFPrioritySettings    iPrioritySettings;
	};

//The controller is same as that of CMemoryFailAudioController. The only difference is NewL()
//is overridden.
class CMemoryPassAudioController : public CMemoryFailAudioController
	{
public:
	static CMMFController* NewL() ;
protected:
	CMemoryPassAudioController();
	};

//Test Controller that panics the Controller thread during playing/recording
//This verifies the fix for DEF127630
class CPanicAudioController : public CMMFAudioController
	{
public:
	static CMMFController* NewL();
	~CPanicAudioController();
	void AddDataSourceL(MDataSource& aSource);
	void AddDataSinkL(MDataSink& aSink);
	void PrimeL();
	void PlayL();
	TTimeIntervalMicroSeconds DurationL() const;
	void SetPositionL(const TTimeIntervalMicroSeconds& aPosition);
protected:
	CPanicAudioController();
	void ConstructL();
	static TInt PanicTimerComplete(TAny* aParent);
private:
	CPeriodic* iPanicTimer;
	};
#endif
