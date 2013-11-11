/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:
*
*/

#include "T_BluetoothSocketNotifier.h"

CT_BluetoothSocketNotifier::CT_BluetoothSocketNotifier()
:	CBase()
,	iBluetoothSocket(NULL)
,	iSocketServerConnected(EFalse)
,	iServer(NULL)
	{
	}

CT_BluetoothSocketNotifier::~CT_BluetoothSocketNotifier()
	{
	delete iBluetoothSocket;
	iBluetoothSocket=NULL;
	}

void CT_BluetoothSocketNotifier::SetServer(MBluetoothSocketNotifier* aServer)
	{
	iServer=aServer;
	}

void CT_BluetoothSocketNotifier::HandleConnectCompleteL(TInt aErr)
	{
	iServer->HandleConnectCompleteL(aErr);
	}

void CT_BluetoothSocketNotifier::HandleAcceptCompleteL(TInt aErr)
	{
	iServer->HandleAcceptCompleteL(aErr);
	}


void CT_BluetoothSocketNotifier::HandleShutdownCompleteL(TInt aErr)
	{
	iServer->HandleShutdownCompleteL(aErr);
	}


void CT_BluetoothSocketNotifier::HandleSendCompleteL(TInt aErr)
	{
	iServer->HandleSendCompleteL(aErr);
	}


void CT_BluetoothSocketNotifier::HandleReceiveCompleteL(TInt aErr)
	{
	iServer->HandleReceiveCompleteL(aErr);
	}

void CT_BluetoothSocketNotifier::HandleIoctlCompleteL(TInt aErr)
	{
	iServer->HandleIoctlCompleteL(aErr);
	}


void CT_BluetoothSocketNotifier::HandleActivateBasebandEventNotifierCompleteL(TInt aErr, TBTBasebandEventNotification& aEventNotification)
	{
	iServer->HandleActivateBasebandEventNotifierCompleteL(aErr, aEventNotification);
	}
