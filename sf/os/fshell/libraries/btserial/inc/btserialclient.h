// btserialclient.h
// 
// Copyright (c) 2008 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

// Transient server example - client interface

#ifndef __BTSERIALCLIENT_H__
#define __BTSERIALCLIENT_H__

#include <e32base.h>
#include <fshell/vtc_controller.h>
#include <bttypes.h>

class RBtSerialSession : public RSessionBase
	{
public:
	IMPORT_C TInt Connect();
	IMPORT_C void Close();
	
	IMPORT_C void WaitForConnection(TRequestStatus& aStatus);
	IMPORT_C void CancelWaitForConnection();
	IMPORT_C TInt IsConnected() const;
	IMPORT_C void Write(const TDesC8& aData, TRequestStatus& aStatus);
	IMPORT_C void CancelWrite();
	IMPORT_C void Read(TDes8& aData, TRequestStatus& aStatus) const;
	IMPORT_C void CancelRead() const;	
	
	IMPORT_C TInt GetConnectedDeviceName(TDes& aName);
	IMPORT_C TInt GetConnectedDeviceAddr(TBTDevAddr& aAddr);
	
private:
	TInt StartServer();
private:
	RThread iSubThread;
	RServer2 iServer;
	};

#endif //__BTSERIALCLIENT_H__
