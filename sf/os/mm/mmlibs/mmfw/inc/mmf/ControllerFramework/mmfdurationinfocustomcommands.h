// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __MMF_DURATION_INFO_CUSTOM_COMMANDS_H__
#define __MMF_DURATION_INFO_CUSTOM_COMMANDS_H__

#ifdef SYMBIAN_CAF_V2

#include <e32base.h>
#include <mmf/common/mmcaf.h>
#include <mmf/common/mmfstandardcustomcommands.h>


/**
@publishedAll
@deprecated 9.2- Replaced by KUidInterfaceMMFDurationInfoControl2
*/

const TUid KUidInterfaceMMFDurationInfoControl = {0x102737C8};

/**
@publishedAll
@released

The duration state of the controller.
*/
enum TMMFDurationInfo
	{
	EMMFDurationInfoValid, 		// Duration is known
	EMMFDurationInfoUnknown,	// Invalid response from controller
	EMMFDurationInfoInfinite,	// Clip is of infinite length
	};

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfdurationinfocustomcommandsimpl.h>
#include <mmf/common/mmfdurationinfocustomcommandsenums.h>
#endif

/**
@publishedAll
@deprecated 9.2 - Replaced by MMMFDurationInfoCustomCommandImplementor2

Mixin class to be derived from by controller plugins wishing to support the Duration information 
custom commands
*/
class MMMFDurationInfoCustomCommandImplementor
	{
public:

	/**
	Is the duration of the clip known? i.e. is the stream infinite?
	
	@param aDurationInfo
		   Controller duration information
		   
	@return TMMFDurationInfo Duration state.
	*/
	virtual TMMFDurationInfo GetDurationInfo(TMMFDurationInfo& aDurationInfo)=0;
	};

/**
@publishedAll
@deprecated 9.2 - Replaced by CMMFDurationInfoCustomCommandParser2
*/
class CMMFDurationInfoCustomCommandParser : public CMMFCustomCommandParserBase
	{
public:

	/**
	Creates a new custom command parser capable of processing duration information controller commands.

	@param  aImplementor
	        A reference to the controller plugin that owns this new object.

	@return A pointer to the object created.

	@since  9.1
	*/
	IMPORT_C static CMMFDurationInfoCustomCommandParser* NewL(MMMFDurationInfoCustomCommandImplementor& aImplementor);

	/**
	Destructor.

	@since  9.1
	*/
	IMPORT_C ~CMMFDurationInfoCustomCommandParser();

	/**
	Handles a request from the client. Called by the controller framework.

	@param  aMessage
	        The message to be handled.

	@since  9.1
	*/
	void HandleRequest(TMMFMessage& aMessage);

private:
	/**
	Constructor.

	@param  aImplementor
	        A reference to the controller plugin that owns this new object.
	@since  9.1
	*/
	CMMFDurationInfoCustomCommandParser(MMMFDurationInfoCustomCommandImplementor& aImplementor);
	
	/**
	Request duration information from the implementor

	@param  aMessage
	        Message for writing results to the client.
	*/
	TInt GetDurationInfo(TMMFMessage& aMessage);

private:

	MMMFDurationInfoCustomCommandImplementor& iImplementor;
	
	};

#endif // SYMBIAN_CAF_V2

#endif // __MMF_DURATION_INFO_CUSTOM_COMMANDS_H__

