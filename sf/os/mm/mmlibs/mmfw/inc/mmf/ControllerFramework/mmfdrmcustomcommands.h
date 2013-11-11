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

#ifndef __MMF_DRM_CUSTOM_COMMANDS_H__
#define __MMF_DRM_CUSTOM_COMMANDS_H__


#include <e32base.h>
#include <mmf/common/mmcaf.h>
#include <mmf/common/mmfstandardcustomcommands.h>

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfdrmcustomcommandsenum.h>
#endif

/**
@publishedAll
@released
*/
const TUid KUidInterfaceMMFDRMControl = {0x101F7D8D};

/**
@publishedAll
@released

Mixin class to be derived from by controller plugins wishing to support the DRM Intent
custom commands.
*/
class MMMFDRMCustomCommandImplementor
	{
public:

	/**
	Disable the automatic (controller specified) execution of intent

	@param  aIntent
	        The intent to execute

	*/
	virtual TInt MdcDisableAutomaticIntent(TBool aDisableAutoIntent)=0;


	/**
	Executes a particular intent

	@param  aIntent
	        The intent to execute

	*/
	virtual TInt MdcExecuteIntent(ContentAccess::TIntent aIntent)=0;

	/**
	Evaluate a particular intent

	@param  aIntent
	        The intent to evaluate

	*/
	virtual TInt MdcEvaluateIntent(ContentAccess::TIntent aIntent)=0;
	
	/**
	
	
	*/
	virtual TInt MdcSetAgentProperty(ContentAccess::TAgentProperty aProperty, TInt aValue)=0;
	};

class CMMFDRMCustomCommandParser : public CMMFCustomCommandParserBase
	{
public:

	/**
	Creates a new custom command parser capable of DRM Intent controller commands.

	@param  aImplementor
	        A reference to the controller plugin that owns this new object.

	@return A pointer to the object created.

	@since  7.0s
	*/
	IMPORT_C static CMMFDRMCustomCommandParser* NewL(MMMFDRMCustomCommandImplementor& aImplementor);

	/**
	Destructor.

	@since  7.0s
	*/
	IMPORT_C ~CMMFDRMCustomCommandParser();

	/**
	Handles a request from the client. Called by the controller framework.

	@param  aMessage
	        The message to be handled.

	@since  7.0s
	*/
	void HandleRequest(TMMFMessage& aMessage);

private:
	/**
	Constructor.

	@param  aImplementor
	        A reference to the controller plugin that owns this new object.
	@since  7.0s
	*/
	CMMFDRMCustomCommandParser(MMMFDRMCustomCommandImplementor& aImplementor);
	// Internal request handling methods.

	void DoHandleRequestL(TMMFMessage& aMessage);
	void DoEvaluateIntentL(TMMFMessage& aMessage);
	void DoExecuteIntentL(TMMFMessage& aMessage);
	void DoDisableAutomaticIntentL(TMMFMessage& aMessage);
	void DoSetAgentPropertyL(TMMFMessage& aMessage);

private:
	MMMFDRMCustomCommandImplementor& iImplementor;
	};
	
	
/**
@publishedAll
@released

Generic Mixin class to be used by a client utility wishing to pass DRM Intent
custom commands to a controller plugin.
*/
class MMMFDRMCustomCommand
	{
public:

	/**
	Disable the automatic (controller specified) execution of intent

	@param  aIntent
	        The intent to execute

	*/
	virtual TInt DisableAutomaticIntent(TBool aDisableAutoIntent)=0;


	/**
	Executes a particular intent

	@param  aIntent
	        The intent to execute

	*/
	virtual TInt ExecuteIntent(ContentAccess::TIntent aIntent)=0;

	/**
	Evaluate a particular intent

	@param  aIntent
	        The intent to evaluate

	*/
	virtual TInt EvaluateIntent(ContentAccess::TIntent aIntent)=0;
	
	/**
	
	
	*/
	virtual TInt SetAgentProperty(ContentAccess::TAgentProperty aProperty, TInt aValue)=0;
	};

class RMMFDRMCustomCommands : public RMMFCustomCommandsBase,
                              public MMMFDRMCustomCommand
	{
public:

	/**
	Constructor.

	@param  aController
	        The client side controller object to be used by this custom command interface.

	*/
	IMPORT_C RMMFDRMCustomCommands(RMMFController& aController);
	
	/**
	*/
	IMPORT_C TInt DisableAutomaticIntent(TBool aBool);

	/**
	*/
	IMPORT_C TInt EvaluateIntent(ContentAccess::TIntent aIntent);
	
	/**
	*/
	IMPORT_C TInt ExecuteIntent(ContentAccess::TIntent aIntent);
	
	/**
	*/
	IMPORT_C TInt SetAgentProperty(ContentAccess::TAgentProperty aProperty, TInt aValue);
	
	/**
	*/
	IMPORT_C TBool IsSupported();
	
	};


#endif // __MMF_DRM_CUSTOM_COMMANDS_H__
