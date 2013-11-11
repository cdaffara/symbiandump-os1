// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __MMFCUSTOMCOMMANDPARSERMANAGER_H__
#define __MMFCUSTOMCOMMANDPARSERMANAGER_H__

/**
@publishedPartner
@released

Class to manage custom command parsers.

An object of this type is owned by the controller framework to contain all the custom command
parsers for the controller plugin.

@since 7.0s
*/
class CMMFCustomCommandParserManager : public CBase
	{
public:
	/**
	Factory constructor.

	@return A pointer to the newly created object.

	@since 7.0s
	*/
	IMPORT_C static CMMFCustomCommandParserManager* NewL();

	/**
	Destructor.

	Deletes all custom command parsers added to the manager.

	@since 7.0s
	*/
	IMPORT_C ~CMMFCustomCommandParserManager();

	/**
	Attempts to find a custom command parser capable of handling the message.

	@param  aMessage
            The message to be handled.

	@return A boolean indicating in the message has been handled. ETrue if the message has been handled, EFalse
	        if it has not.

	@since 7.0s
	*/
	IMPORT_C TBool HandleRequest(TMMFMessage& aMessage);

	/**
	Adds a custom command parser to the manager.  The manager takes ownership of the parser.

	Note:
	This method will leave if it fails to add the parser. If it does leave, ownership
	of the parser will remain with the caller.

	@param  aParser
	        A reference to the parser to be added to the manager.

	@since 7.0s
	*/
	IMPORT_C void AddCustomCommandParserL(CMMFCustomCommandParserBase& aParser);
private:
	CMMFCustomCommandParserManager();
private:
	/**
	The array of custom command parsers.
	*/
	RPointerArray<CMMFCustomCommandParserBase> iParsers;
	};

#endif __MMFCUSTOMCOMMANDPARSERMANAGER_H__
