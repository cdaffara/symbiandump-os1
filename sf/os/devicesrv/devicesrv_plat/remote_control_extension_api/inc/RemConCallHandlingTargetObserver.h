/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Informs client when call handling related function has been 
*				 requested in accessory.
*
*/


#ifndef MREMCONCALLHANDLINGTARGETOBSERVER_H
#define MREMCONCALLHANDLINGTARGETOBSERVER_H

#include <e32base.h>

class MRemConCallHandlingTargetObserver
	{
public:
	
	/**
	* Answer an incoming phone call.
	*/
	virtual void AnswerCall() = 0;

	/**
	* End an incoming/ongoing phone call.
	*/
	virtual void EndCall() = 0;
	
	/**
	* End an incoming/ongoing phone call.
	*/
	virtual void AnswerEndCall() = 0;
	
	/**
	* Make a voice call.
	*/
	virtual void VoiceDial( const TBool aActivate ) = 0;
	
	/**
	* Redial last phone call.
	*/
	virtual void LastNumberRedial() = 0;
	
	/**
	* Dial a phone call.
	*/
	virtual void DialCall( const TDesC8& aTelNumber ) = 0;
	
	/**
	* Make multiparty call.
	*/
	virtual void MultipartyCalling( const TDesC8& aData ) = 0;
	
	/**
	* Generates DTMF signal.
	*/
	virtual void GenerateDTMF( const TChar aChar ) = 0;
	
	/**
	* Make a speed dial call.
	*/
	virtual void SpeedDial( const TInt aIndex ) = 0;
		
	};

#endif // MREMCONCALLHANDLINGTARGETOBSERVER_H
