// Copyright (c) 1994-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Top level window server code
// 
//

#include "wstop.h"

#include <hal.h>
#include <graphics/fbsdefs.h>
#include "W32STD.H"
#include "ANIM.H"
#include "gc.h"
#include "playbackgc.h"
#include "rootwin.h"
#include "windowgroup.h"
#include "EVENT.H"
#include "KEYCLICK.H"
#include "panics.h"
#include "inifile.h"
#include "pointer.h"
#include "WSGRAPHICDRAWERFACTORY.H"
#include "redrawmsgwindow.h"
#include <graphics/WSGRAPHICDRAWERINTERFACE.H>
#include "WsMemMgr.h"
#include "backedupwindow.h"
#include "wsfont.h"
#include "registeredsurfacemap.h"
#include <EGL/egl.h>

#ifdef SYMBIAN_PROCESS_MONITORING_AND_STARTUP
#include <e32power.h>
#endif

#include "registeredsurfacemap.h"
#include "windowelementset.h"
#include "wspluginmanager.h"
#include "renderorientationtracker.h"

// IDs of p&s properties that optionally contain callbacks that may be used
// to release singletons owned by libraries at shutdown in order to make 
// the memory leak tests work.
// By convention, the ID is the same as the UID3 of the libary.
static TBool gReleaseSingletonsOnExit = EFalse;
static const TUid KOpenWfcImplCleanupKey = {0x10286FC4};     
static const TUid KOpenWfcInteropCleanupKey = {0x10286FC5};

typedef CDebugLogBase *(*CreateDebugLog)(TBool aIsFirst, TDesC &aParams);

CWsActiveScheduler* CWsActiveScheduler::Static()
	{
	return static_cast<CWsActiveScheduler*>(CActiveScheduler::Current());
	}

CWsActiveScheduler::CWsActiveScheduler(): iNumSamples(100)
	{
	iRun = User::FastCounter();
	if((KErrNone == HAL::Get(HALData::EFastCounterFrequency,iFastCounterFreq)) && iFastCounterFreq)
		{
		iData = reinterpret_cast<TSample*>(User::AllocZ(sizeof(TSample)*iNumSamples));
		}
	}
	
CWsActiveScheduler::~CWsActiveScheduler()
	{
	User::FreeZ(reinterpret_cast<void*&>(iData));
	}
void CWsActiveScheduler::PrepareDraw()
	{
	iRunDraw = User::FastCounter();
	}
	
void CWsActiveScheduler::CancelPrepare()
	{
	}
	
void CWsActiveScheduler::StartDraw()
	{
	TTime now;
	now.UniversalTime();
	TUint64 duration = now.MicroSecondsFrom(iRunDraw).Int64();
	iPreparing += duration;
	iRunDraw = now;
	if(iData)
		{
		iData[iCurrent].iStart = iRunDraw;
		}
	iDraws++;
	}

void CWsActiveScheduler::DrawStats(TInt& aUpdatesPerSecond,TInt64& aPixelsPerSecond,TInt aWindowInSeconds) const
	{
	aUpdatesPerSecond = 0;
	aPixelsPerSecond = 0;
	if(iData)
		{
		// work out statistic
		TTime window;
		window.UniversalTime();
		window -= TTimeIntervalSeconds(aWindowInSeconds);
		TUint32 pixels = 0;
		TInt64 duration = 0;
		for(TUint i=0; i<iNumSamples; i++)
			{
			if(iData[i].iStart >= window)
				{
				pixels += iData[i].iPixels;
				duration += iData[i].iDuration;
				aUpdatesPerSecond++;
				}
			}
		if(aUpdatesPerSecond && duration)
			{
			const TReal pixelsPerMicroSecond = (static_cast<TReal>(pixels) / static_cast<TReal>(duration));
			aPixelsPerSecond = static_cast<TInt64>(pixelsPerMicroSecond * 1000000.0);
			aUpdatesPerSecond /= aWindowInSeconds;
			}
		}
	}

void CWsActiveScheduler::StopDraw(TInt aPixelsUpdated)
	{
	TTime now;
	now.UniversalTime();
	TUint32 duration = now.MicroSecondsFrom(iRunDraw).Int64();
    // do recording
	if(iData)
		{
		iData[iCurrent].iDuration = now.MicroSecondsFrom(iData[iCurrent].iStart).Int64();
		iData[iCurrent].iPixels = aPixelsUpdated;
		if(++iCurrent >= iNumSamples)
			{
			iCurrent = 0;
			}
		}
	iDrawing += duration;
	}

void CWsActiveScheduler::WaitForAnyRequest()
	{
	TTime stop;
	stop.UniversalTime();
	iTotal += stop.MicroSecondsFrom(iRun).Int64();
	CActiveScheduler::WaitForAnyRequest();
	iRequests++;
	iRun.UniversalTime();
	iIdle += iRun.MicroSecondsFrom(stop).Int64();
	}

	
void CWsActiveScheduler::Error(TInt /*aError*/) const
	{
	iErrors++;
	}

