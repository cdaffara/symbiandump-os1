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

#ifndef MVSEDITMETAINFODIALOG_H
#define MVSEDITMETAINFODIALOG_H

#include <techview/eikdialg.h>
#include "MVSApp.h"
#include "MVSAppUI.h"



class CMVSAppUI;

class CMVSEditMetaInfoDialog : public CEikDialog
	{
public:
    static void SetupDialogLD(RPointerArray<CMMFMetaDataEntry> aMetaArray,
                              CMVSAppUi* aAppUi);
    ~CMVSEditMetaInfoDialog();
private:
    CMVSEditMetaInfoDialog(CMVSAppUi* aAppUi);
    TBool OkToExitL(TInt aButtonId); //handles button presses for this dialog
    void ConstructL(RPointerArray<CMMFMetaDataEntry> aMetaArray);

private:
    RPointerArray<CMMFMetaDataEntry> iMetaArray; //array of meta data
	
	//values used to remember the index of the entry in any existing
    //meta data that corresponds to the editable fields in the dialog
    TInt iTitlePos; 
    TInt iAuthorPos;
    TInt iCopyrightPos;
    TInt iRevisionPos;
    TInt iCategoryPos;
    TInt iCommentsPos;
	
	CEikProgressInfo* iProgInfo; //a pointer to the progress bar for the dlg.
    CMVSAppUi* iAppUi;
    };

#endif MVSEDITMETAINFODIALOG_H
