// Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "smspdelTraces.h"
#endif

#include "smspdel.h"
#include "smspmain.h"

#include "smsuset.h"

#include <exterror.h>
#include "SmsuTimer.h"
#include "smspqueue.h"


/**
 *  Creates new CSmsStoreRead instance
 *  
 *  @param aSmsComm		reference to the MSmsComm class
 *  @param aSmsSettings	reference to the TSmsSettings class
 *  @param aSmsMessaging	reference to the RMobileSmsMessaging class
 */
CSmsPDUDelete* CSmsPDUDelete::NewL(const TSmsSettings& aSmsSettings,
								   RMobileSmsMessaging& aSmsMessaging
								   )
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUDELETE_NEWL_1, "CSmsPDUDelete::NewL()");

	CSmsPDUDelete* smsPduDelete =
		new(ELeave) CSmsPDUDelete(aSmsSettings,aSmsMessaging);
	CleanupStack::PushL(smsPduDelete);
	smsPduDelete->ConstructL();
	CleanupStack::Pop();
	return smsPduDelete;
	} // CSmsPDUDelete::NewL


/**
 *  Constructor
 *  
 *  @param aSmsComm		reference to the MSmsComm class
 *  @param aSmsSettings	reference to the TSmsSettings class
 *  @param aSmsMessaging	reference to the RMobileSmsMessaging class
 */
CSmsPDUDelete::CSmsPDUDelete(const TSmsSettings& aSmsSettings,
							 RMobileSmsMessaging& aSmsMessaging)
  : CSmsuActiveBase(KSmsSessionPriority),
	iState(ESmsPDUDeleteIdle),
	iSmsSettings(aSmsSettings),
	iSmsMessaging(aSmsMessaging)
	{
	// NOP
	} // CSmsPDUDelete::CSmsPDUDelete


/**
 *  Creates instance to the class CSmsuTimeout
 */
void CSmsPDUDelete::ConstructL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUDELETE_CONSTRUCTL_1, "CSmsPDUDelete::ConstructL()");

	ConstructTimeoutL();
	} // CSmsPDUDelete::ConstructL


/**
 *  Starts CSmsPDUDelete active object
 *  Calls private methd OpenStore
 *  
 *  @param aStatus     Reguest status
 *  @param aStoreIndex Index to the store to be read
 */
void CSmsPDUDelete::Start(CArrayFix<TGsmSmsSlotEntry>& aLocationArray, TRequestStatus& aStatus)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUDELETE_START_1, "CSmsPDUDelete::Start()");

	__ASSERT_DEBUG(iState==ESmsPDUDeleteIdle,SmspPanic(KSmspPanicUnexpectedState));

	Queue(aStatus);

	iLocationArray=&aLocationArray;
	iState=ESmsPDUDeleteOpeningStore;
	OpenStore();
	} // CSmsPDUDelete::Start


/**
 *  Called by Active Scheduler
 *  
 *  Deletes slots giving in array. Opens new store if store change
 */
void CSmsPDUDelete::DoRunL()
	{
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUDELETE_DORUNL_1, "CSmsPDUDelete::RunL [iStatus=%d]", iStatus.Int());

	switch (iState)
		{
		case ESmsPDUDeleteOpeningStore:
			{
			//
			// If the store was opened, then delete the PDU...
			//
			if (iStatus.Int() == KErrNone)
				{
				DeletePDU();
				}
			}
			break;

		case ESmsPDUDeleteDeletingPDU:
			{
			//
			// If deleting the PDU returned not found, then this is a valid
			// result so assume KErrNone.
			//
			if (iStatus.Int() == KErrNotFound  ||
				iStatus.Int() == KErrGsmSMSInvalidMemoryIndex)
				{
				iStatus = KErrNone;
				}

			//
			// If the delete was successful (or assumed to be) then remove
			// it from the list. If the next one is the on a different store,
			// then change stores, otherwise delete it.
			//
			if (iStatus.Int() == KErrNone)
				{
				iLocationArray->Delete(0);

				if (iLocationArray->Count() > 0)
					{
					if (iLocationArray->At(0).iStore != iCurrentStore)
						{
						OpenStore();
						}
					else
						{
						DeletePDU();
						}
					}
				}
			}
			break;

		default:
			{
			SmspPanic(KSmspPanicUnexpectedState);
			}
			break;
		}

	//
	// DoRunL() will now return to CSmsuActiveBase which if the object
	// is not active, will call Complete().
	//
	} // CSmsPDUDelete::DoRunL


