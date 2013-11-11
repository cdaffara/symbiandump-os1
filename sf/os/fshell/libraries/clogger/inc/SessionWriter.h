// SessionWriter.h
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

#ifndef SESSIONWRITER_H
#define SESSIONWRITER_H

#include <e32base.h>

class CSessionWriter;
class CSessionWriterServer;
class CCloggerServer;

class CSessionWriterSession : public CSession2
	{
public:
	TBool WaitingForClient();
	void WriteBuffer(const TDesC8& aBuf);

private:
	void ServiceL(const RMessage2 &aMessage);
	CSessionWriterServer& Server();
	~CSessionWriterSession();

private:
	TAny* iClientSharedChunkBase;
	RMessagePtr2 iSessionWriterMessage;
	CSessionWriter* iSessionWriter;
	TBool iWaitingForClient;
	};

class CSessionWriterServer : public CServer2
	{
public:
	CSessionWriterServer(CCloggerServer& aCloggerServer);
	void ConstructL();
	CCloggerServer& CloggerServer() { return iCloggerServer; }

private:
	CSession2* NewSessionL(const TVersion &aVersion, const RMessage2 &aMessage) const;

private:
	CCloggerServer& iCloggerServer;
	};

#endif
