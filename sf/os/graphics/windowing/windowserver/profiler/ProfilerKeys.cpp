// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// HotKey user interface to the PC-sampling Profiler engine in E32utils
// 
//

#include <e32base.h>
#include <w32std.h>
#include <profiler.h>

class CKeys : public CActive, private MProfilerController
	{
	class THotKey
		{
		enum {KAllModifiers = EModifierFunc|EModifierCtrl|EModifierShift};
	public:
		inline THotKey()
			:iCapture(-1)
			{}
		inline void CaptureL(RWindowGroup& aWg, TUint aKey)
			{iCapture=User::LeaveIfError(aWg.CaptureKey(aKey,KAllModifiers,KAllModifiers));}
		inline void Release(RWindowGroup& aWg)
			{if (iCapture != -1) aWg.CancelCaptureKey(iCapture);}
	private:
		TInt32 iCapture;
		};
public:
	IMPORT_C static MProfilerController* NewL(TInt aPriority, MProfilerEngine& aEngine);
private:
	CKeys(TInt aPriority, MProfilerEngine& aEngine);
	void ConstructL();
	~CKeys();
	void Release();
//
	void Queue();
//
	void RunL();
	void DoCancel();
//
private:
	RWsSession iWs;
	RWindowGroup iWg;
	THotKey iStartKey;
	THotKey iStopKey;
	THotKey iCloseKey;
	THotKey iUnloadKey;
	THotKey iTestKey;
	};




EXPORT_C MProfilerController* CKeys::NewL(TInt aPriority, MProfilerEngine& aEngine)
	{
	CKeys* self = new(ELeave) CKeys(aPriority, aEngine);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CKeys::CKeys(TInt aPriority, MProfilerEngine& aEngine)
	:CActive(aPriority), MProfilerController(aEngine)
	{
	CActiveScheduler::Add(this);
	}

void CKeys::ConstructL()
	{
	User::LeaveIfError(iWs.Connect());
	iWg = iWs;
	User::LeaveIfError(iWg.Construct(0,EFalse));
	iWg.AutoForeground(EFalse);
	iStartKey.CaptureL(iWg, '1');
	iStopKey.CaptureL(iWg, '2');
	iCloseKey.CaptureL(iWg, '3');
	iUnloadKey.CaptureL(iWg, '4');
	iTestKey.CaptureL(iWg, '0');
	Queue();
	}

CKeys::~CKeys()
	{
	Cancel();
	iStartKey.Release(iWg);
	iStopKey.Release(iWg);
	iCloseKey.Release(iWg);
	iUnloadKey.Release(iWg);
	iTestKey.Release(iWg);
	iWg.Close();
	iWs.Close();
	}

void CKeys::Release()
	{
	delete this;
	}

void CKeys::Queue()
	{
	iWs.EventReady(&iStatus);
	SetActive();
	}

void CKeys::RunL()
	{
	TWsEvent ev;
	iWs.GetEvent(ev);
	if (ev.Type() == EEventKey)
		{
		switch (ev.Key()->iCode)
			{
		case '0':
			{
			_LIT(KTestPrint,"Profiler is ");
			static const TText* const KStates[] = {_S("running"),_S("waiting"),_S("loaded"),_S("?")};
			TBuf<60> info(KTestPrint);
			info.Append(TPtrC(KStates[GetState()]));
			User::InfoPrint(info);
			}
			break;
		case '1':
		case '2':
		case '3':
		case '4':
			Control(Profiler::TState(ev.Key()->iCode - '1'));
			break;
			}
		}
	Queue();
	}

void CKeys::DoCancel()
	{
	iWs.EventReadyCancel();
	}


