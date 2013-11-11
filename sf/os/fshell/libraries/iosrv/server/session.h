// session.h
// 
// Copyright (c) 2006 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __SESSION_H__
#define __SESSION_H__

#include <e32base.h>
#include "readwrite.h"

class CIoServer;

#ifdef EKA2
class CIoSession : public CSession2
#else
class CIoSession : public CSharableSession
#endif
	{
public:
	CIoSession();
#ifdef EKA2
	void CreateL();
#else
	void CreateL(const CServer& aServer);
#endif
private:
	~CIoSession();
	CIoServer& Server();
	void ServiceL(const RMsg& aMessage);
	void CreateHandleL(CObject& aObject, TBool aDoPop, const RMsg& aMessage);
	CIoObject& FindObjectL(TInt aHandle);
	CIoEndPoint& FindEndPointL(TInt aHandle, const RMsg& aMessage);
	CIoPipe& FindPipeL(TInt aHandle, const RMsg& aMessage);
	CIoConsole& FindConsoleL(TInt aHandle, const RMsg& aMessage);
	CIoPersistentConsole& FindPersistentConsoleL(TInt aHandle, const RMsg& aMessage);
	CIoReadWriteObject& FindReadWriteObjectL(TInt aHandle, const RMsg& aMessage);
	CIoReadObject& FindReadObjectL(TInt aHandle, const RMsg& aMessage);
	CIoWriteObject& FindWriteObjectL(TInt aHandle, const RMsg& aMessage);
	void Complete(const RMsg& aMessage, TInt aError);
	
private:
	CObjectIx* iHandles;
#ifdef IOSRV_LOGGING
	TName iName;
#endif
	RIoHandle::TType iFindByNameType;
	TInt iFindByNameHandle;
	HBufC* iFindByNameMatch;
	};

#endif // __SESSION_H__
