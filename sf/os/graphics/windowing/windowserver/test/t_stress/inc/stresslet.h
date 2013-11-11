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

/**
 @file
 @test
 @internalComponent
*/

#ifndef STRESSLET_H
#define STRESSLET_H

#include <e32base.h>
#include <w32std.h>

class CBaseStressletAllocator;
class MTestStepReporter;

/** CStresslet base class */
class CStresslet : public CBase
	{
public:
	static void LaunchL(CStresslet* aStresslet);
	~CStresslet();
	virtual void HandleRedraw(TWsRedrawEvent &aEvent) = 0;
	virtual void HandleEvent(TWsEvent &aEvent) = 0;
	RWsSession& Session();
	
protected:
	CStresslet(MTestStepReporter& aReporter);
	virtual void StartL()=0;
	void ConcludeNow(void);
	CWindowGc& WindowGc();
	MTestStepReporter& Reporter();
	
private:
	void ConstructL();

	/** Inner class CEventHandler */
	class CEventHandler : public CActive
		{
	public:
		static CEventHandler* NewL (CStresslet& aStresslet);
		~CEventHandler ();
	protected: // from CActive
		void RunL ();
		void DoCancel ();
		void ConstructL ();
	private:
		CEventHandler (CStresslet& aStresslet);
	private:
		CStresslet& iStresslet;
		TWsEvent iEvent;
		};
	
	/** Inner class CRedrawHandler */	
	class CRedrawHandler : public CActive
		{
	public:
		static CRedrawHandler* NewL (CStresslet& aStresslet);
		~CRedrawHandler ();
	protected: // from CActive
		void RunL ();
		void DoCancel ();
		void ConstructL ();
	private:
		CRedrawHandler (CStresslet& aStresslet);
	private:
		CStresslet& iStresslet;
		TWsRedrawEvent iRedrawEvent;
		};		
	
private:
	RWsSession iWs;
	CWindowGc* iGc;
	CWsScreenDevice* iScreenDevice;
	CRedrawHandler* iRedrawHandler;
	CEventHandler* iEventHandler;
	
	MTestStepReporter& iReporter;
	};

#define REPORT_EVENT(value) (Reporter().LogCondition(value,(TText8*)__FILE__,__LINE__))
#define REPORT_EVENT_WITH_TEXT(value,text) (text,Reporter().LogCondition(value,(TText8*)__FILE__,__LINE__))

	
#endif // STRESSLET_H
