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

#ifndef MVSSELECTPLUGINSDIALOG_H
#define MVSSELECTPLUGINSDIALOG_H

#include <techview/eikdialg.h>
#include "MVSAppUI.h"

const TInt KAllSupportedExtBufLength = 32;

class CMVSAppUI; //Forward class declaration

class CMVSSelectPluginsDialog : public CEikDialog
	{
public:
    static void SetupDialogLD(CDesCArrayFlat* aPluginArray,
	                          CDesCArrayFlat* aExtArray, CDesCArrayFlat* aCodecArray,
                              CMVSAppUi* aAppUi);
    void PreLayoutDynInitL();    //seeds dialog controls
	void HandleControlEventL(CCoeControl *aControl, TCoeEvent aEventType);
	~CMVSSelectPluginsDialog();
	TKeyResponse OfferKeyEventL(const TKeyEvent& /*aKeyEvent*/,TEventCode /*aType*/);
private:
    void ConstructL(CDesCArrayFlat* iCodecArray);
    CMVSSelectPluginsDialog(CDesCArrayFlat* aPluginArray,
	                        CDesCArrayFlat* aExtArray, CDesCArrayFlat* aCodecArray, 
                            CMVSAppUi* aAppUi);
    TBool OkToExitL(TInt aButtonId); //handles button presses for this dialog
    
private:
    CDesCArrayFlat* iPluginArray; //The array of plugins
    CDesCArrayFlat* iExtArray; //The array of extensions
    CDesCArrayFlat* iCodecArray;
    TBuf<KAllSupportedExtBufLength> iExt; //The file extension
    CMVSAppUi* iAppUi; 
    CEikChoiceList* iPluginCtl; 
    CEikChoiceList* iExtCtl; 
	TBool iMediaType;//Used to update whether the user is in the Audio/Video mode,gets activated
	                 //when the respective radiobutton is selected.
	};

#endif MVSSELECTPLUGINSDIALOG_H
