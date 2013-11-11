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

#include <SourceOrientationBase.h>
#include <CustomInterfaceUtility.h>
#include "SourceOrientationProxy.h"
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
// CSourceOrientation::CSourceOrientation
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceOrientation::CSourceOrientation()
    {
    }

// Destructor
EXPORT_C CSourceOrientation::~CSourceOrientation()
    {
    }


// -----------------------------------------------------------------------------
// CSourceOrientation::NewL
// Static function for creating an instance of the SourceOrientation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceOrientation* CSourceOrientation::NewL(
	CMdaAudioInputStream& aUtility )
	{

    DEBPRN0;
    CSourceOrientationProxy* sourceOrientationProxy = (CSourceOrientationProxy*)aUtility.CustomInterface(KUidSourceOrientationEffect);

	if (sourceOrientationProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

    return sourceOrientationProxy;
	}

// -----------------------------------------------------------------------------
// CSourceOrientation::NewL
// Static function for creating an instance of the SourceOrientation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceOrientation* CSourceOrientation::NewL(
	CMdaAudioOutputStream& aUtility )
	{

    DEBPRN0;
    CSourceOrientationProxy* sourceOrientationProxy = (CSourceOrientationProxy*)aUtility.CustomInterface(KUidSourceOrientationEffect);

	if (sourceOrientationProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

    return sourceOrientationProxy;
	}

// -----------------------------------------------------------------------------
// CSourceOrientation::NewL
// Static function for creating an instance of the SourceOrientation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceOrientation* CSourceOrientation::NewL(
	CMdaAudioConvertUtility& aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CSourceOrientationProxy* sourceOrientationProxy = (CSourceOrientationProxy*)customInterface->CustomInterface(KUidSourceOrientationEffect);

	if ( !sourceOrientationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return sourceOrientationProxy;
	}

// -----------------------------------------------------------------------------
// CSourceOrientation::NewL
// Static function for creating an instance of the SourceOrientation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceOrientation* CSourceOrientation::NewL(
	CMdaAudioPlayerUtility& aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CSourceOrientationProxy* sourceOrientationProxy = (CSourceOrientationProxy*)customInterface->CustomInterface(KUidSourceOrientationEffect);

	if ( !sourceOrientationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return sourceOrientationProxy;
	}

// -----------------------------------------------------------------------------
// CSourceOrientation::NewL
// Static function for creating an instance of the SourceOrientation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceOrientation* CSourceOrientation::NewL(
	CMdaAudioRecorderUtility& aUtility,
	TBool aRecordStream )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility, aRecordStream);
    CleanupStack::PushL(customInterface);

	CSourceOrientationProxy* sourceOrientationProxy = (CSourceOrientationProxy*)customInterface->CustomInterface(KUidSourceOrientationEffect);

	if ( !sourceOrientationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return sourceOrientationProxy;
	}

// -----------------------------------------------------------------------------
// CSourceOrientation::NewL
// Static function for creating an instance of the SourceOrientation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceOrientation* CSourceOrientation::NewL(
	CMdaAudioToneUtility& aUtility )
	{

	DEBPRN0;
    CSourceOrientationProxy* sourceOrientationProxy = (CSourceOrientationProxy*)aUtility.CustomInterface(KUidSourceOrientationEffect);

	if (sourceOrientationProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

    return sourceOrientationProxy;
	}

// -----------------------------------------------------------------------------
// CSourceOrientation::NewL
// Static function for creating an instance of the SourceOrientation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceOrientation* CSourceOrientation::NewL(
	CMMFDevSound& aDevSound )
	{

	DEBPRN0;
    CSourceOrientationProxy* sourceOrientationProxy = (CSourceOrientationProxy*)aDevSound.CustomInterface(KUidSourceOrientationEffect);

	if (sourceOrientationProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

    return sourceOrientationProxy;
	}

// -----------------------------------------------------------------------------
// CSourceOrientation::NewL
// Static function for creating an instance of the SourceOrientation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceOrientation* CSourceOrientation::NewL(
	CCustomCommandUtility* aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CSourceOrientationProxy* sourceOrientationProxy = (CSourceOrientationProxy*)customInterface->CustomInterface(KUidSourceOrientationEffect);

	if ( !sourceOrientationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return sourceOrientationProxy;
	}

// -----------------------------------------------------------------------------
// CSourceOrientation::NewL
// Static function for creating an instance of the SourceOrientation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceOrientation* CSourceOrientation::NewL(
	MCustomInterface& aCustomInterface )
	{

    DEBPRN0;
	CSourceOrientationProxy* sourceOrientationProxy = (CSourceOrientationProxy*)aCustomInterface.CustomInterface(KUidSourceOrientationEffect);

	if ( !sourceOrientationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

    return sourceOrientationProxy;
	}

// -----------------------------------------------------------------------------
// CSourceOrientation::NewL
// Static function for creating an instance of the SourceOrientation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceOrientation* CSourceOrientation::NewL(
 	CMidiClientUtility& aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CSourceOrientationProxy* sourceOrientationProxy = (CSourceOrientationProxy*)customInterface->CustomInterface(KUidSourceOrientationEffect);

	if ( !sourceOrientationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return sourceOrientationProxy;
	}

// -----------------------------------------------------------------------------
// CSourceOrientation::NewL
// Static function for creating an instance of the SourceOrientation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceOrientation* CSourceOrientation::NewL(
 	CDrmPlayerUtility& aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CSourceOrientationProxy* sourceOrientationProxy = (CSourceOrientationProxy*)customInterface->CustomInterface(KUidSourceOrientationEffect);

	if ( !sourceOrientationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return sourceOrientationProxy;
	}

// -----------------------------------------------------------------------------
// CSourceOrientation::NewL
// Static function for creating an instance of the SourceOrientation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceOrientation* CSourceOrientation::NewL(
 	CVideoPlayerUtility& aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CSourceOrientationProxy* sourceOrientationProxy = (CSourceOrientationProxy*)customInterface->CustomInterface(KUidSourceOrientationEffect);

	if ( !sourceOrientationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return sourceOrientationProxy;
	}

// -----------------------------------------------------------------------------
// CSourceOrientation::Uid
// -----------------------------------------------------------------------------
//
EXPORT_C TUid CSourceOrientation::Uid() const
	{
	return KUidSourceOrientationEffect;
	}


// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File

