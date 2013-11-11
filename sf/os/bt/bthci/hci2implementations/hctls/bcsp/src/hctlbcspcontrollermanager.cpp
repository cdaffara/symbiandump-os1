// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "hctlbcspcontrollermanager.h"

#include "debug.h"
#include "hctlbcsp.h"
#include "bcsputils.h"

#include <bluetooth/hci/controllerstateobserver.h>
#include <bluetooth/hci/hctluartpowermanager.h>

// BlueCore Commands - Must be 18 bytes in length, payload is treated in 16bit units.
//                            |Command |Length |SeqNum |BC Cmd |             Payload                   |
//                            | SetReq |9 units|  N/a  | Reset | Pad                                   |
_LIT8(KBcCmdColdResetCommand, "\x02\x00\x09\x00\x00\x00\x01\x40\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00");
//                            | SetReq |9 units|  N/a  | Halt  | Pad                                   |
_LIT8(KBcCmdColdHaltCommand,  "\x02\x00\x09\x00\x00\x00\x03\x40\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00");


CHCTLBcspControllerManager* CHCTLBcspControllerManager::NewL(CHCTLBcsp& aHCTLBcsp, RBusDevComm& aPort,
		CHCTLUartBase::TPowerControlDetectionMode aPwrCtrlMode)
    {
	LOG_STATIC_FUNC

    CHCTLBcspControllerManager* self = new(ELeave) CHCTLBcspControllerManager(aHCTLBcsp, aPort, aPwrCtrlMode);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
    return self;
    }

CHCTLBcspControllerManager::~CHCTLBcspControllerManager()
    {
	LOG_FUNC

 	delete iPowerDownCallback;
	delete iReadyToResetControllerCallback;
	delete iUartPowerManager;
	}

CHCTLBcspControllerManager::CHCTLBcspControllerManager(CHCTLBcsp& aHCTLBcsp, RBusDevComm& aPort,
		CHCTLUartBase::TPowerControlDetectionMode aPwrCtrlMode)
 :	iPort(aPort),
 	iHCTLBcsp(aHCTLBcsp), 
 	iControllerManagerState(EIdle),
 	iLastPowerRequest(EBTOn),
 	iCurrentPowerState(EBTOn),
	iCurrentTask(ENoTask),
	iPwrCtrlMode(aPwrCtrlMode)
	{
	LOG_FUNC
    }

void CHCTLBcspControllerManager::ConstructL()
	{
	LOG_FUNC

	iColdResetCommand = KBcCmdColdResetCommand();
	iColdHaltCommand = KBcCmdColdHaltCommand();
	
	TCallBack cbHandlePowerDown(HandlePowerDown, this);
	iPowerDownCallback = new(ELeave) CAsyncCallBack(cbHandlePowerDown, CActive::EPriorityStandard);
	
	TCallBack cbHandleReadyToResetController(HandleReadyToResetController, this);
	iReadyToResetControllerCallback = new(ELeave) CAsyncCallBack(cbHandleReadyToResetController, CActive::EPriorityStandard);

	// if the ini file is configured to use CTS line to control the 
	// device's power on/off control than we instantiate iUartPowerManager.
	// otherwise it remains NULL (as implicitly set by CBase base class).
	// So, in the destructor, we can delete it directly. 
	if (iPwrCtrlMode == CHCTLUartBase::EPwrCtrlCTSTimedLow)
		{
		iUartPowerManager = CHCTLUartPowerManager::NewL(*this, iPort);
		iUartPowerManager->Start();
		}
	}


void CHCTLBcspControllerManager::Start()
/*
For future use - called when HCTL is started
*/
	{
	LOG_FUNC
	}

void CHCTLBcspControllerManager::MhupoPowerChange(TInt aError)
	{
	LOG_FUNC

	if (aError == KErrNone)
		{
		// if there is a task underway then the observer will be notified elsewhere
		if (iCurrentTask == ENoTask)
			{
			// unsolicited power change			
			iCurrentPowerState = (iCurrentPowerState == EBTOn) ? EBTOff : EBTOn;
			LOG1(_L8("Unsolicited power change detected, current power state: %d"), iCurrentPowerState);

			if(iObserver)
				{
				iObserver->McsoProcessPowerChange(KErrNone, MControllerStateObserver::EBTFatalChange, iCurrentPowerState);
				}
			}
		}
	else
		{
		// If we get an error from the uart power manager then just log
		// error but do not attempt to restart.
		LOG1(_L8("Error detecting power change: %d"), aError);
		}
	}

