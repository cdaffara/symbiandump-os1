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
* Description:   Implementation of the Loudness effect class
*
*/




// INCLUDE FILES

#ifdef _DEBUG
#include <e32svr.h>
#endif

#include <LoudnessBase.h>
#include <CustomInterfaceUtility.h>
#include "LoudnessProxy.h"
#include <DrmAudioSamplePlayer.h>
#include <mdaaudioinputstream.h>
#include <mdaaudiooutputstream.h>
#include <mdaaudiotoneplayer.h>
#include <mmf/server/sounddevice.h>
#include <videoplayer.h>

#ifdef _DEBUG
#define DEBPRN0         RDebug::Printf( "%s", __PRETTY_FUNCTION__);
#define DEBPRN1(str)    RDebug::Printf( "%s %s", __PRETTY_FUNCTION__, str );
#else
#define DEBPRN0
#define DEBPRN1(str)
#endif

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLoudness::CLoudness
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CLoudness::CLoudness()
    : 	iLoudnessData(),
    	iDataPckgTo(iLoudnessData)
    {
    }

// Destructor
EXPORT_C CLoudness::~CLoudness()
    {
    }

// -----------------------------------------------------------------------------
// CLoudness::NewL
// Static function for creating an instance of the Loudness object.
// -----------------------------------------------------------------------------
//
EXPORT_C CLoudness* CLoudness::NewL(CMdaAudioConvertUtility& aUtility)
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CLoudnessProxy* loudnessProxy = (CLoudnessProxy*)customInterface->CustomInterface(KUidLoudnessEffect);

	if ( !loudnessProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		CleanupStack::Pop(customInterface);
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return loudnessProxy;
	}

