// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#ifndef CONTROLLERINITIALISATIONPLUGINIMPL_H
#define CONTROLLERINITIALISATIONPLUGINIMPL_H

#include <bluetooth/hci/controllerinitialisationplugin.h>
#include <bluetooth/hci/controllerinitialisationinterface.h>
#include <bluetooth/hcicommandqueueclient.h>

class TReadLocalVersionInfoCompleteEvent;
class TVendorDebugCompleteEvent;
class CCommandParser;
class MCoreHci;

/**
	Observer class for CSleepDisableTimer
*/
class MSleepDisableTimerEventObserver
	{
public:
   	virtual void SleepDisableTimerExpired() = 0;
	};  
	
/**
	Timer class used to introduce a delay before sending the
	Sleep Disable command
*/
class CSleepDisableTimer : public CTimer
	{
public:
	static CSleepDisableTimer* NewL(MSleepDisableTimerEventObserver& aTimerObserver);
	void Start();
	
private:
	CSleepDisableTimer(MSleepDisableTimerEventObserver& aTimerObserver);
	void ConstructL();

	void RunL();

private:
	// 0.5 second timer.
	static const TInt KSleepDisableTimerDuration = 500000;
	
	MSleepDisableTimerEventObserver& iTimerObserver;
	};
	
   
class CControllerInitialisationPluginImpl : public CControllerInitialisationPlugin,
											public MControllerInitialisationInterface,
											public MHCICommandQueueClient,
											public MSleepDisableTimerEventObserver
	{
private:	
	enum TInitState
		{
		EIdle,
		EWaitLocalDeviceInfo,
		EWaitForSleepTimerExpiry,
		EWaitSleepDisableCompleteEvent,
		ESendScriptCommands,
		};

public:
	static CControllerInitialisationPluginImpl* NewL();
	virtual ~CControllerInitialisationPluginImpl();
	virtual TAny* Interface(TUid aUid);

protected:
	CControllerInitialisationPluginImpl();

private:
	void ConstructL();

	// Static async CallBack methods.
	static TInt AsyncCallBackForPreReset(TAny* aInitPlugin);
	static TInt AsyncCallBackForPostReset(TAny* aInitPlugin);

	void DoPreReset();
	void DoPostReset();

	void SendNextScriptFileCommandL();

	// MControllerInitialisationInterface	
	virtual void MciiPreResetCommand();
	virtual void MciiPostResetCommand();
	virtual void MciiSetHCICommandQueue(MHCICommandQueue& aHCICommandQueue);
	virtual void MciiSetControllerInitialisationObserver(MControllerInitialisationObserver& aObserver);
	virtual void MciiSetCoreHci(MCoreHci& aCoreHci);
	
	// MHCICommandQueueClient
	virtual void MhcqcCommandEventReceived(const THCIEventBase& aEvent,
										   const CHCICommandBase* aRelatedCommand);
	virtual void MhcqcCommandErrored(TInt aErrorCode,
									 const CHCICommandBase* aCommand);
	// MSleepDisableTimerEventObserver
	virtual void SleepDisableTimerExpired();									 

	void HandleReadLocalVersionInfoCompleteEvent(const TReadLocalVersionInfoCompleteEvent& aEvent);	
	void HandleVendorSpecificCompleteEvent(const TVendorDebugCompleteEvent& aEvent);

	// Helper methods.
	void SetLocalBDAddr(TDes8& aVSCommandData) const;
	void SetSleepProtocol(TBool aEnabled);
		
private:
	// Async Callbacks
	CAsyncCallBack* iAsyncCallBackForPreReset;
	CAsyncCallBack* iAsyncCallBackForPostReset;

 	CCommandParser* iCommandParser;	
 	TInitState iInitState;			

 	MHCICommandQueue* iHCICommandQueue; 	
 	MControllerInitialisationObserver* iControllerInitialisationObserver;

 	TUint32	iDesiredBaudRate;
 	CSleepDisableTimer* iSleepDisableTimer;
	
	TUint16 iSubVersion;

	MCoreHci* iCoreHci;
	};

#endif //HCINITIALISATIONPLUGINIMPL_H
