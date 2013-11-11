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
* Description:   Implementation of the Listener Doppler Effect class
*
*/




// INCLUDE FILES

#ifdef _DEBUG
#include <e32svr.h>
#endif

#include <ListenerDopplerBase.h>
#include <CustomInterfaceUtility.h>
#include "ListenerDopplerProxy.h"
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
// CListenerDoppler::CListenerDoppler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerDoppler::CListenerDoppler()
    {
    }

// Destructor
EXPORT_C CListenerDoppler::~CListenerDoppler()
    {
    }


// -----------------------------------------------------------------------------
// CListenerDoppler::NewL
// Static function for creating an instance of the Doppler object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerDoppler* CListenerDoppler::NewL(
	CMdaAudioInputStream& aUtility )
	{

	DEBPRN0;
    CListenerDopplerProxy* dopplerProxy = (CListenerDopplerProxy*)aUtility.CustomInterface(KUidListenerDopplerEffect);

	if (dopplerProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

    return dopplerProxy;
	}

// -----------------------------------------------------------------------------
// CListenerDoppler::NewL
// Static function for creating an instance of the Doppler object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerDoppler* CListenerDoppler::NewL(
	CMdaAudioOutputStream& aUtility)
	{

    DEBPRN0;
    CListenerDopplerProxy* dopplerProxy = (CListenerDopplerProxy*)aUtility.CustomInterface(KUidListenerDopplerEffect);

	if (dopplerProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

    return dopplerProxy;
	}

// -----------------------------------------------------------------------------
// CListenerDoppler::NewL
// Static function for creating an instance of the Doppler object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerDoppler* CListenerDoppler::NewL(
	CMdaAudioConvertUtility& aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CListenerDopplerProxy* dopplerProxy = (CListenerDopplerProxy*)customInterface->CustomInterface(KUidListenerDopplerEffect);

	if ( !dopplerProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return dopplerProxy;
	}

// -----------------------------------------------------------------------------
// CListenerDoppler::NewL
// Static function for creating an instance of the Doppler object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerDoppler* CListenerDoppler::NewL(
	CMdaAudioPlayerUtility& aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CListenerDopplerProxy* dopplerProxy = (CListenerDopplerProxy*)customInterface->CustomInterface(KUidListenerDopplerEffect);

	if ( !dopplerProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return dopplerProxy;
	}

// -----------------------------------------------------------------------------
// CListenerDoppler::NewL
// Static function for creating an instance of the Doppler object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerDoppler* CListenerDoppler::NewL(
	CMdaAudioRecorderUtility& aUtility,
	TBool aRecordStream )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility, aRecordStream);
    CleanupStack::PushL(customInterface);

	CListenerDopplerProxy* dopplerProxy = (CListenerDopplerProxy*)customInterface->CustomInterface(KUidListenerDopplerEffect);

	if ( !dopplerProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return dopplerProxy;
	}

// -----------------------------------------------------------------------------
// CListenerDoppler::NewL
// Static function for creating an instance of the Doppler object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerDoppler* CListenerDoppler::NewL(
	CMdaAudioToneUtility& aUtility )
	{

	DEBPRN0;
    CListenerDopplerProxy* dopplerProxy = (CListenerDopplerProxy*)aUtility.CustomInterface(KUidListenerDopplerEffect);

	if (dopplerProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

    return dopplerProxy;
	}

// -----------------------------------------------------------------------------
// CListenerDoppler::NewL
// Static function for creating an instance of the Doppler object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerDoppler* CListenerDoppler::NewL(
	CMMFDevSound& aDevSound )
	{

	DEBPRN0;
    CListenerDopplerProxy* dopplerProxy = (CListenerDopplerProxy*)aDevSound.CustomInterface(KUidListenerDopplerEffect);

	if (dopplerProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

    return dopplerProxy;
	}

// -----------------------------------------------------------------------------
// CListenerDoppler::NewL
// Static function for creating an instance of the Doppler object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerDoppler* CListenerDoppler::NewL(
	CCustomCommandUtility* aUtility)
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CListenerDopplerProxy* dopplerProxy = (CListenerDopplerProxy*)customInterface->CustomInterface(KUidListenerDopplerEffect);

	if ( !dopplerProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return dopplerProxy;
	}

// -----------------------------------------------------------------------------
// CListenerDoppler::NewL
// Static function for creating an instance of the Doppler object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerDoppler* CListenerDoppler::NewL(
	MCustomInterface& aCustomInterface )
	{

    DEBPRN0;
	CListenerDopplerProxy* dopplerProxy = (CListenerDopplerProxy*)aCustomInterface.CustomInterface(KUidListenerDopplerEffect);

	if ( !dopplerProxy )
		{
	    DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

    return dopplerProxy;
	}

// -----------------------------------------------------------------------------
// CListenerDoppler::NewL
// Static function for creating an instance of the Doppler object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerDoppler* CListenerDoppler::NewL(
CMidiClientUtility& aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CListenerDopplerProxy* dopplerProxy = (CListenerDopplerProxy*)customInterface->CustomInterface(KUidListenerDopplerEffect);

	if ( !dopplerProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return dopplerProxy;
	}

// -----------------------------------------------------------------------------
// CListenerDoppler::NewL
// Static function for creating an instance of the Doppler object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerDoppler* CListenerDoppler::NewL(
CDrmPlayerUtility& aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CListenerDopplerProxy* dopplerProxy = (CListenerDopplerProxy*)customInterface->CustomInterface(KUidListenerDopplerEffect);

	if ( !dopplerProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return dopplerProxy;
	}

// -----------------------------------------------------------------------------
// CListenerDoppler::NewL
// Static function for creating an instance of the Doppler object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerDoppler* CListenerDoppler::NewL(
CVideoPlayerUtility& aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CListenerDopplerProxy* dopplerProxy = (CListenerDopplerProxy*)customInterface->CustomInterface(KUidListenerDopplerEffect);

	if ( !dopplerProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return dopplerProxy;
	}

// -----------------------------------------------------------------------------
// CListenerDoppler::Uid
// -----------------------------------------------------------------------------
//
EXPORT_C TUid CListenerDoppler::Uid() const
	{
	return KUidListenerDopplerEffect;
	}


// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File

