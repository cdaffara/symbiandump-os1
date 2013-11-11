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
* Description:   This is the implementation of the CRestrictedAudioOutputImpl class.
*
*/




// INCLUDE FILES
#include <e32svr.h>
#include "RestrictedAudioOutputImpl.h"

#ifdef _DEBUG
#define DEBPRN0(str)                RDebug::Print(str, this)
#define DEBPRN1(str)    RDebug::Printf( "%s %s", __PRETTY_FUNCTION__, str );
#else
#define DEBPRN0
#define DEBPRN1(str)
#endif

const TInt KGranularity=20;


// -----------------------------------------------------------------------------
// CRestrictedAudioOutputImpl::CRestrictedAudioOutputImpl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CRestrictedAudioOutputImpl::CRestrictedAudioOutputImpl()
: iAllowedOutputPrefArray(KGranularity)
    {
	DEBPRN0(_L("CRestrictedAudioOutputImpl::[0x%x]::CRestrictedAudioOutputImpl\n"));  
	iAllowedOutputPrefArray.Reset();  
    }


// -----------------------------------------------------------------------------
// CRestrictedAudioOutputImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// assumes that iParent has already been set up properly
// -----------------------------------------------------------------------------
//
void CRestrictedAudioOutputImpl::ConstructL()
    {
	   
    }


// Two-phased constructor.
CRestrictedAudioOutputImpl* CRestrictedAudioOutputImpl::NewL()
    {
#ifdef _DEBUG    
	RDebug::Print(_L("CRestrictedAudioOutputImpl::NewL\n"));
#endif	
	    
	CRestrictedAudioOutputImpl* self = new(ELeave) CRestrictedAudioOutputImpl();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;  

    }
    
// -----------------------------------------------------------------------------
// CRestrictedAudioOutputImpl::~CRestrictedAudioOutputImpl
// Destructor
// -----------------------------------------------------------------------------
//
CRestrictedAudioOutputImpl::~CRestrictedAudioOutputImpl()
    {
	DEBPRN0(_L("CRestrictedAudioOutputImpl[0x%x]::~CRestrictedAudioOutputImpl\n"));     
	
	iAllowedOutputPrefArray.Close();
 
    DEBPRN0(_L("CRestrictedAudioOutputImpl[0x%x]::~CRestrictedAudioOutputImpl:EXIT"));
    } 
       
// ---------------------------------------------------------
// CRestrictedAudioOutputImpl::AppendAllowedOutput
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CRestrictedAudioOutputImpl::AppendAllowedOutput( CRestrictedAudioOutput::TAllowedOutputPreference aOutput)
    {
	DEBPRN0(_L("CRestrictedAudioOutputImpl[0x%x]::AppendAllowedOutput\n"));
	
#ifdef _DEBUG	
    RDebug::Print(_L("Append Output: %d\n"), aOutput);
#endif    
    
    // Check to see if it's in array already
  	TBool found = ExistsInArray(aOutput);
    if (found)
    {
#ifdef _DEBUG    
		RDebug::Print(_L("Append Output ERROR. Output already in List\n"));  
#endif		  
    	return KErrAlreadyExists;
    }
  
  	TInt err = KErrNone;
	TRAP(err,iAllowedOutputPrefArray.AppendL(aOutput)); 
	if (err != KErrNone)
		return err;
    
    return KErrNone;
    }

// ---------------------------------------------------------
// CRestrictedAudioOutputImpl::RemoveAllowedOutput
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CRestrictedAudioOutputImpl::RemoveAllowedOutput(CRestrictedAudioOutput::TAllowedOutputPreference aOutput)
    {
#ifdef _DEBUG    
 	RDebug::Print(_L("Remove Output: %d\n"), aOutput); 
#endif
 	   
    // Make sure it's in list
  	TBool found = ExistsInArray(aOutput);
    if (!found)
    {
	#ifdef _DEBUG    
		RDebug::Print(_L("Remove Output ERROR. Output not in List\n"));  
	#endif		  
    	return KErrNotFound;
    } 
    
    for (TInt i = 0; i < iAllowedOutputPrefArray.Count(); i++)
    {
    	if (iAllowedOutputPrefArray[i] == aOutput)
    	{
    		iAllowedOutputPrefArray.Remove(i);
 	#ifdef _DEBUG   		
    		RDebug::Print(_L("RemoveAllowedOutput: Item Removed\n"));
 	#endif   		
    		break;	
    	}
    }
    	
    return KErrNone;
    }

// ---------------------------------------------------------
// CRestrictedAudioOutputImpl::GetAllowedOutput
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CRestrictedAudioOutputImpl::GetAllowedOutput(TInt aIndex, CRestrictedAudioOutput::TAllowedOutputPreference& aOutput)
    {   
#ifdef _DEBUG    
	RDebug::Print(_L("CRestrictedAudioOutputImpl::GetAllowedOutput for index: %d\n"), aIndex);   
#endif
	   
    // Verify aIndex valid:
    if (aIndex >= iAllowedOutputPrefArray.Count())
    {
    #ifdef _DEBUG
		RDebug::Print(_L("GetAllowedOutput ERROR. Invalid Index: %d\n"),aIndex);   
	#endif 
    	return KErrNotFound;    	
    }
    aOutput = iAllowedOutputPrefArray[aIndex];
    #ifdef _DEBUG
	RDebug::Print(_L("GetAllowedOutput: Returning %d\n"),aOutput);  
	#endif     
    return KErrNone;
    }
    
// ---------------------------------------------------------
// CRestrictedAudioOutputImpl::GetAllowedOutputCount
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CRestrictedAudioOutputImpl::GetAllowedOutputCount(TInt& aSize)
    {
#ifdef _DEBUG    
	RDebug::Print(_L("CRestrictedAudioOutputImpl::GetAllowedOutputCount\n")); 
#endif    
    aSize = iAllowedOutputPrefArray.Count();
    return KErrNone;
    }
    
// ---------------------------------------------------------
// CRestrictedAudioOutputImpl::Reset
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CRestrictedAudioOutputImpl::Reset()
    {
    iAllowedOutputPrefArray.Reset();
    return KErrNone;
    }
    
// ---------------------------------------------------------
// CRestrictedAudioOutputImpl::Commit
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CRestrictedAudioOutputImpl::Commit()
    {
    return KErrNone;
    }    
    
// ---------------------------------------------------------
// CRestrictedAudioOutputImpl::ExistsInArray
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
TBool CRestrictedAudioOutputImpl::ExistsInArray(CRestrictedAudioOutput::TAllowedOutputPreference& aOutput)
    {
    
    for (TInt i = 0; i < iAllowedOutputPrefArray.Count(); i++)
    {
    	if (iAllowedOutputPrefArray[i] == aOutput)
    	return ETrue;
    }
    
    return EFalse;
    }       
    
// End of file

