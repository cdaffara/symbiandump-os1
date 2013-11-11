// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Part of the MVS Application for TechView
//

#ifndef CMVSVOLUMEDIALOG_H
#define CMVSVOLUMEDIALOG_H

#include <techview/eikdialg.h>

class CMVSAppUi;
class CEikProgressInfo;

class CMVSVolumeDialog : public CEikDialog
	{
public:
    static void SetupDialogLD(TInt aVolume, TTimeIntervalMicroSeconds aRamp,
                              TTimeIntervalMicroSeconds aClipLength, TBool aPlayback, CMVSAppUi* aAppUi);   
    void PreLayoutDynInitL(); //     //seeds dialog controls
    void InitControl(const TInt aId, const TInt aPos, const TInt aStart, 
                     const TInt aFinish );
                     
private:
    TInt iVolume;  //the playback volume as a percentage of MaxVolume()
    TTimeIntervalMicroSeconds iRamp; //the Ramp
    TTimeIntervalMicroSeconds iClipLength; //the clip length
    TBool iPlayback;
    CEikProgressInfo* iProgInfo;  //pointer to the progress bar for this dlg.

    CMVSAppUi* iAppUi;
    TInt iOldVolume;
     
private:
    CMVSVolumeDialog(TInt aVolume, TTimeIntervalMicroSeconds aRamp,
                    TTimeIntervalMicroSeconds aClipLength, TBool aPlayback, CMVSAppUi* aAudioAppUi);
    TBool OkToExitL(TInt aButtonId); //handles button presses for this dialog
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
	};

#endif CMVSVOLUMEDIALOG_H
