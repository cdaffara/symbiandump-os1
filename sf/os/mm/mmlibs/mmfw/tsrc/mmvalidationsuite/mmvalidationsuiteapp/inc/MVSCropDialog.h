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

#ifndef CMVSCROPDIALOG_H
#define CMVSCROPDIALOG_H


#include <techview/eikdialg.h>
#include "MVSApp.h"

class CMVSAppUi;

class CMVSCropDialog : public CEikDialog
	{
public:
    static void SetupDialogLD(TTimeIntervalMicroSeconds aPosition,
                              TTimeIntervalMicroSeconds aClipLength,
                              TTimeIntervalMicroSeconds aCropStart,
                              TTimeIntervalMicroSeconds aCropEnd,
                              CMVSAppUi* aAppUi);
    void PreLayoutDynInitL();    //seeds dialog controls
    void InitControl(const TInt aId, const TInt aPos, const TInt aStart, 
                     const TInt aFinish );
     
private:
    CMVSCropDialog(TTimeIntervalMicroSeconds aPosition,
                       TTimeIntervalMicroSeconds aClipLength,
                       TTimeIntervalMicroSeconds aCropStart,
                       TTimeIntervalMicroSeconds aCropEnd,
                       CMVSAppUi* aAppUi);
    TBool OkToExitL(TInt aButtonId); //handles button presses for this dialog

private:
    TTimeIntervalMicroSeconds iPosition; //final clipping position
    TTimeIntervalMicroSeconds iClipLength;  //length of clip
    TTimeIntervalMicroSeconds iCropStart;   //cropping start point
    TTimeIntervalMicroSeconds iCropEnd;     //cropping end point

    TBool iCropFromBeginning; //Crop from beginning (vs. from end) flag
    CEikProgressInfo* iProgInfo; //Pointer to the progress bar for the dlg

    CMVSAppUi* iAppUi;
    };

#endif  CMVSCROPDIALOG_H
