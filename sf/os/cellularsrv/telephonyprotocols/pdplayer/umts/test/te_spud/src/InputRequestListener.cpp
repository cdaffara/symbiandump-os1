// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of CInputRequestListener
// 
//

/**
 @file
 @internalComponent
*/


#include "CPdpFsmInterface.h"

#include "SpudUnitTestServer.h"
#include "InputRequestListener.h"
#include <in_iface.h>
#include "reteldriverinput.h"
#include "ceteldriverfactory.h"
#include "cpdpfsmfactory.h"
#include "DefaultContextParameters.h"

CInputRequestListener* CInputRequestListener::NewL(TBool aUseTestPdpInterface, TThreadId aParentThreadId, TInt aUmtsRelease)
	{
	CInputRequestListener *me = new(ELeave) CInputRequestListener(aUseTestPdpInterface, aUmtsRelease);
	CleanupStack::PushL(me);
	me->ConstructL(aParentThreadId);
	CleanupStack::Pop(me);
	return me;
	}

CInputRequestListener::CInputRequestListener(TBool aUseTestPdpFsmInterface, TInt aUmtsRelease) : CActive(EPriorityStandard), iUseTestPdpFsmInterface(aUseTestPdpFsmInterface)
	{
	iReadyForInputRequest = KRequestPending;
	iThreadDestructed     = KRequestPending;
	iThreadInitialized    = KRequestPending;

	iEtelDriverInputResponseStatus             = KRequestPending;
	iEtelDriverConfigGPRSNotificationStatus    = KRequestPending;
	iEtelDriverContextStatusNotificationStatus = KRequestPending;
	iEtelDriverQosNotificationStatus           = KRequestPending;
	iEtelDriverServiceNotificationStatus       = KRequestPending;
	
	iFsmInputResponseStatus          = KRequestPending;
	iFsmNetworkStatusStatus          = KRequestPending;
	iFsmContextParameterChangeStatus = KRequestPending;
	iFsmContextSuspendedStatus       = KRequestPending;
	iFsmContextResumedStatus         = KRequestPending;	
    iFsmObjectCreateResponse         = KRequestPending;	
    
    iUmtsRelease = aUmtsRelease;
	}
	
void CInputRequestListener::ConstructL(TThreadId aParentThreadId)
	{
    User::LeaveIfError(iParentThread.Open(aParentThreadId));
	}

CInputRequestListener::~CInputRequestListener()
	{
	Cancel();
	if (iEtelDriver)
		{
		iEtelDriver->Close();
		delete iEtelDriver;
		iEtelDriver = NULL;
		}
	
	if (iPdpFsmInterfaceTest)
		{
		iPdpFsmInterfaceTest->Close();
		delete iPdpFsmInterfaceTest;
		iPdpFsmInterfaceTest = NULL;
		}

	if (iPdpFsmInterface)
		{
		iPdpFsmInterface->Close();
		delete iPdpFsmInterface;
		iPdpFsmInterface = NULL;
		}

	iEventHandlers.ResetAndDestroy();
	iParentThread.Close();
	}
	
void CInputRequestListener::DoCancel()
	{
	iStatus = KErrCancel;
	}
	
/**
 Wrapper for private method CActive::SetActive()
 */
void CInputRequestListener::Activate()
	{
	iStatus = KRequestPending;
	SetActive();
	}

/**
 If iStatus was completed with EEtelDriverInputRequest, REtelDriverInput::Input is called with the parameters supplied in iInputData
 If iStatus was completed with EEtelDriverCancelPdpRequest, REtelDriverInput::Cancel is called with the parameters supplied in iEtelDriverCancelRequestData
 If iStatus was completed with EFsmInputRequest, RPdpFsmInterface::Input is called with the parameters supplied in iFsmInputRequestData
 If iStatus was completed with ECleanup, or anything else, the active scheduler is stopped
 */