void CWsActiveScheduler::AccumReclaimedIdleTime(TInt64 aMicroSeconds)
	{
	iReclaimedIdleTime += aMicroSeconds;
	}
	

GLREF_C void StateDump(CWsRootWindow* aRootWindow);
GLREF_C void HeapDump();

// Static objects that need to be destroyed on exit
GLDEF_D CDebugLogBase *wsDebugLog=NULL;
GLDEF_D TInt wsDebugLogLevel=0;
GLDEF_D CIniFile *WsIniFile=NULL;
//
GLDEF_D TPtr nullDescriptor(NULL,0);

LOCAL_D CWsActiveScheduler *TheActiveScheduler;

CScreen* CWsTop::iCurrentFocusScreen;
CArrayPtrFlat<CScreen>* CWsTop::iScreens ;
TInt CWsTop::iNumberOfScreens ;
CWsTop::CShellStarter* CWsTop::iShellStarter;

CWsPluginManager *CWsTop::iPluginManager;
CWindowServer *CWsTop::iServer;
RLibrary CWsTop::iDebugLib;
CWsShellLogon *CWsTop::iShell;
const CWsClient *CWsTop::iShellClient;
TBool CWsTop::iPreviousShellClient=EFalse;
TInt CWsTop::iShellBootMode;
TBool CWsTop::iShuttingDown;
TBool CWsTop::iIsFirstLog=ETrue;
CWsWindowBase *CWsTop::iWindowToSendOffEventsTo=NULL;
RTimer CWsTop::iTimer;
TBool CWsTop::iIgnoreSwitchOffEvent=EFalse;
TBool CWsTop::iFadeEnabled=ETrue;
TBool CWsTop::iFinishEveryFlush=EFalse;
TBool CWsTop::iMultiFocusPolicy=EFalse;
#if defined(__WINS__)
TFullName WsSemName;
#endif
const CWsClient* CWsTop::iTriggerHeapCheckOnClientExit=NULL;
TWsCheckHeapOnDisconnectMode CWsTop::iHeapCheckMode=EWsCheckHeapOnDisconnectModeNone;
TInt CWsTop::iCheckHeapResult=KErrNotReady;
TBool CWsTop::iDoHeapCheckAndRestart=EFalse;
#define RFbsSession_SendCommand_ShutDownMessage 1	// A FBS message that is not published yet and probably never will be.
CWsRenderOrienationTracker* CWsTop::iRenderOrientationTracker=NULL;

static void DefineSingletonKey(const TUid& aSingletonKey)
/**
 * Defines a new property for a singleton key. WServ must only process 
 * singleton keys that it created to prevent a malicious process with the 
 * WriteDeviceData capability causing arbitrary functions to be executed.
 * 
 * @param   aSingeltonKey   The UID of the singleton key to define.
 */
    {
    RThread t;      
    TUid category = { t.SecureId().iId };
    RProperty prop;
    
    // Write access is restricted to THIS process
    TInt err = prop.Define( category, aSingletonKey.iUid, 
            RProperty::EByteArray, TSecurityPolicy( t.SecureId() ), 
            TSecurityPolicy( t.SecureId() ), sizeof( TCallBack ) );
    
    if ( err == KErrNone )
        {
        TCallBack cb( NULL, NULL );
        TPckgC<TCallBack> cbPckg( cb );
        
        // Any error should cause the properties to be ignored
        err = prop.Set( category, aSingletonKey.iUid, cbPckg );    
        }      
    
    if ( err != KErrNone )
        {
        // A problem occured / the property already existed so for safety
        // the release code should be skipped.
        gReleaseSingletonsOnExit = EFalse;
        }
    
    prop.Close();
    t.Close();
    }

static void DeleteSingleton( const TUid& aSingletonKey )
/**
 * Deletes a singleton object that was created on WServ's main heap.
 * 
 * @pre     The ws plugins have not been unloaded.
 * @param   aSingletonKey   The UID of the singleton which correponds to an
 *                          RProperty within WServ's category.                       
 */
    {   
    if ( gReleaseSingletonsOnExit )
        {
        RThread t;
        TPckgBuf<TCallBack> cb;
        RProperty prop; 
        TInt err = prop.Get(TUid::Uid(t.SecureId().iId), aSingletonKey.iUid, cb);
        if (err == KErrNone && cb.Length() == sizeof(TCallBack) && 
                cb().iFunction && cb().iPtr == &User::Heap())
            {
            // Callback is only invoked if the heap for the singleton was the 
            // WServ heap because the WServ memory leak tests only check this
            // heap.
            cb().CallBack();
            }
        // Errors are ignored because the purpose of this function is to free
        // singletons in order top make memory leak checks pass.
        prop.Close();
        t.Close();
        }
    }

