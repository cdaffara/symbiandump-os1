/*
* Copyright (c) 2009 Sony Ericsson Mobile Communications AB
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Sony Ericsson Mobile Communications AB - initial contribution.
* Nokia Corporation - additional changes.
* 
* Contributors:
* 
* Description:
* Implements the Enhanced SMS Predefined and User Defined sound IE.
*
*/


/**
 *  @file
 *  @internalComponent
 *  Defines  CEmsPreDEfSound and CEmsSoundIE  classes
 */

#ifndef __EMSSoundIE_H__
#define __EMSSoundIE_H__

#include <emsinformationelement.h>



class CEmsPreDefSoundIE : public CEmsInformationElement
/**
 *  @internalComponent
 */
{
public:
		enum TPredefinedSound {EChimesHigh=0, EChimesLow, EDing, ETaDa, ENotify, EDrum, EClaps, EFanFar, EChordHigh, EChordLow};
		IMPORT_C static CEmsPreDefSoundIE* NewL(TPredefinedSound aSound=EChimesHigh);
		IMPORT_C CEmsInformationElement* DuplicateL() const;

		IMPORT_C void CopyL(const CEmsPreDefSoundIE& aSrc);
		IMPORT_C TPredefinedSound PredefinedSound() const;
private:
		CEmsPreDefSoundIE();
		virtual void EncodeBodyL(TPtr8 aPtr, TBool aIsForSerialisation) const;
		virtual void DecodeBodyL(const TPtrC8 aPtr, TBool aIsFromSerialisation);
		void ConstructL(TPredefinedSound aSound);
		TPredefinedSound iSound;

};

class CEmsSoundIE : public CEmsInformationElement
/**
 *  CEmsSoundIE wraps the creation & modification of the user & predefined sound
 *  information elements into a single class.
 *  
 *  Format of Extended Object PreDefined Sound.
 *  ------------------------------------------
 *  
 *  Position (Octet)		Bit		Description
 *  1							Start Position
 *  2							Number of predef sound
 *  
 *  Format of Extended Object User Defined Sound.
 *  --------------------------------------------
 *  
 *  Position (Octet)		Bit		Description
 *  1							Start Position
 *  2..n						Sound Data (iMelody)
 *  
 *  @see 3gpp Spec V5 section 9.2.3.24.10.1.2/3
 *  @internalComponent
 */
	{
public:
	enum {EMaxSoundLength=128};

	static CEmsSoundIE* NewL();
	IMPORT_C static CEmsSoundIE* NewL(const TDesC8& aMelody);

	IMPORT_C CEmsInformationElement* DuplicateL() const;
	IMPORT_C ~CEmsSoundIE();
	IMPORT_C void CopyL(const CEmsSoundIE& aSrc);

	IMPORT_C const HBufC8* Melody() const;

private:
	CEmsSoundIE();
	void ConstructL(const TDesC8& aMelody);
	virtual void EncodeBodyL(TPtr8 aPtr, TBool aIsForSerialisation) const;
	virtual void DecodeBodyL(const TPtrC8 aPtr, TBool aIsFromSerialisation);
	void Reset();
	HBufC8* iMelody;
	};


#endif // __EMSSoundIE_H__
