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
// teventhandlingperf.h
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/


#ifndef __TMULTIPTREVENTHANDLINGPERF_H__
#define __TMULTIPTREVENTHANDLINGPERF_H__

#include <e32base.h>
#include <coemain.h>
#include <coeaui.h>
#include <coecntrl.h>

_LIT(KChunkName,"MULTIPOINTERSHAREDCHUNK");


enum TMultiPtrPerfPanics
	{
	EMultiPtrPerfPanicWrongTest,
	};

class CMultiPtrPerfTestAppUi;

class CMultiPtrPerfTestControl : public CCoeControl
	{
	enum {EEventBufferSize=32};
public:
	~CMultiPtrPerfTestControl();
	void ConstructL();
	void ConstructL(TPoint aOrigin, TSize aSize);
	TInt CalculateLatencyAndStartThread();
	void AddExpectedEvent(TAdvancedPointerEvent& aExpEvent);
	TInt Failed(TPoint3D aExp3DPoint, TPoint3D aActual3DPoint, TInt aErrorCode = 0, TInt aExpPtrNum = 0, TInt aActPtrNum = 0);
	TInt PointerEventsNotSupported();
	
	// From CCoeControl
	void HandlePointerEventL(const TPointerEvent& aPointerEvent);

protected:
	CCirBuf<TAdvancedPointerEvent> iEventBuffer;
	CMultiPtrPerfTestAppUi* iPtrAppUi;
	RChunk iChunk;
	TInt iLatLenInLetters;
	TInt64 iAverageLatency;
	TInt64 iLatency;
	TInt iChunkOffset;
	TBool iPointerEventNotSupported;
	};

class CMultiPtrPerfTestAppUi : public CCoeAppUi
	{
public:
	CMultiPtrPerfTestAppUi();
	~CMultiPtrPerfTestAppUi();
	
	void ConstructL();
	TInt CreateEventSimulatingThreads();
	TInt64 TestStartTime() {return iTestStartTime;}
	TInt& TestCaseNum() {return iTestCaseNum;}
	void AddExpectedEvent(TAdvancedPointerEvent& aExpEvent);
	void Failed(TPoint3D aExp3DPoint, TPoint3D aActual3DPoint, TInt aErrorCode = 0, TInt aExpPtrNum = 0, TInt aActPtrNum = 0);
	void PointerEventsNotSupported();
	void CloseThread() {iThread.Close();}
	void SetNumberOfEvents(TInt aNumEvents) {iNumEvents = aNumEvents;}
	void DecrementEventCount() {--iNumEvents;}
	TInt EventCount() {return iNumEvents;}
	RThread& ChildThread() {return iThread;}
	
private:
	TInt iTestCaseNum;
	TInt64 iTestStartTime;
	CMultiPtrPerfTestControl* iControl;
	RThread iThread;
	TInt iNumEvents;
	};

#endif // __TMULTIPTREVENTHANDLINGPERF_H__
