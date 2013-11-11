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
* Description:   Implementation of the Source Doppler Effect class
*
*/




// INCLUDE FILES

#ifdef _DEBUG
#include <e32svr.h>
#endif

#include <SourceDopplerBase.h>
#include <CustomInterfaceUtility.h>
#include "SourceDopplerProxy.h"
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
// CSourceDoppler::CSourceDoppler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceDoppler::CSourceDoppler()
    {
    }

// Destructor
EXPORT_C CSourceDoppler::~CSourceDoppler()
    {
    }


// -----------------------------------------------------------------------------
// CSourceDoppler::NewL
// Static function for creating an instance of the Doppler object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceDoppler* CSourceDoppler::NewL(
	CMdaAudioInputStream& aUtility )
	{

    DEBPRN0;
    CSourceDopplerProxy* dopplerProxy = (CSourceDopplerProxy*)aUtility.CustomInterface(KUidSourceDopplerEffect);

	if (dopplerProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

    return dopplerProxy;
	}

// -----------------------------------------------------------------------------
// CSourceDoppler::NewL
// Static function for creating an instance of the Doppler object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceDoppler* CSourceDoppler::NewL(
	CMdaAudioOutputStream& aUtility )
	{

	DEBPRN0;
    CSourceDopplerProxy* dopplerProxy = (CSourceDopplerProxy*)aUtility.CustomInterface(KUidSourceDopplerEffect);

	if (dopplerProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

    return dopplerProxy;
	}

// -----------------------------------------------------------------------------
// CSourceDoppler::NewL
// Static function for creating an instance of the Doppler object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceDoppler* CSourceDoppler::NewL(
	CMdaAudioConvertUtility& aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CSourceDopplerProxy* dopplerProxy = (CSourceDopplerProxy*)customInterface->CustomInterface(KUidSourceDopplerEffect);

	if ( !dopplerProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return dopplerProxy;
	}

// -----------------------------------------------------------------------------
// CSourceDoppler::NewL
// Static function for creating an instance of the Doppler object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceDoppler* CSourceDoppler::NewL(
	CMdaAudioPlayerUtility& aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CSourceDopplerProxy* dopplerProxy = (CSourceDopplerProxy*)customInterface->CustomInterface(KUidSourceDopplerEffect);

	if ( !dopplerProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return dopplerProxy;
	}

// -----------------------------------------------------------------------------
// CSourceDoppler::NewL
// Static function for creating an instance of the Doppler object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceDoppler* CSourceDoppler::NewL(
	CMdaAudioRecorderUtility& aUtility,
	TBool aRecordStream )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility, aRecordStream);
    CleanupStack::PushL(customInterface);

	CSourceDopplerProxy* dopplerProxy = (CSourceDopplerProxy*)customInterface->CustomInterface(KUidSourceDopplerEffect);

	if ( !dopplerProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return dopplerProxy;
	}

// -----------------------------------------------------------------------------
// CSourceDoppler::NewL
// Static function for creating an instance of the Doppler object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceDoppler* CSourceDoppler::NewL(
	CMdaAudioToneUtility& aUtility )
	{

	DEBPRN0;
    CSourceDopplerProxy* dopplerProxy = (CSourceDopplerProxy*)aUtility.CustomInterface(KUidSourceDopplerEffect);

	if (dopplerProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

    return dopplerProxy;
	}

// -----------------------------------------------------------------------------
// CSourceDoppler::NewL
// Static function for creating an instance of the Doppler object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceDoppler* CSourceDoppler::NewL(
	CMMFDevSound& aDevSound)
	{

	DEBPRN0;
    CSourceDopplerProxy* dopplerProxy = (CSourceDopplerProxy*)aDevSound.CustomInterface(KUidSourceDopplerEffect);

	if (dopplerProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

    return dopplerProxy;
	}

// -----------------------------------------------------------------------------
// CSourceDoppler::NewL
// Static function for creating an instance of the Doppler object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceDoppler* CSourceDoppler::NewL(
	CCustomCommandUtility* aUtility)
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CSourceDopplerProxy* dopplerProxy = (CSourceDopplerProxy*)customInterface->CustomInterface(KUidSourceDopplerEffect);

	if ( !dopplerProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return dopplerProxy;
	}

// -----------------------------------------------------------------------------
// CSourceDoppler::NewL
// Static function for creating an instance of the Doppler object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceDoppler* CSourceDoppler::NewL(
	MCustomInterface& aCustomInterface )
	{

    DEBPRN0;
	CSourceDopplerProxy* dopplerProxy = (CSourceDopplerProxy*)aCustomInterface.CustomInterface(KUidSourceDopplerEffect);

	if ( !dopplerProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

    return dopplerProxy;
	}

// -----------------------------------------------------------------------------
// CSourceDoppler::NewL
// Static function for creating an instance of the Doppler object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceDoppler* CSourceDoppler::NewL(
	CMidiClientUtility& aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CSourceDopplerProxy* dopplerProxy = (CSourceDopplerProxy*)customInterface->CustomInterface(KUidSourceDopplerEffect);

	if ( !dopplerProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return dopplerProxy;
	}

// -----------------------------------------------------------------------------
// CSourceDoppler::NewL
// Static function for creating an instance of the Doppler object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceDoppler* CSourceDoppler::NewL(
	CDrmPlayerUtility& aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CSourceDopplerProxy* dopplerProxy = (CSourceDopplerProxy*)customInterface->CustomInterface(KUidSourceDopplerEffect);

	if ( !dopplerProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return dopplerProxy;
	}

// -----------------------------------------------------------------------------
// CSourceDoppler::NewL
// Static function for creating an instance of the Doppler object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceDoppler* CSourceDoppler::NewL(
	CVideoPlayerUtility& aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CSourceDopplerProxy* dopplerProxy = (CSourceDopplerProxy*)customInterface->CustomInterface(KUidSourceDopplerEffect);

	if ( !dopplerProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return dopplerProxy;
	}

// -----------------------------------------------------------------------------
// CSourceDoppler::Uid
// -----------------------------------------------------------------------------
//
EXPORT_C TUid CSourceDoppler::Uid() const
	{
	return KUidSourceDopplerEffect;
	}


// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File