TInt CHCTLBcspControllerManager::MhpiGetPower(TBTPowerState& aState)
	{
	if(iCurrentTask != ENoTask)
		//Information will not be reliable
		{
		return KErrInUse;
		}

	aState = iCurrentPowerState;
	return KErrNone;
	}
	
TInt CHCTLBcspControllerManager::MhpiSetPower(TBTPowerState aState)
/*
To be called from outside the class only, for example in response to user request
*/
	{
	TInt rerr = KErrNone;
	
	switch(iCurrentTask)
		{
		case EPowerUp:
		case EPowerDown:
			//If SetPower is called whilst performing a power request error the BT client. 
			//There should only be one!
			rerr = KErrInUse;
			break;
			
		case EControllerReset:
			//Just postpone power request. A power down will be handled when
			//the next link establshment comes in.
			iLastPowerRequest = aState;
			rerr = KErrInUse;
			break;
			
		case ENoTask:
			iLastPowerRequest = aState;
			if(iCurrentPowerState == iLastPowerRequest)
				{
				//Nothing to do - need to return an error to complete synchronously
				//(Do not tell stack.)
				rerr = KErrAlreadyExists;
				}
			else
				{
				rerr = DoSetPower(aState);
				}
			break;
			
		default:
			PANIC(KBcspPanicCat, EInvalidCurrentTask);
		};
	
	return rerr;	
	}
	
TInt CHCTLBcspControllerManager::DoSetPower(TBTPowerState aState)
	{
	switch(aState)
		{
		case EBTOff:
			{			
			iPowerDownCallback->CallBack();	//allows asynch callback to BTClient.
			iCurrentTask = EPowerDown;
			iControllerManagerState = EResetHardware;
			iHCTLBcsp.Choke();
			}
			break;

		case EBTOn:
			{
			iHCTLBcsp.UnChoke();
			iHCTLBcsp.ResetMuzzled();  // Reset the muzzled parameter of the BT host
			iHCTLBcsp.WriteBcCmd(iColdResetCommand);
			iCurrentTask = EPowerUp;
			iControllerManagerState = EResetHardware;
			}
			break;

		default:
			PANIC(KBcspPanicCat, EUnexpectedPowerState);
			break;
		};
		
	return KErrNone;
	}

void CHCTLBcspControllerManager::HardReset()
	{
	// Only perform reset if the power is on and the controller is
	// not performing another action.
	if(iLastPowerRequest == EBTOn && iCurrentTask == ENoTask)
		{
		/*
		This implementation assumes a dedicated controller reset will be used.
		However we provide an example of how a power cycle reset might be implemented.
		*/
		iReadyToResetControllerCallback->CallBack();//call back will call reset on controller
		iCurrentTask = EControllerReset;
		iControllerManagerState = EWaiting;
		}
	}

/*
	Called when BCSP state reaches 'garrulous'. 
	Controller Manager may not want BCSP to inform the outside world, 
	for example, if it wishes to do a controller reset, and 
	so wishes to manage the controller itself and keep the outside world at bay.
*/
TBool CHCTLBcspControllerManager::BcspLinkEstablished()
	{
	if(iLastPowerRequest == EBTOff)
		/*
		Should only get here if a power off request occurred whilst 
		awaiting this link establishment - in which case abandon
		whatever we were doing and switch power off.
		*/
		{
		DoSetPower(EBTOff);
		return EFalse;
		}

	TBool doTellStack = ETrue;

	switch(iCurrentTask)
		{
		case ENoTask:
			break; //not to do with us
		case EPowerUp:
			{
			doTellStack = DoBcspLinkEstablishedForPowerUp();
			}
			break;
		case EControllerReset:
			{
			doTellStack = DoBcspLinkEstablishedForControllerReset();
			}
			break;
		case EPowerDown:
		default:
			{
	    	__ASSERT_DEBUG(EFalse, PANIC(KBcspPanicCat, EInvalidCurrentTask));
			}
		}
		
	return doTellStack;
	}

/**
	Called when the controller starts trying to establish a BCSP link whilst we
	think a link is established. This state is intentional if the controller manager
	has called a 'ColdReset' command.
*/
TBool CHCTLBcspControllerManager::ExpectedControllerReset()
	{
	return (iControllerManagerState == EResetHardware);
	}

TBool CHCTLBcspControllerManager::PowerOffRequested()
	{
	return (iLastPowerRequest == EBTOff);
	}

