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

#include <s32mem.h>
#include "saamiscadaptation.h"

#include "startupadaptationadapter.h"
#include "ssmdebug.h"
#include "clayerpanic.h"
/*
 * Creates a new object associated with the passed in CStartupAdaptationAdapter
 * 
 * @internalComponent
 */
CSaaMiscAdaptation* CSaaMiscAdaptation::NewL(CStartupAdaptationAdapter* aAdapter)
	{
	CSaaMiscAdaptation* self = new (ELeave) CSaaMiscAdaptation(aAdapter);
	return self;
	}

/*
 * Destructor for this object
 * 
 * @internalComponent
 */
CSaaMiscAdaptation::~CSaaMiscAdaptation()
	{
	iLanguageList.Close();
	}
	
/*
 * Decrements the reference count for this object, deleting it if necessary
 * 
 * @internalComponent
 */
void CSaaMiscAdaptation::Release()
	{
	// This MClass is owned by the singleton CStartupAdaptationAdapter class so
	// release should do nothing.
	}

/*
 * 
 * 
 * @internalComponent
 */
void CSaaMiscAdaptation::SecurityStateChange(TInt aState, TDes8& aResponsePckg, TRequestStatus& aStatus)
	{
	// If this adaptation is busy then complete with KErrInUse
	if(Busy())
		{
		TRequestStatus* statusPtr = &aStatus;
		User::RequestComplete(statusPtr, KErrInUse);
		return;
		}
	// Set this request status
	SetRequestStatus(&aStatus);
	aStatus = KRequestPending;
	// No outstanding requests so set up command id
	SetCommandId(StartupAdaptation::ESecurityStateChange);
	// Set parameters
	iSecurityStateChangePckg() = static_cast<StartupAdaptation::TSecurityState>(aState);
	// Set return descriptor
	iReturnPckg = &aResponsePckg;
	// Pass this to the adapter
	TRAPD(err, iAdapter->QueueDispatchL(this));
	if(err != KErrNone)
		{
		// Failed to queue adaptation, complete with error
		SetRequestStatus(NULL);
		TRequestStatus* statusPtr = &aStatus;
		User::RequestComplete(statusPtr, err);
		}
	}

/*
 * 
 * 
 * @internalComponent
 */
void CSaaMiscAdaptation::GetGlobalStartupMode(TDes8& aModePckg, TRequestStatus& aStatus)
	{
	// If this adaptation is busy then complete with KErrInUse
	if(Busy())
		{
		TRequestStatus* statusPtr = &aStatus;
		User::RequestComplete(statusPtr, KErrInUse);
		return;
		}
	// Set this request status
	SetRequestStatus(&aStatus);
	aStatus = KRequestPending;
	// No outstanding requests so set up command id
	SetCommandId(StartupAdaptation::EGetGlobalStartupMode);
	
	// No parameters to set to pass in
	
	// Set return descriptor
	iReturnPckg = &aModePckg;
	// Pass this to the adapter
	TRAPD(err, iAdapter->QueueDispatchL(this));
	if(err != KErrNone)
		{
		// Failed to queue adaptation, complete with error
		SetRequestStatus(NULL);
		TRequestStatus* statusPtr = &aStatus;
		User::RequestComplete(statusPtr, err);
		}
	
	}

void CSaaMiscAdaptation::GetHiddenReset(TDes8& aHiddenResetPckg, TRequestStatus& aStatus)
	{
	// If this adaptation is busy then complete with KErrInUse
	if(Busy())
		{
		TRequestStatus* statusPtr = &aStatus;
		User::RequestComplete(statusPtr, KErrInUse);
		return;
		}
	// Set this request status
	SetRequestStatus(&aStatus);
	aStatus = KRequestPending;
	// No outstanding requests so set up command id
	SetCommandId(StartupAdaptation::EGetHiddenReset);
	// No parameters to set to pass in
	
	// Set return package
	iReturnPckg = &aHiddenResetPckg;
	// Pass this to the adapter
	TRAPD(err, iAdapter->QueueDispatchL(this));
	if(KErrNone != err)
		{
		// Failed to queue adaptation, complete with error
		SetRequestStatus(NULL);
		TRequestStatus* statusPtr = &aStatus;
		User::RequestComplete(statusPtr, err);
		}
	
	}

