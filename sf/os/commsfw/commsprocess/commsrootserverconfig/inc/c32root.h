// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__C32ROOT_H__)
#define __C32ROOT_H__

/**
@file

@publishedPartner
@released
*/

#include <rsshared.h>
using namespace RootServer; // Allowing the usage of definitions from the RS
using namespace CommsFW; // Allowing the usage of definitions from the Comms Framework

class RRootServ : public RSessionBase
/** The client interface to the Rootserver. Allows e.g. the Comms Reference Configurator
application to configure the Comms Process with desired modules and bindings.
@publishedPartner
@released
*/
    {
    public:
    IMPORT_C TInt Connect();

    IMPORT_C TVersion Version() const;
    IMPORT_C TBool IsCallerConfigurationAuthority() const;    

    IMPORT_C TInt SetMBufPoolSize(TUint aPoolSize);
    IMPORT_C TInt SetMBufPoolSize(TUint aInitPoolSize, TUint aMaxPoolSize);
    IMPORT_C TInt AddMBufAllocInfo(const TRSAddMBufAllocInfoContainer& aMBufSizeAllocInfo);

    IMPORT_C void LoadCpm(TRequestStatus& aStatus, const TRSStartModuleParams& aParams, const TDesC8& aIniData);
    IMPORT_C void UnloadCpm(TRequestStatus& aStatus, const TCFModuleName& aName, TRSUnLoadType aType);
    IMPORT_C void Bind(TRequestStatus& aStatus, TRSBindingInfo& aBindInfo);
    IMPORT_C void Unbind(TRequestStatus& aStatus, TRSUnBindingInfo& aUnBindInfo);

   	IMPORT_C TInt GetModuleInfo(const TCFModuleName& aName, TRSModuleInfo& aModuleInfo);
   	IMPORT_C TInt EnumerateModules(TRSIter &aPosition, TCFModuleName& aModuleName);
   	IMPORT_C TInt EnumerateSubModules(const TCFModuleName& aModuleName, TRSIter &aPosition, TCFSubModuleName& aSubModuleName);
   	IMPORT_C TInt EnumerateBindings(const TCFSubModuleAddress& aSubModuleAddr, TRSIter &aPosition, TRSBindingInfo& aBinding);

    IMPORT_C void CancelLoadCpm(const TCFModuleName& aName);
    IMPORT_C void CancelUnloadCpm(const TCFModuleName& aName);
    IMPORT_C void CancelBind(TRSSubModuleAddress& aName1, TRSSubModuleAddress& aName2);
    IMPORT_C void CancelUnbind(TRSSubModuleAddress& aName1, TRSSubModuleAddress& aName2);

    IMPORT_C void SendMessage(TRequestStatus& aStatus, const TCFModuleName& aName, TInt aType, TDes8& aData);
    IMPORT_C TInt SendMessage(const TCFModuleName& aName, TInt aType, TDes8& aData);

    IMPORT_C TInt Shutdown();

	IMPORT_C void Close();

	IMPORT_C TInt __DbgMarkHeap();
	IMPORT_C TInt __DbgCheckHeap(TInt aCount);
	IMPORT_C TInt __DbgMarkEnd(TInt aCount);
	IMPORT_C TInt __DbgFailNext(TInt aCount);
	IMPORT_C TInt __DbgFailNextMbuf(TInt aCount);
	IMPORT_C TInt __DbgSetMbufPoolLimit(TInt asize);
	IMPORT_C TInt __DbgCheckMbuf(TInt asize);
	IMPORT_C TInt __DbgMbufFreeSpace();
	IMPORT_C TInt __DbgMbufTotalSpace();

    };

#endif // __C32ROOT_H__

