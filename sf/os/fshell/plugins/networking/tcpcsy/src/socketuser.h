// socketuser.h
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

#ifndef __SOCKETUSER_H__
#define __SOCKETUSER_H__

#include <e32std.h>
#include <e32base.h>
#include <es_sock.h>


class CSocketUser : public CActive
	{
public:
	void SocketConnected();
protected:
	CSocketUser(RSocket& aSocket);
	~CSocketUser();
	TBool Connected() const;
	virtual void HandleSocketConnected() = 0;
protected:
	RSocket& iSocket;
private:
	TBool iConnected;
	};


#endif // __SOCKETUSER_H__

