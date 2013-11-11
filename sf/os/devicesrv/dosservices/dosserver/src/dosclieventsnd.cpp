/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
*    Implementation for the RDosEventSnd class.
*
*/


#include "doseventsender.h"
#include "dosclientserver.h"

//
// ---------------------------------------------------------
// RDosEventSnc::Register
// ---------------------------------------------------------
//  	
TInt RDosEventSnd::Register(RDosServer& aServer)
{
	return CreateSubSession(aServer,EDosCreateEventSndSubSession, TIpcArgs(TIpcArgs::ENothing, TIpcArgs::ENothing, KAutoComplete));
}

//Implementation of the events

/****************************************
	MESSAGE SCTRUCTURE FOR EVENTS

	0 -> Event name
	1 -> Pointer to the data
	2 -> Length of the data
	3 -> Reserved for Client/Server Comms
****************************************/

//
// ---------------------------------------------------------
// RDosEventSnd::SendEvent
// ---------------------------------------------------------
// 
TInt RDosEventSnd::SendEvent(TIpcArgs &aParams)
{
	return SendReceive(EDosEventFiring, aParams);
}
