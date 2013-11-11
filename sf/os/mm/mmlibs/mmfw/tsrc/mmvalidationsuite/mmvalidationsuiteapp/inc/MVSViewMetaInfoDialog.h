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

#ifndef MVSVIEWMETAINFODIALOG_H
#define MVSVIEWMETAINFODIALOG_H

#include "MVSAppUI.h"

class CMVSAppUI;

class CMVSViewMetaInfoDialog : public CEikDialog
	{
public:
    static void SetupDialogLD(RPointerArray<CMMFMetaDataEntry> aMetaArray, CMVSAppUi* aAppUi);
    void PreLayoutDynInitL();    //seeds dialog controlsivate:

private:
    CMVSViewMetaInfoDialog(CMVSAppUi* aAppUi);
    TBool OkToExitL(TInt aButtonId); //handles button presses for this dialog
	void ConstructL(RPointerArray<CMMFMetaDataEntry> aMetaArray);
private:
    RPointerArray<CMMFMetaDataEntry> iMetaArray; //array of meta-data entries
    CEikProgressInfo* iProgInfo;  //pointer to the progress bar for this dlg.
	
	//values used to remember the index of the entry in any existing
    //meta data that corresponds to the editable fields in the dialog
    TInt iTitlePos; 
    TInt iAuthorPos;
    TInt iCopyrightPos;
    TInt iRevisionPos;
    TInt iCategoryPos;
    TInt iCommentsPos;
    
    CMVSAppUi* iAppUi;
    };

#endif MVSVIEWMETAINFODIALOG_H
