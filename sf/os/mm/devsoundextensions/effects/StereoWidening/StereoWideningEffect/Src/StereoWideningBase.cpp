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
* Description:   Implementation of the StereoWidening effect class
*
*/




// INCLUDE FILES

#ifdef _DEBUG
#include <e32svr.h>
#endif

#include <StereoWideningData.h>
#include <StereoWideningBase.h>
#include <CustomInterfaceUtility.h>
#include "StereoWideningProxy.h"
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
// CStereoWidening::CStereoWidening
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CStereoWidening::CStereoWidening()
    : 	iStereoWideningData(0, 0),
    	iDataPckgTo(iStereoWideningData)
    {
    }

// Destructor
EXPORT_C CStereoWidening::~CStereoWidening()
    {
    }

// -----------------------------------------------------------------------------
// CStereoWidening::NewL
// Static function for creating an instance of the StereoWidening object.
// -----------------------------------------------------------------------------
//
EXPORT_C CStereoWidening* CStereoWidening::NewL()
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

// -----------------------------------------------------------------------------
// CStereoWidening::NewL
// Static function for creating an instance of the StereoWidening object.
// -----------------------------------------------------------------------------
//
EXPORT_C CStereoWidening* CStereoWidening::NewL(CMdaAudioConvertUtility& aUtility ,	TBool aEnable, TInt8 aInitialLevel)
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CStereoWideningProxy* stereoWideningProxy = (CStereoWideningProxy*)customInterface->CustomInterface(KUidStereoWideningEffect);

	if ( !stereoWideningProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

	stereoWideningProxy->SetStereoWideningLevelL(aInitialLevel);

	if(aEnable)
	{
		stereoWideningProxy->EnableL();
	}

    return stereoWideningProxy;
	}

