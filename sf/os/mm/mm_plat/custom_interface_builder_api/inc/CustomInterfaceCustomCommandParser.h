/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Custom Interface CustomCommand Parser
*
*/

#ifndef __CUSTOMINTERFACECUSTOMCOMMANDPARSER_H
#define __CUSTOMINTERFACECUSTOMCOMMANDPARSER_H

#include <mmf/common/mmfcontroller.h>

class MCustomInterfaceCustomCommandImplementor;

class CCustomInterfaceCustomCommandParser : public CMMFCustomCommandParserBase
	{
public:
	IMPORT_C static CCustomInterfaceCustomCommandParser* NewL(MCustomInterfaceCustomCommandImplementor& aImplementor);
	
	virtual ~CCustomInterfaceCustomCommandParser();

	virtual void HandleRequest(TMMFMessage& aMessage);

private:
	CCustomInterfaceCustomCommandParser(MCustomInterfaceCustomCommandImplementor& aImplementor);
	virtual void ConstructL();
	
	void DoHandleRequestL(TMMFMessage& aMessage);
	void DoGetBuilderL(TMMFMessage& aMessage);

private:
	MCustomInterfaceCustomCommandImplementor& iImplementor;
	};

#endif