/**
 *  private method for opening the store where slot is saved
 */
void CSmsPDUDelete::OpenStore()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUDELETE_OPENSTORE_1, "CSmsPDUDelete::OpenStore()");

	//
	// Open the SMS Store for the first PDU...
	//
	if (iLocationArray->Count() > 0)
		{
		//
		// Close it first, just in case it was left open...
		//
		iSmsStorage.Close();

		//
		// Now open the store...
		//
		TInt  ret;
		
		ret = iSmsStorage.Open(iSmsMessaging, iLocationArray->At(0).iStore);
		if (ret == KErrNone)
			{
			iCurrentStore = iLocationArray->At(0).iStore;
			}

		//
		// Complete this object to allow correct flow through DoRunL()
		// and Complete() if needed.
		//			
		iState = ESmsPDUDeleteOpeningStore;

		CompleteMyself(ret);
		}
	} // CSmsPDUDelete::OpenStore


/**
 *  private method for deleting slot from the store
 */
void CSmsPDUDelete::DeletePDU()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUDELETE_DELETEPDU_1, "CSmsPDUDelete::DeletePDU()");

	//
	// Delete the first entry in the list...
	//
	if (iLocationArray->Count() > 0)
		{
		iSmsStorage.Delete(iStatus, iLocationArray->At(0).iIndex);

		iState = ESmsPDUDeleteDeletingPDU;

		TimedSetActive(iSmsSettings.Timeout());
		}
	} // CSmsPDUDelete::DeletePDU


/**
 *  Called by Active Scheduler
 */
void CSmsPDUDelete::DoCancel()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUDELETE_DOCANCEL_1, "CSmsPDUDelete::DoCancel()");

	TimedSetActiveCancel();

	switch (iState)
		{
		case ESmsPDUDeleteOpeningStore:
			{
			//
			// Will never happen as CompleteMyself() was used...
			//
			}
			break;

		case ESmsPDUDeleteDeletingPDU:
			{
			iSmsStorage.CancelAsyncRequest(EMobilePhoneStoreDelete);
			}
			break;

		default:
			{
			SmspPanic(KSmspPanicUnexpectedState);
			}
		}

	//
	// Handle completion of this Active Object. Note that the object
	// may well still be active at this point...
	//
	if (TimedOut())
		{
		Complete(KErrTimedOut);
		}
	else
		{
		Complete(KErrCancel);
		}
	} // CSmsPDUDelete::DoCancel


/**
 *  Completes this object
 */
void CSmsPDUDelete::Complete(TInt aStatus)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSMSPDUDELETE_COMPLETE_1, "CSmsPDUDelete::Complete()");

	//
	// Only close the SMS Storage if the object is not active (and it may be
	// if the operation was cancelled). Otherwise it will be closed on
	// destruction.
	//
	if (!IsActive())
		{
		iSmsStorage.Close();
		}

	//
	// Resets slot array...
	//
	iLocationArray->Reset();

	//
	// Call the base function to perform the actual complete...
	//
	CSmsuActiveBase::Complete(aStatus);

	//
	// This active object has completed...
	//
	iState = ESmsPDUDeleteIdle;
	} // CSmsPDUDelete::Complete


/**
 *  Destructor
 */
CSmsPDUDelete::~CSmsPDUDelete()
	{
	Cancel();
	iSmsStorage.Close();
	} // CSmsPDUDelete::~CSmsPDUDelete

