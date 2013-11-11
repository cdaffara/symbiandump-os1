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
* Description:   This is the implementation of the CAudioOutput class.
*
*/




// INCLUDE FILES
#include <mmf/server/sounddevice.h>
#include "AudioOutputProxy.h"
#include "AudioOutputMessageTypes.h"
#include <CustomInterfaceUtility.h>
#include <CustomCommandUtility.h>
#include <mdaaudiotoneplayer.h>
#include <mdaaudiooutputstream.h>
#include <AudioOutput.h>
#include <DrmAudioSamplePlayer.h>
#include <videoplayer.h>

#ifdef _DEBUG
#define DEBPRN0         RDebug::Printf( "%s", __PRETTY_FUNCTION__);
#define DEBPRN1(str)    RDebug::Printf( "%s %s", __PRETTY_FUNCTION__, str );
#else
#define DEBPRN0
#define DEBPRN1(str)
#endif

// Two-phased constructor.
EXPORT_C CAudioOutput* CAudioOutput::NewL(CMdaAudioPlayerUtility& aUtility)
    {
    DEBPRN0;
    CAudioOutputProxy* audioOutput = NULL;
    CCustomInterfaceUtility* customInterface;

    customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL( customInterface );

    audioOutput = (CAudioOutputProxy*)customInterface->CustomInterface(KUidAudioOutput);

	if (audioOutput == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);
    return audioOutput;
    }


// Two-phased constructor.
EXPORT_C CAudioOutput* CAudioOutput::NewL(CMdaAudioRecorderUtility& aUtility, TBool aRecordStream)
    {
    DEBPRN0;
    CAudioOutputProxy* audioOutput = NULL;
    CCustomInterfaceUtility* customInterface;

    customInterface = CCustomInterfaceUtility::NewL(aUtility,aRecordStream);
    CleanupStack::PushL( customInterface );

    audioOutput = (CAudioOutputProxy*)customInterface->CustomInterface(KUidAudioOutput);

	if (audioOutput == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);
    return audioOutput;
    }


// Two-phased constructor.
EXPORT_C CAudioOutput* CAudioOutput::NewL(CMdaAudioOutputStream& aUtility)
    {
    DEBPRN0;
    CAudioOutputProxy* audioOutput = NULL;
    audioOutput = (CAudioOutputProxy*)aUtility.CustomInterface(KUidAudioOutput);

	if (audioOutput == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

    return audioOutput;
    }

// Two-phased constructor.
EXPORT_C CAudioOutput* CAudioOutput::NewL(CMdaAudioToneUtility& aUtility)
    {
    DEBPRN0;
    CAudioOutputProxy* audioOutput = NULL;
    audioOutput = (CAudioOutputProxy*)aUtility.CustomInterface(KUidAudioOutput);

	if (audioOutput == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

    return audioOutput;
    }

// Two-phased constructor.
EXPORT_C CAudioOutput* CAudioOutput::NewL(CMMFDevSound& aDevSound)
    {
    DEBPRN0;
    CAudioOutputProxy* audioOutput = NULL;
    audioOutput = (CAudioOutputProxy*)aDevSound.CustomInterface(KUidAudioOutput);

	if (audioOutput == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

    return audioOutput;
    }

// Two-phased constructor.
EXPORT_C CAudioOutput* CAudioOutput::NewL(MCustomInterface& aUtility)
    {
    DEBPRN0;
	CAudioOutputProxy* audioOutput = (CAudioOutputProxy*)aUtility.CustomInterface(KUidAudioOutput);

	if ( !audioOutput )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

    return audioOutput;
    }

// Two-phased constructor.
EXPORT_C CAudioOutput* CAudioOutput::NewL(MCustomCommand& aUtility)
    {
    DEBPRN0;
    CAudioOutputProxy* audioOutput = NULL;
    CCustomInterfaceUtility* customInterface;

    customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL( customInterface );

    audioOutput = (CAudioOutputProxy*)customInterface->CustomInterface(KUidAudioOutput);

	if (audioOutput == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);
    return audioOutput;
    }

// Two-phased constructor.
EXPORT_C CAudioOutput* CAudioOutput::NewL(CMidiClientUtility& aUtility)
    {
    DEBPRN0;
    CAudioOutputProxy* audioOutput = NULL;
    CCustomInterfaceUtility* customInterface;

    customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL( customInterface );

    audioOutput = (CAudioOutputProxy*)customInterface->CustomInterface(KUidAudioOutput);

	if (audioOutput == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);
    return audioOutput;
    }

// Two-phased constructor.
EXPORT_C CAudioOutput* CAudioOutput::NewL(CDrmPlayerUtility& aUtility)
    {
    DEBPRN0;
    CAudioOutputProxy* audioOutput = NULL;
    CCustomInterfaceUtility* customInterface;

    customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL( customInterface );

    audioOutput = (CAudioOutputProxy*)customInterface->CustomInterface(KUidAudioOutput);

	if (audioOutput == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);
    return audioOutput;
    }

EXPORT_C CAudioOutput* CAudioOutput::NewL(CVideoPlayerUtility& aUtility)
    {
    DEBPRN0;
    CAudioOutputProxy* audioOutput = NULL;
    CCustomInterfaceUtility* customInterface;

    customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL( customInterface );

    audioOutput = (CAudioOutputProxy*)customInterface->CustomInterface(KUidAudioOutput);

	if (audioOutput == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);
    return audioOutput;
    }

// End of file

