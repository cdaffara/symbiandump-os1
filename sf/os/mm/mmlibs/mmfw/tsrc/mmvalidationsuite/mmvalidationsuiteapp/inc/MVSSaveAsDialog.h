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

#ifndef MVSSAVEASDIALOG_H
#define MVSSAVEASDIALOG_H

#include <techview/eikcfdlg.h>

class CMVSSaveAsDialog : public CEikFileSaveAsDialog
	{
public:
    static TInt SetupDialogLD(TDes& aFilename, TBool* aReplace); //Setup method, initializes dialog

protected:
	TBool OkToExitL(TInt aButtonId);

private:
    CMVSSaveAsDialog(TDes& aFilename, TBool* aReplace);

private:
    TDes& iFilename;   //Filename
    TBool* iReplace;
    };

#endif MVSSAVEASDIALOG_H