void CWsTop::DeleteStaticsL()
	{
	iShuttingDown=ETrue;
	delete iRenderOrientationTracker;	
	
	CClick::DeleteStatics();
	TWsPointer::Stop();
	CWsClient::DeleteStatics();
//
	delete iShellStarter;
	iServer->DestroySessionsForShutdown();

	CWsBackedUpWindow::StaticDestroy();
	delete iShell;
	DisableLogging();

	CEventQueue::DeleteStaticsL();
		
	CWsSpriteBase::DeleteStatics();
	CWsGc::DeleteStatics();
	CPlaybackGc::DeleteStatics();
	CWsAnimDll::DeleteStatics();
	CWsFontCache::DestroyInstance();

	iScreens->ResetAndDestroy();
	delete iScreens;

	delete iServer;
	TWindowServerEvent::DeleteStatics();
	
	// Free singletons on WServ heap created by libraries. Must be called
	// BEFORE iPluginManager is deleted otherwise the library code could have
	// been unloaded.
	DeleteSingleton(KOpenWfcImplCleanupKey);
	DeleteSingleton(KOpenWfcInteropCleanupKey);
	eglReleaseThread();

	if (!iDoHeapCheckAndRestart)
		{
		RFbsSession::GetSession()->SendCommand(RFbsSession_SendCommand_ShutDownMessage);		//This line is just being tidy, never really does anything useful.
		}

	RFbsSession::Disconnect();
	iDebugLib.Close();
	iTimer.Close();
	TWsPointer::DeleteStatics();
	delete TheActiveScheduler;
	
	// iPluginManager is not created in InitStaticsL because plugins need to be loaded 
	// before E32Main's loop for EcomSession allocations issues
	delete iPluginManager;
	}

RWsTextCursor *CWsTop::CurrentTextCursor()
	{
	if(iCurrentFocusScreen && FocusWindowGroup())
		{
		return FocusWindowGroup()->TextCursor();
		}
	else
		{
		return NULL;
		}
	}

CWsClient *CWsTop::FocusWindowGroupOwner()
	{
	return(FocusWindowGroup() ? FocusWindowGroup()->WsOwner():NULL);
	}

void CWsTop::ClientDestroyed(const CWsClient *aClient)
	{
	if (aClient==iShellClient)
		iShellClient=NULL;
	}

void CWsTop::NewSession(const CWsClient *aClient)
	{	
	if (iShellClient==NULL && iShell)
		{
#if defined(__WINS__)
		RThread proc;
		proc=aClient->Client();
#else
		RProcess proc;
		aClient->Client().Process(proc);
#endif
		TFullName procName = proc.FullName();
		// Before comparing the proc name with iShell name , truncate the proc name up to the actual name
		// referring to the process, by removing the part which starts with ':', if exists.
		TInt colonLocation = procName.Locate(':');
		if( KErrNotFound != colonLocation)
			{
			procName = procName.Left(colonLocation);
			}
		if (procName ==iShell->FullName())
			{
			iShellClient=aClient;
			if (!iPreviousShellClient)
				{
				iPreviousShellClient=ETrue;
				aClient->Screen()->RootWindow()->SetColorIfClear();
				}
			}
#if !defined(__WINS__)
		proc.Close();
#endif
		}	
	}

void CWsTop::SessionExited(CWsClient *aClient)
	{
	if (iShuttingDown)
		{
		RProcess proc;
		TInt err=aClient->Client().Process(proc);
		if (err==KErrNone && proc.Id()!=RProcess().Id())
			proc.Kill(0);
		else
			const_cast<RThread&>(aClient->Client()).Kill(0);
		if (err==KErrNone)
			proc.Close();
		}
	else if (iHeapCheckMode!=EWsCheckHeapOnDisconnectModeNone && aClient==iTriggerHeapCheckOnClientExit)
		{
		if (iHeapCheckMode==EWsCheckHeapOnDisconnectModeOnce)
			{
			iHeapCheckMode=EWsCheckHeapOnDisconnectModeNone;
			}
		iTriggerHeapCheckOnClientExit=NULL;
		iDoHeapCheckAndRestart=ETrue;
		Exit();
		}
	if (iServer->SessionCount()==1 && iShellBootMode==EShellBootModeNoReboot && iShell==NULL)
		StartShell();
	}


void CWsTop::RunServerL()
	{
	InitStaticsL();
	CWsMemoryManager* memMgr = CWsMemoryManager::NewLC();
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(memMgr);
	DeleteStaticsL();
	}

