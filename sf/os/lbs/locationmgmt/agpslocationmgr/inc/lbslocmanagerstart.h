// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
*/

#ifndef LOCLASSTART_H
#define LOCLASSTART_H

class TLocationManagerStart
	{
public:
	static TInt StartLocationManager();

private:		
	static void DoStartL();
	
	};

#endif //LOCLASSTART_H