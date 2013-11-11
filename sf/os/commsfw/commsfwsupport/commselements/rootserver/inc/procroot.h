// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__PROCROOT_H__)
#define __PROCROOT_H__

/**
@file

@internalTechnology
@released
*/

#include <rscommon.h>

class RProcessRoot : public RSessionBase
/** The client interface to the Process Rootserver. Allows e.g. the Process Reference Configurator 
application to configure the Process with desired modules, bindings and 
later resources (memory,cpu,...).
@internalTechnology 
@released
*/
    {
    friend class RProcessRootDbg;
    
public:
    IMPORT_C TInt Connect(const TDesC& aServerName);

    IMPORT_C TVersion Version() const;

    IMPORT_C void LoadModule(TRequestStatus& aStatus, const RootServer::TRSStartModuleParams& aParams, const TDesC8& aIniData);
    IMPORT_C void UnloadModule(TRequestStatus& aStatus, const CommsFW::TCFModuleName& aName, RootServer::TRSUnLoadType aType);
    IMPORT_C void Bind(TRequestStatus& aStatus, RootServer::TRSBindingInfo& aBindInfo);
    IMPORT_C void Unbind(TRequestStatus& aStatus, RootServer::TRSUnBindingInfo& aUnBindInfo);
    
   	IMPORT_C TInt GetModuleInfo(const CommsFW::TCFModuleName& aName, RootServer::TRSModuleInfo& aModuleInfo);
   	IMPORT_C TInt EnumerateModules(RootServer::TRSIter &aPosition, CommsFW::TCFModuleName& aModuleName);
   	IMPORT_C TInt EnumerateSubModules(const CommsFW::TCFModuleName& aModuleName, RootServer::TRSIter &aPosition, CommsFW::TCFSubModuleName& aSubModuleName);
   	IMPORT_C TInt EnumerateBindings(const CommsFW::TCFSubModuleAddress& aSubModuleAddr, RootServer::TRSIter &aPosition, RootServer::TRSBindingInfo& aBinding);

    IMPORT_C void CancelLoadModule(const CommsFW::TCFModuleName& aName);
    IMPORT_C void CancelUnloadModule(const CommsFW::TCFModuleName& aName);
    IMPORT_C void CancelBind(RootServer::TRSSubModuleAddress& aName1, RootServer::TRSSubModuleAddress& aName2);
    IMPORT_C void CancelUnbind(RootServer::TRSSubModuleAddress& aName1, RootServer::TRSSubModuleAddress& aName2);

    IMPORT_C void SendMessage(TRequestStatus& aStatus, const CommsFW::TCFModuleName& aName, TInt aType, TDes8& aData);
    IMPORT_C TInt SendMessage(const CommsFW::TCFModuleName& aName, TInt aType, TDes8& aData);

    IMPORT_C TInt Shutdown();

	IMPORT_C void Close();

private:
    inline TInt SendReceive(TInt aFunction,const TIpcArgs& aArgs) const
    { return RSessionBase::SendReceive( aFunction, aArgs );}
        
    };

class RProcessRootDbg
    {
/** Process root debugging capabilities.
@internalTechnology 
@released
*/
    IMPORT_C void Open(RProcessRoot& aProcessRoot);
    
	IMPORT_C TInt __DbgMarkHeap();
	IMPORT_C TInt __DbgCheckHeap(TInt aCount);
	IMPORT_C TInt __DbgMarkEnd(TInt aCount);
	IMPORT_C TInt __DbgFailNext(TInt aCount);

	IMPORT_C void Close();
	
private:
    RProcessRoot& iProcessRoot;
    };

#endif // __PROCROOT_H__

