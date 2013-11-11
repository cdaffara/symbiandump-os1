// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 
 Gets the Alphabet for the User Data.
 
 @return Alphabet 
 
*/
inline TSmsDataCodingScheme::TSmsAlphabet TSmsUserDataSettings::Alphabet() const
	{
	return iAlphabet;
	}


/**
 *  Sets the Alphabet for the User Data.
 *  
 *  @param aAlphabet Alphabet 
 */
inline void TSmsUserDataSettings::SetAlphabet(TSmsDataCodingScheme::TSmsAlphabet aAlphabet)
	{
	iAlphabet=aAlphabet;
	}


/**
 *  Tests if the message text is compressed.
 *  
 *  @return True if the message text is compressed 
 */
inline TBool TSmsUserDataSettings::TextCompressed() const
	{
	return iFlags&ESmsFlagCompressed;
	}


/**
 *  Sets if the message text is compressed.
 *  
 *  @param aCompressed True if the message text is compressed 
 */
inline void TSmsUserDataSettings::SetTextCompressed(TBool aCompressed)
	{
	iFlags=aCompressed? iFlags|ESmsFlagCompressed: iFlags&(~ESmsFlagCompressed);
	}