/*
 * 
 * 
 * @internalComponent
 */
void CSaaMiscAdaptation::PrepareSimLanguages(TSsmLanguageListPriority aPriority, TDes8& aSizePckg, TRequestStatus& aStatus)
	{
	// If this adaptation is busy then complete with KErrInUse
	if(Busy())
		{
		TRequestStatus* statusPtr = &aStatus;
		User::RequestComplete(statusPtr, KErrInUse);
		return;
		}
	// Set this request status
	SetRequestStatus(&aStatus);
	aStatus = KRequestPending;
	// No outstanding requests so set up command id
	SetCommandId(StartupAdaptation::EGetSIMLanguages);
	// Set up parameters
	switch(aPriority)
	{
		case EPrimaryLanguages:
			iLanguageListPriorityPckg() = StartupAdaptation::EPrimaryLanguages; 
			break;
		case ESecondaryLanguages:
			iLanguageListPriorityPckg() = StartupAdaptation::ESecondaryLanguages;	
			break;
		case ETertiaryLanguages:
			iLanguageListPriorityPckg() = StartupAdaptation::ETertiaryLanguages;
			break;	
		default:
			// Unknown language, pass across unchanged
			iLanguageListPriorityPckg() = static_cast<StartupAdaptation::TLanguageListPriority>(aPriority);
			break;
	}
	// Set up the return value
	iReturnPckg = &aSizePckg;
	// Pass this to the adapter
	TRAPD(err, iAdapter->QueueDispatchL(this));
	if(err != KErrNone)
		{
		// Failed to queue adaptation, complete with error
		SetRequestStatus(NULL);
		TRequestStatus* statusPtr = &aStatus;
		User::RequestComplete(statusPtr, err);
		}
	
	}

/*
 * 
 * 
 * @internalComponent
 */
void CSaaMiscAdaptation::GetSimLanguagesL(CBufBase* aBuf, TInt aCount)
	{
	const TInt languageCount = iLanguageList.Count();
	if(aCount < languageCount)
		{
		DEBUGPRINT3A("Language count(%d) given is less than prepared languages count(%d)", aCount, languageCount);
		// Buffer passed in isn't large enough 
		User::Leave(KErrArgument);		
		}

	RBufWriteStream writeStream(*aBuf);
	CleanupClosePushL(writeStream); 

	for (TInt index =0; index < languageCount; ++index)
		{
		writeStream.WriteInt32L(iLanguageList[index]);
		}
	writeStream.CommitL();
	CleanupStack::PopAndDestroy();
	iLanguageList.Reset(); //iLanguageList is reset so that it can be used to contain new language list prepared by the method PrepareSimLanguages()
	}

/*
 * 
 * 
 * @internalComponent
 */
void CSaaMiscAdaptation::Cancel()
	{
	CancelRequest();
	}

/*
 * Constructs a new state adaptation object and associates it with aAdapter
 * 
 * @internalComponent
 */
CSaaMiscAdaptation::CSaaMiscAdaptation(CStartupAdaptationAdapter* aAdapter)
: CAdaptationBase(aAdapter)
	{

	}

/**
 * See CAdaptationBase for description of method.
 *  
 * @internalComponent
 */
