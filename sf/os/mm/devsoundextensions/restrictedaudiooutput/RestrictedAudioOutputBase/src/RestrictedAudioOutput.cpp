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
* Description:   This is the implementation of the CRestrictedAudioOutput class.
*
*/




// INCLUDE FILES
#include <RestrictedAudioOutputProxy.h>
#include <RestrictedAudioOutputMessageTypes.h>
#include <RestrictedAudioOutput.h>
#include "RestrictedAudioOutputImpl.h"


    
EXPORT_C CRestrictedAudioOutput::CRestrictedAudioOutput()

	{
	RDebug::Print(_L("CRestrictedAudioOutput[%x]:CRestrictedAudioOutput\n"),this);
	}    
// -----------------------------------------------------------------------------
// CRestrictedAudioOutput::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CRestrictedAudioOutput::ConstructL()
    {
#ifdef _DEBUG    
	RDebug::Print(_L("CRestrictedAudioOutput::ConstructL\n"));  
#endif	  
    iImpl = CRestrictedAudioOutputImpl::NewL();
    }
       
// -----------------------------------------------------------------------------
// CMMFDevSoundAdaptation::DestructL
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CRestrictedAudioOutput::~CRestrictedAudioOutput()
    {
#ifdef _DEBUG    
	RDebug::Print(_L("CRestrictedAudioOutput[%x]RestrictedAudioOutput\n"),this); 
#endif	
    delete iImpl;
    }

// ---------------------------------------------------------
// CRestrictedAudioOutput::AppendAllowedOutput
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CRestrictedAudioOutput::AppendAllowedOutput(TAllowedOutputPreference aOutput)
    {
#ifdef _DEBUG    
	RDebug::Print(_L("CRestrictedAudioOutput[%x]AppendAllowedOutput\n"),this); 
#endif   
    return iImpl->AppendAllowedOutput(aOutput);
    }

// ---------------------------------------------------------
// CRestrictedAudioOutput::RemoveAllowedOutput
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CRestrictedAudioOutput::RemoveAllowedOutput(TAllowedOutputPreference aOutput)
    {
    
    return iImpl->RemoveAllowedOutput(aOutput);
    }

// ---------------------------------------------------------
// CRestrictedAudioOutput::GetAllowedOutput
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CRestrictedAudioOutput::GetAllowedOutput(TInt aIndex, TAllowedOutputPreference& aOutput)
    {
    return iImpl->GetAllowedOutput(aIndex, aOutput);
    }
    
// ---------------------------------------------------------
// CRestrictedAudioOutput::GetAllowedOutputCount
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CRestrictedAudioOutput::GetAllowedOutputCount(TInt& aSize)
    {
#ifdef _DEBUG    
	RDebug::Print(_L("CRestrictedAudioOutput[%x]GetAllowedOutputCount\n"),this);
#endif	      
    return iImpl->GetAllowedOutputCount(aSize);
    }
    
// ---------------------------------------------------------
// CRestrictedAudioOutput::Reset
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CRestrictedAudioOutput::Reset()
    {
    return iImpl->Reset();
    }
    
// ---------------------------------------------------------
// CRestrictedAudioOutput::Commit
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt CRestrictedAudioOutput::Commit()
    {
    return iImpl->Commit();
    }    
    
// End of file

