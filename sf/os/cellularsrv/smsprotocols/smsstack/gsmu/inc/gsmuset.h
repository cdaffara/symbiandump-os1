// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the class definition for TSmsUserDataSettings
// 
//

/**
 @file
 @publishedAll
 @released
*/

#ifndef __GSMUSET_H__
#define __GSMUSET_H__

#include <gsmuelem.h>

/**
 *  Operations on TP-UD User Data.
 */
class TSmsUserDataSettings
	{
public:
	IMPORT_C TSmsUserDataSettings();

	inline TSmsDataCodingScheme::TSmsAlphabet Alphabet() const;
	inline void SetAlphabet(TSmsDataCodingScheme::TSmsAlphabet aAlphabet);
	inline TBool TextCompressed() const;
	inline void SetTextCompressed(TBool aCompressed);
	IMPORT_C TBool TextConcatenated(TBool* aIs16Bit=NULL) const;
	IMPORT_C void SetTextConcatenated(TBool aConcatenated,TBool aIs16Bit=EFalse);

	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

private:
	enum TSmsUserDataSettingsFlags
		{
		ESmsFlagCompressed=0x01,
		ESmsFlagConcatenated=0x02,
		ESmsFlagConcatenatedUsing16BitReference=0x04
		};
private:
	TSmsDataCodingScheme::TSmsAlphabet iAlphabet;
	TInt iFlags;
	};


#include "gsmuset.inl"

#endif // !defined __GSMUSET_H__
