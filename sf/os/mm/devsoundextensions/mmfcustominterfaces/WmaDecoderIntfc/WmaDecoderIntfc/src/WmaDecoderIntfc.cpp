/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Definition of APIs for the Windows Media Audio 10 Pro decoder 
*              : Decoder Custom Interface.
*
*/



// INCLUDE FILES
#include "WmaDecoderIntfcProxy.h"
#include "WmaDecoderIntfc.h"
#include <mdaaudioinputstream.h>
#include <mdaaudiooutputstream.h>
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

//--------------------------------------------------------------------------------
// CWmaDecoderIntfc::NewL
// Creates the custom interface.
//--------------------------------------------------------------------------------
EXPORT_C CWmaDecoderIntfc* 
CWmaDecoderIntfc::NewL(CMMFDevSound& aDevSound)
    {
    CWmaDecoderIntfcProxy* wmaDecoderIntfcProxy;
    wmaDecoderIntfcProxy =
       (CWmaDecoderIntfcProxy*)aDevSound.CustomInterface(
                                              KUidWmaDecoderIntfc);
    if (!wmaDecoderIntfcProxy)
        {
        User::Leave(KErrNotFound);
        }
    return wmaDecoderIntfcProxy;
    }

//--------------------------------------------------------------------------------
// CWmaDecoderIntfc::NewL
// Creates the custom interface.
//--------------------------------------------------------------------------------
EXPORT_C CWmaDecoderIntfc*
CWmaDecoderIntfc::NewL(CMdaAudioInputStream& /*aUtility*/)
    {
	CWmaDecoderIntfc* ptr(NULL);
	User::Leave(KErrNotSupported);
	return ptr;
    }

//--------------------------------------------------------------------------------
// CWmaDecoderIntfc::NewL
// Creates the custom interface.
//--------------------------------------------------------------------------------
EXPORT_C CWmaDecoderIntfc*
CWmaDecoderIntfc::NewL(CMdaAudioOutputStream& aUtility)
    {
    CWmaDecoderIntfcProxy* wmaDecoderIntfcProxy;
    wmaDecoderIntfcProxy = (CWmaDecoderIntfcProxy*)aUtility.CustomInterface(KUidWmaDecoderIntfc);
    if (!wmaDecoderIntfcProxy)
        {
        User::Leave(KErrNotFound);
        }     
    return wmaDecoderIntfcProxy;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
