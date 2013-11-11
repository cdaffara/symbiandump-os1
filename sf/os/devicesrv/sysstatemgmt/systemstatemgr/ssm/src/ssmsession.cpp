// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <ssm/ssmstatemanager.h>
#include "ssmserver.h"
#include "ssmsession.h"
#include "ssmclisrv.h"
#include "ssmdebug.h"

CSsmSession::~CSsmSession( )
	{
	}

CSsmSession::CSsmSession( )
	{
	}

CSsmSession* CSsmSession::NewL( )
	{
	CSsmSession* session = new(ELeave) CSsmSession();
	return session;
	}

void CSsmSession::ServiceL(const RMessage2& aMessage)
	{
	switch( aMessage.Function() )
		{
		case ERequestStateTransition:
			{
			TSsmState newState;
			newState.SetFromInt(aMessage.Int0());
			const TInt reason = aMessage.Int1();
#ifdef _DEBUG
			TSsmStateName name = newState.Name();
			DEBUGPRINT2(_L("SysStateSession received new request: ERequestStateTransition %S"), &name);
#endif
			SsmServer()->RequestStateTransitionL(newState, reason, aMessage);
			break;
			} //lint !e529 'name' not subsequently referenced in release builds

		case ERequestStateTransitionCancel:
			DEBUGPRINT1(_L ("SysStateSession received new request: ERequestStateTransitionCancel"));
			aMessage.Complete (KErrNone);
			SsmServer()->RequestStateTransitionCancel(this);
			break;

		case ERequestSwpChange:
			{
			const TUint swpKey = {aMessage.Int0()};
			const TSsmSwp newSwpValue(swpKey, aMessage.Int1());
			DEBUGPRINT3(_L ("SysStateSession received new request: ERequestSwpChange %x : %d"), newSwpValue.Key(), newSwpValue.Value());
			SsmServer()->RequestSwpChangeL(newSwpValue, aMessage);
			break;
			}

		case ERequestSwpChangeCancel:
			{
			DEBUGPRINT1(_L ("SysStateSession received new request: ERequestSwpChangeCancel"));
			aMessage.Complete (KErrNone);
			SsmServer()->RequestSwpChangeCancel(this);
			break;
			}
	
		case ERequestRegisterSwpMapping:
			{
			DEBUGPRINT1(_L ("SysStateSession received new request: ERequestRegisterSwpMapping"));
			const TUint swpKey = {aMessage.Int0()};
			const TInt deslen = aMessage.GetDesLengthL(1);
			RBuf filename;
			filename.CreateL(deslen);
			filename.CleanupClosePushL();
			aMessage.ReadL(1, filename);
			SsmServer()->RequestRegisterSwpMappingL(swpKey, filename);
			CleanupStack::PopAndDestroy(&filename);
			aMessage.Complete(KErrNone);
			break;
			}
			
		case ERequestMarkHeap:
			{
			#ifdef _DEBUG
			__UHEAP_MARK;
			DEBUGPRINT2(_L("CSsmSession - EDebugMarkHeap: Alloc Cells: %d."), User::CountAllocCells());
			#endif
			aMessage.Complete(KErrNone);
			break;
			}
			
		case ERequestMarkHeapEnd:
			{
			#ifdef _DEBUG
			//Type of cleanup required.Passed from the test case
			TInt aCleanup = (aMessage.Int0());
			//Deletes the data members of the transition engine which are on the heap
			//This is done to ensure that each test cleans up after itself
			switch(aCleanup)
				{
				case ENoCleanup:
					{
					//No cleanup required
					break;
					}
				case EDoCleanup:
					{
					//Cleanup both  state transition engine and swp request handler
					SsmServer()->DoCleanupStateTransitionEngine();
					SsmServer()->DoCleanupSwpRequestHandler();
					break;
					}
				case ECleanupStateTransitionEngineOnly:
					{
					//cleanup only state transition engine
					SsmServer()->DoCleanupStateTransitionEngine();
					break;
					}
				case ECleanupSwpRequestHandlerOnly:
					{
					//cleanup only the swp request handler
					SsmServer()->DoCleanupSwpRequestHandler();
					break;
					}
				default:
					{
					break;
					}
				}				
			DEBUGPRINT2(_L("CSsmSession - EDebugMarkHeapEndC: Alloc Cells: %d."), User::CountAllocCells());
			__UHEAP_MARKEND;
			
			#endif
			aMessage.Complete(KErrNone);
			break;
			}	
		case ERequestDeRegisterSwpMapping:
			{
			#ifdef _DEBUG
			DEBUGPRINT1(_L ("SysStateSession received new request: ERequestDeRegisterSwpMapping"));
			const TUint swpKey = {aMessage.Int0()};
			const TInt length = aMessage.GetDesLengthL(1);
			RBuf filename;
			filename.CreateL(length);
			filename.CleanupClosePushL();
			aMessage.ReadL(1, filename);
			SsmServer()->RequestDeRegisterSwpMappingL(swpKey, filename);
			CleanupStack::PopAndDestroy(&filename);
			#endif
			aMessage.Complete(KErrNone);
			break;
			}	
		default:
			{
			DEBUGPRINT2(_L ("SysStateSession received unknown request %d. PolicyServer configuration must be wrong."), aMessage.Function());
			aMessage.Complete(KErrNotSupported);
			break;
			}
		}
	}

void CSsmSession::ServiceError(const RMessage2 &aMessage, TInt aError )
	{
	DEBUGPRINT2(_L ("SysStateSession: ServiceError %d"), aError);
	CSession2::ServiceError(aMessage, aError);
	}

/**
 CSsmSession::SsmServer() returns a pointer to the server object
 */
CSsmServer* CSsmSession::SsmServer( ) const
	{
	return static_cast<CSsmServer*> (const_cast<CServer2*> (Server()));
	}
