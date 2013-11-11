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
* Description:   Implementation of the Environmental Reverb effect class
*
*/




// INCLUDE FILES

#ifdef _DEBUG
#include <e32svr.h>
#endif

#include <EnvironmentalReverbBase.h>
#include <CustomInterfaceUtility.h>
#include "EnvironmentalReverbProxy.h"
#include <DrmAudioSamplePlayer.h>
#include <mdaaudioinputstream.h>
#include <mdaaudiooutputstream.h>
#include <mdaaudiotoneplayer.h>
#include <mmf/server/sounddevice.h>

#ifdef _DEBUG
#define DEBPRN0         RDebug::Printf( "%s", __PRETTY_FUNCTION__);
#define DEBPRN1(str)    RDebug::Printf( "%s %s", __PRETTY_FUNCTION__, str );
#else
#define DEBPRN0
#define DEBPRN1(str)
#endif

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CEnvironmentalReverb::CEnvironmentalReverb
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CEnvironmentalReverb::CEnvironmentalReverb()
    : 	iReverbData(),
    	iDataPckgTo(iReverbData)
    {
    }

// Destructor
EXPORT_C CEnvironmentalReverb::~CEnvironmentalReverb()
    {
    }

// -----------------------------------------------------------------------------
// CEnvironmentalReverb::NewL
// Static function for creating an instance of the Environmental Reverb object.
// -----------------------------------------------------------------------------
//
EXPORT_C CEnvironmentalReverb* CEnvironmentalReverb::NewL()
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