// -----------------------------------------------------------------------------
// CLoudness::NewL
// Static function for creating an instance of the Loudness object.
// -----------------------------------------------------------------------------
//
EXPORT_C CLoudness* CLoudness::NewL(
	CMdaAudioInputStream& aUtility , TBool aEnable )
	{

    DEBPRN0;
    CLoudnessProxy* loudnessProxy = (CLoudnessProxy*)aUtility.CustomInterface(KUidLoudnessEffect);

	if (loudnessProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

	if(aEnable)
	    {
		loudnessProxy->EnableL();
	    }

    return loudnessProxy;
	}

// -----------------------------------------------------------------------------
// CLoudness::NewL
// Static function for creating an instance of the Loudness object.
// -----------------------------------------------------------------------------
//
EXPORT_C CLoudness* CLoudness::NewL(
	CMdaAudioOutputStream& aUtility, TBool aEnable )
	{

    DEBPRN0;
    CLoudnessProxy* loudnessProxy = (CLoudnessProxy*)aUtility.CustomInterface(KUidLoudnessEffect);

	if (loudnessProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

	if(aEnable)
	    {
		loudnessProxy->EnableL();
	    }

    return loudnessProxy;
	}

// -----------------------------------------------------------------------------
// CLoudness::NewL
// Static function for creating an instance of the Loudness object.
// -----------------------------------------------------------------------------
//
EXPORT_C CLoudness* CLoudness::NewL(
	CMdaAudioPlayerUtility& aUtility , TBool aEnable)
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CLoudnessProxy* loudnessProxy = (CLoudnessProxy*)customInterface->CustomInterface(KUidLoudnessEffect);

	if ( !loudnessProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		CleanupStack::Pop(customInterface);
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

	if(aEnable)
        {
		loudnessProxy->EnableL();
	    }

    return loudnessProxy;
	}

// -----------------------------------------------------------------------------
// CLoudness::NewL
// Static function for creating an instance of the Loudness object.
// -----------------------------------------------------------------------------
//
EXPORT_C CLoudness* CLoudness::NewL(
	CMdaAudioRecorderUtility& aUtility,	TBool aRecordStream, TBool aEnable)
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility, aRecordStream);
    CleanupStack::PushL(customInterface);

	CLoudnessProxy* loudnessProxy = (CLoudnessProxy*)customInterface->CustomInterface(KUidLoudnessEffect);

	if ( !loudnessProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		CleanupStack::Pop(customInterface);
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

	if(aEnable)
	{
		loudnessProxy->EnableL();
	}

    return loudnessProxy;
	}

// -----------------------------------------------------------------------------
// CLoudness::NewL
// Static function for creating an instance of the Loudness object.
// -----------------------------------------------------------------------------
//
EXPORT_C CLoudness* CLoudness::NewL(
	CMdaAudioToneUtility& aUtility, TBool aEnable )
	{

	DEBPRN0;
    CLoudnessProxy* loudnessProxy = (CLoudnessProxy*)aUtility.CustomInterface(KUidLoudnessEffect);

	if (loudnessProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

	if(aEnable)
	    {
		loudnessProxy->EnableL();
	    }

    return loudnessProxy;
	}


// -----------------------------------------------------------------------------
// CLoudness::NewL
// Static function for creating an instance of the Loudness object.
// -----------------------------------------------------------------------------
//
EXPORT_C CLoudness* CLoudness::NewL(
	CMMFDevSound& aDevSound , TBool aEnable )
	{

	DEBPRN0;
    CLoudnessProxy* loudnessProxy = (CLoudnessProxy*)aDevSound.CustomInterface(KUidLoudnessEffect);

	if (loudnessProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

	if(aEnable)
	    {
		loudnessProxy->EnableL();
	    }

    return loudnessProxy;
	}



// -----------------------------------------------------------------------------
// CLoudness::NewL
// Static function for creating an instance of the Loudness object.
// -----------------------------------------------------------------------------
//
EXPORT_C CLoudness* CLoudness::NewL(
	CCustomCommandUtility* aUtility , TBool aEnable)
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CLoudnessProxy* loudnessProxy = (CLoudnessProxy*)customInterface->CustomInterface(KUidLoudnessEffect);

	if ( !loudnessProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		CleanupStack::Pop(customInterface);
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

	if(aEnable)
	    {
		loudnessProxy->EnableL();
	    }

    return loudnessProxy;
	}

// -----------------------------------------------------------------------------
// CLoudness::NewL
// Static function for creating an instance of the Loudness object.
// -----------------------------------------------------------------------------
//
EXPORT_C CLoudness* CLoudness::NewL(
	MCustomInterface& aCustomInterface , TBool aEnable)
	{

    DEBPRN0;
	CLoudnessProxy* loudnessProxy = (CLoudnessProxy*)aCustomInterface.CustomInterface(KUidLoudnessEffect);

	if ( !loudnessProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	if(aEnable)
	    {
		loudnessProxy->EnableL();
	    }

    return loudnessProxy;
	}

// -----------------------------------------------------------------------------
// CLoudness::NewL
// Static function for creating an instance of the Loudness object.
// -----------------------------------------------------------------------------
//
EXPORT_C CLoudness* CLoudness::NewL(
    CMidiClientUtility& aUtility , TBool aEnable)
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CLoudnessProxy* loudnessProxy = (CLoudnessProxy*)customInterface->CustomInterface(KUidLoudnessEffect);

	if ( !loudnessProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		CleanupStack::Pop(customInterface);
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

	if(aEnable)
    	{
		loudnessProxy->EnableL();
	    }

    return loudnessProxy;
	}

// -----------------------------------------------------------------------------
// CLoudness::NewL
// Static function for creating an instance of the Loudness object.
// -----------------------------------------------------------------------------
//
EXPORT_C CLoudness* CLoudness::NewL(
    CDrmPlayerUtility& aUtility, TBool aEnable)
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CLoudnessProxy* loudnessProxy = (CLoudnessProxy*)customInterface->CustomInterface(KUidLoudnessEffect);

	if ( !loudnessProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		CleanupStack::Pop(customInterface);
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

	if(aEnable)
	    {
		loudnessProxy->EnableL();
	    }

    return loudnessProxy;
	}

// -----------------------------------------------------------------------------
// CLoudness::NewL
// Static function for creating an instance of the Loudness object.
// -----------------------------------------------------------------------------
//
EXPORT_C CLoudness* CLoudness::NewL(
    CVideoPlayerUtility& aUtility, TBool aEnable)
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CLoudnessProxy* loudnessProxy = (CLoudnessProxy*)customInterface->CustomInterface(KUidLoudnessEffect);

	if ( !loudnessProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		CleanupStack::Pop(customInterface);
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

	if(aEnable)
	    {
		loudnessProxy->EnableL();
	    }

    return loudnessProxy;
	}

// -----------------------------------------------------------------------------
// CLoudness::Uid
// -----------------------------------------------------------------------------
//
EXPORT_C TUid CLoudness::Uid() const
	{
	return KUidLoudnessEffect;
	}


// -----------------------------------------------------------------------------
// CLoudness::DoEffectData
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CLoudness::DoEffectData()
	{
    DEBPRN0;
	iDataPckgTo = iLoudnessData;
	return iDataPckgTo;
	}

// -----------------------------------------------------------------------------
// CLoudness::SetEffectData
// -----------------------------------------------------------------------------
//
EXPORT_C void CLoudness::SetEffectData(
	const TDesC8& aEffectDataBuffer )
	{
    DEBPRN0;
	TEfLoudnessDataPckg dataPckg;
	dataPckg.Copy(aEffectDataBuffer);
	iLoudnessData = dataPckg();
	iEnabled = iLoudnessData.iEnabled;
	iEnforced = iLoudnessData.iEnforced;
	iHaveUpdateRights = iLoudnessData.iHaveUpdateRights;

	}


// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File

