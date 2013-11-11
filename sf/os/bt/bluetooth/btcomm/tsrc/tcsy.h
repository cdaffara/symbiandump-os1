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

#ifndef TCSY_H
#define TCSY_H

// forward declarations
#include <e32test.h>
#include <e32math.h>
#include <es_sock.h>
#include <f32file.h>
#include <bt_sock.h>
#include <c32comm.h>
#include <btmanclient.h>
#include <btextnotifiers.h>

#include <bluetooth/hci/hcierrors.h>

class CActiveConsole;
class CActiveRootConsole;
class CActiveCommConsole;

class CCSY;
class CAutoJimmier;

_LIT(KDateFormatter,"%-B%:0%J%:1%T%:2%S%.%*C4%:3%+B");
const TInt KAutoRemoveTime = 6000000;
const TInt KAutoStartTime = 3000000;

NONSHARABLE_CLASS(CCSYApp) : public CBase
	{
	public:
		static CCSYApp* NewL(CConsoleBase& aConsole, TBool aAutoAuto = ETrue);
		void NewCSYL(TBool aStartReadingOnOpen=EFalse);
		void DeleteAllConnections();
		void DeleteConnection(const CCSY* aComm);
		void AutoMode();
		void ReStartAutoMode();
		void SingleCSYL();
		void FetchWebPageL();
		void C32UHeapMark();
		void C32UHeapMarkEnd();
		void StartExclusiveSessionL();
		void StopExclusiveSession();

		void StartL();
		void Stop();
		~CCSYApp();

	private:
		CCSYApp(CConsoleBase& aConsole);
		void ConstructL(CConsoleBase& aConsole, TBool aAutoAuto = ETrue);

	private:
		CActiveRootConsole*	iActiveRootConsole;
		CConsoleBase& iConsole;

		RPointerArray<CCSY> iConnections;
		RCommServ			iCommServer;
		RSocketServ			iExclusiveSession;
		CAutoJimmier*		iJimmier;
	};

NONSHARABLE_CLASS(CActiveConsoleBase) : public CActive
	{
	public:
		void DoCancel();
		virtual void DrawMenu() = 0;
		virtual void DrawCursor();
		void TimePrint();
		~CActiveConsoleBase();

	protected:
		CActiveConsoleBase(CConsoleBase& aConsole);
		void RequestKey();
		TInt RunError(TInt aError);

	protected:
		CConsoleBase&		iConsole;
		TTime				iTime;
	};

NONSHARABLE_CLASS(CActiveCommConsole) : public CActiveConsoleBase
	{
	public:
		static CActiveCommConsole* NewL(CConsoleBase& aConsole, CCSY& aCSY);
		virtual void DrawMenu();
	private:
		void RunL();
		CActiveCommConsole(CConsoleBase& aConsole, CCSY& aApp);
	private:
		CCSY&			iComm;

	};

NONSHARABLE_CLASS(CActiveRootConsole) : public CActiveConsoleBase
	{
	public:
		static CActiveRootConsole* NewL(CConsoleBase& aConsole, CCSYApp& aApp);
		virtual void DrawMenu();
	private:
		void RunL();
		CActiveRootConsole(CConsoleBase& aConsole, CCSYApp& aApp);
	private:
		CCSYApp&		iApp;
	};

NONSHARABLE_CLASS(CCSY) : public CActive
	{
	// abstract class for derivation - deriveees implement RunL and Start
	public:
		~CCSY();
		static CCSY* NewL(CConsoleBase& aConsole, CCSYApp& aApp, RCommServ& aCommServer);
		void Read();
		void Close();
		void Destroy();
		static CConsoleBase* AutoSizeNewL(const TDesC& aTitle, TSize aSize);

	protected:
		CCSY(CConsoleBase& aConsole, CCSYApp& aApp, RCommServ& aCommServer);
		void ConstructL();
		void DoCancel();
		void RunL();

	protected:
		CConsoleBase&		iRootConsole;	// may bin
		CConsoleBase*		iCommConsole;
		CCSYApp&			iApp;
		RCommServ&			iCommServer;
		RComm				iComm;
		CActiveCommConsole*	iActiveCommConsole;
		TBuf8<10>			iBuffer; // not interested in large data at present

	private:
		TInt RunError(TInt aError);

	private:
		static TInt			iCommInstance;
		enum TCommStates
			{
			EReading,
			EWriting,
			};
		TCommStates			iCommState;

	};


NONSHARABLE_CLASS(CAutoJimmier) : public CTimer
	{
	public:
		static CAutoJimmier* NewL(CConsoleBase& aConsole, CCSYApp& aApp);
		void StartTimer();

	private:
		CAutoJimmier(CConsoleBase& aConsole, CCSYApp& aApp);
		void ConstructL();
		void RunL();
		TInt RunError(TInt aError);

	private:
		CConsoleBase&		iConsole;
		CCSYApp&			iApp;
		TInt64				iSeed;
		TBool				iRemove;
	};


#endif // TCSY_H
