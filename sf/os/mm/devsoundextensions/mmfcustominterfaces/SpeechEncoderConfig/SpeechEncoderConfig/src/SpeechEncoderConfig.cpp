/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Interface definition for speech encoder configuration inteface.
*
*/



// INCLUDE FILES
#include "SpeechEncoderConfigProxy.h"
#include "SpeechEncoderConfig.h"

#include <mdaaudioinputstream.h>
#include <sounddevice.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSpeechEncoderConfig::NewL
// Creates the custom interface.
// Calls the custom interface method on the devsound passed in.
// Devsound will return the handle to the custom interface proxy.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSpeechEncoderConfig* 
CSpeechEncoderConfig::NewL(CMMFDevSound& aDevSound)
    {
    CSpeechEncoderConfigProxy* speechEncoderConfigProxy;
    speechEncoderConfigProxy = 
        (CSpeechEncoderConfigProxy*)aDevSound.CustomInterface(
                                              KUidSpeechEncoderConfig);
    if (!speechEncoderConfigProxy)
        {
        User::Leave(KErrNotFound);
        }
    return speechEncoderConfigProxy;
    }

// -----------------------------------------------------------------------------
// CSpeechEncoderConfig::NewL
// Creates the custom interface.
// Calls the custom interface method on the CMdaAudioInputStream passed in.
// The CMdaAudioInputStream utility will return the handle to the custom 
// interface proxy.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C CSpeechEncoderConfig* 
CSpeechEncoderConfig::NewL(CMdaAudioInputStream& aUtility)
    {
    CSpeechEncoderConfigProxy* speechEncoderConfigProxy;
    speechEncoderConfigProxy = 
        (CSpeechEncoderConfigProxy*)aUtility.CustomInterface(
                                             KUidSpeechEncoderConfig);
    if (!speechEncoderConfigProxy)
        {
        User::Leave(KErrNotFound);
        }
    return speechEncoderConfigProxy;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
