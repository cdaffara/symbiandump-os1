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
* Description:   This file contains definitions of GlobalAudioSettings.
*
*/




#ifndef GLOBALAUDIOSETTINGSDATA_H
#define GLOBALAUDIOSETTINGSDATA_H

//  INCLUDES
#include <e32std.h>
#include <GlobalAudioSettings.h>

// CLASS DECLARATION

/**
*  This class defines the GlobalAudioSettings data structure.
*
*  @lib GlobalAudioSettings.lib
*  @since 3.2
*/
class TGlobalAudioSettings
    {
    public:

        /**
        * Constructor.
        */
        TGlobalAudioSettings() : iWarningTones(EFalse),iMessageTones(EFalse),iSilentProfile(EFalse),
                                 iVibra(EFalse),iPublicSilence(EFalse),iMessageToneName(NULL),iEmailToneName(NULL),iRingingType(CGlobalAudioSettings::EGASRingingTypeRinging),iVideoCallToneName(NULL),
                                 iRingingTone1Name(NULL),iRingingTone2Name(NULL),iKeyPadVolume(CGlobalAudioSettings::EGASKeypadVolumeOff)
        {}

		TBool iWarningTones;
		TBool iMessageTones;
		TBool iSilentProfile;
		TBool iVibra;
		TBool iPublicSilence;
		HBufC *iMessageToneName;
		HBufC *iEmailToneName;
		CGlobalAudioSettings::TGASRingingType iRingingType;
		HBufC *iVideoCallToneName;
		HBufC *iRingingTone1Name;
		HBufC *iRingingTone2Name;
		CGlobalAudioSettings::TGASKeypadVolume iKeyPadVolume;
		TBool iSilenceMode;
    };

#endif      // GlobalAudioSETTINGSDATA_H

// End of File
