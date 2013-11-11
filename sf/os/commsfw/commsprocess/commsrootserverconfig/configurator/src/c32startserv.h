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

#if !defined(__C32STARTSERV_H__)
#define __C32STARTSERV_H__

#include <e32base.h>
#include "c32start.h"


class CRsConfiguratorServer : public CPolicyServer
/** The main class implementing the Configurator server.
@internalComponent
*/
	{
	public:
	CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;

	public :
	
	static CRsConfiguratorServer* NewLC(CCommsProcessStarter& aCommsProcess);
	~CRsConfiguratorServer();
	static CRsConfiguratorServer* NewL(CCommsProcessStarter& aCommsProcess);

	TInt EnumerateModules(const TCFGroupNameF& aGroupName, TInt& aStartAt, TCFModuleNameF& aName);
	TInt LoadCpm(const TCFModuleNameF& aName,const RMessage2& aMessage);
	TInt GetModuleIniData(const TCFModuleNameF& aName, TDes8& aIniData,TInt& aActualSize);//to be deleted
	TInt GetModuleIniData(const TCFModuleNameF& aName, const RMessage2& aMessage);
	TInt UnLoadCpm(const TCFModuleNameF& aName, const RMessage2& aMessage);
	TInt CancelLoadCpm(const TCFModuleNameF& aName);
	TInt CancelUnLoadCpm(const TCFModuleNameF& aName);

	private:
	RPointerArray<CModuleRef>& GetModulesList() // Get the modules listed.
		{
		return iCommsProcess.GetModulesList();
		};

	protected:
	CRsConfiguratorServer(CCommsProcessStarter& aCommsProcess);
	void ConstructL();
	static const CPolicyServer::TPolicy Policy;

	private:
	CModuleRef* FindCpm(const TCFModuleNameF& aName);
	CCommsProcessStarter& iCommsProcess;
	static const TUint iPolicyRangeCount;
	static const TInt iPolicyRanges[];
	static const TUint8 iPolicyElementsIndex[];
	static const CPolicyServer::TPolicyElement iPolicyElements[];
	static const CPolicyServer::TPolicy iPolicy;
	// Debug build logging
	__FLOG_DECLARATION_MEMBER;
	};

class CRsConfiguratorServerSession : public CSession2
/** Session object as required by the client/server framework. 
Implements the "Secure IPC" API for configurator server.
@internalComponent 
*/
	{
	public:
	CRsConfiguratorServerSession(const CRsConfiguratorServer* aConfiguratorServer);

	  //service request
	void ServiceL(const RMessage2& aMessage);

	 //services available 
	private:
	TInt LoadCpm(const RMessage2& aMessage);
	TInt UnloadCpm(const RMessage2& aMessage);
	TInt CancelLoadCpm(const RMessage2& aMessage);
	TInt CancelUnloadCpm(const RMessage2& aMessage);
	TInt EnumerateModules(const RMessage2& aMessage);
	TInt GetModuleIniData(const RMessage2& aMessage);
	
	private:
	CRsConfiguratorServer* iConfiguratorServer;
	// Debug build logging
	__FLOG_DECLARATION_MEMBER;
	
	};


#endif