void CInputRequestListener::RunL()
	{
	TRequestStatus status = iStatus;
	iStatus = KRequestPending;
	
	if (status == EEtelDriverInputRequest     || 
	    status == EEtelDriverCancelPdpRequest ||
	    status == EFsmInputRequest            || 
	    status == EFsmObjectCreate)
		{
		switch (status.Int())
		    {
		    case EEtelDriverInputRequest:
		        {
    			// the main test thread requested a call to REtelDriverInput::Input
    			iEtelDriver->Input(iEtelDriverInputRequestData.iPdpId, iEtelDriverInputRequestData.iOperation); // error
    			break;
    			}
		    case EEtelDriverCancelPdpRequest:
		        {
    			iEtelDriver->CancelPdp(iEtelDriverCancelRequestPdpId);
    			break;
    			}
		    case EFsmInputRequest:
		        {
    			iPdpFsmInterface->Input(iFsmInputRequestData.iPdpId, iFsmInputRequestData.iOperation);
    			break;
    			}
		    case EFsmObjectCreate:
    			{
				CSpudUnitTestEventHandler* handler = CSpudUnitTestEventHandler::NewL(*this, iUmtsRelease);
				CleanupStack::PushL(handler);
				iEventHandlers.AppendL(handler);
				CleanupStack::Pop(handler);
				SpudMan::TPdpContextType contextType;
				if(iFsmInputRequestData.iPdpId == 0)
					{
					contextType=SpudMan::EPrimary;
					}
				else
					{
					contextType=SpudMan::ESecondary;
					}	
				
    			TRAPD(err, iFsmInputRequestData.iPdpId = iPdpFsmInterface->NewFsmContextL(*handler,contextType));
				handler->SetContextId(iFsmInputRequestData.iPdpId);
				
                // Reproduce completion  as per 'SpudManInterfaceTest::Input()' 
                // - since we're not being called asynchronously from EtelPkt
                
        		iFsmInputResponseData.iPdpId = iFsmInputRequestData.iPdpId;

        		TRequestStatus *readyStatus = &iFsmObjectCreateResponse;
        	 	iParentThread.RequestComplete(readyStatus, err);
        	 	break;
    			}
    		default:
    		    ASSERT(EFalse);
    		}

		// we are finished with the iInputData or iControlData parameters, so this signals to the main test thread
		// that this data can be updated with new parameters
		TRequestStatus *readyStatus = &iReadyForInputRequest;
	 	iParentThread.RequestComplete(readyStatus, KErrNone);
	 	
		SetActive(); 	
		}
	else
		{
		// the status was ECleanup, or we don't recognize the status, so abort the active scheduler, and the thread will be exited
		CActiveScheduler::Stop();
		}
	}
	
/**
 Creates the unit under test - the FSM if iUseTestPdpFsmInterface is false, and the EtelDriver otherwise
 */
void CInputRequestListener::CreateUnitUnderTestL()
	{
	if (iUseTestPdpFsmInterface)
		{
		CreateEtelDriverL();
		}
	else
		{
		CreatePdpFsmL();
		}
	}

/**
 Create and initialize the SPUDTEL
 */
void CInputRequestListener::CreateEtelDriverL()
	{
	iEtelDriver = NewEtelDriverInputL();
	
	iPdpFsmInterfaceTest = new(ELeave)CPdpFsmInterfaceTest(*this);

	iPdpFsmInterfaceTest->TsyName();
	iEtelDriver->OpenL(*iPdpFsmInterfaceTest);
	iPdpFsmInterfaceTest->OpenL(SimTsyName(), iUmtsRelease);
	
	for (TContextId i = 0; i < KMaxPdpContexts; i++)
		{
		if (i!=0)
			{
			iEtelDriver->CreatePdpL(i,SpudMan::ESecondary);
			}
		else
			{
			iEtelDriver->CreatePdpL(i,SpudMan::EPrimary);	
			}
		}
	}

/**
 Create and initialize the SPUD FSM
 */
void CInputRequestListener::CreatePdpFsmL()
	{
	iPdpFsmInterface = new(ELeave)CPdpFsmInterface();
	iPdpFsmInterface->OpenL(SimTsyName(), iUmtsRelease);
	}

/**
 Delete and cleanup and resources associated with SPUDTEL and SPUDFSM
 */
