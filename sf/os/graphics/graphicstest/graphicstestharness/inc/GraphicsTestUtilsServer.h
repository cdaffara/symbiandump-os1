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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/


#ifndef __GRAPHICSTESTUTILSSERVER_H__
#define __GRAPHICSTESTUTILSSERVER_H__

#include <e32base.h>
#include <f32file.h>

enum TTestPanic
	{
	ETestPanicBadDescriptor,
	ETestPanicIllegalFunction,
	ETestPanicAlreadyReceiving,
	ETestPanicAlreadyActive
	};

void PanicClient(RMessage2& aMessage,TTestPanic TMyPanic);

class CShutdown : public CTimer
	{
	enum {KMyShutdownDelay=0x200000};	// approx 2s
public:
	inline CShutdown();
	inline void ConstructL();
	inline void Start();
private:
	void RunL();
	};


class CSmlTestUtilsServer : public CServer2
	{
public:
	static CServer2* NewLC();
	void AddSession();
	void DropSession();
private:
	CSmlTestUtilsServer();
	void ConstructL();
	CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
private:
	TInt iSessionCount;
	CShutdown iShutdown;
	};


class CSmlTestUtilsSession : public CSession2
	{
public:
	CSmlTestUtilsSession();
	void CreateL();
private:
	~CSmlTestUtilsSession();
	inline CSmlTestUtilsServer& Server();
	void ServiceL(const RMessage2& aMessage);
	void ServiceError(const RMessage2& aMessage,TInt aError);
	inline TBool ReceivePending() const;
private:
	TInt DoCreateDirectoryL(const RMessage2& aMessage);
	TInt DoRenameDirectoryL(const RMessage2& aMessage);
	TInt DoDeleteDirectoryL(const RMessage2& aMessage);
	TInt DoCreateFileL(const RMessage2& aMessage);
	TInt DoDeleteFileL(const RMessage2& aMessage);
	TInt DoDeleteFileUsingWildcardL(const RMessage2& aMessage);
	TInt DoCopyFileL(const RMessage2& aMessage);
	TInt DoReplaceFileL(const RMessage2& aMessage);
	TInt DoIsFilePresentL(const RMessage2& aMessage);
	TInt DoSetReadOnlyL(const RMessage2& aMessage);
	TInt DoGetAttL( const RMessage2& aMessage );
	TInt DoSetAttL( const RMessage2& aMessage );	
	TInt DoCopyDirectoryL(const RMessage2& aMessage);
	TInt DoChangeFilePermissionL(const RMessage2& aMessage);

private:
	RFs 				iFs;
	CFileMan* 			iFileMan; 
	};

#endif //__GRAPHICSTESTUTILSSERVER_H__
