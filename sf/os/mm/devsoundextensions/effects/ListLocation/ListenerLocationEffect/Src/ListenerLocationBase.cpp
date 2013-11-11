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
* Description:   Implementation of the listenerlocation effect class
*
*/




// INCLUDE FILES

#ifdef _DEBUG
#include <e32svr.h>
#endif

#include <ListenerLocationBase.h>
#include <CustomInterfaceUtility.h>
#include "ListenerLocationProxy.h"
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
// CListenerLocation::CListenerLocation
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerLocation::CListenerLocation()
    {
    }

// Destructor
EXPORT_C CListenerLocation::~CListenerLocation()
    {
    }


// -----------------------------------------------------------------------------
// CListenerLocation::NewL
// Static function for creating an instance of the ListenerLocation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerLocation* CListenerLocation::NewL(
	CMdaAudioInputStream& aUtility )
	{

    DEBPRN0;
    CListenerLocationProxy* listenerlocationProxy = (CListenerLocationProxy*)aUtility.CustomInterface(KUidListenerLocationEffect);

	if (listenerlocationProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

    return listenerlocationProxy;
	}

// -----------------------------------------------------------------------------
// CListenerLocation::NewL
// Static function for creating an instance of the ListenerLocation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerLocation* CListenerLocation::NewL(
	CMdaAudioOutputStream& aUtility )
	{

    DEBPRN0;
    CListenerLocationProxy* listenerlocationProxy = (CListenerLocationProxy*)aUtility.CustomInterface(KUidListenerLocationEffect);

	if (listenerlocationProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

    return listenerlocationProxy;
	}

// -----------------------------------------------------------------------------
// CListenerLocation::NewL
// Static function for creating an instance of the ListenerLocation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerLocation* CListenerLocation::NewL(
	CMdaAudioConvertUtility& aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CListenerLocationProxy* listenerlocationProxy = (CListenerLocationProxy*)customInterface->CustomInterface(KUidListenerLocationEffect);

	if ( !listenerlocationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return listenerlocationProxy;
	}

// -----------------------------------------------------------------------------
// CListenerLocation::NewL
// Static function for creating an instance of the ListenerLocation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerLocation* CListenerLocation::NewL(
	CMdaAudioPlayerUtility& aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CListenerLocationProxy* listenerlocationProxy = (CListenerLocationProxy*)customInterface->CustomInterface(KUidListenerLocationEffect);

	if ( !listenerlocationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return listenerlocationProxy;
	}

// -----------------------------------------------------------------------------
// CListenerLocation::NewL
// Static function for creating an instance of the ListenerLocation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerLocation* CListenerLocation::NewL(
	CMdaAudioRecorderUtility& aUtility,
	TBool aRecordStream )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility, aRecordStream);
    CleanupStack::PushL(customInterface);

	CListenerLocationProxy* listenerlocationProxy = (CListenerLocationProxy*)customInterface->CustomInterface(KUidListenerLocationEffect);

	if ( !listenerlocationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return listenerlocationProxy;
	}

// -----------------------------------------------------------------------------
// CListenerLocation::NewL
// Static function for creating an instance of the ListenerLocation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerLocation* CListenerLocation::NewL(
	CMdaAudioToneUtility& aUtility )
	{

	DEBPRN0;
    CListenerLocationProxy* listenerlocationProxy = (CListenerLocationProxy*)aUtility.CustomInterface(KUidListenerLocationEffect);

	if (listenerlocationProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

    return listenerlocationProxy;
	}

// -----------------------------------------------------------------------------
// CListenerLocation::NewL
// Static function for creating an instance of the ListenerLocation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerLocation* CListenerLocation::NewL(
	CMMFDevSound& aDevSound )
	{

	DEBPRN0;
    CListenerLocationProxy* listenerlocationProxy = (CListenerLocationProxy*)aDevSound.CustomInterface(KUidListenerLocationEffect);

	if (listenerlocationProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

    return listenerlocationProxy;
	}

// -----------------------------------------------------------------------------
// CListenerLocation::NewL
// Static function for creating an instance of the ListenerLocation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerLocation* CListenerLocation::NewL(
	CCustomCommandUtility* aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CListenerLocationProxy* listenerlocationProxy = (CListenerLocationProxy*)customInterface->CustomInterface(KUidListenerLocationEffect);

	if ( !listenerlocationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return listenerlocationProxy;
	}

// -----------------------------------------------------------------------------
// CListenerLocation::NewL
// Static function for creating an instance of the ListenerLocation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerLocation* CListenerLocation::NewL(
	MCustomInterface& aCustomInterface )
	{

    DEBPRN0;
	CListenerLocationProxy* listenerlocationProxy = (CListenerLocationProxy*)aCustomInterface.CustomInterface(KUidListenerLocationEffect);

	if ( !listenerlocationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

    return listenerlocationProxy;
	}

// -----------------------------------------------------------------------------
// CListenerLocation::NewL
// Static function for creating an instance of the ListenerLocation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerLocation* CListenerLocation::NewL(
	CMidiClientUtility& aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CListenerLocationProxy* listenerlocationProxy = (CListenerLocationProxy*)customInterface->CustomInterface(KUidListenerLocationEffect);

	if ( !listenerlocationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return listenerlocationProxy;
	}

// -----------------------------------------------------------------------------
// CListenerLocation::NewL
// Static function for creating an instance of the ListenerLocation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerLocation* CListenerLocation::NewL(
	CDrmPlayerUtility& aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CListenerLocationProxy* listenerlocationProxy = (CListenerLocationProxy*)customInterface->CustomInterface(KUidListenerLocationEffect);

	if ( !listenerlocationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return listenerlocationProxy;
	}

// -----------------------------------------------------------------------------
// CListenerLocation::NewL
// Static function for creating an instance of the ListenerLocation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerLocation* CListenerLocation::NewL(
	CVideoPlayerUtility& aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CListenerLocationProxy* listenerlocationProxy = (CListenerLocationProxy*)customInterface->CustomInterface(KUidListenerLocationEffect);

	if ( !listenerlocationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return listenerlocationProxy;
	}

// -----------------------------------------------------------------------------
// CListenerLocation::Uid
// -----------------------------------------------------------------------------
//
EXPORT_C TUid CListenerLocation::Uid() const
	{
	return KUidListenerLocationEffect;
	}


// ========================== OTHER EXPORTED FUNCTIONS =========================

