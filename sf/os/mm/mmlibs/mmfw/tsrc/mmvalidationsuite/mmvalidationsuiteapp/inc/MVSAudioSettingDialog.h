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

#ifndef CMVSAUDIOSETTINGDIALOG_H
#define CMVSAUDIOSETTINGDIALOG_H

#include <techview/eikdialg.h>
#include "MVSAppUI.h"


class CMVSAppUi;

class CMVSAudioSettingDialog : public CEikDialog
							   
     {
public:
     static void SetupDialogLD(TInt aNoRepeats, 
                          TTimeIntervalMicroSeconds aTrailingSilence,TInt aAudioPriority,
                          TMdaPriorityPreference aPriorityPreference, TInt& aMaxFileBuf,
                          CMVSAppUi* aAppUi); //constructs and launches dialog
     void PreLayoutDynInitL();    //seeds dialog controls

private:
     CMVSAudioSettingDialog(TInt aNoRepeats, 
                          TTimeIntervalMicroSeconds aTrailingSilence,TInt aAudioPriority,
                          TMdaPriorityPreference aPriorityPreference, TInt& aMaxFileBuf,
                          CMVSAppUi* aAppUi); //standard C++ constructor
     TBool OkToExitL(TInt aButtonId); //handles button presses for this dialog
	 TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	 
private:
     CMVSAppUi* iAppUi;
     TInt iAudioPriority; //The audio priority
     TMdaPriorityPreference iPriorityPreference; //The priority preference
	 TInt iNoRepeats;    //The number of repeats
	 TTimeIntervalMicroSeconds iTrailingSilence; //the trailing silence
	 TInt& iMaxFileBuf;
     CEikNumberEditor* iMaxFileSizeCtrl;
     };
#endif  CMVSAUDIOSETTINGDIALOG_H
