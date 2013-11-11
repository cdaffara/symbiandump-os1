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

#include <ListenerOrientationBase.h>
#include <CustomInterfaceUtility.h>
#include "ListenerOrientationProxy.h"
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
// CListenerOrientation::CListenerOrientation
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerOrientation::CListenerOrientation()
    {
    }

// Destructor
EXPORT_C CListenerOrientation::~CListenerOrientation()
    {
    }


// -----------------------------------------------------------------------------
// CListenerOrientation::NewL
// Static function for creating an instance of the ListenerOrientation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerOrientation* CListenerOrientation::NewL(
	CMdaAudioInputStream& aUtility )
	{

	DEBPRN0;
    CListenerOrientationProxy* listenerOrientationProxy = (CListenerOrientationProxy*)aUtility.CustomInterface(KUidListenerOrientationEffect);

	if (listenerOrientationProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

    return listenerOrientationProxy;
	}

// -----------------------------------------------------------------------------
// CListenerOrientation::NewL
// Static function for creating an instance of the ListenerOrientation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerOrientation* CListenerOrientation::NewL(
	CMdaAudioOutputStream& aUtility )
	{

	DEBPRN0;
    CListenerOrientationProxy* listenerOrientationProxy = (CListenerOrientationProxy*)aUtility.CustomInterface(KUidListenerOrientationEffect);

	if (listenerOrientationProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

    return listenerOrientationProxy;
	}

// -----------------------------------------------------------------------------
// CListenerOrientation::NewL
// Static function for creating an instance of the ListenerOrientation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerOrientation* CListenerOrientation::NewL(
	CMdaAudioConvertUtility& aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CListenerOrientationProxy* listenerOrientationProxy = (CListenerOrientationProxy*)customInterface->CustomInterface(KUidListenerOrientationEffect);

	if ( !listenerOrientationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return listenerOrientationProxy;
	}

// -----------------------------------------------------------------------------
// CListenerOrientation::NewL
// Static function for creating an instance of the ListenerOrientation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerOrientation* CListenerOrientation::NewL(
	CMdaAudioPlayerUtility& aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CListenerOrientationProxy* listenerOrientationProxy = (CListenerOrientationProxy*)customInterface->CustomInterface(KUidListenerOrientationEffect);

	if ( !listenerOrientationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return listenerOrientationProxy;
	}

// -----------------------------------------------------------------------------
// CListenerOrientation::NewL
// Static function for creating an instance of the ListenerOrientation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerOrientation* CListenerOrientation::NewL(
	CMdaAudioRecorderUtility& aUtility,
	TBool aRecordStream )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility, aRecordStream);
    CleanupStack::PushL(customInterface);

	CListenerOrientationProxy* listenerOrientationProxy = (CListenerOrientationProxy*)customInterface->CustomInterface(KUidListenerOrientationEffect);

	if ( !listenerOrientationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return listenerOrientationProxy;
	}

// -----------------------------------------------------------------------------
// CListenerOrientation::NewL
// Static function for creating an instance of the ListenerOrientation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerOrientation* CListenerOrientation::NewL(
	CMdaAudioToneUtility& aUtility)
	{

	DEBPRN0;
    CListenerOrientationProxy* listenerOrientationProxy = (CListenerOrientationProxy*)aUtility.CustomInterface(KUidListenerOrientationEffect);

	if (listenerOrientationProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

    return listenerOrientationProxy;
	}

// -----------------------------------------------------------------------------
// CListenerOrientation::NewL
// Static function for creating an instance of the ListenerOrientation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerOrientation* CListenerOrientation::NewL(
	CMMFDevSound& aDevSound )
	{

	DEBPRN0;
    CListenerOrientationProxy* listenerOrientationProxy = (CListenerOrientationProxy*)aDevSound.CustomInterface(KUidListenerOrientationEffect);

	if (listenerOrientationProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

    return listenerOrientationProxy;
	}

// -----------------------------------------------------------------------------
// CListenerOrientation::NewL
// Static function for creating an instance of the ListenerOrientation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerOrientation* CListenerOrientation::NewL(
	CCustomCommandUtility* aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CListenerOrientationProxy* listenerOrientationProxy = (CListenerOrientationProxy*)customInterface->CustomInterface(KUidListenerOrientationEffect);

	if ( !listenerOrientationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return listenerOrientationProxy;
	}

// -----------------------------------------------------------------------------
// CListenerOrientation::NewL
// Static function for creating an instance of the ListenerOrientation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerOrientation* CListenerOrientation::NewL(
	MCustomInterface& aCustomInterface )
	{

    DEBPRN0;
	CListenerOrientationProxy* listenerOrientationProxy = (CListenerOrientationProxy*)aCustomInterface.CustomInterface(KUidListenerOrientationEffect);

	if ( !listenerOrientationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

    return listenerOrientationProxy;
	}

// -----------------------------------------------------------------------------
// CListenerOrientation::NewL
// Static function for creating an instance of the ListenerOrientation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerOrientation* CListenerOrientation::NewL(
	CMidiClientUtility& aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CListenerOrientationProxy* listenerOrientationProxy = (CListenerOrientationProxy*)customInterface->CustomInterface(KUidListenerOrientationEffect);

	if ( !listenerOrientationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return listenerOrientationProxy;
	}

// -----------------------------------------------------------------------------
// CListenerOrientation::NewL
// Static function for creating an instance of the ListenerOrientation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerOrientation* CListenerOrientation::NewL(
	CDrmPlayerUtility& aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CListenerOrientationProxy* listenerOrientationProxy = (CListenerOrientationProxy*)customInterface->CustomInterface(KUidListenerOrientationEffect);

	if ( !listenerOrientationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return listenerOrientationProxy;
	}

// -----------------------------------------------------------------------------
// CListenerOrientation::NewL
// Static function for creating an instance of the ListenerOrientation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CListenerOrientation* CListenerOrientation::NewL(
	CVideoPlayerUtility& aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CListenerOrientationProxy* listenerOrientationProxy = (CListenerOrientationProxy*)customInterface->CustomInterface(KUidListenerOrientationEffect);

	if ( !listenerOrientationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return listenerOrientationProxy;
	}

// -----------------------------------------------------------------------------
// CListenerOrientation::Uid
// -----------------------------------------------------------------------------
//
EXPORT_C TUid CListenerOrientation::Uid() const
	{
	return KUidListenerOrientationEffect;
	}


// ========================== OTHER EXPORTED FUNCTIONS =========================


