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
* Description:   Implementation of the RoomLevel effect class
*
*/




// INCLUDE FILES

#ifdef _DEBUG
#include <e32svr.h>
#endif

#include "RoomLevelProxy.h"

#include <RoomLevelBase.h>
#include <CustomInterfaceUtility.h>
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
// CRoomLevel::CRoomLevel
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CRoomLevel::CRoomLevel()
    : 	iRoomLevelData(),
    	iDataPckgTo(iRoomLevelData)
    {

    }


// -----------------------------------------------------------------------------
// CRoomLevel::~CRoomLevel
// Destructor
// -----------------------------------------------------------------------------
//
//
EXPORT_C CRoomLevel::~CRoomLevel()
    {

    }


// -----------------------------------------------------------------------------
// CRoomLevel::NewL
// Static function for creating an instance of the RoomLevel object.
// -----------------------------------------------------------------------------
//
EXPORT_C CRoomLevel* CRoomLevel::NewL(
	CMdaAudioConvertUtility& aUtility, CEnvironmentalReverb& aReverb )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CRoomLevelProxy* roomLevelProxy = (CRoomLevelProxy*)customInterface->CustomInterface(KUidRoomLevelEffect);

	if ( !roomLevelProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    roomLevelProxy->iClientReverb = &aReverb;
    roomLevelProxy->AttachReverb(aReverb);

    return roomLevelProxy;

	}

// -----------------------------------------------------------------------------
// CRoomLevel::NewL
// Static function for creating an instance of the RoomLevel object.
// -----------------------------------------------------------------------------
//
EXPORT_C CRoomLevel* CRoomLevel::NewL(
	CMdaAudioInputStream& aUtility, CEnvironmentalReverb& aReverb )
	{

    DEBPRN0;
    CRoomLevelProxy* roomLevelProxy = (CRoomLevelProxy*)aUtility.CustomInterface(KUidRoomLevelEffect);

	if (roomLevelProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

	roomLevelProxy->iClientReverb = &aReverb;
    roomLevelProxy->AttachReverb(aReverb);

    return roomLevelProxy;
	}

// -----------------------------------------------------------------------------
// CRoomLevel::NewL
// Static function for creating an instance of the RoomLevel object.
// -----------------------------------------------------------------------------
//
EXPORT_C CRoomLevel* CRoomLevel::NewL(
	CMdaAudioOutputStream& aUtility, CEnvironmentalReverb& aReverb )
	{

    DEBPRN0;
    CRoomLevelProxy* roomLevelProxy = (CRoomLevelProxy*)aUtility.CustomInterface(KUidRoomLevelEffect);

	if (roomLevelProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

	roomLevelProxy->iClientReverb = &aReverb;
    roomLevelProxy->AttachReverb(aReverb);

    return roomLevelProxy;
	}

// -----------------------------------------------------------------------------
// CRoomLevel::NewL
// Static function for creating an instance of the RoomLevel object.
// -----------------------------------------------------------------------------
//
EXPORT_C CRoomLevel* CRoomLevel::NewL(
	CMdaAudioPlayerUtility& aUtility, CEnvironmentalReverb& aReverb )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CRoomLevelProxy* roomLevelProxy = (CRoomLevelProxy*)customInterface->CustomInterface(KUidRoomLevelEffect);

	if ( !roomLevelProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);


    roomLevelProxy->iClientReverb = &aReverb;
    roomLevelProxy->AttachReverb(aReverb);

    return roomLevelProxy;
	}

// -----------------------------------------------------------------------------
// CRoomLevel::NewL
// Static function for creating an instance of the RoomLevel object.
// -----------------------------------------------------------------------------
//
EXPORT_C CRoomLevel* CRoomLevel::NewL(
	CMdaAudioRecorderUtility& aUtility,
	TBool aRecordStream, CEnvironmentalReverb& aReverb )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility, aRecordStream);
    CleanupStack::PushL(customInterface);

	CRoomLevelProxy* roomLevelProxy = (CRoomLevelProxy*)customInterface->CustomInterface(KUidRoomLevelEffect);

	if ( !roomLevelProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    roomLevelProxy->iClientReverb = &aReverb;
    roomLevelProxy->AttachReverb(aReverb);

    return roomLevelProxy;
	}