void CWsTop::InitStaticsL()
	{
	iShuttingDown=EFalse;
	// By default shell should be started.
	TBool startShell = ETrue;
	
	iCurrentFocusScreen = 0;

	// The windows server has been invoked.
	// This may have been from the system starter (via a
	// start up rss file)  
	// This block looks for a "-NoShell" argument in the invocation.
	// The existence of the argument means that shell does not need to be 
	// invoked from here because the new system starter
	// is in charge, and it will do it if required.
	
	_LIT(KNoShell,"-NOSHELL");
	
	TInt argLen = User::CommandLineLength();
	if(argLen)
		{
		HBufC* arg = HBufC::NewLC(argLen);
		TPtr argPtr = arg->Des();
		User::CommandLine(argPtr);
		argPtr.UpperCase();

		if(KErrNotFound != argPtr.Find(KNoShell))
			{
			// Don't start the shell. It will be started if required by 
			// the system starter.
			startShell = EFalse;
			}
		CleanupStack::PopAndDestroy(arg);
		}
		
	TheActiveScheduler=new(ELeave) CWsActiveScheduler;
	CActiveScheduler::Install(TheActiveScheduler);
	// WsIniFile is already created (before E32Main's Loop)
	_LIT(KWSERVIniFileVarLogEnable,"LOGENABLE");
	TInt loggingLevel;
	if (WsIniFile->FindVar(KWSERVIniFileVarLogEnable,loggingLevel))
		{
		EnableLogging(EDoNotReloadWsIni);
		if (wsDebugLog)
			{
			wsDebugLog->SetLoggingLevel(loggingLevel);
			}
		}
		
	_LIT(KWSERVIniFileVarFadeEnable,"FADEDISABLE");
	iFadeEnabled = !WsIniFile->FindVar(KWSERVIniFileVarFadeEnable);

	_LIT(KWSERVIniFileVarFinishEveryFlush,"FINISHEVERYFLUSH");
	iFinishEveryFlush = WsIniFile->FindVar(KWSERVIniFileVarFinishEveryFlush);
	
	_LIT(KWSERVIniFileVarSwitchOffEvent,"IGNORESWITCHOFFEVENT");
	iIgnoreSwitchOffEvent = WsIniFile->FindVar(KWSERVIniFileVarSwitchOffEvent);	

	iPluginManager = CWsPluginManager::NewL(); // need to be constructed before iServer!
	iServer=CWindowServer::NewL();
	CClick::InitStaticsL();
	
	RProcess wservProc;
	if (!wservProc.DefaultDataPaged())
	{
		iServer->SetPinClientDescriptors(ETrue);
	}
	User::LeaveIfError(FbsStartup());
	User::LeaveIfError(RFbsSession::Connect());
	User::LeaveIfError(iTimer.CreateLocal());

	TWindowServerEvent::InitStaticsL();
	CWsClient::InitStaticsL();
	//-------------------------------------------
	User::LeaveIfError(  HAL::Get( HAL::EDisplayNumberOfScreens, iNumberOfScreens ) ) ; 
	// Check that the INI file matches the HAL
	WS_ASSERT_ALWAYS(WsIniFile->NumberOfScreens()<=iNumberOfScreens, EWsPanicScreenInformationError);

	iScreens = new (ELeave) CArrayPtrFlat<CScreen>( iNumberOfScreens ) ;  //
	// now construct screens for as long as there is information
	
	TInt ii ;
	for ( ii = 0 ; ii < iNumberOfScreens ; ++ii )
		{
		InitScreenL( ii ) ;
		}
	//---------------------------------------------
	iCurrentFocusScreen = (*iScreens)[0] ;
	iServer->StartL();

	CWsFontCache::CreateInstanceL();
	CWsGc::InitStaticsL();
	CPlaybackGc::InitStaticsL();
	CWsSpriteBase::InitStaticsL();
	CEventQueue::InitStaticsL();

//
	CWsAnimDll::InitStaticsL();
//
	TInt bootMode=0;
	_LIT(KWSERVIniFileVarReboot,"REBOOT");
	WsIniFile->FindVar(KWSERVIniFileVarReboot,bootMode);
	if (bootMode>=0 && bootMode<=2)
		iShellBootMode=bootMode;
//
	CWsBackedUpWindow::StaticInitL();
	CWsRedrawMsgWindow::StaticInitL();
//
	TWsPointer::InitStaticsL();
	iShellStarter=new (ELeave) CShellStarter;
	iShellStarter->ConstructL();
	_LIT(KPreProcess,"REMOVEFADINGONFOCUSGAIN");
	CWsWindowGroup::SetFocusGainPreprocessing(WsIniFile->FindVar(KPreProcess));
	_LIT(KAbsFade,"ABSOLUTEFADING");
	CWsClientWindow::SetAbsoluteFading(WsIniFile->FindVar(KAbsFade));

//Set the focus policy
	_LIT(KFocusPolicy,"MULTIFOCUSPOLICY");
	if(WsIniFile->FindVar(KFocusPolicy))
		{
		iMultiFocusPolicy = ETrue;	
		}
	RProcess::Rendezvous(KErrNone);
	// Start the shell from here unless the 'NoShell' option has been 
	// received indicating that the system starter will start the shell directly.
	if(startShell)
		{
		StartShell();
		}
	UserSvr::WsRegisterSwitchOnScreenHandling(ETrue);
	
	iRenderOrientationTracker = CWsRenderOrienationTracker::NewL();	
	}


