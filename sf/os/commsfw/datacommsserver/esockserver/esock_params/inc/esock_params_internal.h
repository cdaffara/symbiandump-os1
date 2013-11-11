/**
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file esock_params_internal.h
 @internalTechnology
*/

#ifndef ESOCK_PARAMS_INTERNAL_H
#define ESOCK_PARAMS_INTERNAL_H

#include <e32base.h>
#include <e32std.h>	

#include <comms-infras/metadata.h>
#include <es_prot.h>

#include <elements/sd_std.h>

class TConnCSRPref : public Meta::SMetaDataECom
	{
public:
	enum
		{		
		EUid = 0x10285E27,
		ETypeId = 1
		};

	//Each of the flags may be set, alone, all or in any combinations
	enum TSelectionScope
		{
		ENone               = 0x00, //For initialisation only
		ESelectFromExisting = 0x01, //If not set, means the opposite (select even if does not exist, create).
		ESelectWholePlane   = 0x02, //If not set, means the opposite (select only the top provider).
		ERequestCommsBinder = 0x04  //If not set, means the opposite (do not request comms binder).
		};

    enum TSelectionFlags
        {
        EMonitor = 0x01
        };

public:
	inline static TConnCSRPref* NewL();
	
	inline TUint Scope() const;
    inline void SetScope(TUint aScope);
    inline TUint Flags() const;
    inline void SetFlags(TUint aFlags);
	inline Den::TSubSessionUniqueId SubSessionUniqueId() const;
	inline void SetSubSessionUniqueId(Den::TSubSessionUniqueId aSubSessionUniqueId);

    DATA_VTABLE

private:
	TUint16 iScope;
	TUint16 iFlags;
	Den::TSubSessionUniqueId iSubSessionUniqueId;
    };

#include <comms-infras/esock_params_internal.inl>


#endif //ESOCK_PARAMS_INTERNAL_H

