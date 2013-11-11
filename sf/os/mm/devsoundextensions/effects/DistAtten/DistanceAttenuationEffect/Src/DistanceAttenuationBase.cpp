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
* Description:   Implementation of the DistanceAttenuation effect class
*
*/




// INCLUDE FILES

#ifdef _DEBUG
#include <e32svr.h>
#endif

#include <DistanceAttenuationBase.h>
#include <CustomInterfaceUtility.h>
#include "DistanceAttenuationProxy.h"
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
// CDistanceAttenuation::CDistanceAttenuation
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CDistanceAttenuation::CDistanceAttenuation()
    : 	iDistanceAttenuationData(0, 0, EFalse, 0,0),
    	iDataPckgTo(iDistanceAttenuationData)
    {
    }

// Destructor
EXPORT_C CDistanceAttenuation::~CDistanceAttenuation()
    {
    }

// -----------------------------------------------------------------------------
// CDistanceAttenuation::NewL
// Static function for creating an instance of the DistanceAttenuation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CDistanceAttenuation* CDistanceAttenuation::NewL(CMdaAudioConvertUtility& aUtility)
	{
	    
    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CDistanceAttenuationProxy* distanceAttenuationProxy = (CDistanceAttenuationProxy*)customInterface->CustomInterface(KUidDistanceAttenuationEffect);

	if ( !distanceAttenuationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return distanceAttenuationProxy;
	}

// -----------------------------------------------------------------------------
// CDistanceAttenuation::NewL
// Static function for creating an instance of the DistanceAttenuation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CDistanceAttenuation* CDistanceAttenuation::NewL(
	CMdaAudioInputStream& aUtility )
	{
	    
    DEBPRN0;
    CDistanceAttenuationProxy* distanceAttenuationProxy = (CDistanceAttenuationProxy*)aUtility.CustomInterface(KUidDistanceAttenuationEffect);
	
	if (distanceAttenuationProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");    
    	User::Leave(KErrNotSupported);
		}
	    
    return distanceAttenuationProxy;
	}

// -----------------------------------------------------------------------------
// CDistanceAttenuation::NewL
// Static function for creating an instance of the DistanceAttenuation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CDistanceAttenuation* CDistanceAttenuation::NewL(
	CMdaAudioOutputStream& aUtility)
	{
	    
    DEBPRN0;
    CDistanceAttenuationProxy* distanceAttenuationProxy = (CDistanceAttenuationProxy*)aUtility.CustomInterface(KUidDistanceAttenuationEffect);
	
	if (distanceAttenuationProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");    
    	User::Leave(KErrNotSupported);
		}
	    
    return distanceAttenuationProxy;	
    }

// -----------------------------------------------------------------------------
// CDistanceAttenuation::NewL
// Static function for creating an instance of the DistanceAttenuation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CDistanceAttenuation* CDistanceAttenuation::NewL(
	CMdaAudioPlayerUtility& aUtility )
	{
	    
    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CDistanceAttenuationProxy* distanceAttenuationProxy = (CDistanceAttenuationProxy*)customInterface->CustomInterface(KUidDistanceAttenuationEffect);

	if ( !distanceAttenuationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return distanceAttenuationProxy;
	}

// -----------------------------------------------------------------------------
// CDistanceAttenuation::NewL
// Static function for creating an instance of the DistanceAttenuation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CDistanceAttenuation* CDistanceAttenuation::NewL(
	CMdaAudioRecorderUtility& aUtility,	TBool aRecordStream)
	{
	    
    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility, aRecordStream);
    CleanupStack::PushL(customInterface);

	CDistanceAttenuationProxy* distanceAttenuationProxy = (CDistanceAttenuationProxy*)customInterface->CustomInterface(KUidDistanceAttenuationEffect);

	if ( !distanceAttenuationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return distanceAttenuationProxy;
	}