void CWsTop::InitScreenL( TInt aScreenNumber)  // static
	{
	// create a new screen,  read ini file for aScreenNumber (this happens in CScreen - just need to pass a screen number from here
	CScreen* screen = new (ELeave) CScreen() ;
	CleanupStack::PushL( screen ) ;
	TRect digitiserArea;
	if (aScreenNumber==0)
		{
		TMachineInfoV1Buf machineBuf;
		UserHal::MachineInfo(machineBuf);
		TMachineInfoV1& machineInfo=*(TMachineInfoV1*)machineBuf.Ptr();
		digitiserArea = TRect(-machineInfo.iOffsetToDisplayInPixels,machineInfo.iXYInputSizeInPixels);
		}
	screen->ConstructL(digitiserArea, aScreenNumber);
	iScreens->AppendL( screen ) ; 
	CleanupStack::Pop( screen ) ;
	}

void CWsTop::ClearAllRedrawStores()
	{
	TInt ii;
	if (iScreens && iScreens->Count()>=iNumberOfScreens)
		{	//All screens are initialized
		for (ii=0; ii<iNumberOfScreens; ++ii)
			{
			CScreen *screen=(*iScreens)[ii];
			CWsWindowGroup* group=screen->RootWindow()->Child();
			while (group!=NULL)
				{
				CWsWindowBase* win=group->Child();
				if (win)
					{
					while (win!=group)
						{
						static_cast<CWsWindow*>(win)->Redraw()->ClearRedrawStore(ETrue);
						//On orientation switch, window is no longer present on screen
						static_cast<CWsWindow*>(win)->SetDrawnToScreen(EFalse);
						if (win->BaseChild())
							win=win->BaseChild();
						else
							{
							do	
								{
								if (win->NextSibling())
									{
									win=win->NextSibling();
									break;
									}
								win=win->BaseParent();
								}
							while (win!=group);
							}
						}
					}
				group=group->NextSibling();
				}
			TriggerRedraws(screen->RootWindow());
			}
		}
	}

void CWsTop::Exit()
	{
	CActiveScheduler::Stop();
	}

void CWsTop::TriggerRedraws(CWsRootWindow* aRootWindow)
	{
	for(CWsWindowGroup *group=aRootWindow->Child();group!=NULL;group=group->NextSibling())
		group->WsOwner()->TriggerRedraw();
	}

void CWsTop::StartShell()
	{
	TRAPD(err,iShell=new(ELeave) CWsShellLogon());
	if (err==KErrNone)
		{
		RFs fs;
		if ((err=fs.Connect())==KErrNone)
			{
			fs.SetNotifyUser(EFalse);
			TRAP(err,iShell->ConstructL(fs));
			fs.Close();
			}
		}
	if (err!=KErrNone)
		{
#ifdef _DEBUG
		RDebug::Print(_L("Failed to start shell: err=%d\n"),err);
#endif
		delete iShell;
		iShell=NULL;
		iShellStarter->After(TTimeIntervalMicroSeconds32(1000000));
		}
	}

void CWsTop::ShellExited()
	{
	delete iShell;
	iShell=NULL;
	switch(iShellBootMode)
		{
		case EShellBootModeReboot:
			StartShell();
			break;
		case EShellBootModeNoReboot:
			if (iServer->SessionCount()==0)
				StartShell();
			break;
		case EShellBootModeExit:
			Exit();
			break;
		}
	}

TInt CWsTop::SetSendOffEventsToShell(CWsClient *aClient,const TWsClCmdOffEventsToShell &aData)
	{
	CWsWindowBase *window=NULL;
	if (aData.window==0)
		{
		if (aData.on)
			aClient->PPanic(EWservPanicNoWindowSpecifed);
		}
	else
		aClient->HandleToWindow(aData.window,&window);
	if (aData.on)
		{
		if (iWindowToSendOffEventsTo!=NULL && aClient!=iShellClient)		//Allow the shell to pinch it
			return KErrAlreadyExists;
		iWindowToSendOffEventsTo=window;
		}
	else
		{
		if (iWindowToSendOffEventsTo==window || (!window && aClient==iWindowToSendOffEventsTo->WsOwner()))
			iWindowToSendOffEventsTo=NULL;
		}
	return KErrNone;
	}

void CWsTop::StopWindowGettingOffEvents(CWsWindowBase* aWindow)
	{
	if (aWindow==iWindowToSendOffEventsTo)
		iWindowToSendOffEventsTo=NULL;
	}

