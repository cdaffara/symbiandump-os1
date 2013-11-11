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


#if (!defined __T_BLUETOOTH_SOCKET_NOTIFIER_H__)
#define __T_BLUETOOTH_SOCKET_NOTIFIER_H__

//	EPOC include
#include <bt_sock.h>


class CT_BluetoothSocketNotifier : public CBase, public MBluetoothSocketNotifier
	{
public:
	//	Constructor/Destructor
	CT_BluetoothSocketNotifier();
	~CT_BluetoothSocketNotifier();

	void			SetServer(MBluetoothSocketNotifier* aServer);

	//	MBluetoothSocketNotifier implementation
	virtual void	HandleConnectCompleteL(TInt aErr);
	virtual void	HandleAcceptCompleteL(TInt aErr);
	virtual void	HandleShutdownCompleteL(TInt aErr);
	virtual void	HandleSendCompleteL(TInt aErr);
	virtual void	HandleReceiveCompleteL(TInt aErr);
	virtual void	HandleIoctlCompleteL(TInt aErr);
	virtual void	HandleActivateBasebandEventNotifierCompleteL(TInt aErr, TBTBasebandEventNotification& aEventNotification);

public:
	CBluetoothSocket*			iBluetoothSocket;
	RSocketServ					iSocketServer;
	TBool						iSocketServerConnected;
	RConnection					iConnection;

private:
	MBluetoothSocketNotifier*	iServer;
	};

#endif /* __T_BLUETOOTH_SOCKET_NOTIFIER_H__ */
