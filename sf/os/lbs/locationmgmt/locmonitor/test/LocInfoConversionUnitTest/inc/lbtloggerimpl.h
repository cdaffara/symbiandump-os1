/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This header file describes the triggering logger implementation
*
*/


// INCLUDES
#include <e32base.h>

// FORWARD DECLARATION
class RFileLogger;

NONSHARABLE_CLASS(  CLbtLoggerImpl ) : public CBase
	{
	public:
		IMPORT_C static CLbtLoggerImpl* CreateLogger();
	
		IMPORT_C static void Destroy();
		
		IMPORT_C RFileLogger& GetFileLogger();
		
	private:
		CLbtLoggerImpl();
		
		~CLbtLoggerImpl();
		
		RFileLogger iLogger;
		
		static CLbtLoggerImpl* iSelf;
	};