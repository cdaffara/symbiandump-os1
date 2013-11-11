/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Backend for the Qt app. Starts the Remcon observer.
*
*/

#include <tspclientmapper.h>
#include "audiostuborbit.h"
#include "audioclientstubremconobserver.h"

audiostuborbit::audiostuborbit(QWidget *parent)
    : HbMainWindow(parent)
{
//	ui.setupUi(this);
	iTspClientMapper = CTspClientMapper::NewL();
	iTspClientMapper->SetTspTargetClient( CTspClientMapper::EPlayingClients, RProcess().Id() );
	iRemConObserver = CAudioClientStubRemConObserver::NewL();
}

audiostuborbit::~audiostuborbit()
{
    if ( iTspClientMapper )
        {
        iTspClientMapper->RemoveTspTargetClient( CTspClientMapper::EPlayingClients, RProcess().Id() );
        }
    delete iTspClientMapper;
    delete iRemConObserver;
}
