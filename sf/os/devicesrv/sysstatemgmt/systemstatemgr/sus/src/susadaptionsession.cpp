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

#include "susadaptionserver.h"
#include "susadaptionsession.h"
#include "susadaptionclisrv.h"
#include "ssmdebug.h"

CSsmAdaptationSession::~CSsmAdaptationSession()
	{
	AdaptionServer()->RemovePriorityClient(this);
	}

CSsmAdaptationSession::CSsmAdaptationSession()
	{
	}

CSsmAdaptationSession* CSsmAdaptationSession::NewL()
	{	
	CSsmAdaptationSession* session = new(ELeave) CSsmAdaptationSession();
	return session;
	}

void CSsmAdaptationSession::ServiceL(const RMessage2& aMessage)
	{
	switch (aMessage.Function())
		{

		case ERequestCoopSysStateChange:
			{
		 	DEBUGPRINT1(_L ("CSsmAdaptationSession - opcode: ERequestCoopSysStateChange "));
		 	AdaptionServer()->DoRequestCoopSysStateChangeL(aMessage);
			break;	
			} 
		case ERequestCoopSysSelfTest:
			{
		 	DEBUGPRINT1(_L ("CSsmAdaptationSession - opcode: ERequestCoopSysSelfTest "));
			AdaptionServer()->DoRequestCoopSysSelfTestL(aMessage);
			break;	
			} 			
		case ERequestCoopSysPerformRestartActions:
			{
		 	DEBUGPRINT1(_L ("CSsmAdaptationSession - opcode: ERequestCoopSysPerformRestartActions "));
			AdaptionServer()->DoRequestCoopSysPerformRestartActionsL(aMessage);
			break;	
			} 
		case ERequestCoopSysPerformShutdownActions:
			{
		 	DEBUGPRINT1(_L ("CSsmAdaptationSession - opcode: ERequestCoopSysPerformShutdownActions "));
			AdaptionServer()->DoRequestCoopSysPerformShutdownActionsL(aMessage);
			break;	
			} 
		case ERequestCoopSysPerformRfsActions:
			{
		 	DEBUGPRINT1(_L ("CSsmAdaptationSession - opcode: ERequestCoopSysPerformRfsActions "));
			AdaptionServer()->DoRequestCoopSysPerformRfsActionsL(aMessage);
			break;	
			} 
		case ERequestCoopSysCancel:
			{
		 	DEBUGPRINT1(_L ("CSsmAdaptationSession - opcode: ERequestCoopSysCancel "));
			AdaptionServer()->DoRequestCoopSysCancelL(aMessage);
			break; 	
			} 
		case ENotifyCoopSysEvent:
			{
		 	DEBUGPRINT1(_L ("CSsmAdaptationSession - opcode: ENotifyCoopSysEvent "));
			AdaptionServer()->DoNotifyCoopSysEventL(aMessage);
			break;	
			} 
		case ENotifyCoopSysCancel:
			{
		 	DEBUGPRINT1(_L ("CSsmAdaptationSession - opcode: ENotifyCoopSysCancel "));
			AdaptionServer()->DoNotifyCoopSysEventCancelL(aMessage);
			break;	
			} 
		case EGetLastCoopSysEvent:
			{
		 	DEBUGPRINT1(_L ("CSsmAdaptationSession - opcode: EGetLastCoopSysEvent "));
			AdaptionServer()->DoGetLastCoopSysEventL(aMessage);
			break;	
			} 
		case EGetSimOwned:
			{
		 	DEBUGPRINT1(_L ("CSsmAdaptationSession - opcode: EGetSimOwned "));
			AdaptionServer()->DoGetSimOwnedL(aMessage);
			break;	
			}
		case EGetSimCancel:
			{
		 	DEBUGPRINT1(_L ("CSsmAdaptationSession - opcode: EGetSimCancel "));
			AdaptionServer()->DoGetSimCancelL(aMessage);
			break;	
			} 
		case ENotifySimEvent:
			{
		 	DEBUGPRINT1(_L ("CSsmAdaptationSession - opcode: ENotifySimEvent "));
			AdaptionServer()->DoNotifySimEventL(aMessage);
			break;	
			} 
		case ENotifySimCancel:
			{
		 	DEBUGPRINT1(_L ("CSsmAdaptationSession - opcode: ENotifySimCancel "));
			AdaptionServer()->DoNotifySimCancelL(aMessage);
			break;	
			}
		case EGetLastSimEvent:
			{
		 	DEBUGPRINT1(_L ("CSsmAdaptationSession - opcode: EGetLastSimEvent"));
			AdaptionServer()->DoGetLastSimEventL(aMessage);
			break;	
			} 
		case EValidateRtc:
			{
		 	DEBUGPRINT1(_L ("CSsmAdaptationSession - opcode: EValidateRtc "));
			AdaptionServer()->DoValidateRtcL(aMessage);
			break;	
			}
		case ESetWakeupAlarm:
			{
		 	DEBUGPRINT1(_L ("CSsmAdaptationSession - opcode: ESetWakeupAlarm "));
			AdaptionServer()->DoSetWakeupAlarmL(aMessage);
			break;	
			}
		case EUnsetWakeupAlarm:
			{
		 	DEBUGPRINT1(_L ("CSsmAdaptationSession - opcode: EUnsetWakeupAlarm "));
			AdaptionServer()->DoUnsetWakeupAlarmL(aMessage);
			break;	
			}
		case ERtcCancel:
			{
		 	DEBUGPRINT1(_L ("CSsmAdaptationSession - opcode: ERtcCancel "));
			AdaptionServer()->DoRtcCancelL(aMessage);
			break;	
			}
		case ESecurityStateChange:
			{
		 	DEBUGPRINT1(_L ("CSsmAdaptationSession - opcode: ESecurityStateChange "));
			AdaptionServer()->DoSecurityStateChangeL(aMessage);
			break;	
			}
		case EGetGlobalStartupMode:
			{
		 	DEBUGPRINT1(_L ("CSsmAdaptationSession - opcode: EGetGlobalStartupMode "));
			AdaptionServer()->DoGetGlobalStartupModeL(aMessage);
			break;	
			}
		case EPrepareSimLanguages:
			{
		 	DEBUGPRINT1(_L ("CSsmAdaptationSession - opcode: EPrepareSimLanguages "));
			AdaptionServer()->DoPrepareSimLanguagesL(aMessage);
			break;	
			}
		case EMiscCancel:
			{
		 	DEBUGPRINT1(_L ("CSsmAdaptationSession - opcode: EMiscCancel "));
			AdaptionServer()->DoMiscAdaptationCancelL(aMessage);
			break;	
			}			
		case EGetHiddenReset:
			{
		 	DEBUGPRINT1(_L("CSsmAdaptationSession - opcode: EGetHiddenReset"));
			AdaptionServer()->DoMiscAdaptationHiddenResetL(aMessage);
			break;
			}
		case EGetSimLanguagesArray:
			{
		 	DEBUGPRINT1(_L("CSsmAdaptationSession - opcode: EGetSimLanguagesArray"));
			AdaptionServer()->DoGetSimLanguagesArrayL(aMessage);
			aMessage.Complete(KErrNone);
			break;
			}
		case EActivateRfForEmergencyCall:
			{
		 	DEBUGPRINT1(_L ("CSsmAdaptationSession - opcode: EActivateRfForEmergencyCall "));
			AdaptionServer()->DoActivateRfForEmergencyCallL(aMessage);
			break;	
			}
		case EDeactivateRfForEmergencyCall:
			{
		 	DEBUGPRINT1(_L ("CSsmAdaptationSession - opcode: EDeactivateRfForEmergencyCall "));
			AdaptionServer()->DoDeactivateRfForEmergencyCallL(aMessage);
			break;	
			}
		case EEmergencyCallRfCancel:
			{
		 	DEBUGPRINT1(_L ("CSsmAdaptationSession - opcode: EEmergencyCallRfCancel "));
			AdaptionServer()->DoEmergencyCallRfCancelL(aMessage);
			break;	
			}
		case EDebugMarkHeap:
			{
#ifdef _DEBUG
		 	DEBUGPRINT1(_L ("CSsmAdaptationSession - opcode: EDebugMarkHeap."));
			__UHEAP_MARK;
			DEBUGPRINT2(_L("CSsmAdaptationSession - Mark Heap: Alloc Cells- %d"), User::CountAllocCells());
#endif
			aMessage.Complete(KErrNone);
			break;
			}
		case EDebugMarkHeapEnd:
			{
#ifdef _DEBUG
		 	DEBUGPRINT1(_L ("CSsmAdaptationSession - opcode: EDebugMarkHeapEnd."));
			DEBUGPRINT2(_L("CSsmAdaptationSession - Mark Heap End: Alloc Cells- %d"), User::CountAllocCells());
			__UHEAP_MARKEND;
#endif
			aMessage.Complete(KErrNone);
			break;
			}
		case EDebugCleanupAdaptations:
			{
#ifdef _DEBUG
		 	DEBUGPRINT1(_L ("CSsmAdaptationSession - opcode: EDebugCleanupAdaptations."));
			AdaptionServer()->UnloadAdaptationsAndObservers();
#endif
			aMessage.Complete(KErrNone);
			break;
			}
        case ESetAsPriorityClient:
            {
            DEBUGPRINT1(_L ("CSsmAdaptationSession - opcode: ESetAsPriorityClient."));
            TInt err = AdaptionServer()->SetAsPriorityClientL(this);
            aMessage.Complete(err);
            break;
            }
        case EDebugSetHeapFailure:
            {
#ifdef _DEBUG
            DEBUGPRINT2(_L("CSsmAdaptationSession - EDebugSetHeapFailure: Value of Failure Rate: %d."), aMessage.Int0());
            __UHEAP_SETFAIL(RAllocator::EDeterministic,aMessage.Int0());
#endif
            aMessage.Complete(KErrNone);
            break;
            }
        case EDebugUnSetHeapFailure:
            {
#ifdef _DEBUG
            DEBUGPRINT1(_L("CSsmAdaptationSession - EDebugUnSetHeapFailure"));
            __UHEAP_TOTAL_RESET;
#endif
            aMessage.Complete(KErrNone);
            break;
            }
		default:
			{
			aMessage.Complete (KErrNotSupported);
			break;
			}
		}
	}
 

void CSsmAdaptationSession::ServiceError(const RMessage2 &aMessage, TInt aError)
	{
	DEBUGPRINT2(_L ("CSsmAdaptationSession: ServiceError %d"), aError);
	aMessage.Complete (aError);
	}

/**
 Returns a pointer to the server object
 */
CSsmAdaptationServer* CSsmAdaptationSession::AdaptionServer() const
	{
	return static_cast<CSsmAdaptationServer*> (const_cast<CServer2*> (Server()));
	}

