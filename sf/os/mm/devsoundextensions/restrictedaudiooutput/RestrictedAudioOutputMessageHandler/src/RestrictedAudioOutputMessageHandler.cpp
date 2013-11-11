/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   This is the implementation of the CRestrictedAudioOutputMessageHandler class.
*
*/



// INCLUDE FILES
#include    "RestrictedAudioOutput.h"
#include    "RestrictedAudioOutputMessageHandler.h"
#include    "RestrictedAudioOutputMessageTypes.h"
                

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CRestrictedAudioOutputMessageHandler::CRestrictedAudioOutputMessageHandler(CRestrictedAudioOutput* aAudioOutput)
	 :	CMMFObject(KUidRestrictedAudioOutput)
    {
    iAudioOutput = aAudioOutput;
    }

// Two-phased constructor.
EXPORT_C CRestrictedAudioOutputMessageHandler* CRestrictedAudioOutputMessageHandler::NewL(TAny* aCustomInterface)
    {
    CRestrictedAudioOutput* audioOutput = (CRestrictedAudioOutput*)aCustomInterface;
    CRestrictedAudioOutputMessageHandler* self = new (ELeave) CRestrictedAudioOutputMessageHandler(audioOutput);

	self->ConstructL();
    return self;
    }
    
// -----------------------------------------------------------------------------
// CRestrictedAudioOutputMessageHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CRestrictedAudioOutputMessageHandler::ConstructL()
	{
	}

    
// Destructor
CRestrictedAudioOutputMessageHandler::~CRestrictedAudioOutputMessageHandler()
	{
	delete iAudioOutput;
	}


// ---------------------------------------------------------
// CRestrictedAudioOutputMessageHandler::AudioOutputL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TUid CRestrictedAudioOutputMessageHandler::Uid()
	{
	return KUidRestrictedAudioOutput;
	}

// ---------------------------------------------------------
// CRestrictedAudioOutputMessageHandler::SetAudioOutputL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CRestrictedAudioOutputMessageHandler::HandleRequest(TMMFMessage& aMessage)
	{
	ASSERT(aMessage.Destination().InterfaceId() == KUidRestrictedAudioOutput);
	TRAPD(error,DoHandleRequestL(aMessage));
	if(error)
		{
		aMessage.Complete(error);
		}
	}

// ---------------------------------------------------------
// CRestrictedAudioOutputMessageHandler::DoHandleRequestL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CRestrictedAudioOutputMessageHandler::DoHandleRequestL(TMMFMessage& aMessage)
	{

	switch(aMessage.Function())
		{
		case ERaofCommit:
			{
			DoCommitL(aMessage);
			break;
			}			
		default:
			{
			aMessage.Complete(KErrNotSupported);
			}
		}
	}
	
	
// ---------------------------------------------------------
// CRestrictedAudioOutputMessageHandler::DoCommitL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CRestrictedAudioOutputMessageHandler::DoCommitL(TMMFMessage& aMessage)
	{
#ifdef _DEBUG 	
	RDebug::Print(_L("CRestrictedAudioOutputMessageHandler::DoCommitL"));
#endif
	
	TPckgBuf<TInt> countPckg;
	aMessage.ReadData1FromClient(countPckg);
	TInt count = countPckg();
	
#ifdef _DEBUG 	
	RDebug::Print(_L("CRestrictedAudioOutputMessageHandler::DoCommitL Count: %d"),count);
#endif
		
	CArrayFixFlat<CRestrictedAudioOutput::TAllowedOutputPreference>* outputArray =
		new(ELeave) CArrayFixFlat<CRestrictedAudioOutput::TAllowedOutputPreference>(4);
	CleanupStack::PushL(outputArray);
	outputArray->ResizeL(count);
	
	TInt length = count * outputArray->Length();
	TPtr8 arrayPtr((TUint8*)&(*outputArray)[0],length, length);
	aMessage.ReadData2FromClient(arrayPtr);
	
	CRestrictedAudioOutput::TAllowedOutputPreference pref;	
	
	TInt err = iAudioOutput->Reset();
	
	for (TInt i = 0; i < count; i++)
	{
		pref = (outputArray->Array())[i];
#ifdef _DEBUG 
		RDebug::Print(_L("DoCommitL Calling Append for: %d"),pref);
#endif		
		TInt err = iAudioOutput->AppendAllowedOutput(pref);
	}
	
#ifdef _DEBUG 		
	RDebug::Print(_L("CRestrictedAudioOutputMessageHandler::DoCommitL Calling Commit..."));
#endif

	iAudioOutput->Commit();	
	CleanupStack::PopAndDestroy();	//outputArray
		
	aMessage.Complete(KErrNone);
	}
		

// End of File
