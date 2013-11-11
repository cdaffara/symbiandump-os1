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


#ifndef MVSAPP_H
#define MVSAPP_H


#include <eikenv.h>
#include <techview/eikmenup.h>
#include <eikdoc.h>
#include <eikapp.h>
#include <eikappui.h>
#include <coecntrl.h>
#include <coeccntx.h>
#include <techview/eikprogi.h>
#include <techview/eikchlst.h>
#include <e32base.h>
#include <techview/eikmfne.h>
#include <techview/eikon.hrh>
#include <mvsapp.rsg>

class CMVSAppDocument : public CEikDocument
     {
public:
     CMVSAppDocument (CEikApplication& aApp);
private:
     CEikAppUi* CreateAppUiL();
     };

#endif MVSAPP_H
