// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @internalComponent
*/

#ifndef HCISERVERCLIENT_H
#define HCISERVERCLIENT_H

#include <es_sock.h>
#include <e32base.h>

/**
Class to be used to transfer requests from a Bluetooth client
to the HCI using the client server mechanism to bypass Esock and the
Bluetooth stack.
*/
class RHCIServerSession : public RSessionBase
	{
public:
	IMPORT_C RHCIServerSession();

	IMPORT_C TInt Open(TInt aInterfaceUid);
	IMPORT_C TInt SendSync(TUint aCommand);
	IMPORT_C void SendAsync(TUint aCommand, TRequestStatus &aStatus);
	IMPORT_C TInt SendSync(TUint aCommand, TDes8* aDescIn, TDes8* aDescOut, TUint aValue);
	IMPORT_C void SendAsync(TUint aCommand, TDes8* aDescIn, TDes8* aDescOut, TUint aValue, TRequestStatus &aStatus);
	IMPORT_C void Close();

private:
	void OpenL(TInt aInterfaceUid);

private:
	RSocketServ	iSocketServer;
	};

#endif // HCISERVERCLIENT_H											   

