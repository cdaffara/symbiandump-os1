/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef GLOBALS_H
#define GLOBALS_H

namespace Parser
	{
	class CDefinitionParser;
	}

// This is a horrible temporary hack - need some refactoring
extern Parser::CDefinitionParser* gMsgDefParser;
extern bool gQuiet;

#endif
// GLOBALS_H