// -----------------------------------------------------------------------------
// CRoomLevel::NewL
// Static function for creating an instance of the RoomLevel object.
// -----------------------------------------------------------------------------
//
EXPORT_C CRoomLevel* CRoomLevel::NewL(
	CMdaAudioToneUtility& aUtility, CEnvironmentalReverb& aReverb )
	{

	DEBPRN0;
    CRoomLevelProxy* roomLevelProxy = (CRoomLevelProxy*)aUtility.CustomInterface(KUidRoomLevelEffect);

	if (roomLevelProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

	roomLevelProxy->iClientReverb = &aReverb;
    roomLevelProxy->AttachReverb(aReverb);

    return roomLevelProxy;
	}

// -----------------------------------------------------------------------------
// CAudioEqualizer::NewL
// Static function for creating an instance of the RoomLevel object.
// -----------------------------------------------------------------------------
//
EXPORT_C CRoomLevel* CRoomLevel::NewL(
	CMMFDevSound& aDevSound, CEnvironmentalReverb& aReverb )
	{

	DEBPRN0;
    CRoomLevelProxy* roomLevelProxy = (CRoomLevelProxy*)aDevSound.CustomInterface(KUidRoomLevelEffect);

	if (roomLevelProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

	roomLevelProxy->iClientReverb = &aReverb;
    roomLevelProxy->AttachReverb(aReverb);

    return roomLevelProxy;
	}

// -----------------------------------------------------------------------------
// CRoomLevel::NewL
// Static function for creating an instance of the RoomLevel object.
// -----------------------------------------------------------------------------
//
EXPORT_C CRoomLevel* CRoomLevel::NewL(
	CCustomCommandUtility* aUtility, CEnvironmentalReverb& aReverb )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CRoomLevelProxy* roomLevelProxy = (CRoomLevelProxy*)customInterface->CustomInterface(KUidRoomLevelEffect);

	if ( !roomLevelProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

 	CleanupStack::Pop(customInterface);

    roomLevelProxy->iClientReverb = &aReverb;
    roomLevelProxy->AttachReverb(aReverb);

    return roomLevelProxy;
	}

// -----------------------------------------------------------------------------
// CRoomLevel::NewL
// Static function for creating an instance of the RoomLevel object.
// -----------------------------------------------------------------------------
//
EXPORT_C CRoomLevel* CRoomLevel::NewL(
	MCustomInterface& aCustomInterface, CEnvironmentalReverb& aReverb )
	{

    DEBPRN0;
	CRoomLevelProxy* roomLevelProxy = (CRoomLevelProxy*)aCustomInterface.CustomInterface(KUidRoomLevelEffect);
	if ( !roomLevelProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

    roomLevelProxy->iClientReverb = &aReverb;

    roomLevelProxy->AttachReverb(aReverb);

    return roomLevelProxy;
	}

// -----------------------------------------------------------------------------
// CRoomLevel::NewL
// Static function for creating an instance of the RoomLevel object.
// -----------------------------------------------------------------------------
//
EXPORT_C CRoomLevel* CRoomLevel::NewL(
    CMidiClientUtility& aUtility, CEnvironmentalReverb& aReverb)
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CRoomLevelProxy* roomLevelProxy = (CRoomLevelProxy*)customInterface->CustomInterface(KUidRoomLevelEffect);

	if ( !roomLevelProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    roomLevelProxy->iClientReverb = &aReverb;

    roomLevelProxy->AttachReverb(aReverb);

    return roomLevelProxy;
	}

// -----------------------------------------------------------------------------
// CRoomLevel::NewL
// Static function for creating an instance of the RoomLevel object.
// -----------------------------------------------------------------------------
//
EXPORT_C CRoomLevel* CRoomLevel::NewL(
    CDrmPlayerUtility& aUtility, CEnvironmentalReverb& aReverb)
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CRoomLevelProxy* roomLevelProxy = (CRoomLevelProxy*)customInterface->CustomInterface(KUidRoomLevelEffect);

	if ( !roomLevelProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    roomLevelProxy->iClientReverb = &aReverb;

    roomLevelProxy->AttachReverb(aReverb);

    return roomLevelProxy;
	}

// -----------------------------------------------------------------------------
// CRoomLevel::NewL
// Static function for creating an instance of the RoomLevel object.
// -----------------------------------------------------------------------------
//
EXPORT_C CRoomLevel* CRoomLevel::NewL(
    CVideoPlayerUtility& aUtility, CEnvironmentalReverb& aReverb)
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CRoomLevelProxy* roomLevelProxy = (CRoomLevelProxy*)customInterface->CustomInterface(KUidRoomLevelEffect);

	if ( !roomLevelProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    roomLevelProxy->iClientReverb = &aReverb;

    roomLevelProxy->AttachReverb(aReverb);

    return roomLevelProxy;
	}

// -----------------------------------------------------------------------------
// CRoomLevel::RoomLevel
// -----------------------------------------------------------------------------
//
EXPORT_C TInt32 CRoomLevel::Level() const
	{
	return iRoomLevelData.iStreamRoomLevel;
	}

// -----------------------------------------------------------------------------
// CRoomLevel::RoomLevelLevelRange
// -----------------------------------------------------------------------------
//
EXPORT_C void CRoomLevel::LevelRange(
	TInt32& aMin,
	TInt32& aMax)
	{

	aMin = iRoomLevelData.iStreamMinRoomLevel;
	aMax = iRoomLevelData.iStreamMaxRoomLevel;
	}


// -----------------------------------------------------------------------------
// CRoomLevel::SetRoomLevelL
// -----------------------------------------------------------------------------
//
EXPORT_C void CRoomLevel::SetRoomLevelL(
	TInt32 aRoomLevel )
	{
	if ( (aRoomLevel >= iRoomLevelData.iStreamMinRoomLevel) && (aRoomLevel <= iRoomLevelData.iStreamMaxRoomLevel) )
		{
		iRoomLevelData.iStreamRoomLevel = aRoomLevel;
		}
	else
		{
		User::Leave(KErrArgument);
		}
	}


// -----------------------------------------------------------------------------
// CRoomLevel::Uid
// -----------------------------------------------------------------------------
//
EXPORT_C TUid CRoomLevel::Uid() const
	{
	return KUidRoomLevelEffect;
	}


// -----------------------------------------------------------------------------
// CRoomLevel::DoEffectData
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CRoomLevel::DoEffectData()
	{
    DEBPRN0;
	iDataPckgTo = iRoomLevelData;
	return iDataPckgTo;
	}

// -----------------------------------------------------------------------------
// CRoomLevel::SetEffectData
// -----------------------------------------------------------------------------
//
EXPORT_C void CRoomLevel::SetEffectData(
	const TDesC8& aEffectDataBuffer )
	{
    DEBPRN0;
	TEfRoomLevelDataPckg dataPckg;
	dataPckg.Copy(aEffectDataBuffer);
	iRoomLevelData = dataPckg();
	iEnabled = iRoomLevelData.iEnabled;
	iEnforced = iRoomLevelData.iEnforced;
	iHaveUpdateRights = iRoomLevelData.iHaveUpdateRights;
	}


// ========================== OTHER EXPORTED FUNCTIONS =========================


