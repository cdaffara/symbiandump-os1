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

#include "ControllerInitialisationPluginImpl.h"
#include "ControllerInitialisationCmndParser.h"
#include "tiinitialisorutils.h"

#include <bluetooth/hci/vendordebugcommand.h>
#include <bluetooth/hci/controllerinitialisationobserver.h>
#include <bluetooth/hcicommandqueue.h>

#include <bluetooth/hci/readlocalversioninfocommand.h>

#include <bluetooth/hci/commandcompleteevent.h>
#include <bluetooth/hci/readlocalversioninfocompleteevent.h>
#include <bluetooth/hci/vendordebugcompleteevent.h>

#include <bluetooth/hci/hctlconfiginterface.h>
#include <bluetooth/hci/corehci.h>

#include <bluetooth/logger.h>

#include <es_sock.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_INITIALISOR_TI);
#endif

_LIT(KScriptPath, "z:\\private\\101f7989\\Bluetooth\\");
_LIT(KScriptPathFileFormat, "%Stiinit_%d.%d.%d.bts");

const TUint16 KHciVSUpdateUartHCIBaudRate = 0xFF36;
const TUint16 KHciVSEnableSleepMode = 0xFF0C;
const TUint16 KHciVSSleepModeConfig = 0xFD0C; // BRF6300 version
const TUint16 KHciVSSetBDAddr = 0xFC06;

const TUint16 KProjectTypeMask = 0x7C00;	// mask needed to extract the project type
const TUint16 KProjectTypeShift = 10;		// shift needed to extract the project type
const TUint16 KMinorVersionMask = 0x007F;
const TUint16 KMajorVersionMask = 0x0380;	// mask needed to extract the major version
const TUint16 KMajorVersionShift = 7;		// shift needed to extract the major version
const TUint16 KBit15Mask = 0x8000;
const TUint16 KBit15Shift = 12;				// shift needed to move bit15 to bit3
											// (it will be pre-pended as an extra hi-bit for
											// the 3-bit major version number)