/** Routes "Off" events to the shutdown manager if one is registered. Othwerwise calls Power API's directly to
set the power mode of the device accordingly.
@internalTechnology
@released
@param aEvent Type of event that Wserv passes to client as OFF events. 
It can be EEventSwitchOff, EEventCaseClosed, EEventKeySwitchOff or EEventRestartSystem.
@param aDoSwitchOff ETrue if the switch-off should be performed when no shutdown manager is registered
and IGNORESWITCHOFFEVENT is not defined in wsini.ini
*/
void CWsTop::HandleSwitchOff(TInt aEvent,TBool aDoSwitchOff)
	{
	// If any shutdown manager is registered, forward the event to it
	if (iWindowToSendOffEventsTo && iWindowToSendOffEventsTo->QueueEvent(aEvent))
		return;
	// Otherwise, do the shutdown here
	if(!iIgnoreSwitchOffEvent && aDoSwitchOff)
		{
#ifdef SYMBIAN_PROCESS_MONITORING_AND_STARTUP		
		// if LaF is not registered, restart or shutdown using Power API 
		if (aEvent == EEventRestartSystem) // restart
			{
			if (Power::EnableWakeupEvents(EPwRestart) == KErrNone)
				{
				// Should reboot/power-cycle the system, so no need to RequestWakeupEventNotification
				Power::PowerDown();
				}
			}
		else // everything else maps to standby for now
			{
			if (Power::EnableWakeupEvents(EPwStandby) == KErrNone)
				{
				// Prepare to wake up from standby
				TRequestStatus s;
				Power::RequestWakeupEventNotification(s);
				Power::PowerDown();
				User::WaitForRequest(s);
				}
			}
#else
		UserHal::SwitchOff();
#endif			
		}
	}

void CWsTop::RedrawScreens()
	{
	// apply to all screens
	TInt screenNo;
	for(screenNo=0; screenNo<iNumberOfScreens; ++screenNo)
		{
		CScreen* screen = Screen(screenNo);
		TRegionFix<1> screenArea(TRect(screen->DrawableArea()));
		}
	}

void CWsTop::EnableLogging(TReloadWsIni aSetting)
	{
	TInt errorLoc=0;
	if (wsDebugLog)
		{
		if (wsDebugLogLevel==CDebugLogBase::ELogEverything)
			{
			wsDebugLogLevel=CDebugLogBase::ELogImportant;
			wsDebugLog->SetLoggingLevel(wsDebugLogLevel);
			return;
			}
		DisableLogging();
		}

	if(aSetting == EDoReloadWsIni)
	    {
	    CIniFile* temp = NULL;
	    TRAPD(err, temp = CIniFile::NewL());
	    if(err == KErrNone)
	        {
	        //loading successful, replace the previous one
	        delete WsIniFile;
	        WsIniFile = temp;
	        }
	    }
	
	TPtrC dlog;
	_LIT(KWSERVDebugLogFileName,"DLOG");
	TBuf<KMaxFileName> fname(KWSERVDebugLogFileName);
	_LIT(KWSERVIniFileVarLog,"LOG");
	if (WsIniFile->FindVar(KWSERVIniFileVarLog,dlog) && dlog.Length()==2)
		{
		fname.Append(dlog);
		TInt err=iDebugLib.Load(fname);
		if (err==KErrNone)
			{
			TUidType uid=iDebugLib.Type();
			if (uid[1]!=KWservLoggingDllUid)
				{
				err=KErrNotSupported;
				errorLoc=2;
				}
			else
				{
				TPtrC params;
				_LIT(KWSERVIniFileVarLogP,"LOGP");
				if (!WsIniFile->FindVar(KWSERVIniFileVarLogP,params))
					params.Set(NULL,0);
				CreateDebugLog func=(CreateDebugLog)iDebugLib.Lookup(1);
				if (func!=NULL)
					{
					TRAP(err,wsDebugLog=(*func)(iIsFirstLog, params));
					if (err==KErrNone)
						{
						iIsFirstLog=EFalse;
						wsDebugLogLevel=CDebugLogBase::ELogEverything;
						}
					else
						errorLoc=4;
					}
				else
					errorLoc=3;
				}
			}
		else
			errorLoc=1;
		if (iCurrentFocusScreen)
			{
			TWindowServerEvent::ProcessErrorMessages(TWsErrorMessage::ELogging,(errorLoc<<8)-err);
			}
		}
	}

void CWsTop::DisableLogging()
	{
	delete wsDebugLog;
	wsDebugLog=NULL;
	iDebugLib.Close();
	}

void CWsTop::LogCommand(RWsSession::TLoggingCommand aCommand)
	{
	switch(aCommand)
		{
	case RWsSession::ELoggingEnable:
		EnableLogging();
		break;
	case RWsSession::ELoggingDisable:
		DisableLogging();
		break;
	case RWsSession::ELoggingStatusDump:
		StateDump();
		break;
	case RWsSession::ELoggingHeapDump:
		HeapDump();
		break;
		}
	}

void CWsTop::StateDump()
	{
	TInt screenNo;
	for (screenNo=0; screenNo<iNumberOfScreens; ++screenNo)
		::StateDump( (*iScreens)[screenNo]->RootWindow());
	}
	
void CWsTop::SetCurrentFocusScreen(CScreen* aScreen)	
	{
	iCurrentFocusScreen = aScreen;
	}

TInt CWsTop::SetCurrentFocusScreen(TInt aScreenNo)
	{
	CScreen* newFocusScreen=CWsTop::Screen(aScreenNo);
	if (newFocusScreen==iCurrentFocusScreen)
		return KErrNone;

	CWsWindowGroup* newFocusGroup=newFocusScreen->FocusWindowGroup();
	if (!newFocusGroup)
		return KErrNotReady;

	CWsWindowGroup* oldFocusGroup=iCurrentFocusScreen->FocusWindowGroup();
	if (oldFocusGroup)
		oldFocusGroup->LostFocus();
	iCurrentFocusScreen=newFocusScreen;
	newFocusGroup->ReceivedFocus();
	TWsPointer::UpdatePointerCursor();
	TWindowServerEvent::SendFocusChangedEvents();
	return KErrNone;
	}

