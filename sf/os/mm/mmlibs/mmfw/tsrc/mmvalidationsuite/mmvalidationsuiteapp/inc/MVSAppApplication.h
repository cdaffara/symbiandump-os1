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


#ifndef MVSAPPAPPLICATION_H
#define MVSAPPAPPLICATION_H

#include "MVSApp.hrh"

class CMVSAppApplication : public CEikApplication 
	{
private:
    CApaDocument* CreateDocumentL();
    TUid AppDllUid() const;
    };

#endif MVSAPPAPPLICATION_H
