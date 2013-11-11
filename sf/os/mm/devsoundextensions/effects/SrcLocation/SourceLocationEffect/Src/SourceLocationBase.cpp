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

#include <SourceLocationBase.h>
#include <CustomInterfaceUtility.h>
#include "SourceLocationProxy.h"
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
// CSourceLocation::CSourceLocation
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceLocation::CSourceLocation()
    {
    }

// Destructor
EXPORT_C CSourceLocation::~CSourceLocation()
    {
    }


// -----------------------------------------------------------------------------
// CSourceLocation::NewL
// Static function for creating an instance of the SourceLocation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceLocation* CSourceLocation::NewL(
	CMdaAudioInputStream& aUtility )
	{

	DEBPRN0;
    CSourceLocationProxy* sourceLocationProxy = (CSourceLocationProxy*)aUtility.CustomInterface(KUidSourceLocationEffect);

	if (sourceLocationProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

    return sourceLocationProxy;
	}

// -----------------------------------------------------------------------------
// CSourceLocation::NewL
// Static function for creating an instance of the SourceLocation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceLocation* CSourceLocation::NewL(
	CMdaAudioOutputStream& aUtility )
	{

	DEBPRN0;
    CSourceLocationProxy* sourceLocationProxy = (CSourceLocationProxy*)aUtility.CustomInterface(KUidSourceLocationEffect);

	if (sourceLocationProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

    return sourceLocationProxy;
	}

// -----------------------------------------------------------------------------
// CSourceLocation::NewL
// Static function for creating an instance of the SourceLocation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceLocation* CSourceLocation::NewL(
	CMdaAudioConvertUtility& aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CSourceLocationProxy* sourceLocationProxy = (CSourceLocationProxy*)customInterface->CustomInterface(KUidSourceLocationEffect);

	if ( !sourceLocationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return sourceLocationProxy;
	}

// -----------------------------------------------------------------------------
// CSourceLocation::NewL
// Static function for creating an instance of the SourceLocation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceLocation* CSourceLocation::NewL(
	CMdaAudioPlayerUtility& aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CSourceLocationProxy* sourceLocationProxy = (CSourceLocationProxy*)customInterface->CustomInterface(KUidSourceLocationEffect);

	if ( !sourceLocationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return sourceLocationProxy;
	}

// -----------------------------------------------------------------------------
// CSourceLocation::NewL
// Static function for creating an instance of the SourceLocation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceLocation* CSourceLocation::NewL(
	CMdaAudioRecorderUtility& aUtility,
	TBool aRecordStream )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility, aRecordStream);
    CleanupStack::PushL(customInterface);

	CSourceLocationProxy* sourceLocationProxy = (CSourceLocationProxy*)customInterface->CustomInterface(KUidSourceLocationEffect);

	if ( !sourceLocationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return sourceLocationProxy;
	}

// -----------------------------------------------------------------------------
// CSourceLocation::NewL
// Static function for creating an instance of the SourceLocation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceLocation* CSourceLocation::NewL(
	CMdaAudioToneUtility& aUtility)
	{

	DEBPRN0;
    CSourceLocationProxy* sourceLocationProxy = (CSourceLocationProxy*)aUtility.CustomInterface(KUidSourceLocationEffect);

	if (sourceLocationProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

    return sourceLocationProxy;
	}

// -----------------------------------------------------------------------------
// CSourceLocation::NewL
// Static function for creating an instance of the SourceLocation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceLocation* CSourceLocation::NewL(
	CMMFDevSound& aDevSound )
	{

	DEBPRN0;
    CSourceLocationProxy* sourceLocationProxy = (CSourceLocationProxy*)aDevSound.CustomInterface(KUidSourceLocationEffect);

	if (sourceLocationProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

    return sourceLocationProxy;
	}

// -----------------------------------------------------------------------------
// CSourceLocation::NewL
// Static function for creating an instance of the SourceLocation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceLocation* CSourceLocation::NewL(
	CCustomCommandUtility* aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CSourceLocationProxy* sourceLocationProxy = (CSourceLocationProxy*)customInterface->CustomInterface(KUidSourceLocationEffect);

	if ( !sourceLocationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return sourceLocationProxy;
	}

// -----------------------------------------------------------------------------
// CSourceLocation::NewL
// Static function for creating an instance of the SourceLocation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceLocation* CSourceLocation::NewL(
	MCustomInterface& aCustomInterface )
	{

    DEBPRN0;
	CSourceLocationProxy* sourceLocationProxy = (CSourceLocationProxy*)aCustomInterface.CustomInterface(KUidSourceLocationEffect);

	if ( !sourceLocationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

    return sourceLocationProxy;
	}

// -----------------------------------------------------------------------------
// CSourceLocation::NewL
// Static function for creating an instance of the SourceLocation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceLocation* CSourceLocation::NewL(
    CMidiClientUtility& aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CSourceLocationProxy* sourceLocationProxy = (CSourceLocationProxy*)customInterface->CustomInterface(KUidSourceLocationEffect);

	if ( !sourceLocationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return sourceLocationProxy;
	}

// -----------------------------------------------------------------------------
// CSourceLocation::NewL
// Static function for creating an instance of the SourceLocation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceLocation* CSourceLocation::NewL(
    CDrmPlayerUtility& aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CSourceLocationProxy* sourceLocationProxy = (CSourceLocationProxy*)customInterface->CustomInterface(KUidSourceLocationEffect);

	if ( !sourceLocationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return sourceLocationProxy;
	}

// -----------------------------------------------------------------------------
// CSourceLocation::NewL
// Static function for creating an instance of the SourceLocation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CSourceLocation* CSourceLocation::NewL(
    CVideoPlayerUtility& aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CSourceLocationProxy* sourceLocationProxy = (CSourceLocationProxy*)customInterface->CustomInterface(KUidSourceLocationEffect);

	if ( !sourceLocationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return sourceLocationProxy;
	}

// -----------------------------------------------------------------------------
// CSourceLocation::Uid
// -----------------------------------------------------------------------------
//
EXPORT_C TUid CSourceLocation::Uid() const
	{
	return KUidSourceLocationEffect;
	}


// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File

