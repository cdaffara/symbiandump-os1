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

#include <e32capability.h>

#include "clesrv.h"
#include "clesess.h"
#include "cleconst.h"
#include "ssmcommandutilprovider.h"


/**
 In the case of ECleSrvExecuteCmdList, the RMessage is completed by the 
 Executor call into the server's ListExecutionComplete method.
*/
void CCleSession::ServiceL( const RMessage2& aMessage )
	{
	switch( aMessage.Function() )
		{
		
		case ECleSrvExecuteCmdList:
			{
			ExecuteCommandListL( aMessage ); 	
			}
			break;
			
		case ECleSrvExecuteCmdListCancel:
			{
			aMessage.Complete( KErrNone );
			ExecuteCommandListCancel();
			}
			break;
			
		case EDebugMarkHeap:
			{
			#ifdef _DEBUG
			__UHEAP_MARK;
			DEBUGPRINT2(_L("CCleSession - EDebugMarkHeap: Alloc Cells: %d."), User::CountAllocCells());
			#endif
			aMessage.Complete(KErrNone);
			break;
			}
			
		case EDebugMarkHeapEnd:
			{
			#ifdef _DEBUG
			DEBUGPRINT2(_L("CCleSession - EDebugMarkHeapEnd: Alloc Cells: %d."), User::CountAllocCells());
			__UHEAP_MARKEND;
			#endif
			aMessage.Complete(KErrNone);
			break;
			}
			
		case EDebugSetHeapFailure:
			{
			#ifdef _DEBUG
			__UHEAP_SETFAIL(RAllocator::EFailNext,aMessage.Int0());
			DEBUGPRINT2(_L("CCleSession - EDebugSetHeapFailure: Value of Failure Rate: %d."), aMessage.Int0());
			#endif
			aMessage.Complete(KErrNone);
			break;
			}
			
		case EDebugHeapReset:
			{
			#ifdef _DEBUG
			__UHEAP_RESET;
			DEBUGPRINT1(_L("CCleSession - EDebugHeapReset"));
			#endif
			aMessage.Complete(KErrNone);
			break;
			}
				
		case EDebugCloseNeverUnloadLibs:
			{
			TInt ret = KErrNone;
			#ifdef _DEBUG
			ret = iCleSvr.CloseNeverUnloadLibrariesL();
			DEBUGPRINT2(_L("CCleSession - EDebugReleaseNeverUnloadLibs: # of libraries released: %d."), ret);
			#endif
			aMessage.Complete(ret);
			break;
			}

		default:
			aMessage.Complete( KErrUnknown );
			break;			
		}
	}
	
	

void CCleSession::ExecuteCommandListL( const RMessage2& aMessage )
	{
	const CSsmCommandList* srcCmdList = reinterpret_cast<const CSsmCommandList*>(aMessage.Ptr0());
	__ASSERT_DEBUG(srcCmdList, User::Panic(KCleCmdListEmpty, KCleSrvBadCmdList));
	
	CSsmCommandList* cmdList = CSsmCommandList::NewLC(srcCmdList, iCommandUtilProvider);
	// The CSsmCmdListExecutor takes ownership of cmdList
	iCleSvr.ExecuteCommandListL( cmdList, aMessage, iSessionIndex );
	CleanupStack::Pop( cmdList );
	}



void CCleSession::ExecuteCommandListCancel()
	{
	iCleSvr.ExecuteCommandListCancel( iSessionIndex );
	}
	
	

CCleSession* CCleSession::NewL( CCleServer& aCleServer )
	{
	CCleSession* self = new(ELeave) CCleSession( aCleServer );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
	

CCleSession::CCleSession( CCleServer& aCleServer )
: iCleSvr( aCleServer )
	{
	}



void CCleSession::ConstructL()
	{
	iCleSvr.RegisterSessionL( iSessionIndex );
	iCommandUtilProvider = CSsmCommandUtilProvider::NewL();
	}


	
CCleSession::~CCleSession()
	{
	iCleSvr.DeregisterSession( iSessionIndex );
	delete iCommandUtilProvider;
	}

