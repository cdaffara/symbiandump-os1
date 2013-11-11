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
 @internalComponent
*/
#if !defined(__C32STARTSHARED_H__)
#define __C32STARTSHARED_H__

#include <e32base.h>
#include <rsshared.h>

_LIT(KRsConfiguratorServerName,"!C32ConfiguratorServer");

using namespace RootServer;

/** Name of a group of Comms Provider Modules.
@internalTechnology 
*/
typedef TBuf8<CommsFW::KCFMaxModuleName> TCFGroupName;

/** Folded (case-insensitive) name of a group of Comms Provider Modules.
@internalTechnology
*/
class TCFGroupNameF : public TCFGroupName
    {
public:
    inline TCFGroupNameF()
        {
        }
    explicit inline TCFGroupNameF(const TDesC8& aGroup)
    : TCFGroupName(aGroup)
        {
        }
    inline TBool operator==(const TDesC8& aRHS) const
        {
        return CompareF(aRHS) == 0;
        }
    inline TBool operator!=(const TDesC8& aRHS) const
        {
        return CompareF(aRHS) != 0;
        }
    };


/**Configurator server IPC numbers/messages
@internalComponent
*/
enum
	{
	CSEnumerateModules=1,
	CSGetModuleIniData,
	CSLoadCpm,
	CSCancelLoadCpm,
	CSUnloadCpm,
	CSCancelUnloadCpm	
	};

enum TConfiguratorServerPanic
	{
	/** Failed while starting configurator server
	 */
	EPanicConfigServer
	};
	
	

NONSHARABLE_CLASS(TConfiguratorModuleParams) : public RootServer::TRSStartModuleParamContainer
	/** Container for necessary parameters to start a CPM.
	@internalTechnology 
	*/
	{
	public:
	/**Specifies this module is for On Demand loading.
	*/	
	TBool iOnDemand;	
	/** Specifies group for a module.
	* @see TCFGroupName
	*/
	TCFGroupName iGroupName;
	};
#endif

