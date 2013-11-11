/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains definitions of audio effects data structures for
*                equalizer.
*
*/



#ifndef EQUALIZERDATA_H
#define EQUALIZERDATA_H

//  INCLUDES
#include <e32std.h>
#include <s32mem.h>
#include <AudioEffectData.h>

// CLASS DECLARATION

/**
*  This class defines the effect data structure of a particular frequency band.
*
*  @lib EqualizerEffect.lib
*  @since 3.0
*/
class TEfAudioEqualizerBand
    {
    public:

        /**
        * Constructor.
        */
        TEfAudioEqualizerBand() {}

        void ExternalizeL( RWriteStream& aStream ) const
        	{
			aStream.WriteUint8L(iBandId);
			aStream.WriteUint32L(iBandLevel);
			aStream.WriteUint32L(iBandWidth);
			aStream.WriteUint32L(iCenterFrequency);
			aStream.WriteUint32L(iCrossoverFrequency);
			}

        void InternalizeL( RReadStream& aStream )
        	{
			iBandId = aStream.ReadUint8L();
			iBandLevel = aStream.ReadUint32L();
			iBandWidth = aStream.ReadUint32L();
			iCenterFrequency = aStream.ReadUint32L();
			iCrossoverFrequency = aStream.ReadUint32L();
			}

		TUint8 iBandId;
		TInt32 iBandLevel;
		TInt32 iBandWidth;
		TInt32 iCenterFrequency;
		TInt32 iCrossoverFrequency;

    };

/**
*  This class defines the effect data structure to be passed between client and
*  server.
*
*  @lib EqualizerEffect.lib
*  @since 3.0
*/
class TEfAudioEqualizer : public TEfCommon
    {
    public:

        /**
        * Constructor.
        */
        TEfAudioEqualizer() {}

        /**
        * Constructor.
        */
        TEfAudioEqualizer( TInt32 aMindB, TInt32 aMaxdB ) :
        	iMindB(aMindB), iMaxdB(aMaxdB) {}

		TInt32 iMindB;
		TInt32 iMaxdB;

    };


class TEfAudioEqualizerCombined : public TEfAudioEqualizer, public TEfAudioEqualizerBand
    {
    public:

		// Indicates the data is valid for Equalizer
		TBool iAudioEqualizerDataValid;
		// Indicates the data is valid for Equalizer Band
		TBool iAudioEqualizerBandDataValid;
    };


typedef TPckgBuf<TEfAudioEqualizer> TEfAudioEqualizerDataPckg;
typedef TPckgBuf<TEfAudioEqualizerBand> TEfAudioEqualizerBandDataPckg;
typedef TPckgBuf<TEfAudioEqualizerCombined> TEfAudioEqualizerCombinedDataPckg;

#endif      // EQUALIZERDATA_H

// End of File