void CInputRequestListener::DeleteUnitUnderTest()
	{
	if (iEtelDriver)
		{
		iEtelDriver->Close();
		delete iEtelDriver;
		iEtelDriver = NULL;
		}
		
	if (iPdpFsmInterfaceTest)
		{
		iPdpFsmInterfaceTest->Close();
		delete iPdpFsmInterfaceTest;
		iPdpFsmInterfaceTest = NULL;
		}

	if (iPdpFsmInterface)
		{
		iPdpFsmInterface->Close();
		delete iPdpFsmInterface;
		iPdpFsmInterface = NULL;
		}
	iEventHandlers.ResetAndDestroy();		
	}

/**
Identifier used in a SpudInput call to signal the SpudMan that the UmtsGprsSCPR is shutting
down and that the real CPdpFsmInterface object will be deleted.

definition moved here from MSpudmaninterface.h as that file no longer exists
*/
const TUint KPdpFsmShuttingDown = 0xff; 

/**
   callback handler for the fsm.
*/
void CSpudUnitTestEventHandler::Event(TInt aEvent, TInt aParam)
	{
	if (aEvent == KPdpFsmShuttingDown)
		{
		// This event requires no notification - In the real SpudMan it would cause SpudMan
		// to close down (NULL) the pointer to the PdpFsmInterface provided by the UmtsGprsSCPR.
		return;
		}
	
	// The aOperation parameters are broken up into those that are notifications and those that aren't
	// Since notifications can happen anytime, separate request status members need to be kept for each one
	// Other input requests happen serially, so only one request can be pending at any time
	TRequestStatus *status = NULL;
	
    // Here, rather than in the case statement due to rvct compiler warning
#ifdef SYMBIAN_NETWORKING_UMTSR5
    RPacketQoS::TQoSR5Negotiated neg;
#else
    RPacketQoS::TQoSR99_R4Negotiated neg;
#endif
// SYMBIAN_NETWORKING_UMTSR5
	
	switch (aEvent)
		{
	case KContextParametersChangeEvent:
		iListener.iContextParametersEventData.iPdpId = iPdpId;
		iListener.iPdpFsmInterface->Get(iPdpId, neg);
		iListener.iContextParametersEventData.iNegotiated = neg;
		switch (iUmtsRelease)
			{
			case TPacketDataConfigBase::KConfigGPRS:
				{
				iListener.iPdpFsmInterface->Get(iPdpId, iListener.iContextParametersEventData.iContextConfig);
				}
				break;
	     
			case TPacketDataConfigBase::KConfigRel99Rel4:
				{
				iListener.iPdpFsmInterface->Get(iPdpId, iListener.iContextParametersEventData.iContextConfigR99_R4);
				}
				break;
	     
			case TPacketDataConfigBase::KConfigRel5:
				{
				iListener.iPdpFsmInterface->Get(iPdpId, iListener.iContextParametersEventData.iContextConfig_R5);
				}
				break;
			}

		status = &iListener.iFsmContextParameterChangeStatus;
		break;
	case KNetworkStatusEvent:
		status = &iListener.iFsmNetworkStatusStatus;
		break;
	case KContextBlockedEvent:
		iListener.iContextBlockedEventPdpId = iPdpId;
		status = &iListener.iFsmContextSuspendedStatus;
		break;
	case KContextUnblockedEvent:
		iListener.iContextUnblockedEventPdpId = iPdpId;
		status = &iListener.iFsmContextResumedStatus;
		break;
	default:
		iListener.iFsmInputResponseData.iPdpId = iPdpId;
		iListener.iFsmInputResponseData.iEvent = aEvent;
		iListener.iFsmInputResponseData.iParam = aParam;
		status = &iListener.iFsmInputResponseStatus;
		break;
		}
	iListener.iParentThread.RequestComplete(status, KErrNone);	
	}

CSpudUnitTestEventHandler *CSpudUnitTestEventHandler::NewL(CInputRequestListener& aListener, TInt aUmtsRelease)
	{
	return new(ELeave) CSpudUnitTestEventHandler(aListener, aUmtsRelease);
	}
	
CSpudUnitTestEventHandler::CSpudUnitTestEventHandler(CInputRequestListener& aListener, TInt aUmtsRelease)
	: iListener(aListener), iUmtsRelease(aUmtsRelease)
	{
	}