void CWsTop::SetCheckHeapOnDisconnectClient(const CWsClient* aClient)
	{
	iTriggerHeapCheckOnClientExit=aClient;
	}

void CWsTop::SetCheckHeapOnDisconnectMode(TWsCheckHeapOnDisconnectMode aCheckHeapOnDisconnectMode)
	{
	iHeapCheckMode=aCheckHeapOnDisconnectMode;
	}

TBool CWsTop::NeedsHeapCheckAndRestart(TInt aStartHeapCount)
	{
	if (!iDoHeapCheckAndRestart)
		return(EFalse);
	iDoHeapCheckAndRestart=EFalse;
	iCheckHeapResult=User::Heap().Count()-aStartHeapCount;
	if(iCheckHeapResult > 0)
	    {
	    const TUint32 orphanedCell = User::Heap().__DbgMarkEnd(aStartHeapCount);
	    RDebug::Printf("Memory leak detected in wserv. First orphaned cell: 0x%8x", orphanedCell);
	    }
	return(ETrue);
	}

TInt CWsTop::FetchCheckHeapResult()
	{
	TInt ret=iCheckHeapResult;
	iCheckHeapResult=KErrNotReady;
	return(ret);
	}

/**
This function looks for memory which isn't essential to the correct running of the window server
and frees some of it.
Returns true if some memory was freed, and false if it was unable to free anything.
Called from the memory manager in low memory conditions.
*/
TBool CWsTop::ReleaseMemory()
	{
	return iServer->ReleaseMemory();
	}
	
TBool CWsTop::MultiFocusPolicy()
	{
	return	iMultiFocusPolicy;
	}

void CWsTop::ClearSurfaceMap(CWsClient *aClient)
	{
	TInt tempScreens = CWsTop::NumberOfScreens();
	for (TInt ii = 0; ii < tempScreens; ii++)
		{
		__DEBUG_ONLY(TInt err=)
		CWsTop::Screen(ii)->SurfaceMap()->RemoveAll(*aClient);
		WS_ASSERT_DEBUG((err==KErrNone||err==KErrNotFound||err==KErrInUse), EWsPanicSurfaceMapError);
		}
	}

TBool CWsTop::SearchDuplicateSurfaceId(const TSurfaceId& aSurfaceId)
	{
	TInt tempScreens = CWsTop::NumberOfScreens();
	for (TInt ii = 0; ii < tempScreens; ii++)
		{
		if (CWsTop::Screen(ii)->WindowElements().SearchDuplicateSurfaceId(aSurfaceId))
			{
			return ETrue;
			}
		}
	return EFalse;
	}

/**
Checks to see if the render orientation has changed, and publishes any new orientaion
via publish and subscribe

@see KRenderOrientationCategory
@see KRenderOrientationKey 
*/
void CWsTop::CheckRenderOrientation()
    {
    iRenderOrientationTracker->CheckRenderOrientation();
    }

typedef TInt (*ShellEntryPoint)(TAny *);

#if defined(__WINS__)
LOCAL_D const TUint KHeapSize=0x10000;
LOCAL_D const TUint KMaxHeapSize=0x400000;
#endif

CWsShellLogon::CWsShellLogon() : CActive(EWsShellLogonPriority)
	{
#if defined (__WINS__)
	// Clear handles to NULL so we can later detect which one gets used
	iShellThread.SetHandle(NULL);
	iShellProcess.SetHandle(NULL);
#endif
	}

CWsShellLogon::~CWsShellLogon()
	{
	Cancel();
#if defined (__WINS__)
	iShellThread.Close();
	iShellProcess.Close();
#else
	iShell.Close();
#endif
#if defined(__WINS__)
	iLib.Close();
#endif
	}