void CSaaMiscAdaptation::RequestComplete(const StartupAdaptation::TCommand __DEBUG_ONLY(aCommandId), TDesC8& aRetPckg)
	{
	DEBUGPRINT3A("SAA - Response received from adaptation with commandId: %d, expecting %d", aCommandId, CommandId());
	__ASSERT_DEBUG(aCommandId == CommandId(), CLAYER_PANIC(ECLayerUnexpectedCommandResponse));
	switch(CommandId())
		{
		case StartupAdaptation::ESecurityStateChange:
			{
			StartupAdaptation::TSecurityStateResponsePckg responsePckg;
			responsePckg.Copy(aRetPckg);
			// Only copy across the response if it's error free
			if(responsePckg().iErrorCode == KErrNone)
				{
				TPckgBuf<TInt> replyPckg;
				replyPckg() = responsePckg().iValue;
				iReturnPckg->Copy(replyPckg);
				// Finished with iReturnPckg
				iReturnPckg = NULL;
				}
			CompleteRequestStatus(responsePckg().iErrorCode);
			break;
			}
		case StartupAdaptation::EGetSIMLanguages:
			{
			// aRetPckg contains an RClass so we use it in place by creating a reference to it
			StartupAdaptation::TLanguageListResponsePckg& languageListResponsePckg =
				*(static_cast<StartupAdaptation::TLanguageListResponsePckg*>(&aRetPckg));
			if(languageListResponsePckg().iErrorCode == KErrNone)
				{
				// Clear array
				iLanguageList.Reset();
				// make a copy of the array
				for(TInt i = 0; i < languageListResponsePckg().iLanguages.Count(); ++i)
		            {
		            iLanguageList.Append( languageListResponsePckg().iLanguages[i] ); // ignore errors for compatibility reasons
		            }
				// Copy size into the return buffer
				TPckgBuf<TInt> sizeBuf(iLanguageList.Count());
				iReturnPckg->Copy(sizeBuf);
				// done with return buffer
				iReturnPckg = NULL;
				}
			CompleteRequestStatus(languageListResponsePckg().iErrorCode);
			break;
			}
		case StartupAdaptation::EGetGlobalStartupMode:
			{
			StartupAdaptation::TGlobalStartupModeResponsePckg responsePckg;
			responsePckg.Copy(aRetPckg);
			// Only copy across the response if it's error free
			if(responsePckg().iErrorCode == KErrNone)
				{
				TPckgBuf<TInt> replyPckg;
				replyPckg() = responsePckg().iValue;
				iReturnPckg->Copy(replyPckg);
				// Finished with iReturnPckg
				iReturnPckg = NULL;
				}
			CompleteRequestStatus(responsePckg().iErrorCode);
			break;
			}
		case StartupAdaptation::EGetHiddenReset:
			{
			StartupAdaptation::TBooleanResponsePckg booleanResponsePckg;
			booleanResponsePckg.Copy(aRetPckg);
			if(KErrNone == booleanResponsePckg().iErrorCode)
				{
				// Copy the boolean value across to the descriptor passed in
				TPckgBuf<TBool> boolPckg;
				boolPckg() = booleanResponsePckg().iValue;
				iReturnPckg->Copy(boolPckg);
				// Null iReturnPckg as it has no further use
				iReturnPckg = NULL;
				}
			CompleteRequestStatus(booleanResponsePckg().iErrorCode);
			break;
			}

		default:
			CLAYER_PANIC(ECLayerUnknownCommandResponse);
			break;
		}
	}

/**
 * See CAdaptationBase for description of method.
 *  
 * @internalComponent
 */
TDesC8* CSaaMiscAdaptation::ParameterPckg()
	{
	TDesC8* ptr = NULL;
	switch(CommandId())
		{
		case StartupAdaptation::ESecurityStateChange:
			ptr = &iSecurityStateChangePckg;
			break;
		case StartupAdaptation::EGetSIMLanguages:
			ptr = &iLanguageListPriorityPckg;
			break;
		case StartupAdaptation::EGetGlobalStartupMode:
			ptr = &iNullBuf;
			break;
		case StartupAdaptation::EGetHiddenReset:
			//No parameters to be passed
			ptr = &iNullBuf;
			break;
		default:
			ptr = NULL;
			break;
		}
	return ptr;
	}
