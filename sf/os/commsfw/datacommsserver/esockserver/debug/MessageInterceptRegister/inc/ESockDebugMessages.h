/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file ESockDebugMessages.h
*/

#ifndef __ESOCKDEBUG_MESSAGES_H__
#define __ESOCKDEBUG_MESSAGES_H__

#include <comms-infras/metavirtctor.h>

class CSockSession;

namespace ESockDebug
	{
	const TInt KMaxIpcMsgLength = 256;

	// Registration of message interfaces
	//-----------------------------------
	class TMessageTable
		{
		public:
			IMPORT_C static void RegisterL();
			IMPORT_C static void DeRegister();
		};

	// Base message
	class TControlMsg : public Meta::SMetaDataNetCtor
		{
		public:
			IMPORT_C virtual TInt DispatchL(CSockSession* aHostSession) = 0;
		};
	}

namespace ESockDebug
	{
	namespace MemoryDebug
		{
		// TODO_CDG not supported yet, place holder for extending with replacement for legacy debug behaviour
		// Implementation interface ID for ESockDebug::MemoryDebug messages
		static const TInt KImplementationUid = 0xDEADDEAD;

		// Base
		class TMemoryDebugControlMsg : public TControlMsg
			{
			};

		class TCheckFailNextMsg : public TMemoryDebugControlMsg
			{
			public:	
				IMPORT_C virtual TInt DispatchL(CSockSession* aHostSession);
				DECLARE_MVIP_CTR(TCheckFailNextMsg)
				EXPORT_DATA_VTABLE_AND_FN
			};
		}
	}

#endif // __ESOCKDEBUG_MESSAGES_H__