// -----------------------------------------------------------------------------
// CDistanceAttenuation::NewL
// Static function for creating an instance of the DistanceAttenuation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CDistanceAttenuation* CDistanceAttenuation::NewL(
	CMdaAudioToneUtility& aUtility)
	{
	
	DEBPRN0;
    CDistanceAttenuationProxy* distanceAttenuationProxy = (CDistanceAttenuationProxy*)aUtility.CustomInterface(KUidDistanceAttenuationEffect);
	
	if (distanceAttenuationProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");    
    	User::Leave(KErrNotSupported);
		}
	    
    return distanceAttenuationProxy;
	}


// -----------------------------------------------------------------------------
// CDistanceAttenuation::NewL
// Static function for creating an instance of the DistanceAttenuation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CDistanceAttenuation* CDistanceAttenuation::NewL(
	CMMFDevSound& aDevSound )
	{
	
	DEBPRN0;
    CDistanceAttenuationProxy* distanceAttenuationProxy = (CDistanceAttenuationProxy*)aDevSound.CustomInterface(KUidDistanceAttenuationEffect);
	
	if (distanceAttenuationProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");    
    	User::Leave(KErrNotSupported);
		}
	    
    return distanceAttenuationProxy;
	}



// -----------------------------------------------------------------------------
// CDistanceAttenuation::NewL
// Static function for creating an instance of the DistanceAttenuation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CDistanceAttenuation* CDistanceAttenuation::NewL(
	CCustomCommandUtility* aUtility)
	{
    
    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CDistanceAttenuationProxy* distanceAttenuationProxy = (CDistanceAttenuationProxy*)customInterface->CustomInterface(KUidDistanceAttenuationEffect);

	if ( !distanceAttenuationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return distanceAttenuationProxy;
	}

// -----------------------------------------------------------------------------
// CDistanceAttenuation::NewL
// Static function for creating an instance of the DistanceAttenuation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CDistanceAttenuation* CDistanceAttenuation::NewL(
	MCustomInterface& aCustomInterface )
	{
	    
    DEBPRN0;
	CDistanceAttenuationProxy* distanceAttenuationProxy = (CDistanceAttenuationProxy*)aCustomInterface.CustomInterface(KUidDistanceAttenuationEffect);

	if ( !distanceAttenuationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

    return distanceAttenuationProxy;
	}
	
// -----------------------------------------------------------------------------
// CDistanceAttenuation::NewL
// Static function for creating an instance of the DistanceAttenuation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CDistanceAttenuation* CDistanceAttenuation::NewL( 
	CMidiClientUtility& aUtility )
	{
	    
    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CDistanceAttenuationProxy* distanceAttenuationProxy = (CDistanceAttenuationProxy*)customInterface->CustomInterface(KUidDistanceAttenuationEffect);

	if ( !distanceAttenuationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return distanceAttenuationProxy;
	}

// -----------------------------------------------------------------------------
// CDistanceAttenuation::NewL
// Static function for creating an instance of the DistanceAttenuation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CDistanceAttenuation* CDistanceAttenuation::NewL( 
	CDrmPlayerUtility& aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CDistanceAttenuationProxy* distanceAttenuationProxy = (CDistanceAttenuationProxy*)customInterface->CustomInterface(KUidDistanceAttenuationEffect);

	if ( !distanceAttenuationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return distanceAttenuationProxy;
	}

// -----------------------------------------------------------------------------
// CDistanceAttenuation::NewL
// Static function for creating an instance of the DistanceAttenuation object.
// -----------------------------------------------------------------------------
//
EXPORT_C CDistanceAttenuation* CDistanceAttenuation::NewL( 
	CVideoPlayerUtility& aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CDistanceAttenuationProxy* distanceAttenuationProxy = (CDistanceAttenuationProxy*)customInterface->CustomInterface(KUidDistanceAttenuationEffect);

	if ( !distanceAttenuationProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return distanceAttenuationProxy;
	}

// -----------------------------------------------------------------------------
// CDistanceAttenuation::DistanceAttenuation
// -----------------------------------------------------------------------------
//

EXPORT_C void CDistanceAttenuation::DistanceAttenuation(
	TInt32& aRMin, TInt32& aRMax, TBool& aMuteAfterMax,
	TUint32& aRollOffFactor, TUint32& aRoomRollOffFactor)
	{

        DEBPRN0;
		aRMin = iDistanceAttenuationData.iRMin;
		aRMax = iDistanceAttenuationData.iRMax;
		aMuteAfterMax = iDistanceAttenuationData.iMuteAfterMax;
		aRollOffFactor = iDistanceAttenuationData.iRollOffFactor;
		aRoomRollOffFactor = iDistanceAttenuationData.iRoomRollOffFactor;
	}


// -----------------------------------------------------------------------------
// CDistanceAttenuation::RollOffFactorMax
// -----------------------------------------------------------------------------
//
EXPORT_C void CDistanceAttenuation::RollOffFactorMax(TUint32& aRollOfFactorMax)
	{
		aRollOfFactorMax = iDistanceAttenuationData.iRollOffFactorMax;
	}

// -----------------------------------------------------------------------------
// CDistanceAttenuation::RoomRollOffFactorMax
// -----------------------------------------------------------------------------
//
EXPORT_C void CDistanceAttenuation::RoomRollOffFactorMax( TUint32& aRoomRollOfFactorMax )
	{
		aRoomRollOfFactorMax = iDistanceAttenuationData.iRoomRollOffFactorMax;
	}


// -----------------------------------------------------------------------------
// CDistanceAttenuation::SetDistanceAttenuationL
// -----------------------------------------------------------------------------
//

EXPORT_C void CDistanceAttenuation::SetDistanceAttenuationL(
	TInt32 aRMin, TInt32 aRMax, TBool aMuteAfterMax,
	TUint32 aRollOffFactor, TUint32 aRoomRollOffFactor )
	{
	    
        DEBPRN0;
		iDistanceAttenuationData.iRMin = aRMin;
		iDistanceAttenuationData.iRMax = aRMax;
		iDistanceAttenuationData.iMuteAfterMax = aMuteAfterMax;

		if(aRollOffFactor <= iDistanceAttenuationData.iRollOffFactorMax)
			iDistanceAttenuationData.iRollOffFactor = aRollOffFactor;
		else
			User::Leave(KErrArgument);

		if(aRoomRollOffFactor <= iDistanceAttenuationData.iRoomRollOffFactorMax)
			iDistanceAttenuationData.iRoomRollOffFactor = aRoomRollOffFactor;
		else
			User::Leave(KErrArgument);

	}


// -----------------------------------------------------------------------------
// CDistanceAttenuation::Uid
// -----------------------------------------------------------------------------
//
EXPORT_C TUid CDistanceAttenuation::Uid() const
	{
	return KUidDistanceAttenuationEffect;
	}


// -----------------------------------------------------------------------------
// CDistanceAttenuation::DoEffectData
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CDistanceAttenuation::DoEffectData()
	{
	    
    DEBPRN0;
	iDataPckgTo = iDistanceAttenuationData;
	return iDataPckgTo;
	}

// -----------------------------------------------------------------------------
// CDistanceAttenuation::SetEffectData
// -----------------------------------------------------------------------------
//
EXPORT_C void CDistanceAttenuation::SetEffectData(
	const TDesC8& aEffectDataBuffer )
	{
	    
    DEBPRN0;
	TEfDistanceAttenuationDataPckg dataPckg;
	dataPckg.Copy(aEffectDataBuffer);
	iDistanceAttenuationData = dataPckg();
	iEnabled = iDistanceAttenuationData.iEnabled;
	iEnforced = iDistanceAttenuationData.iEnforced;
	iHaveUpdateRights = iDistanceAttenuationData.iHaveUpdateRights;
	}


// ========================== OTHER EXPORTED FUNCTIONS =========================

