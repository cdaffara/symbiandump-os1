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

#ifndef __MMF_DURATION_INFO_CUSTOM_COMMANDS_IMPL_H__
#define __MMF_DURATION_INFO_CUSTOM_COMMANDS_IMPL_H__

#ifdef SYMBIAN_CAF_V2

/**
@publishedPartner
@released
*/

const TUid KUidInterfaceMMFDurationInfoControl2 = {0x10273812};
	
/**
@publishedPartner
@released

Mixin class to be derived from by controller plugins wishing to support the Duration information 
custom commands
*/
class MMMFDurationInfoCustomCommandImplementor2
	{
public:

	/**
	Is the duration of the clip known? i.e. is the stream infinite?
	
	@param aDurationInfo
		   Controller duration information
		   
	@return KErrNone or System wide error code
	*/
	virtual TInt GetDurationInfo(TMMFDurationInfo& aDurationInfo)=0;
	};
	
/**
@publishedPartner
@released

Custom command parser capable of processing duration information controller commands
*/
class CMMFDurationInfoCustomCommandParser2 : public CMMFCustomCommandParserBase
	{
public:

	/**
	Creates a new custom command parser capable of processing duration information controller commands.

	@param  aImplementor
	        A reference to the controller plugin that owns this new object.

	@return A pointer to the object created.

	@since  9.2
	*/
	IMPORT_C static CMMFDurationInfoCustomCommandParser2* NewL(MMMFDurationInfoCustomCommandImplementor2& aImplementor);

	/**
	Destructor.

	@since  9.2
	*/
	IMPORT_C ~CMMFDurationInfoCustomCommandParser2();

	/**
	Handles a request from the client. Called by the controller framework.

	@param  aMessage
	        The message to be handled.

	@since  9.2
	*/
	void HandleRequest(TMMFMessage& aMessage);

private:
	/**
	Constructor.

	@param  aImplementor
	        A reference to the controller plugin that owns this new object.
	@since  9.2
	*/
	CMMFDurationInfoCustomCommandParser2(MMMFDurationInfoCustomCommandImplementor2& aImplementor);
	
	/**
	Request duration information from the implementor

	@param  aMessage
	        Message for writing results to the client.
	*/
	TInt GetDurationInfo(TMMFMessage& aMessage);

private:
	MMMFDurationInfoCustomCommandImplementor2& iImplementor;
	};

#endif // SYMBIAN_CAF_V2

#endif // __MMF_DURATION_INFO_CUSTOM_COMMANDS_IMPL_H__

