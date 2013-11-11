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
* Description:   This is the implementation of the CRestrictedAudioOutputProxy class.
*
*/



// INCLUDE FILES
#include <e32svr.h>
#include <RestrictedAudioOutput.h>
#include <RestrictedAudioOutputProxy.h>
#include <RestrictedAudioOutputMessageTypes.h>
#include <CustomInterfaceUtility.h>

    
// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CRestrictedAudioOutputProxy::CRestrictedAudioOutputProxy(
	TMMFMessageDestinationPckg aMessageHandler,
	MCustomCommand& aCustomCommand,
	CCustomInterfaceUtility* aCustomInterfaceUtility )
	: 	iCustomCommand(&aCustomCommand),
		iMessageHandler(aMessageHandler),
		iCustomInterfaceUtility(aCustomInterfaceUtility),
		iArray(4)

    {  

    }

// Two-phased constructor.
EXPORT_C CRestrictedAudioOutputProxy* CRestrictedAudioOutputProxy::NewL(
	TMMFMessageDestinationPckg aMessageHandler,
    MCustomCommand& aCustomCommand,
    CCustomInterfaceUtility* aCustomInterfaceUtility)
    {
    
#ifdef _DEBUG    
	RDebug::Print(_L("CRestrictedAudioOutputProxy::NewL:\n"));
#endif


	    
  	CRestrictedAudioOutputProxy* self = new(ELeave) CRestrictedAudioOutputProxy(aMessageHandler, aCustomCommand, aCustomInterfaceUtility);
 	CleanupStack::PushL(self);
 	self->ConstructL();  // Call base class ConstructL
    CleanupStack::Pop(self);    
    return self;
    
    }	
      
 EXPORT_C CRestrictedAudioOutputProxy* CRestrictedAudioOutputProxy::NewL(CMMFDevSound& aDevSound)
    {
    
#ifdef _DEBUG    
	RDebug::Print(_L("CRestrictedAudioOutputProxy::NewL (DevSound):\n"));
#endif

  	CRestrictedAudioOutputProxy* audioOutputProxy = (CRestrictedAudioOutputProxy*)aDevSound.CustomInterface(KUidRestrictedAudioOutput);	  	
   	
	if (audioOutputProxy == NULL)
	{
#ifdef _DEBUG	
        RDebug::Print(_L("No Adaptation Support - leaving")); 
#endif          
    	User::Leave(KErrNotSupported);
	} 
		  	 
    return audioOutputProxy;
    
    }	       

// Destructor
CRestrictedAudioOutputProxy::~CRestrictedAudioOutputProxy()
    {
#ifdef _DEBUG 
	RDebug::Print(_L("CRestrictedAudioOutputProxy::~CRestrictedAudioOutputProxy\n"));
#endif 
	  
	iArray.Reset();	   
    // Remove the custom interface message handler before we destroy the proxy.
    if (iCustomInterfaceUtility)
    {
    iCustomInterfaceUtility->RemoveCustomInterface(iMessageHandler);
    }
	delete iCustomInterfaceUtility;
 
    }
    
// ---------------------------------------------------------
// CRestrictedAudioOutput::Uid
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C const TUid CRestrictedAudioOutputProxy::Uid()
	{
	return KUidRestrictedAudioOutput;
	}

// ---------------------------------------------------------
// CRestrictedAudioOutputProxy::Commit
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CRestrictedAudioOutputProxy::Commit()
	{
#ifdef _DEBUG 	
	RDebug::Print(_L("CRestrictedAudioOutputProxy::Commit"));
#endif
	
	TInt count;
	TAllowedOutputPreference pref;
	iArray.Reset();
	
	TInt err = GetAllowedOutputCount(count);
		
#ifdef _DEBUG 		
	RDebug::Print(_L("CRestrictedAudioOutputProxy::Commit: Current array count = %d"), count);
#endif		
	
	for (TInt i=0; i < count;i++)
	{
		err = GetAllowedOutput(i, pref);		
		TRAP(err,iArray.AppendL(pref));		
	}

	TInt length = iArray.Length() * count;
	TPtrC8 arrayPtr((const TUint8*)&iArray[0], length);
	TPckgC<TInt> countPckg(count);
	
	iCustomCommand->CustomCommandSync(iMessageHandler, ERaofCommit, countPckg, arrayPtr );
 	
	
	return KErrNone;	
	}
	

// End of File
