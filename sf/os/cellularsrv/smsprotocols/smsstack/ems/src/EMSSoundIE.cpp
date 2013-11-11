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
 *  
 *  Implements  CEMSPreDefSoundIE and CEmsSoundIE classes
 */

#include <emssoundie.h>



EXPORT_C CEmsPreDefSoundIE* CEmsPreDefSoundIE::NewL(TPredefinedSound aSound)
/**
 *  @capability None
 */
	{
	if(aSound > EChordLow)User::Leave(KErrArgument);
	CEmsPreDefSoundIE* self = new (ELeave) CEmsPreDefSoundIE();
	CleanupStack::PushL(self);
	self->ConstructL(aSound);
	CleanupStack::Pop(self);
	return self;
	}


EXPORT_C CEmsInformationElement* CEmsPreDefSoundIE::DuplicateL() const
/**
 *  @capability None
 */
	{
	CEmsPreDefSoundIE* copy = new (ELeave) CEmsPreDefSoundIE();
	CleanupStack::PushL(copy);
	copy->CopyL(*this);
	CleanupStack::Pop(copy);
	return copy;
	}

EXPORT_C CEmsPreDefSoundIE::TPredefinedSound CEmsPreDefSoundIE::PredefinedSound() const
/**
 *  @capability None
 */
	{
	return iSound;
	}


CEmsPreDefSoundIE::CEmsPreDefSoundIE(): CEmsInformationElement(CSmsInformationElement::ESmsEnhancedPredefinedSound){iEncodedBodyLength=1;}

void CEmsPreDefSoundIE::ConstructL(TPredefinedSound aSound)
	{
	iSound = aSound;
	}


EXPORT_C void CEmsPreDefSoundIE::CopyL(const CEmsPreDefSoundIE& aSrc)
/**
 *  @capability None
 */
	{
	CEmsInformationElement::CopyL(aSrc);
	iSound = aSrc.iSound;
	}

/**
 *  Encodes the information element into its raw format. (no IE id)
 *  
 *  @param aPtr the buffer to be used which is to contain the data
 *  @param TBool boolean to indicate if it is for serialisation or encoding
 */
void CEmsPreDefSoundIE::EncodeBodyL(TPtr8 aPtr, TBool) const
	{
	aPtr.Append(static_cast<TUint8>(iSound));
	}


/**
 *  Decodes the raw data out of an information element into this class.
 *  
 *  @param aPtr The raw predefined animation data
 *  @param TBool boolean to indicate if it is from serialisation
 *  @leave KErrCorrupt If the size of the data does not match what is expected.
 */
void CEmsPreDefSoundIE::DecodeBodyL(const TPtrC8 aPtr, TBool)
	{
	__ASSERT_ALWAYS( aPtr.Length()==1, User::Leave(KErrCorrupt));
	iSound = static_cast<TPredefinedSound>(aPtr[0]);
	}


CEmsSoundIE* CEmsSoundIE::NewL()
	{
	CEmsSoundIE* self = new (ELeave) CEmsSoundIE();
	return self;
	}

EXPORT_C CEmsSoundIE* CEmsSoundIE::NewL(const TDesC8& aMelody)
/**
 *  @capability None
 */
	{
	CEmsSoundIE* self = new (ELeave) CEmsSoundIE();
	CleanupStack::PushL(self);
	self->ConstructL(aMelody);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CEmsInformationElement* CEmsSoundIE::DuplicateL() const
/**
 *  @capability None
 */
	{
	CEmsSoundIE* copy = new (ELeave) CEmsSoundIE();
	CleanupStack::PushL(copy);
	copy->CopyL(*this);
	CleanupStack::Pop(copy);
	return copy;
	}

CEmsSoundIE::CEmsSoundIE() : CEmsInformationElement(CSmsInformationElement::ESmsEnhancedUserDefinedSound){}


void CEmsSoundIE::ConstructL(const TDesC8& aMelody)
	{
	__ASSERT_ALWAYS(aMelody.Length()<=EMaxSoundLength, User::Leave(KErrCorrupt));
	iMelody = aMelody.AllocL();
	iEncodedBodyLength=iMelody->Length();
	}

EXPORT_C void CEmsSoundIE::CopyL(const CEmsSoundIE& aSrc)
/**
 *  @capability None
 */
	{
	Reset();
	CEmsInformationElement::CopyL(aSrc);
	__ASSERT_ALWAYS(aSrc.iMelody, User::Leave(KErrCorrupt));
	iMelody = aSrc.iMelody->AllocL();
	}

EXPORT_C const HBufC8* CEmsSoundIE::Melody() const
/**
 *  @capability None
 */
	{
	return iMelody;
	}

/**
 *  Encodes the information element into its raw format. (no IE id)
 *  
 *  @param aPtr the buffer to be used which is to contain the data
 *  @param TBool boolean to indicate if it is for serialisation or encoding
 *  @leave KErrCorrupt If the melody has not been defined.
 */
void CEmsSoundIE::EncodeBodyL(TPtr8 aPtr, TBool) const
	{
	__ASSERT_ALWAYS(iMelody!=NULL, User::Leave(KErrCorrupt));
	aPtr.Append(*iMelody);
	}

/**
 *  Decodes the raw data out of an information element into this class.
 *  
 *  @param aPtr The raw predefined animation data
 *  @param TBool boolean to indicate if it is from serialisation
 *  @leave KErrCorrupt If the size of the data does not match what is expected.
 */
void CEmsSoundIE::DecodeBodyL(const TPtrC8 aPtr, TBool)
	{
	__ASSERT_ALWAYS(aPtr.Length()<=EMaxSoundLength, User::Leave(KErrCorrupt));

	Reset();
	iMelody = aPtr.AllocL();
	}

EXPORT_C CEmsSoundIE::~CEmsSoundIE()
/**
 *  @capability None
 */
	{
	Reset();
	}

void CEmsSoundIE::Reset()
	{
	delete iMelody;
	iMelody = NULL;
	}