void CWsShellLogon::ConstructL(RFs &)
	{
	CActiveScheduler::Add(this);
	TPtrC shellCmd;

	_LIT(KWSERVIniFileVarShellCmd,"SHELLCMD");
	WsIniFile->FindVar(KWSERVIniFileVarShellCmd,shellCmd);
	_LIT(KWSERVShellName,"SHELL");
	TPtrC startUp(KWSERVShellName);
	_LIT(KWSERVIniFileVarStartUp,"STARTUP");
	WsIniFile->FindVar(KWSERVIniFileVarStartUp,startUp);


	TParse fileName;
	_LIT(KWSERVArmShellLocationPattern,"Z:\\sys\\bin\\.EXE");
	User::LeaveIfError(fileName.SetNoWild(startUp,&KWSERVArmShellLocationPattern,NULL));
#if defined(__WINS__)
	TInt err=iShellProcess.Create(fileName.FullName(),shellCmd);
	if (err == KErrNone)
		{
		Request();
		iShellProcess.Resume();
		}
	else
		{
		// Try loading the matching DLL name instead?
		_LIT(KWSERVShellExtension,"Z:.DLL");
		User::LeaveIfError(fileName.Set(KWSERVShellExtension,&startUp,NULL));
		User::LeaveIfError(iLib.Load(fileName.FullName()));
		ShellEntryPoint libFunc=(ShellEntryPoint)iLib.Lookup(1);
		if (!libFunc)
			User::Leave(KErrGeneral);
		TBuf<256> name;
		TInt num=0;
		TInt ret=KErrNone;
		do
			{
			_LIT(KWSERVWinsShellInstanceName,"Shell%d");
			name.Format(KWSERVWinsShellInstanceName,num++);
			ret=iShellThread.Create(name,libFunc,KDefaultStackSize,&shellCmd,&iLib,NULL,KHeapSize,KMaxHeapSize,EOwnerProcess);
			} while(ret==KErrAlreadyExists);
		User::LeaveIfError(ret);
		Request();
		iShellThread.Resume();
		}
#else // not __WINS__
	User::LeaveIfError(iShell.Create(fileName.FullName(),shellCmd));
	Request();
	iShell.Resume();
#endif	
	return;

	}

void CWsShellLogon::DoCancel()
	{
#if defined (__WINS__)
	if(iShellThread.Handle())
		iShellThread.LogonCancel(iStatus);
	else
		iShellProcess.LogonCancel(iStatus);
#else
	iShell.LogonCancel(iStatus);
#endif
	}

void CWsShellLogon::RunL()
	{
	if (iStatus.Int()!=KErrCancel)
		CWsTop::ShellExited();
	}

void CWsShellLogon::Request()
	{
#if defined (__WINS__)
	if(iShellThread.Handle())
		iShellThread.Logon(iStatus);
	else
		iShellProcess.Logon(iStatus);
#else
	iShell.Logon(iStatus);
#endif
	SetActive();
	}

#if defined(__WINS__) 
TFullName CWsShellLogon::FullName()
	{
	if(iShellThread.Handle())
		return(iShellThread.FullName());
	else
		return(iShellProcess.FullName());
	}
#else
TFullName CWsShellLogon::FullName()
	{return(iShell.FullName());}
#endif	

TInt E32Main()
	{
	__UHEAP_MARK;

	UserSvr::WsRegisterThread();

	RThread thread;
	// Set wserv's main thread to system permanent
	TInt err;
	err = User::SetCritical(User::ESystemPermanent);
	if (err!=KErrNone)
		{
		WS_PANIC_ALWAYS(EWsPanicFailedToSetThread);
		}
// If in the future wserv becomes multi-threaded,
// we can uncomment the following lines to set any new threads to be system permanent as well.
//	err = User::SetProcessCritical(User::ESystemPermanent); 
//	if (err!=KErrNone)
//		{
//		WS_PANIC_ALWAYS(EWsPanicFailedToSetProcess);
//		}

	thread.SetPriority(EPriorityMore);
	err = User::RenameThread(KWSERVThreadName);
	if (err==KErrNone)
		{
		thread.Close();
		}
	else
		{
		return err;
		}

	CTrapCleanup* CleanUpStack=CTrapCleanup::New();	
	TRAP(err, WsIniFile = CIniFile::NewL());
	if (err!=KErrNone)
		{
		WS_PANIC_ALWAYS(EWsPanicFailedToInitialise);
		}	
		
	_LIT(KMemLeakCheck, "MEMLEAKCHECK");
	if (WsIniFile->FindVar(KMemLeakCheck))
		{
		RDebug::Printf("The Memory Leak Check is Enabled => ECOM plugins are preload");
		CWsPluginManager* pm = NULL;
		TRAP(err,pm = CWsPluginManager::NewL());
		delete pm;
		if (err!=KErrNone)
			{
			WS_PANIC_ALWAYS(EWsPanicFailedToInitialise);
			}
		}

	// Define properties for singleton callbacks. This must only be done ONCE
	// to ensure the properties can't be hijacked.
	gReleaseSingletonsOnExit = ETrue;
	DefineSingletonKey(KOpenWfcInteropCleanupKey);
	DefineSingletonKey(KOpenWfcImplCleanupKey);
	
	TInt startCount;
	do
		{		
		User::Heap().__DbgMarkStart();
		startCount=User::Heap().Count();
		TRAP(err,CWsTop::RunServerL());
		if (err!=KErrNone)
			{
			WS_PANIC_ALWAYS(EWsPanicFailedToInitialise);
			}
		UserSvr::ReleaseEventHook();						
		} while	(CWsTop::NeedsHeapCheckAndRestart(startCount));
		
	REComSession::FinalClose(); // Now we can unload the plugins' dlls
	delete WsIniFile;
	delete CleanUpStack;		
	
	__UHEAP_MARKEND;
	return(err);
	}