/*static*/ CControllerInitialisationPluginImpl* CControllerInitialisationPluginImpl::NewL()
	{
	LOG_STATIC_FUNC

	CControllerInitialisationPluginImpl* self = new (ELeave) CControllerInitialisationPluginImpl();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CControllerInitialisationPluginImpl::ConstructL()
	{
	LOG_FUNC

	// Add Async Callbacks
	TCallBack preResetCallBack(AsyncCallBackForPreReset, this);
	iAsyncCallBackForPreReset = new (ELeave)CAsyncCallBack(preResetCallBack, CActive::EPriorityStandard);
	TCallBack postResetCallBack(AsyncCallBackForPostReset, this);
	iAsyncCallBackForPostReset = new (ELeave)CAsyncCallBack(postResetCallBack, CActive::EPriorityStandard);

	iSleepDisableTimer = CSleepDisableTimer::NewL(*this);
	}
	
CControllerInitialisationPluginImpl::CControllerInitialisationPluginImpl()
  : iInitState(EIdle)
	{
	LOG_FUNC
	}

CControllerInitialisationPluginImpl::~CControllerInitialisationPluginImpl()
	{
	LOG_FUNC

	delete iCommandParser;
	// Delete async CallBacks.	If running, these should be cancelled by the
	// d'tor of CAsyncOneShot.
	delete iAsyncCallBackForPreReset;
	delete iAsyncCallBackForPostReset;		
	delete iSleepDisableTimer;
	}

	
/*virtual*/ TAny* CControllerInitialisationPluginImpl::Interface(TUid aUid)
	{	
	LOG_FUNC

	TAny* ret = NULL;
	switch(aUid.iUid)
		{
		case KControllerInitialisationInterfaceUid:
			ret = reinterpret_cast<TAny*>(static_cast<MControllerInitialisationInterface*>(this));
			break;

		default:
			break;
		};
	return ret;
	}

// Static async CallBack methods.
/*static*/ TInt CControllerInitialisationPluginImpl::AsyncCallBackForPreReset(TAny* aInitPlugin)
	{
	LOG_STATIC_FUNC

	CControllerInitialisationPluginImpl* initPlugin = static_cast<CControllerInitialisationPluginImpl*>(aInitPlugin);
	initPlugin->DoPreReset();
	return EFalse;	// Don't call back any more.
	}
	
/*static*/ TInt CControllerInitialisationPluginImpl::AsyncCallBackForPostReset(TAny* aInitPlugin)
	{
	LOG_STATIC_FUNC

	CControllerInitialisationPluginImpl* initPlugin = static_cast<CControllerInitialisationPluginImpl*>(aInitPlugin);
	initPlugin->DoPostReset();
	return EFalse;	// Don't call back any more.
	}

void CControllerInitialisationPluginImpl::DoPreReset()
	{
	LOG_FUNC

	iInitState = EIdle;
	
	// Inform stack that we have finished	
	iControllerInitialisationObserver->McioPreResetCommandComplete(KErrNone);
	}

void CControllerInitialisationPluginImpl::DoPostReset()
	{
	LOG_FUNC

	// Request local version information.
	CReadLocalVersionInfoCommand* cmd = NULL;
	TRAPD(err, cmd = CReadLocalVersionInfoCommand::NewL());
	if(err == KErrNone)
		{
		TRAP(err, iHCICommandQueue->MhcqAddInitCommandL(cmd, *this));
		}
		
	if(err != KErrNone)
		{
		// Inform the stack of the initialisation status.
		iControllerInitialisationObserver->McioPostResetCommandComplete(err);
		}
	iInitState = EWaitLocalDeviceInfo;
	}

/*virtual*/ void CControllerInitialisationPluginImpl::MciiPreResetCommand()
	{
	LOG_FUNC

	__ASSERT_DEBUG(iControllerInitialisationObserver, PANIC(KTiControllerInitialisorPanic, ENullInitialisationObserver));

	// Use async callback
	iAsyncCallBackForPreReset->CallBack();	
	}

/*virtual*/ void CControllerInitialisationPluginImpl::MciiPostResetCommand()
	{	
	LOG_FUNC

	__ASSERT_DEBUG(iControllerInitialisationObserver, PANIC(KTiControllerInitialisorPanic, ENullInitialisationObserver));
	
	// Use async callback
	iAsyncCallBackForPostReset->CallBack();
	}

/*virtual*/ void CControllerInitialisationPluginImpl::MciiSetHCICommandQueue(MHCICommandQueue& aHCICommandQueue)
	{
	LOG_FUNC

	__ASSERT_DEBUG(iHCICommandQueue == 0, PANIC(KTiControllerInitialisorPanic, EHciCommandQueueOverwritten));
	iHCICommandQueue = &aHCICommandQueue;
	}

/*virtual*/ void CControllerInitialisationPluginImpl::MciiSetControllerInitialisationObserver(MControllerInitialisationObserver& aObserver)
	{
	LOG_FUNC

	__ASSERT_DEBUG(iControllerInitialisationObserver == 0, PANIC(KTiControllerInitialisorPanic, EInitialisationObserverOverwritten));
	iControllerInitialisationObserver = &aObserver;
	}

/*virtual*/ void CControllerInitialisationPluginImpl::MciiSetCoreHci(MCoreHci& aCoreHci)
	{
	LOG_FUNC

	__ASSERT_DEBUG(iCoreHci == 0, PANIC(KTiControllerInitialisorPanic, ECoreHciOverwritten));
	iCoreHci = &aCoreHci;
	}

void CControllerInitialisationPluginImpl::SendNextScriptFileCommandL()
	{
	LOG_FUNC

	// Create default Vendor command.
	CVendorDebugCommand* cmd = CVendorDebugCommand::NewL(KVendorDebugOGF);
	CleanupStack::PushL(cmd);
	LEAVEIFERRORL(iCommandParser->GetNextVendorCommand(*cmd));

	// Make any command specific changes.
	switch(cmd->Opcode())
		{
		case KHciVSUpdateUartHCIBaudRate:
			{
			iDesiredBaudRate = LittleEndian::Get32(cmd->Command().Ptr());
			}
			break;

		case KHciVSSetBDAddr:
			{
			SetLocalBDAddr(cmd->Command());
			}
			break;

		default:
			// Nothing to do.
			break;			
		};
		
		
	// Send the command.
	CleanupStack::Pop();
	iHCICommandQueue->MhcqAddInitCommandL(cmd, *this);
	}

void CControllerInitialisationPluginImpl::MhcqcCommandEventReceived(const THCIEventBase& aEvent,
																	const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC

	if (aEvent.EventCode() == ECommandCompleteEvent)
		{
		// Switch on the opcode.
		THCICommandCompleteEvent& event = THCICommandCompleteEvent::Cast(aEvent);
		switch(event.CommandOpcode())
			{
			case KReadLocalVersionInfoOpcode:
				{
				TReadLocalVersionInfoCompleteEvent& lvEvent = TReadLocalVersionInfoCompleteEvent::Cast(event);
				HandleReadLocalVersionInfoCompleteEvent(lvEvent);
				}
				break;

			default:
				// Check for VS commands.
				if((event.CommandOpcode() & KVendorDebugOGF) == KVendorDebugOGF)
					{
					TVendorDebugCompleteEvent& vEvent = TVendorDebugCompleteEvent::Cast(event);
					HandleVendorSpecificCompleteEvent(vEvent);
					}						
				break;	
			}
		}
	}

void CControllerInitialisationPluginImpl::HandleReadLocalVersionInfoCompleteEvent(const TReadLocalVersionInfoCompleteEvent& aEvent)
	{
	LOG_FUNC
	
	TInt err = KErrNone;				// innocent until proved guilty
	
	if (iInitState == EWaitLocalDeviceInfo)
		{
		if (aEvent.ErrorCode() != EOK)
			{
			err = KHCIErrorBase - aEvent.ErrorCode();
			}
		else
			{
			iSubVersion = aEvent.LMPSubversion();
			
			// Attempt to open the script file.
			TFileName fileName;
			fileName.Format(KScriptPathFileFormat,
							&KScriptPath,
			                ((iSubVersion & KProjectTypeMask) >> KProjectTypeShift),
			                ((iSubVersion & KMajorVersionMask) >> KMajorVersionShift) | ((iSubVersion & KBit15Mask) >> KBit15Shift),
			                (iSubVersion & KMinorVersionMask));

			TRAP(err, iCommandParser = CCommandParser::NewL(fileName));
			}
		if(err == KErrNone)
			{
			iSleepDisableTimer->Start();
			iInitState = EWaitForSleepTimerExpiry;
			}
		else
			{
			iControllerInitialisationObserver->McioPostResetCommandComplete(err);
			iInitState = EIdle;
			}
		}
	}

void CControllerInitialisationPluginImpl::HandleVendorSpecificCompleteEvent(const TVendorDebugCompleteEvent& aEvent)
	{
	LOG_FUNC

	switch(iInitState)
		{
		case ESendScriptCommands:
			{
			switch(aEvent.CommandOpcode())
				{
				case KHciVSUpdateUartHCIBaudRate:
					{
					MHctlConfigInterface* configIf = static_cast<MHctlConfigInterface*>(iCoreHci->MchHctlPluginInterface(TUid::Uid(KHctlConfigInterfaceUid)));
					__ASSERT_DEBUG(configIf, PANIC(KTiControllerInitialisorPanic, EConfigInterfaceNull));

					TInt err = configIf->MhciUpdateBaudRate(iDesiredBaudRate);
					if(err != KErrNone)
						{
						iControllerInitialisationObserver->McioPostResetCommandComplete(err);
						}
					}
					break;

				case KHciVSEnableSleepMode:
				case KHciVSSleepModeConfig:
					{
					SetSleepProtocol(EFalse);
					}
					break;
					
				default:
					break;
				};

			// Send the next command.
			TRAPD(err, SendNextScriptFileCommandL());
			// End of File is not an error that needs to be reported.
			if(err == KErrEof)
				{
				// All commands have been sent.
				iControllerInitialisationObserver->McioPostResetCommandComplete(KErrNone);
				}
			else if(err != KErrNone)
				{
				// Inform the stack of the initialisation status.
				iControllerInitialisationObserver->McioPostResetCommandComplete(err);
				}
			}
			break;
			
		case EWaitSleepDisableCompleteEvent:
			{
			TUint16 opcode = aEvent.CommandOpcode();
			if(opcode == KHciVSEnableSleepMode ||
			   opcode == KHciVSSleepModeConfig)
				{
				SetSleepProtocol(EFalse);

				// Response received.  Start sending commands from the script file.
				iInitState = ESendScriptCommands;
				
				TRAPD(err, SendNextScriptFileCommandL());
				// End of File is not an error that needs to be reported.
				if(err == KErrEof)
					{
					// All commands have been sent.
					iControllerInitialisationObserver->McioPostResetCommandComplete(KErrNone);
					}
				else if(err != KErrNone)
					{
					// Inform the stack of the initialisation status.
					iControllerInitialisationObserver->McioPostResetCommandComplete(err);
					}
				}
			}
			break;
		};
	}
	
void CControllerInitialisationPluginImpl::MhcqcCommandErrored(TInt aErrorCode,
					  										  const CHCICommandBase* /*aCommand*/)
	{
	LOG_FUNC

	switch(iInitState)
		{
		case EWaitLocalDeviceInfo:
		case ESendScriptCommands:
			{
			// Inform the stack of the initialisation failure.
			iControllerInitialisationObserver->McioPostResetCommandComplete(aErrorCode);
			}
			break;

		default:
			PANIC(KTiControllerInitialisorPanic, EInvalidInitialisorState);
			break;
		};
	}


void CControllerInitialisationPluginImpl::SleepDisableTimerExpired()
	{
	LOG_FUNC

	TInt err = KErrNone;
	CVendorDebugCommand* cmd = NULL;
	SetSleepProtocol(EFalse);
	
	// Send the sleep disable command.
	if(((iSubVersion & KProjectTypeMask) >> KProjectTypeShift) >= 4)
		{
		TRAP(err, cmd = CVendorDebugCommand::NewL(KHciVSSleepModeConfig));
		if(err == KErrNone)
			{
			// Populate the command.
			TDes8& cmdData = cmd->Command();
			cmdData.FillZ(9);
			cmdData[3] = 0xff;
			cmdData[4] = 0xff;
			cmdData[5] = 0xff;
			}
		}
	else
		{
		TRAP(err, cmd = CVendorDebugCommand::NewL(KHciVSEnableSleepMode));
		if(err == KErrNone)
			{
			// Populate the command.
			TDes8& cmdData = cmd->Command();
			cmdData.FillZ(9);
			}
		}

	if(err == KErrNone)
		{
		TRAP(err, iHCICommandQueue->MhcqAddInitCommandL(cmd, *this));
		}
		
	if(err != KErrNone)
		{
		// Inform the stack of the initialisation status.
		iControllerInitialisationObserver->McioPostResetCommandComplete(err);
		}
	iInitState = EWaitSleepDisableCompleteEvent;
	}

void CControllerInitialisationPluginImpl::SetLocalBDAddr(TDes8& /*aVSCommandData*/) const
	{
	LOG_FUNC

	// Update the address from the script file if required.
	}

void CControllerInitialisationPluginImpl::SetSleepProtocol(TBool /*aEnabled*/)
	{
	LOG_FUNC

	// Adjust the controller sleep protocol.
	}
	
//
//
CSleepDisableTimer* CSleepDisableTimer::NewL(MSleepDisableTimerEventObserver& aTimerObserver)
	{
	LOG_STATIC_FUNC

   	CSleepDisableTimer* self = new (ELeave) CSleepDisableTimer(aTimerObserver);
   	CleanupStack::PushL(self);
   	self->ConstructL();
   	CleanupStack::Pop(self);
   	return self;
	}

void CSleepDisableTimer::ConstructL()
	{
	LOG_FUNC

   	CTimer::ConstructL();
   	CActiveScheduler::Add(this);
	}

CSleepDisableTimer::CSleepDisableTimer(MSleepDisableTimerEventObserver& aTimerObserver)
  : CTimer(EPriorityStandard),
    iTimerObserver(aTimerObserver)
	{
	LOG_FUNC
	}

void CSleepDisableTimer::Start()
	{
	LOG_FUNC

	if(!IsActive())
		{
		After(KSleepDisableTimerDuration);
		}
	}
	
void CSleepDisableTimer::RunL()
	{
 	LOG_FUNC

  	iTimerObserver.SleepDisableTimerExpired();
   	}