// -----------------------------------------------------------------------------
// CStereoWidening::NewL
// Static function for creating an instance of the StereoWidening object.
// -----------------------------------------------------------------------------
//
EXPORT_C CStereoWidening* CStereoWidening::NewL(
	CMdaAudioInputStream& aUtility, TBool aEnable, TInt8 aInitialLevel )
	{

	DEBPRN0;
    CStereoWideningProxy* stereoWideningProxy = (CStereoWideningProxy*)aUtility.CustomInterface(KUidStereoWideningEffect);

	if (stereoWideningProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

    stereoWideningProxy->SetStereoWideningLevelL(aInitialLevel);

	if(aEnable)
	    {
		stereoWideningProxy->EnableL();
	    }

    return stereoWideningProxy;
	}

// -----------------------------------------------------------------------------
// CStereoWidening::NewL
// Static function for creating an instance of the StereoWidening object.
// -----------------------------------------------------------------------------
//
EXPORT_C CStereoWidening* CStereoWidening::NewL(
	CMdaAudioOutputStream& aUtility, TBool aEnable, TInt8 aInitialLevel )
	{

    DEBPRN0;
    CStereoWideningProxy* stereoWideningProxy = (CStereoWideningProxy*)aUtility.CustomInterface(KUidStereoWideningEffect);

	if (stereoWideningProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

    stereoWideningProxy->SetStereoWideningLevelL(aInitialLevel);

	if(aEnable)
	    {
		stereoWideningProxy->EnableL();
	    }

    return stereoWideningProxy;
	}

// -----------------------------------------------------------------------------
// CStereoWidening::NewL
// Static function for creating an instance of the StereoWidening object.
// -----------------------------------------------------------------------------
//
EXPORT_C CStereoWidening* CStereoWidening::NewL(
	CMdaAudioPlayerUtility& aUtility , TBool aEnable, TInt8 aInitialLevel)
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CStereoWideningProxy* stereoWideningProxy = (CStereoWideningProxy*)customInterface->CustomInterface(KUidStereoWideningEffect);

	if ( !stereoWideningProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

	stereoWideningProxy->SetStereoWideningLevelL(aInitialLevel);

	if(aEnable)
	{
		stereoWideningProxy->EnableL();
	}

    return stereoWideningProxy;
	}

// -----------------------------------------------------------------------------
// CStereoWidening::NewL
// Static function for creating an instance of the StereoWidening object.
// -----------------------------------------------------------------------------
//
EXPORT_C CStereoWidening* CStereoWidening::NewL(
	CMdaAudioRecorderUtility& aUtility,
	TBool aRecordStream, TBool aEnable, TInt8 aInitialLevel)
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility, aRecordStream);
    CleanupStack::PushL(customInterface);

	CStereoWideningProxy* stereoWideningProxy = (CStereoWideningProxy*)customInterface->CustomInterface(KUidStereoWideningEffect);

	if ( !stereoWideningProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

	stereoWideningProxy->SetStereoWideningLevelL(aInitialLevel);

	if(aEnable)
	{
		stereoWideningProxy->EnableL();
	}

    return stereoWideningProxy;
	}

// -----------------------------------------------------------------------------
// CStereoWidening::NewL
// Static function for creating an instance of the StereoWidening object.
// -----------------------------------------------------------------------------
//
EXPORT_C CStereoWidening* CStereoWidening::NewL(
	CMdaAudioToneUtility& aUtility, TBool aEnable, TInt8 aInitialLevel)
	{

	DEBPRN0;
    CStereoWideningProxy* stereoWideningProxy = (CStereoWideningProxy*)aUtility.CustomInterface(KUidStereoWideningEffect);

	if (stereoWideningProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

    stereoWideningProxy->SetStereoWideningLevelL(aInitialLevel);

	if(aEnable)
	    {
		stereoWideningProxy->EnableL();
	    }

    return stereoWideningProxy;
	}


// -----------------------------------------------------------------------------
// CStereoWidening::NewL
// Static function for creating an instance of the StereoWidening object.
// -----------------------------------------------------------------------------
//
EXPORT_C CStereoWidening* CStereoWidening::NewL(
	CMMFDevSound& aDevSound, TBool aEnable, TInt8 aInitialLevel)
	{

	DEBPRN0;
    CStereoWideningProxy* stereoWideningProxy = (CStereoWideningProxy*)aDevSound.CustomInterface(KUidStereoWideningEffect);

	if (stereoWideningProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

    stereoWideningProxy->SetStereoWideningLevelL(aInitialLevel);

	if(aEnable)
	    {
		stereoWideningProxy->EnableL();
	    }

    return stereoWideningProxy;
	}



// -----------------------------------------------------------------------------
// CStereoWidening::NewL
// Static function for creating an instance of the StereoWidening object.
// -----------------------------------------------------------------------------
//
EXPORT_C CStereoWidening* CStereoWidening::NewL(
	CCustomCommandUtility* aUtility , TBool aEnable, TInt8 aInitialLevel)
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CStereoWideningProxy* stereoWideningProxy = (CStereoWideningProxy*)customInterface->CustomInterface(KUidStereoWideningEffect);

	if ( !stereoWideningProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

	stereoWideningProxy->SetStereoWideningLevelL(aInitialLevel);

	if(aEnable)
	{
		stereoWideningProxy->EnableL();
	}

    return stereoWideningProxy;
	}

// -----------------------------------------------------------------------------
// CStereoWidening::NewL
// Static function for creating an instance of the StereoWidening object.
// -----------------------------------------------------------------------------
//
EXPORT_C CStereoWidening* CStereoWidening::NewL(
	MCustomInterface& aCustomInterface , TBool aEnable, TInt8 aInitialLevel)
	{

    DEBPRN0;
	CStereoWideningProxy* stereoWideningProxy = (CStereoWideningProxy*)aCustomInterface.CustomInterface(KUidStereoWideningEffect);

	if ( !stereoWideningProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	stereoWideningProxy->SetStereoWideningLevelL(aInitialLevel);

	if(aEnable)
    	{
		stereoWideningProxy->EnableL();
	    }

    return stereoWideningProxy;
	}

// -----------------------------------------------------------------------------
// CStereoWidening::NewL
// Static function for creating an instance of the StereoWidening object.
// -----------------------------------------------------------------------------
//
EXPORT_C CStereoWidening* CStereoWidening::NewL(
	CMidiClientUtility& aUtility , TBool aEnable, TInt8 aInitialLevel )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CStereoWideningProxy* stereoWideningProxy = (CStereoWideningProxy*)customInterface->CustomInterface(KUidStereoWideningEffect);

	if ( !stereoWideningProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

	stereoWideningProxy->SetStereoWideningLevelL(aInitialLevel);

	if(aEnable)
	    {
		stereoWideningProxy->EnableL();
	    }

    return stereoWideningProxy;
	}

// -----------------------------------------------------------------------------
// CStereoWidening::NewL
// Static function for creating an instance of the StereoWidening object.
// -----------------------------------------------------------------------------
//
EXPORT_C CStereoWidening* CStereoWidening::NewL(
	CDrmPlayerUtility& aUtility , TBool aEnable, TInt8 aInitialLevel )
	{
    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CStereoWideningProxy* stereoWideningProxy = (CStereoWideningProxy*)customInterface->CustomInterface(KUidStereoWideningEffect);

	if ( !stereoWideningProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

	stereoWideningProxy->SetStereoWideningLevelL(aInitialLevel);

	if(aEnable)
	    {
		stereoWideningProxy->EnableL();
	    }

    return stereoWideningProxy;
	}

// -----------------------------------------------------------------------------
// CStereoWidening::NewL
// Static function for creating an instance of the StereoWidening object.
// -----------------------------------------------------------------------------
//
EXPORT_C CStereoWidening* CStereoWidening::NewL(
	CVideoPlayerUtility& aUtility , TBool aEnable, TInt8 aInitialLevel )
	{
    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CStereoWideningProxy* stereoWideningProxy = (CStereoWideningProxy*)customInterface->CustomInterface(KUidStereoWideningEffect);

	if ( !stereoWideningProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

	stereoWideningProxy->SetStereoWideningLevelL(aInitialLevel);

	if(aEnable)
	    {
		stereoWideningProxy->EnableL();
	    }

    return stereoWideningProxy;
	}

// -----------------------------------------------------------------------------
// CStereoWidening::BandLevel
// -----------------------------------------------------------------------------
//

EXPORT_C TBool CStereoWidening::IsContinuousLevelSupported() const
	{
		return iStereoWideningData.iContinuousLevelSupported;
	}

// -----------------------------------------------------------------------------
// CStereoWidening::BandWidth
// -----------------------------------------------------------------------------
//

EXPORT_C void CStereoWidening::SetStereoWideningLevelL( TUint8 aLevel )
	{
	iStereoWideningData.iLevel = aLevel;
	}

// -----------------------------------------------------------------------------
// CStereoWidening::CenterFrequency
// -----------------------------------------------------------------------------
//
EXPORT_C TUint8 CStereoWidening::StereoWideningLevel() const
	{
		return iStereoWideningData.iLevel;
	}


// -----------------------------------------------------------------------------
// CStereoWidening::Uid
// -----------------------------------------------------------------------------
//
EXPORT_C TUid CStereoWidening::Uid() const
	{
	return KUidStereoWideningEffect;
	}


// -----------------------------------------------------------------------------
// CStereoWidening::DoEffectData
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CStereoWidening::DoEffectData()
	{
    DEBPRN0;
	iDataPckgTo = iStereoWideningData;
	return iDataPckgTo;
	}

// -----------------------------------------------------------------------------
// CStereoWidening::SetEffectData
// -----------------------------------------------------------------------------
//
EXPORT_C void CStereoWidening::SetEffectData(
	const TDesC8& aEffectDataBuffer )
	{
    DEBPRN0;
	TEfStereoWideningDataPckg dataPckg;
	dataPckg.Copy(aEffectDataBuffer);
	iStereoWideningData = dataPckg();
	iEnabled = iStereoWideningData.iEnabled;
	iEnforced = iStereoWideningData.iEnforced;
	iHaveUpdateRights = iStereoWideningData.iHaveUpdateRights;
	}


// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File

