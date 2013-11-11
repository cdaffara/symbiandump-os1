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

#ifndef __TSI_MMFCTLFRMSTEP_H__
#define __TSI_MMFCTLFRMSTEP_H__

#include <testframework.h>
#include <mmf/common/mmfcontroller.h>
#include <mmf/server/mmffile.h>

// Test controller/data plugin UIDs.
// These are custom controllers provided by Integration Testing

// dummy controller
#define KTSIMmfControllerUid                   0x101f7bfa
#define KTSIMmfController2Uid                  0x101f7bfc
#define KTSIMmfDataSinkUid					   0x101f7bfe
#define KTSIMmfDataSourceUid                   0x101f7c00

#define KTextBufLen	64

// priorities for controllers
enum TTSIMmfPriority
	{
	ETSIMmfPriorityLow = 1,
	ETSIMmfPriorityHigh = 11
	};

// -----------------------------------
// RTestStepMmfCtlfrm, which is the base class for all 
// the MMF CTLFRM suite test steps.
class RTestStepMmfCtlfrm : public RTestStep
	{
public:
	RTestStepMmfCtlfrm();
	};

// RTestStepMmfCtlfrmTest1
// base class with preamble / postamble to open and close a test controller
class RTestStepMmfCtlfrmTest1 : public RTestStepMmfCtlfrm
	{
public:
	TVerdict OpenL();
	void Close();
protected:
	RMMFController iController;
	TMMFPrioritySettings iSettings;
	};

// RTestStepMmfCtlfrmTest2
// base class with preamble / postamble to open and close a test controller 2
class RTestStepMmfCtlfrmTest2 : public RTestStepMmfCtlfrm
	{
public:
	TVerdict OpenL();
	void Close();
protected:
	RMMFController iController;
	TMMFPrioritySettings iSettings;
	};

// RTestStepMmfCtlfrmAudio
// base class with preamble / postamble to open and close the audio controller
// configured to receive events from the controlled
class RTestStepMmfCtlfrmAudio : public RTestStepMmfCtlfrm
	{
public:
	TVerdict OpenL();
	void Close();

protected:
	 //[ check if the event status is pending ]
	 TBool IsTimeOut( TRequestStatus& aEventStatus );
	 //[ cancel received events ]
	 void  CancelReceivedEvents();
	 //[ is event sought the one we received ] 
	 TBool IsSoughtEvent( TMMFEvent& aExpectedEvent, TMMFEventPckg& receivedEvent );
	 // [ search for expected event ]
	 TVerdict SearchForEvent( TMMFEvent& aExpectedEvent );

protected:
	RMMFController		 iController;
	TMMFPrioritySettings iSettings;
	RTimer               iTimer     ; // escape timer for waiting on events  
	TMMFFileConfig       iFileConfig; // audio file 
	};
	 
// RTestStepMmfCtlfrmAudioPrimed
// same as above, with source/sink handles, and is primed in the preamble
class RTestStepMmfCtlfrmAudioPrimed : public RTestStepMmfCtlfrmAudio
	{
public:
	TVerdict OpenL();
	void Close();

protected:
	TMMFMessageDestination* iSourceHandlePtr;
	TMMFMessageDestination* iSinkHandlePtr;
	};
	 
// RTestStepMmfCtlfrmAudioNoSource
// same as RTestStepMmfCtlfrmAudio, but has no source or sink loaded yet
class RTestStepMmfCtlfrmAudioNoSource : public RTestStepMmfCtlfrmAudio
	{
public:
	TVerdict OpenL();
	};
	 
// RTestStepMmfCtlfrmDualAudio
// same as RTestStepMmfCtlfrmAudio, with 2 identical controllers
class RTestStepMmfCtlfrmDualAudio : public RTestStepMmfCtlfrm
	{
public:
	TVerdict OpenL();
	void Close();

protected:
	 //[ check if the event status is pending ]
	 TBool IsTimeOut( TRequestStatus& aEventStatus );
	 //[ cancel received events ]
	 void  CancelReceivedEvents( TInt aWhichController );
	 //[ is event sought the one we received ] 
	 TBool IsSoughtEvent( TMMFEvent& aExpectedEvent, TMMFEventPckg& receivedEvent );
	 // [ search for expected event ]
	 TVerdict SearchForEvent( TMMFEvent& aExpectedEvent, TInt aWhichController );

protected:
	RMMFController		 iController1;
	RMMFController		 iController2;
	TMMFPrioritySettings iSettings;
	RTimer               iTimer     ; // escape timer for waiting on events  
	TMMFFileConfig       iFileConfig1; // audio file 
	TMMFFileConfig       iFileConfig2; // audio file 
	};

// RTestStepMmfCtlfrmVideo
// base class with preamble / postamble to open and close the video controller
class RTestStepMmfCtlfrmVideo : public RTestStepMmfCtlfrm
	{
public:
	TVerdict OpenL();
	void Close();

protected:
	RMMFController		 iController;
	TMMFPrioritySettings iSettings;
	TMMFFileConfig       iFileConfig; // video file 
	};
	 

#endif	// __TSI_MMFCTLFRMSTEP_H__