void CHCTLBcspControllerManager::ProcessBcCmdEvent(const TDesC8& /*aEvent*/)
	{
	//unlikely to happen - but just drop if it does
	//maybe useful later if more BCCMDs (BlueCore VSCs) are used.
	}
	
TBool CHCTLBcspControllerManager::DoBcspLinkEstablishedForPowerUp()
	{
	__ASSERT_DEBUG(iControllerManagerState == EResetHardware, PANIC(KBcspPanicCat, EUnexpectedControllerMgrState));
	if(iControllerManagerState == EResetHardware)
		{
		if(iObserver)
			{
			iObserver->McsoProcessPowerChange(KErrNone, MControllerStateObserver::EBTFatalChange, EBTOn);
			}
		iCurrentPowerState = EBTOn; 
		iCurrentTask = ENoTask;
		iControllerManagerState = EIdle; //controller reset finished
		}
	return ETrue;
	}
	
TBool CHCTLBcspControllerManager::DoBcspLinkEstablishedForControllerReset()
	{
	TBool doTellStack = ETrue;
	
	switch(iControllerManagerState)
		{
		case EResetBCSP:
			{
			iHCTLBcsp.WriteBcCmd(iColdResetCommand); //should result in callback to CHCTLBcsp::HandlePeerReset
			iControllerManagerState = EResetHardware;
			doTellStack = EFalse;
			}
			break;
		case EResetHardware:
			{
			if(iObserver)
				{
				iObserver->McsoProcessHardResetPhaseChange(KErrNone, MControllerStateObserver::EBTFatalChange, EBTResetComplete);
				}
			iCurrentTask = ENoTask;
			iControllerManagerState = EIdle; //controller reset finished
			}
			break;
		case EIdle:
			break;

		default:
			{
			PANIC(KBcspPanicCat, EUnexpectedControllerMgrState);
			}
		}

	return doTellStack;
	}

/*static*/TInt CHCTLBcspControllerManager::HandlePowerDown(TAny* aThis)
	{
	LOG_STATIC_FUNC
	
	reinterpret_cast<CHCTLBcspControllerManager*>(aThis)->DoHandlePowerDown();
	return KErrNone;
	}

void CHCTLBcspControllerManager::DoHandlePowerDown()
	{
	LOG_FUNC
	
	if(iObserver)
		{
		iObserver->McsoProcessPowerChange(KErrNone, MControllerStateObserver::EBTFatalChange, EBTOff);
		}
	iCurrentPowerState = EBTOff;
	iCurrentTask = ENoTask;
	iControllerManagerState = EIdle;
	}

/*static*/TInt CHCTLBcspControllerManager::HandleReadyToResetController(TAny* aThis)
	{
	LOG_STATIC_FUNC
	
	reinterpret_cast<CHCTLBcspControllerManager*>(aThis)->DoHandleReadyToResetController();
	return KErrNone;
	}

void CHCTLBcspControllerManager::DoHandleReadyToResetController()
	{
	LOG_FUNC
	
	// asynchronous call to tell stack hard reset has started
	LOG(_L8("HCTLUART: ***Hard Reset Started***"));
	if(iObserver)
		{
		iObserver->McsoProcessHardResetPhaseChange(KErrNone, MControllerStateObserver::EBTFatalChange, EBTResetStarted);
		}
	iHCTLBcsp.Reset(); //causes BCSP to re-establish BCSP link (causing call back to CHCLTBcsp::Unchoke())
	iControllerManagerState = EResetBCSP;
	}

void CHCTLBcspControllerManager::McroControllerResetComplete()
	{
	LOG_FUNC
	
	if(iObserver)
		{
		iObserver->McsoProcessHardResetPhaseChange(KErrNone, MControllerStateObserver::EBTFatalChange, EBTResetComplete);
		}
	iCurrentTask = ENoTask;
	iControllerManagerState = EIdle;
	}

void CHCTLBcspControllerManager::McpooPowerOnComplete()
	{
	if(iObserver)
		{
		iObserver->McsoProcessPowerChange(KErrNone, MControllerStateObserver::EBTFatalChange, EBTOn);
		}
	iCurrentTask = ENoTask;
	iControllerManagerState = EIdle;
	}

/**
 Setter for the observer
 @param aObserver A event observer the power man can use to notify power changes
 */
void CHCTLBcspControllerManager::SetObserver(MControllerStateObserver& aObserver)
	{
	iObserver = &aObserver;
	}
