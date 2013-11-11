// Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Sets compiler switches to control WSERV's build
// 
//

#ifndef __WSTOP_H__
#define __WSTOP_H__

#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include <w32debug.h>
#include "w32cmd.h"
#include "server.h"
#include "tcursor.h"
#include "ScrDev.H"
#include "Direct.H"

/**
@internalTechnology
*/
class CWsRootWindow;
class CWsPluginManager;
typedef CDebugLogBase *(*CreateDebugLog)(TBool aIsFirst, TDesC &aParams);

class CWsShellLogon : public CActive
	{
public:
	CWsShellLogon();
	~CWsShellLogon();
	void ConstructL(RFs &aFs);
	inline TFullName FullName();
private:
	void DoCancel();
	void RunL();
	void Request();
private:
#if defined(__WINS__)
	RLibrary iLib;
	RThread iShellThread;
	RProcess iShellProcess;
#else
	RProcess iShell;
#endif
	};

class CWsRenderOrienationTracker;
class CWsTop
	{
	enum {EShellBootModeReboot=0,EShellBootModeNoReboot=1,EShellBootModeExit=2};
private:
	class CShellStarter : public CTimer
		{
		public:
			CShellStarter() : CTimer(EPriorityNormal) {CActiveScheduler::Add(this);}
			void ConstructL() {CTimer::ConstructL();}
		private:
			void RunL() { CWsTop::StartShell(); }
		};
	
public:
	static void RunServerL();
	static void InitStaticsL();
	static void DeleteStaticsL();
	static inline CWsWindowGroup *FocusWindowGroup();
	static CWsClient *FocusWindowGroupOwner();

	static RWsTextCursor *CurrentTextCursor();
	static void SetCurrentTextCursor(RWsTextCursor *aCurrentTextCursor);
	static inline void KillForegroundSession();
//
	static void TriggerRedraws(CWsRootWindow* aRootWindow);
	static void ClientDestroyed(const CWsClient *aClient);
	static void NewSession(const CWsClient *aClient);
	static inline TBool ShuttingDown();
	static void StartShell();
	static void SessionExited(CWsClient *aClient);
	static void ShellExited();
	static void Exit();
    enum TReloadWsIni 
        {
        EDoReloadWsIni,
        EDoNotReloadWsIni
        };
    static void EnableLogging(TReloadWsIni aSetting = EDoReloadWsIni);
	static void DisableLogging();
	static TInt SetSendOffEventsToShell(CWsClient *aClient,const TWsClCmdOffEventsToShell &aData);
	static void StopWindowGettingOffEvents(CWsWindowBase* aWindow);
	static void HandleSwitchOff(TInt aEvent,TBool aDoSwitchOff);
	static inline CScreen* Screen();
	static inline CScreen* Screen(TInt aScreenId);
	static inline TInt NumberOfScreens();
	static void RedrawScreens();
	static void ClearAllRedrawStores();
	static void LogCommand(RWsSession::TLoggingCommand aCommand);
	static inline RTimer& Timer();
	static void StateDump();
	static void SetCurrentFocusScreen(CScreen* aScreen);
	static inline CScreen* CurrentFocusScreen();
	static TInt SetCurrentFocusScreen(TInt aScreenNo);
	static inline CWindowServer* WindowServer();
	static void SetCheckHeapOnDisconnectClient(const CWsClient* aClient);
	static void SetCheckHeapOnDisconnectMode(TWsCheckHeapOnDisconnectMode aCheckHeapOnDisconnectMode);
	static TInt FetchCheckHeapResult();
	static TBool NeedsHeapCheckAndRestart(TInt aStartHeapCount);
	static TBool ReleaseMemory();
	static inline TBool IsFadeEnabled();
	static inline TBool FinishEveryFlush();
	static void ClearSurfaceMap(CWsClient* aClient);
	static TBool SearchDuplicateSurfaceId(const TSurfaceId& aSurfaceId);
	static TBool MultiFocusPolicy();
	
	static inline CWsPluginManager *PluginManager();
	static void CheckRenderOrientation();
	
private:
	static void InitLogging();
	static void InitScreenL( TInt aScreenNumber)  ; 	

private:
// Data
	static CShellStarter *iShellStarter;
	static CWindowServer *iServer;
	static RLibrary iDebugLib;
	static CWsShellLogon *iShell;
	static const CWsClient *iShellClient;
	static TBool iPreviousShellClient;
	static TInt iShellBootMode;
	static TBool iShuttingDown;
	static TBool iIsFirstLog;
	static CWsWindowBase *iWindowToSendOffEventsTo;
	static CArrayPtrFlat<CScreen>* iScreens ;
	static CScreen* iCurrentFocusScreen;
	static TInt iNumberOfScreens ;
	static RTimer iTimer;
	static TBool iIgnoreSwitchOffEvent;
	static TBool iFadeEnabled;
	static TBool iFinishEveryFlush;
	static TBool iMultiFocusPolicy;
	static const CWsClient* iTriggerHeapCheckOnClientExit;
	static TWsCheckHeapOnDisconnectMode iHeapCheckMode;
	static TInt iCheckHeapResult;
	static TBool iDoHeapCheckAndRestart;
	static CWsPluginManager *iPluginManager;
    static CWsRenderOrienationTracker* iRenderOrientationTracker;	
	};
	
