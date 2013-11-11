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
* Description: Audio input base implementation  
*
*/



// INCLUDE FILES
#include <AudioInput.h>
#include <mmf/server/sounddevice.h>
#include "AudioInputProxy.h"
#include "AudioInputMessageTypes.h"
#include <CustomInterfaceUtility.h>
#include <CustomCommandUtility.h>
#include <mdaaudioinputstream.h>
#include <videorecorder.h>

#ifdef _DEBUG
#define DEBPRN0         RDebug::Printf( "%s", __PRETTY_FUNCTION__);
#define DEBPRN1(str)    RDebug::Printf( "%s %s", __PRETTY_FUNCTION__, str );
#else
#define DEBPRN0
#define DEBPRN1(str)
#endif

// Two-phased constructor.
EXPORT_C CAudioInput* CAudioInput::NewL(CMdaAudioRecorderUtility& aUtility)
    {
    DEBPRN0;
    CAudioInputProxy* audioInput;
    CCustomInterfaceUtility* customInterface;
    
    //customInterface = CCustomInterfaceUtility::NewL(aUtility,ECCRecording);
    customInterface = CCustomInterfaceUtility::NewL(aUtility,ETrue);
    CleanupStack::PushL(customInterface);
    
    // The ownership of customInterface is taken by the audioInput
    audioInput = (CAudioInputProxy*)customInterface->CustomInterface(KUidAudioInput);
    
    if (audioInput == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}
	CleanupStack::Pop(customInterface);
    return audioInput;
    }

EXPORT_C CAudioInput* CAudioInput::NewL(CMMFDevSound& aDevSound)
    {
    DEBPRN0;
    CAudioInputProxy* audioInput = NULL;
    audioInput = (CAudioInputProxy*)aDevSound.CustomInterface(KUidAudioInput);
	if (audioInput == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}
    return audioInput;
    }

// Two-phased constructor.
EXPORT_C CAudioInput* CAudioInput::NewL(MCustomCommand& aUtility)
    {
    DEBPRN0;
    CAudioInputProxy* audioInput;
    CCustomInterfaceUtility* customInterface;
    
    customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);
    
    audioInput = (CAudioInputProxy*)customInterface->CustomInterface(KUidAudioInput);
	if (audioInput == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}
	
	CleanupStack::Pop(customInterface);	
    return audioInput;
    }

// Two-phased constructor.
EXPORT_C CAudioInput* CAudioInput::NewL(MCustomInterface& aUtility)
    {
    DEBPRN0;
	CAudioInput* audioInput = (CAudioInputProxy*)aUtility.CustomInterface(KUidAudioInput);

	if ( !audioInput )
		{
		DEBPRN1("No Adaptation Support - leaving"); 
		User::Leave(KErrNotSupported);
		}

    return audioInput;
    }

EXPORT_C CAudioInput* CAudioInput::NewL(CMdaAudioInputStream& aUtility)
    {
    DEBPRN0;
    CAudioInputProxy* audioInput = NULL;
    audioInput = (CAudioInputProxy*)aUtility.CustomInterface(KUidAudioInput);
	if (audioInput == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}
    return audioInput;
    }

EXPORT_C CAudioInput* CAudioInput::NewL(CVideoRecorderUtility& aUtility)
    {
    DEBPRN0;
    CAudioInputProxy* audioInput;
    CCustomInterfaceUtility* customInterface;
    
    customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);
    
    // The ownership of customInterface is taken by the audioInput
    audioInput = (CAudioInputProxy*)customInterface->CustomInterface(KUidAudioInput);
	if (audioInput == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);
    return audioInput;
    }
    
// End of File
