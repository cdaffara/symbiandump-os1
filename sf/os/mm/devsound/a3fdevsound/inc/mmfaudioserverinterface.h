/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef MMFAUDIOSERVERINTERFACE_H
#define MMFAUDIOSERVERINTERFACE_H

/**
@publishedPartner
@file
@released
*/

//  INCLUDES
#include <e32std.h>
#include <e32base.h>

// CLASS DECLARATION

/**
This is interface class for creating AudioServer.

*/
class CAudioSvrLoader : public CBase
	{
public:
	/**
	Constructs and returns a pointer to a new CAudioSvrLoader object.
	Leaves on failure. 
	@return CAudioSvrLoader* - on success, pointer to new class instance. 
	*/	
	IMPORT_C static CAudioSvrLoader* NewL();
	/**
	Destructor.
	*/
	~CAudioSvrLoader();
protected:
	/**
	Constructor
	*/
	CAudioSvrLoader(); 
	};
#endif // MMFAUDIOSERVERINTERFACE_H

// End of File
