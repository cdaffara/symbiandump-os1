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
* Description:   Implementation of the Audio Equalizer effect class
*
*/




// INCLUDE FILES

#ifdef _DEBUG
#include <e32svr.h>
#endif

#include <AudioEqualizerBase.h>
#include <CustomInterfaceUtility.h>
#include "AudioEqualizerProxy.h"
#include <s32mem.h>
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
// CAudioEqualizer::CAudioEqualizer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CAudioEqualizer::CAudioEqualizer()
    : 	iAudioEqualizerData(0, 0),
    	iDataPckgTo(iAudioEqualizerData),
    	iBandChange(0)
    {
    }

// Destructor
EXPORT_C CAudioEqualizer::~CAudioEqualizer()
    {
        delete iBandDataBuf;
	iBandsData.Close();
    }

// -----------------------------------------------------------------------------
// CAudioEqualizer::NewL
// Static function for creating an instance of the AudioEqualizer object.
// -----------------------------------------------------------------------------
//
EXPORT_C CAudioEqualizer* CAudioEqualizer::NewL()
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

// -----------------------------------------------------------------------------
// CAudioEqualizer::NewL
// Static function for creating an instance of the AudioEqualizer object.
// -----------------------------------------------------------------------------
//
EXPORT_C CAudioEqualizer* CAudioEqualizer::NewL(
	CMdaAudioConvertUtility& aUtility )
	{
    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CAudioEqualizerProxy* audioEqualizerProxy = (CAudioEqualizerProxy*)customInterface->CustomInterface(KUidAudioEqualizerEffect);

	if ( !audioEqualizerProxy )
		{
        DEBPRN1("No Adaptation Support - leaving"); 
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return audioEqualizerProxy;
	}

// -----------------------------------------------------------------------------
// CAudioEqualizer::NewL
// Static function for creating an instance of the AudioEqualizer object.
// -----------------------------------------------------------------------------
//
EXPORT_C CAudioEqualizer* CAudioEqualizer::NewL(
	CMdaAudioInputStream& aUtility )
	{
    DEBPRN0;
    CAudioEqualizerProxy* audioEqualizerProxy = (CAudioEqualizerProxy*)aUtility.CustomInterface(KUidAudioEqualizerEffect);
	if (audioEqualizerProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving"); 
		User::Leave(KErrNotSupported);
		}
    return audioEqualizerProxy;
	}

// -----------------------------------------------------------------------------
// CAudioEqualizer::NewL
// Static function for creating an instance of the AudioEqualizer object.
// -----------------------------------------------------------------------------
//
EXPORT_C CAudioEqualizer* CAudioEqualizer::NewL(
	CMdaAudioOutputStream& aUtility )
	{
    DEBPRN0;
    CAudioEqualizerProxy* audioEqualizerProxy = (CAudioEqualizerProxy*)aUtility.CustomInterface(KUidAudioEqualizerEffect);
	if (audioEqualizerProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");    
    	User::Leave(KErrNotSupported);
		}
    return audioEqualizerProxy;
	}

// -----------------------------------------------------------------------------
// CAudioEqualizer::NewL
// Static function for creating an instance of the AudioEqualizer object.
// -----------------------------------------------------------------------------
//
EXPORT_C CAudioEqualizer* CAudioEqualizer::NewL(
	CMdaAudioPlayerUtility& aUtility )
	{
    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CAudioEqualizerProxy* audioEqualizerProxy = (CAudioEqualizerProxy*)customInterface->CustomInterface(KUidAudioEqualizerEffect);

	if ( !audioEqualizerProxy )
		{
        DEBPRN1("No Adaptation Support - leaving"); 
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

    return audioEqualizerProxy;
	}

// -----------------------------------------------------------------------------
// CAudioEqualizer::NewL
// Static function for creating an instance of the AudioEqualizer object.
// -----------------------------------------------------------------------------
//
EXPORT_C CAudioEqualizer* CAudioEqualizer::NewL(
	CMdaAudioRecorderUtility& aUtility,
	TBool aRecordStream )
	{
    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility, aRecordStream);
    CleanupStack::PushL(customInterface);

	CAudioEqualizerProxy* audioEqualizerProxy = (CAudioEqualizerProxy*)customInterface->CustomInterface(KUidAudioEqualizerEffect);

	if ( !audioEqualizerProxy )
		{
        DEBPRN1("No Adaptation Support - leaving"); 
		User::Leave(KErrNotSupported);
		}

    CleanupStack::Pop(customInterface);

    return audioEqualizerProxy;
	}

// -----------------------------------------------------------------------------
// CAudioEqualizer::NewL
// Static function for creating an instance of the AudioEqualizer object.
// -----------------------------------------------------------------------------
//
EXPORT_C CAudioEqualizer* CAudioEqualizer::NewL(
	CMdaAudioToneUtility& aUtility )
	{
    DEBPRN0;
    CAudioEqualizerProxy* audioEqualizerProxy = (CAudioEqualizerProxy*)aUtility.CustomInterface(KUidAudioEqualizerEffect);
	if (audioEqualizerProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving"); 
		User::Leave(KErrNotSupported);
		}
    return audioEqualizerProxy;
	}

// -----------------------------------------------------------------------------
// CAudioEqualizer::NewL
// Static function for creating an instance of the AudioEqualizer object.
// -----------------------------------------------------------------------------
//
EXPORT_C CAudioEqualizer* CAudioEqualizer::NewL(
	CMMFDevSound& aDevSound )
	{
    DEBPRN0;
    CAudioEqualizerProxy* audioEqualizerProxy = (CAudioEqualizerProxy*)aDevSound.CustomInterface(KUidAudioEqualizerEffect);
	if (audioEqualizerProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving"); 
		User::Leave(KErrNotSupported);
		}
    return audioEqualizerProxy;
	}

// -----------------------------------------------------------------------------
// CAudioEqualizer::NewL
// Static function for creating an instance of the AudioEqualizer object.
// -----------------------------------------------------------------------------
//
EXPORT_C CAudioEqualizer* CAudioEqualizer::NewL(
	CCustomCommandUtility* aUtility )
	{
    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CAudioEqualizerProxy* audioEqualizerProxy = (CAudioEqualizerProxy*)customInterface->CustomInterface(KUidAudioEqualizerEffect);
	if ( !audioEqualizerProxy )
		{
		DEBPRN1("No Adaptation Support - leaving"); 
		User::Leave(KErrNotSupported);
		}

    CleanupStack::Pop(customInterface);

    return audioEqualizerProxy;
	}

// -----------------------------------------------------------------------------
// CAudioEqualizer::NewL
// Static function for creating an instance of the AudioEqualizer object.
// -----------------------------------------------------------------------------
//
EXPORT_C CAudioEqualizer* CAudioEqualizer::NewL(
	MCustomInterface& aCustomInterface )
	{
    DEBPRN0;
	CAudioEqualizerProxy* audioEqualizerProxy = (CAudioEqualizerProxy*)aCustomInterface.CustomInterface(KUidAudioEqualizerEffect);

	if ( !audioEqualizerProxy )
		{
		DEBPRN1("No Adaptation Support - leaving"); 
		User::Leave(KErrNotSupported);
		}

    return audioEqualizerProxy;
	}

// -----------------------------------------------------------------------------
// CAudioEqualizer::NewL
// Static function for creating an instance of the AudioEqualizer object.
// -----------------------------------------------------------------------------
//
EXPORT_C CAudioEqualizer* CAudioEqualizer::NewL( 
	CMidiClientUtility& aUtility )
    {
    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CAudioEqualizerProxy* audioEqualizerProxy = (CAudioEqualizerProxy*)customInterface->CustomInterface(KUidAudioEqualizerEffect);

	if ( !audioEqualizerProxy )
		{
		DEBPRN1("No Adaptation Support - leaving"); 
		User::Leave(KErrNotSupported);
		}

    CleanupStack::Pop(customInterface);

    return audioEqualizerProxy;
    }

// -----------------------------------------------------------------------------
// CAudioEqualizer::NewL
// Static function for creating an instance of the AudioEqualizer object.
// -----------------------------------------------------------------------------
//
EXPORT_C CAudioEqualizer* CAudioEqualizer::NewL( 
	CDrmPlayerUtility& aUtility )
    {
    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CAudioEqualizerProxy* audioEqualizerProxy = (CAudioEqualizerProxy*)customInterface->CustomInterface(KUidAudioEqualizerEffect);

	if ( !audioEqualizerProxy )
		{
        DEBPRN1("No Adaptation Support - leaving"); 
		User::Leave(KErrNotSupported);
		}

    CleanupStack::Pop(customInterface);

    return audioEqualizerProxy;
    }

// -----------------------------------------------------------------------------
// CAudioEqualizer::NewL
// Static function for creating an instance of the AudioEqualizer object.
// -----------------------------------------------------------------------------
//
EXPORT_C CAudioEqualizer* CAudioEqualizer::NewL( 
	CVideoPlayerUtility& aUtility )
    {
    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CAudioEqualizerProxy* audioEqualizerProxy = (CAudioEqualizerProxy*)customInterface->CustomInterface(KUidAudioEqualizerEffect);

	if ( !audioEqualizerProxy )
		{
        DEBPRN1("No Adaptation Support - leaving"); 
		User::Leave(KErrNotSupported);
		}

    CleanupStack::Pop(customInterface);

    return audioEqualizerProxy;
    }
// -----------------------------------------------------------------------------
// CAudioEqualizer::BandLevel
// -----------------------------------------------------------------------------
//
EXPORT_C TInt32 CAudioEqualizer::BandLevel(
	TUint8 aBand ) const
	{
	if( aBand > NumberOfBands() )
		{
		return KErrArgument;
		}
	else
		{
		return iBandsData[aBand-1].iBandLevel;
		}
	}

// -----------------------------------------------------------------------------
// CAudioEqualizer::BandWidth
// -----------------------------------------------------------------------------
//
EXPORT_C TInt32 CAudioEqualizer::BandWidth(
	TUint8 aBand ) const
	{
	if( aBand > NumberOfBands() )
		{
		return KErrArgument;
		}
	else
		{
		return iBandsData[aBand-1].iBandWidth;
		}
	}

// -----------------------------------------------------------------------------
// CAudioEqualizer::CenterFrequency
// -----------------------------------------------------------------------------
//
EXPORT_C TInt32 CAudioEqualizer::CenterFrequency(
	TUint8 aBand ) const
	{
	if( aBand > NumberOfBands() )
		{
		return KErrArgument;
		}
	else
		{
		return iBandsData[aBand-1].iCenterFrequency;
		}
	}

// -----------------------------------------------------------------------------
// CAudioEqualizer::CrossoverFrequency
// -----------------------------------------------------------------------------
//
EXPORT_C TInt32 CAudioEqualizer::CrossoverFrequency(
	TUint8 aBand ) const
	{
	if( aBand > NumberOfBands() )
		{
		return KErrArgument;
		}
	else
		{
		return iBandsData[aBand-1].iCrossoverFrequency;
		}
	}

// -----------------------------------------------------------------------------
// CAudioEqualizer::DbLevelLimits
// -----------------------------------------------------------------------------
//
EXPORT_C void CAudioEqualizer::DbLevelLimits(
	TInt32& aMin,
	TInt32& aMax )
	{
	aMin = iAudioEqualizerData.iMindB;
	aMax = iAudioEqualizerData.iMaxdB;
	}

// -----------------------------------------------------------------------------
// CAudioEqualizer::CrossoverFrequency
// -----------------------------------------------------------------------------
//
EXPORT_C TUint8 CAudioEqualizer::NumberOfBands() const

	{
	return iBandsData.Count();
	}

// -----------------------------------------------------------------------------
// CAudioEqualizer::SetBandLevelL
// -----------------------------------------------------------------------------
//
EXPORT_C void CAudioEqualizer::SetBandLevelL(
	TInt8 aBand,
	TInt32 aLevel )
	{
	if( aBand > NumberOfBands() )
		{
		User::Leave(KErrArgument);
		}
	else
		{
		TUint32 mask = 1;
		iBandChange = iBandChange | (mask << aBand-1);
		iBandsData[aBand-1].iBandLevel = aLevel;
		}
	}

// -----------------------------------------------------------------------------
// CAudioEqualizer::Uid
// -----------------------------------------------------------------------------
//
EXPORT_C TUid CAudioEqualizer::Uid() const
	{
	return KUidAudioEqualizerEffect;
	}


// -----------------------------------------------------------------------------
// CAudioEqualizer::DoEffectData
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CAudioEqualizer::DoEffectData()
	{
    DEBPRN0;
	iDataPckgTo = iAudioEqualizerData;
	return iDataPckgTo;
	}

// -----------------------------------------------------------------------------
// CAudioEqualizer::SetEffectData
// -----------------------------------------------------------------------------
//
EXPORT_C void CAudioEqualizer::SetEffectData(
	const TDesC8& aEffectDataBuffer )
	{
    DEBPRN0;
	TEfAudioEqualizerDataPckg dataPckg;
	dataPckg.Copy(aEffectDataBuffer);
	iAudioEqualizerData = dataPckg();
	iEnabled = iAudioEqualizerData.iEnabled;
	iEnforced = iAudioEqualizerData.iEnforced;
	iHaveUpdateRights = iAudioEqualizerData.iHaveUpdateRights;
	}

// -----------------------------------------------------------------------------
// CAudioEqualizer::DoBandData
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CAudioEqualizer::DoBandDataL(
	TUint8 aBand,
	TBool aAllBands )
	{
	    
    DEBPRN0;
	if ( aAllBands )
		{
		if ( iBandDataBuf )
			{
			delete iBandDataBuf;
			iBandDataBuf = NULL;
			}

		iBandDataBuf = HBufC8::NewL(NumberOfBands() * sizeof(TEfAudioEqualizerBand));
		TPtr8 des = iBandDataBuf->Des();
		des.SetLength(iBandDataBuf->Size());
		RDesWriteStream writeStream(des);

		for (TInt i=0; i<NumberOfBands(); i++)
			{
			writeStream << iBandsData[i];
			}
		writeStream.CommitL();
		return *iBandDataBuf;
		}
	else
		{
		if ( aBand > NumberOfBands() )
			{
			User::Leave(KErrArgument);
			}
		iBandDataPckgTo = iBandsData[aBand-1];
		return iBandDataPckgTo;
		}
	}

// -----------------------------------------------------------------------------
// CAudioEqualizer::SetBandData
// -----------------------------------------------------------------------------
//
EXPORT_C void CAudioEqualizer::SetBandDataL(
	TInt aNumberOfBands,
	const TDesC8& aBandDataBuffer )
	{
    DEBPRN0;
	if ( aNumberOfBands > 1 )
		{
		RDesReadStream readStream(aBandDataBuffer);
		for (TInt i=0; i<NumberOfBands(); i++)
			{
			readStream >> iBandsData[i];
			}
		}
	else
		{
		TEfAudioEqualizerBandDataPckg dataPckg;
		TEfAudioEqualizerBand band;
		dataPckg.Copy(aBandDataBuffer);
		band = dataPckg();
		if ( band.iBandId > NumberOfBands() )
			{
			User::Leave(KErrArgument);
			}
		iBandsData[band.iBandId-1] = band;
		}
	}

// ========================== OTHER EXPORTED FUNCTIONS =========================


