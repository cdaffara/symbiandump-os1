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

#ifndef __CFSHARED_H__
#define __CFSHARED_H__

//#include <es_mbman.h> //this is not necessary here
#include <elements/rworkerlock.h>     // RWorkerThread decleration


namespace CommsFW 
//CommsFW namespace doxygen is supplied in cfmsgs.h. The namespace is @publishedPartner, @released
{

/** Maximum length of a module name.
@publishedPartner 
@released
*/
const TInt KCFMaxModuleName = 0x20;

/** Maximum length of a submodule name.
@publishedPartner 
@released
*/
const TInt KCFMaxSubModuleName = 0x20;

/** Maximum number of queues i the channels used for the Comms Framework.
This is not a restriction imposed on private/custom channels created by
the modules.
@publishedPartner 
@released
*/
const TInt KCFMaxChannelQueues = 1; 

/** Name of a Comms Provider Module.
@publishedPartner 
@released
*/
typedef TBuf8<KCFMaxModuleName> TCFModuleName;

/** Folded (case-insensitive) name of a Comms Provider Module,
@publishedPartner 
@released
*/
class TCFModuleNameF : public TCFModuleName
    {
public:
    inline TCFModuleNameF();
    explicit inline TCFModuleNameF(const TDesC8& aModule);
    inline TBool operator==(const TDesC8& aRHS) const;
    inline TBool operator!=(const TDesC8& aRHS) const;
    };


/** Name of a Sub-module in a Comms Provider Module
@publishedPartner
@released
*/
typedef TBuf8<KCFMaxSubModuleName> TCFSubModuleName;

/** Folded (case-insensitive) name of a Sub-module in a Comms Provider Module,
@publishedPartner 
@released
*/
class TCFSubModuleNameF : public TCFSubModuleName
    {
public:
    inline TCFSubModuleNameF();
    explicit inline TCFSubModuleNameF(const TDesC8& aSubModule);
    inline TBool operator==(const TDesC8& aRHS) const;
    inline TBool operator!=(const TDesC8& aRHS) const;
    };

/** Name of a Comms Provider Module and an associated sub-module
in the form "module name:submodule name"
@publishedPartner
@released
*/
typedef TBuf8<KCFMaxModuleName + KCFMaxSubModuleName + 1> TCFFullModuleName;

/** Type of shutdown message
@publishedPartner
@released
*/
enum TCFShutdownType
	{
	/** Shutdown is requested but is not mandatory.
	*/
	EOptional,
	/** Shutdown is requested. It should be done in as graceful a way as possible.
	E.g. if a server it could be deferred to such time when no client are
	connected or connected clients could be cancelled in a controlled manner.
	*/
	EGraceful,
	/** Immediate shutdown is requested. Graceful shutdown is preferred where possible
	but not if it would not be immediate. E.g. a server might decide to delete all sessions.
	*/
	EImmediate,
	/** Module thread is killed by the RootServer.
	*/
	EUnGraceful
	};

/** Types of a message send to a module via RRootServ::SendMessage that are recognised by the
module CWorkerThread instance directly
@publishedPartner
@released since 9.1
*/
enum TCFMessageType
	{
	/** message for factories' instances residing in the module thread
	*/
	EFactoryMsg
	};

/** Comms Framework PANIC
*/
_LIT(KCFSharedPanic, "CommsFW");

#if defined _DEBUG
#define __DEBUG_STMT(a) a
#else
#define __DEBUG_STMT(a)
#endif

class TCFSubModuleAddress
/** Used to uniquely identify a specific submodule (e.g. protocol) within a 
module (e.g. a CPM).
@publishedPartner 
@released
*/
	{
public:
	IMPORT_C TCFSubModuleAddress();
	IMPORT_C TCFSubModuleAddress(const TCFSubModuleAddress& aAddress);
	IMPORT_C TCFSubModuleAddress(const TCFModuleName& aModule, const TCFSubModuleName& aSubModule);
	
	inline const TCFModuleNameF& Module() const;
	inline const TCFSubModuleNameF& SubModule() const;
	IMPORT_C void SetModule(const TCFModuleName& aModule);
	IMPORT_C void SetSubModule(const TCFSubModuleName& aSubModule);
	
	IMPORT_C TBool operator==(const TCFSubModuleAddress& aAddress) const;
	IMPORT_C TBool operator!=(const TCFSubModuleAddress& aAddress) const;
	IMPORT_C TCFSubModuleAddress& operator=(const TCFSubModuleAddress& aAddress);

	IMPORT_C TCFFullModuleName& Printable(TCFFullModuleName& aFullName) const;
private:
	TCFModuleNameF iModule;
	TCFSubModuleNameF iSubModule;
	};

#include "cfshared.inl"

} // namespace CommsFW

#endif // __CFSHARED_H__