NONSHARABLE_CLASS(CWsActiveScheduler): public CActiveScheduler, public MWsActiveSchedulerDebug
	{
public:
	static CWsActiveScheduler* Static();
	CWsActiveScheduler();
	~CWsActiveScheduler();
	void PrepareDraw();
	void CancelPrepare();
	void StartDraw();
	void StopDraw(TInt aPixelsUpdated);
	void DrawStats(TInt& aUpdatesPerSecond,TInt64& aPixelsPerSecond,TInt aWindowInSeconds) const;
	void WaitForAnyRequest();
	void Error(TInt aError) const;
	void AccumReclaimedIdleTime(TInt64 aMicroSeconds);
	TInt64 ReclaimedIdleTime() const { return iReclaimedIdleTime; }
	TInt64 Requests() const { return iRequests; }
	TInt64 Errors() const { return iErrors; }
	virtual TInt64 Draws() const { return iDraws; }
	virtual TInt64 Total() const { return iTotal; }
	virtual TInt64 Preparing() const { return iPreparing; }
	virtual TInt64 Drawing() const { return iDrawing; }
	virtual TInt64 Idle() const { return iIdle;	}
private:
#ifdef _DEBUG
	enum TState
		{
		ENormal,
		EPreparingDraw,
		EDrawing,
		EDrawn
		};
	TState iState;
#endif
	// request stats
	TInt64 iRequests;
	mutable TInt64 iErrors;
	TInt64 iDraws;
	TTime iRun;
	TTime iRunDraw;
	TInt64 iTotal;
	TInt64 iPreparing;
	TInt64 iDrawing;
	TInt64 iIdle;
	TInt64 iReclaimedIdleTime;
	//TBool iPaused;
	// pixels-per-second stats
	NONSHARABLE_STRUCT(TSample)
		{
		TTime iStart;
		TUint64 iDuration;
		TUint32 iPixels;
		};
	TInt iFastCounterFreq;
	TUint iNumSamples;
	TUint iCurrent;
	TSample* iData;	
	};


//
// inlines			//
//

//
// CWsTop
//
inline TBool CWsTop::ShuttingDown()
	{return(iShuttingDown);}
inline CWsWindowGroup *CWsTop::FocusWindowGroup()
	{return(iCurrentFocusScreen->FocusWindowGroup());}
inline void CWsTop::KillForegroundSession()
	{iCurrentFocusScreen->KillForegroundSession();}
inline CScreen* CWsTop::Screen()
	{return CWsTop::Screen( 0 ) ;}
inline CScreen* CWsTop::Screen(TInt aScreen)
	{return (*iScreens)[aScreen];}
inline TInt CWsTop::NumberOfScreens()
	{return iNumberOfScreens;}
inline CScreen* CWsTop::CurrentFocusScreen()	
	{return iCurrentFocusScreen;}
inline RTimer& CWsTop::Timer()
	{return iTimer;}
inline CWindowServer* CWsTop::WindowServer()
	{return iServer;}
inline TBool CWsTop::IsFadeEnabled()
	{return iFadeEnabled;}
inline TBool CWsTop::FinishEveryFlush()
	{return iFinishEveryFlush;}
inline CWsPluginManager *CWsTop::PluginManager()
	{return iPluginManager;}

#endif