// -----------------------------------------------------------------------------
// CEnvironmentalReverb::NewL
// Static function for creating an instance of the EnvironmentalReverb object.
// -----------------------------------------------------------------------------
//
EXPORT_C CEnvironmentalReverb* CEnvironmentalReverb::NewL(
	CMdaAudioConvertUtility& aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CEnvironmentalReverbProxy* environmentalReverbProxy = (CEnvironmentalReverbProxy*)customInterface->CustomInterface(KUidEnvironmentalReverbEffect);

	if ( !environmentalReverbProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return environmentalReverbProxy;
	}

// -----------------------------------------------------------------------------
// CEnvironmentalReverb::NewL
// Static function for creating an instance of the EnvironmentalReverb object.
// -----------------------------------------------------------------------------
//
EXPORT_C CEnvironmentalReverb* CEnvironmentalReverb::NewL(
	CMdaAudioInputStream& aUtility )
	{

    DEBPRN0;
    CEnvironmentalReverbProxy* environmentalReverbProxy = (CEnvironmentalReverbProxy*)aUtility.CustomInterface(KUidEnvironmentalReverbEffect);

	if (environmentalReverbProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

    return environmentalReverbProxy;
	}

// -----------------------------------------------------------------------------
// CEnvironmentalReverb::NewL
// Static function for creating an instance of the EnvironmentalReverb object.
// -----------------------------------------------------------------------------
//
EXPORT_C CEnvironmentalReverb* CEnvironmentalReverb::NewL(
	CMdaAudioOutputStream& aUtility )
	{

    DEBPRN0;
    CEnvironmentalReverbProxy* environmentalReverbProxy = (CEnvironmentalReverbProxy*)aUtility.CustomInterface(KUidEnvironmentalReverbEffect);

	if (environmentalReverbProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

    return environmentalReverbProxy;
	}

// -----------------------------------------------------------------------------
// CEnvironmentalReverb::NewL
// Static function for creating an instance of the EnvironmentalReverb object.
// -----------------------------------------------------------------------------
//
EXPORT_C CEnvironmentalReverb* CEnvironmentalReverb::NewL(
	CMdaAudioPlayerUtility& aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CEnvironmentalReverbProxy* environmentalReverbProxy = (CEnvironmentalReverbProxy*)customInterface->CustomInterface(KUidEnvironmentalReverbEffect);

	if ( !environmentalReverbProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return environmentalReverbProxy;
	}

// -----------------------------------------------------------------------------
// CEnvironmentalReverb::NewL
// Static function for creating an instance of the EnvironmentalReverb object.
// -----------------------------------------------------------------------------
//
EXPORT_C CEnvironmentalReverb* CEnvironmentalReverb::NewL(
	CMdaAudioRecorderUtility& aUtility,
	TBool aRecordStream )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility, aRecordStream);
    CleanupStack::PushL(customInterface);

	CEnvironmentalReverbProxy* environmentalReverbProxy = (CEnvironmentalReverbProxy*)customInterface->CustomInterface(KUidEnvironmentalReverbEffect);

	if ( !environmentalReverbProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return environmentalReverbProxy;
	}

// -----------------------------------------------------------------------------
// CEnvironmentalReverb::NewL
// Static function for creating an instance of the EnvironmentalReverb object.
// -----------------------------------------------------------------------------
//
EXPORT_C CEnvironmentalReverb* CEnvironmentalReverb::NewL(
	CMdaAudioToneUtility& aUtility )
	{

	DEBPRN0;
    CEnvironmentalReverbProxy* environmentalReverbProxy = (CEnvironmentalReverbProxy*)aUtility.CustomInterface(KUidEnvironmentalReverbEffect);

	if (environmentalReverbProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

    return environmentalReverbProxy;
	}

// -----------------------------------------------------------------------------
// CAudioEqualizer::NewL
// Static function for creating an instance of the EnvironmentalReverb object.
// -----------------------------------------------------------------------------
//
EXPORT_C CEnvironmentalReverb* CEnvironmentalReverb::NewL(
	CMMFDevSound& aDevSound )
	{

	DEBPRN0;
    CEnvironmentalReverbProxy* environmentalReverbProxy = (CEnvironmentalReverbProxy*)aDevSound.CustomInterface(KUidEnvironmentalReverbEffect);

	if (environmentalReverbProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");
    	User::Leave(KErrNotSupported);
		}

    return environmentalReverbProxy;
	}

// -----------------------------------------------------------------------------
// CEnvironmentalReverb::NewL
// Static function for creating an instance of the EnvironmentalReverb object.
// -----------------------------------------------------------------------------
//
EXPORT_C CEnvironmentalReverb* CEnvironmentalReverb::NewL(
	CCustomCommandUtility* aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CEnvironmentalReverbProxy* environmentalReverbProxy = (CEnvironmentalReverbProxy*)customInterface->CustomInterface(KUidEnvironmentalReverbEffect);

	if ( !environmentalReverbProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

 	CleanupStack::Pop(customInterface);

    return environmentalReverbProxy;
	}

// -----------------------------------------------------------------------------
// CEnvironmentalReverb::NewL
// Static function for creating an instance of the EnvironmentalReverb object.
// -----------------------------------------------------------------------------
//
EXPORT_C CEnvironmentalReverb* CEnvironmentalReverb::NewL(
	MCustomInterface& aCustomInterface )
	{

    DEBPRN0;
	CEnvironmentalReverb* environmentalReverbProxy = (CEnvironmentalReverb*)aCustomInterface.CustomInterface(KUidEnvironmentalReverbEffect);
	if ( !environmentalReverbProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

    return environmentalReverbProxy;
	}


// -----------------------------------------------------------------------------
// CEnvironmentalReverb::NewL
// Static function for creating an instance of the EnvironmentalReverb object.
// -----------------------------------------------------------------------------
//
EXPORT_C CEnvironmentalReverb* CEnvironmentalReverb::NewL(CMidiClientUtility& aUtility )
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CEnvironmentalReverbProxy* environmentalReverbProxy = (CEnvironmentalReverbProxy*)customInterface->CustomInterface(KUidEnvironmentalReverbEffect);

	if ( !environmentalReverbProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return environmentalReverbProxy;
	}

// -----------------------------------------------------------------------------
// CEnvironmentalReverb::NewL
// Static function for creating an instance of the EnvironmentalReverb object.
// -----------------------------------------------------------------------------
//
EXPORT_C CEnvironmentalReverb* CEnvironmentalReverb::NewL(CDrmPlayerUtility& aUtility)
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CEnvironmentalReverbProxy* environmentalReverbProxy = (CEnvironmentalReverbProxy*)customInterface->CustomInterface(KUidEnvironmentalReverbEffect);

	if ( !environmentalReverbProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return environmentalReverbProxy;
	}

// -----------------------------------------------------------------------------
// CEnvironmentalReverb::NewL
// Static function for creating an instance of the EnvironmentalReverb object.
// -----------------------------------------------------------------------------
//
EXPORT_C CEnvironmentalReverb* CEnvironmentalReverb::NewL(CVideoPlayerUtility& aUtility)
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CEnvironmentalReverbProxy* environmentalReverbProxy = (CEnvironmentalReverbProxy*)customInterface->CustomInterface(KUidEnvironmentalReverbEffect);

	if ( !environmentalReverbProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return environmentalReverbProxy;
	}

// -----------------------------------------------------------------------------
// CEnvironmentalReverb::DecayHFRatio
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CEnvironmentalReverb::DecayHFRatio() const
	{
	return iReverbData.iDecayHFRatio;
	}

// -----------------------------------------------------------------------------
// CEnvironmentalReverb::DecayHFRatioRange
// -----------------------------------------------------------------------------
//
EXPORT_C void CEnvironmentalReverb::DecayHFRatioRange(
	TUint32& aMin,
	TUint32& aMax )
	{
	aMin = iReverbData.iDecayHFRatioMin;
	aMax = iReverbData.iDecayHFRatioMax;
	}

// -----------------------------------------------------------------------------
// CEnvironmentalReverb::DecayTime
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CEnvironmentalReverb::DecayTime() const
	{
	return iReverbData.iDecayTime;
	}

// -----------------------------------------------------------------------------
// CEnvironmentalReverb::DecayTimeRange
// -----------------------------------------------------------------------------
//
EXPORT_C void CEnvironmentalReverb::DecayTimeRange(
	TUint32& aMin,
	TUint32& aMax)
	{
	aMin = iReverbData.iDecayTimeMin;
	aMax = iReverbData.iDecayTimeMax;
	}

// -----------------------------------------------------------------------------
// CEnvironmentalReverb::Density
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CEnvironmentalReverb::Density() const
	{
	return iReverbData.iDensity;
	}

// -----------------------------------------------------------------------------
// CEnvironmentalReverb::Diffusion
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CEnvironmentalReverb::Diffusion() const
	{
	return iReverbData.iDiffusion;
	}

// -----------------------------------------------------------------------------
// CEnvironmentalReverb::ReflectionsDelay
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CEnvironmentalReverb::ReflectionsDelay() const
	{
	return iReverbData.iReflectionsDelay;
	}

// -----------------------------------------------------------------------------
// CEnvironmentalReverb::ReflectionsDelayMax
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CEnvironmentalReverb::ReflectionsDelayMax() const
	{
	return iReverbData.iReflectionsDelayMax;
	}

// -----------------------------------------------------------------------------
// CEnvironmentalReverb::ReflectionsLevel
// -----------------------------------------------------------------------------
//
EXPORT_C TInt32 CEnvironmentalReverb::ReflectionsLevel() const
	{
	return iReverbData.iReflectionsLevel;
	}

// -----------------------------------------------------------------------------
// CEnvironmentalReverb::ReflectionLevelRange
// -----------------------------------------------------------------------------
//
EXPORT_C void CEnvironmentalReverb::ReflectionLevelRange(
	TInt32& aMin,
	TInt32& aMax)
	{
	aMin = iReverbData.iReflectionLevelMin;
	aMax = iReverbData.iReflectionLevelMax;
	}

// -----------------------------------------------------------------------------
// CEnvironmentalReverb::ReverbDelay
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CEnvironmentalReverb::ReverbDelay() const
	{
	return iReverbData.iReverbDelay;
	}

// -----------------------------------------------------------------------------
// CEnvironmentalReverb::ReverbDelayMax
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CEnvironmentalReverb::ReverbDelayMax() const
	{
	return iReverbData.iReverbDelayMax;
	}


// -----------------------------------------------------------------------------
// CEnvironmentalReverb::ReverbLevel
// -----------------------------------------------------------------------------
//
EXPORT_C TInt32 CEnvironmentalReverb::ReverbLevel() const
	{
	return iReverbData.iReverbLevel;
	}

// -----------------------------------------------------------------------------
// CEnvironmentalReverb::ReverbLevelRange
// -----------------------------------------------------------------------------
//
EXPORT_C void CEnvironmentalReverb::ReverbLevelRange(
	TInt32& aMin,
	TInt32& aMax)
	{
	aMin = iReverbData.iReverbLevelMin;
	aMax = iReverbData.iReverbLevelMax;
	}


// -----------------------------------------------------------------------------
// CEnvironmentalReverb::RoomHFLevel
// -----------------------------------------------------------------------------
//
EXPORT_C TInt32 CEnvironmentalReverb::RoomHFLevel() const
	{
	return iReverbData.iRoomHFLevel;
	}


// -----------------------------------------------------------------------------
// CEnvironmentalReverb::RoomHFLevelRange
// -----------------------------------------------------------------------------
//
EXPORT_C void CEnvironmentalReverb::RoomHFLevelRange(
	TInt32& aMin,
	TInt32& aMax)
	{
	aMin = iReverbData.iRoomHFLevelMin;
	aMax = iReverbData.iRoomHFLevelMax;
	}


// -----------------------------------------------------------------------------
// CEnvironmentalReverb::RoomLevel
// -----------------------------------------------------------------------------
//
EXPORT_C TInt32 CEnvironmentalReverb::RoomLevel() const

	{
	return iReverbData.iRoomLevel;
	}


// -----------------------------------------------------------------------------
// CEnvironmentalReverb::RoomLevelRange
// -----------------------------------------------------------------------------
//
EXPORT_C void CEnvironmentalReverb::RoomLevelRange(
	TInt32& aMin,
	TInt32& aMax)
	{
	aMin = iReverbData.iRoomLevelMin;
	aMax = iReverbData.iRoomLevelMax;
	}



// -----------------------------------------------------------------------------
// CEnvironmentalReverb::SetDecayHFRatioL
// -----------------------------------------------------------------------------
//
EXPORT_C void CEnvironmentalReverb::SetDecayHFRatioL(
	TUint32 aDecayHFRatio )
	{
	if ( (aDecayHFRatio >= iReverbData.iDecayHFRatioMin) && (aDecayHFRatio <= iReverbData.iDecayHFRatioMax) )
		{
		iReverbData.iDecayHFRatio = aDecayHFRatio;
		}
	else
		{
		User::Leave(KErrArgument);
		}
	}


// -----------------------------------------------------------------------------
// CEnvironmentalReverb::SetDecayTimeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CEnvironmentalReverb::SetDecayTimeL(
	TUint32 aDecayTime )
	{
	if ( (aDecayTime >= iReverbData.iDecayTimeMin) && (aDecayTime <= iReverbData.iDecayTimeMax) )
		{
		iReverbData.iDecayTime = aDecayTime;
		}
	else
		{
		User::Leave(KErrArgument);
		}
	}


// -----------------------------------------------------------------------------
// CEnvironmentalReverb::SetDensityL
// -----------------------------------------------------------------------------
//
EXPORT_C void CEnvironmentalReverb::SetDensityL(
	TUint32 aDensity )
	{
	iReverbData.iDensity = aDensity;
	}

// -----------------------------------------------------------------------------
// CEnvironmentalReverb::SetDiffusionL
// -----------------------------------------------------------------------------
//
EXPORT_C void CEnvironmentalReverb::SetDiffusionL(
	TUint32 aDiffusion )
	{
	iReverbData.iDiffusion = aDiffusion;
	}


// -----------------------------------------------------------------------------
// CEnvironmentalReverb::SetReflectionsDelayL
// -----------------------------------------------------------------------------
//
EXPORT_C void CEnvironmentalReverb::SetReflectionsDelayL(
	TUint32 aReflectionsDelay )
	{
	if ( aReflectionsDelay > iReverbData.iReflectionsDelayMax )
		{
		User::Leave(KErrArgument);
		}
	else
		{
		iReverbData.iReflectionsDelay = aReflectionsDelay;
		}
	}

// -----------------------------------------------------------------------------
// CEnvironmentalReverb::SetReflectionsLevelL
// -----------------------------------------------------------------------------
//
EXPORT_C void CEnvironmentalReverb::SetReflectionsLevelL(
	TInt32 aReflectionsLevel )
	{
	if ( (aReflectionsLevel >= iReverbData.iReflectionLevelMin) && (aReflectionsLevel <= iReverbData.iReflectionLevelMax) )
		{
		iReverbData.iReflectionsLevel = aReflectionsLevel;
		}
	else
		{
		User::Leave(KErrArgument);
		}
	}

// -----------------------------------------------------------------------------
// CEnvironmentalReverb::SetReverbDelayL
// -----------------------------------------------------------------------------
//
EXPORT_C void CEnvironmentalReverb::SetReverbDelayL(
	TUint32 aReverbDelay )
	{
	if ( aReverbDelay > iReverbData.iReverbDelayMax )
		{
		User::Leave(KErrArgument);
		}
	else
		{
		iReverbData.iReverbDelay = aReverbDelay;
		}
	}


// -----------------------------------------------------------------------------
// CEnvironmentalReverb::SetReverbLevelL
// -----------------------------------------------------------------------------
//
EXPORT_C void CEnvironmentalReverb::SetReverbLevelL(
	TInt32 aReverbLevel )
	{
	if ( (aReverbLevel >= iReverbData.iReverbLevelMin) && (aReverbLevel <= iReverbData.iReverbLevelMax) )
		{
		iReverbData.iReverbLevel = aReverbLevel;
		}
	else
		{
		User::Leave(KErrArgument);
		}
	}

// -----------------------------------------------------------------------------
// CEnvironmentalReverb::SetRoomHFLevelL
// -----------------------------------------------------------------------------
//
EXPORT_C void CEnvironmentalReverb::SetRoomHFLevelL(
	TInt32 aRoomHFLevel )
	{
	if ( (aRoomHFLevel >= iReverbData.iRoomHFLevelMin) && (aRoomHFLevel <= iReverbData.iRoomHFLevelMax) )
		{
		iReverbData.iRoomHFLevel = aRoomHFLevel;
		}
	else
		{
		User::Leave(KErrArgument);
		}
	}

// -----------------------------------------------------------------------------
// CEnvironmentalReverb::SetRoomLevelL
// -----------------------------------------------------------------------------
//
EXPORT_C void CEnvironmentalReverb::SetRoomLevelL(
	TInt32 aRoomLevel )
	{
	if ( (aRoomLevel >= iReverbData.iRoomLevelMin) && (aRoomLevel <= iReverbData.iRoomLevelMax) )
		{
		iReverbData.iRoomLevel = aRoomLevel;
		}
	else
		{
		User::Leave(KErrArgument);
		}
	}

// -----------------------------------------------------------------------------
// CEnvironmentalReverb::DelayMax
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CEnvironmentalReverb::DelayMax() const
	{
	return iReverbData.iDelayMax;
	}


// -----------------------------------------------------------------------------
// CEnvironmentalReverb::Uid
// -----------------------------------------------------------------------------
//
EXPORT_C TUid CEnvironmentalReverb::Uid() const
	{
	return KUidEnvironmentalReverbEffect;
	}


// -----------------------------------------------------------------------------
// CEnvironmentalReverb::DoEffectData
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CEnvironmentalReverb::DoEffectData()
	{
    DEBPRN0;
	iDataPckgTo = iReverbData;
	return iDataPckgTo;
	}

// -----------------------------------------------------------------------------
// CEnvironmentalReverb::SetEffectData
// -----------------------------------------------------------------------------
//
EXPORT_C void CEnvironmentalReverb::SetEffectData(
	const TDesC8& aEffectDataBuffer )
	{
    DEBPRN0;
	TEfEnvReverbDataPckg dataPckg;
	dataPckg.Copy(aEffectDataBuffer);
	iReverbData = dataPckg();
	iEnabled = iReverbData.iEnabled;
	iEnforced = iReverbData.iEnforced;
	iHaveUpdateRights = iReverbData.iHaveUpdateRights;
	}


// ========================== OTHER EXPORTED FUNCTIONS =========================


