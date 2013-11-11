/*
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



#ifndef MMFAUDIOSERVERFACTORY_H
#define MMFAUDIOSERVERFACTORY_H

//  INCLUDES
#include <e32base.h>
#include <ecom/ecom.h>
#include <badesca.h>
#include <a3f/mmfaudiosvrservice.h>

// FORWARD DECLARATIONS
class CMMFAudioServer;

// CLASS DECLARATION

/**
*  Utility factory class that can be used to launch additional servers/objects
*  in Audio Server.
*
*  @lib AudioServerFactory.lib
*  @since 
*/
class CMMFAudioServerFactory : public CBase
	{
public:  // Constructors and destructor

	/**
	* Constructs, and returns a pointer to, a new CMMFAudioServerFactory
	* object.
	* Leaves on failure..
	* @return CMMFAudioServerFactory* A pointer to newly created object.
	*/
	IMPORT_C static CMMFAudioServerFactory* NewL();

	/**
	* Destructor.
	*/
	IMPORT_C ~CMMFAudioServerFactory();

public: // New functions

	/**
	* Called by Audio Server when Audio Server is started.
	* @since 
	* @param const CMMFAudioServer& aAudioServer - Audio Server
	*/
	IMPORT_C void StartL(const CMMFAudioServer& aAudioServer);

	/**
	* Called by Audio Server when Audio Server is shutting down
	* @since 
	* @param const CMMFAudioServer& aAudioServer - Audio Server
	*/
	IMPORT_C void Stop(const CMMFAudioServer& aAudioServer);

private:

	/**
	* C++ default constructor.
	*/
	CMMFAudioServerFactory();

	/**
	* By default Symbian 2nd phase constructor is private.
	*/
	void ConstructL();
	RPointerArray<MAudioSvrService> iAudioServList;

	};

#endif // MMFAUDIOSERVERFACTORY_H

// End of File
