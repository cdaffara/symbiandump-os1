// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// include\mmf\common\mmfutilities.h
// 
//

#ifndef __MMF_COMMON_UTILITIES_H__
#define __MMF_COMMON_UTILITIES_H__

#include <e32base.h>
#include <mmf/server/mmfdatabuffer.h>
#include <mmf/common/mmffourcc.h>


/** 
@publishedAll
@released

Identifies the particular stream of the given media type.
*/
class TMediaId
	{
public:

	/**
	Constructs the class with the supplied arguments.

	This constructor is used when there are multiple streams of the same media type on the same
	MDataSource.

	@param  aMediaType
	        The media type (video, audio etc.).
	@param  aStreamId
	        Identifies a specific stream when there are multiple streams of the same media type on 
	        the same MDataSource.
	*/
	TMediaId(TUid aMediaType, TUint aStreamId);

	/**
    Constructs the class with the supplied argument.

	This constructor is used when there is a single stream of the same media type on the same
	MDataSource.

	@param  aMediaType
	        The media type (video, audio etc.).
	*/
	TMediaId(TUid aMediaType);

	/**
	Default constructor.
	*/
	TMediaId();
public:
	/**
	The media type (KUidMediaTypeAudio, KUidMediaTypeVideo, etc).
	*/
	TUid iMediaType;

	/**
	Identifies a particular media stream used in case where multiple streams
	of the same media type are present on the same MDataSource.
	*/
	TUint iStreamId;
private:
	/**
	This member is internal and not intended for use.
	*/
	TInt iReserved1;
	TInt iReserved2;
	TInt iReserved3;
	};

/**
@publishedAll
@released

The FourCC code that represents NULL.
*/
const TInt KFourCCNULL = KMMFFourCCCodeNULL;

/**
@publishedAll
@released

A class that holds a four character code, representing supported data encodings for the
conversion destination. The four character codes are packed into a single TUint32.

FourCC codes are a representation of the datatypes used to identify codecs. FourCC codes are used in 
codec .rss files as a match string used by ECOM to correctly load the required DLL.
*/
class TFourCC
{
public:

	/**
	Default constructor initialises the class to KMMFFourCCCodeNULL.
	*/
	TFourCC() {iFourCC = KMMFFourCCCodeNULL;} //'NULL'

	/**
	Packs the four arguments into a single TUint32.

	The four arguments are packed in little-endian format.

	@param  aChar1
	        A character that represents part of the FourCC code. This character will be the least 
	        significant byte of the code.
	@param  aChar2
            A character that represents part of the FourCC code.
	@param  aChar3
            A character that represents part of the FourCC code.
	@param  aChar4
	        A character that represents part of the FourCC code. This character will be the most 
	        significant byte of the code.
	*/
	TFourCC(TUint8 aChar1, TUint8 aChar2, TUint8 aChar3, TUint8 aChar4) {iFourCC =(aChar4<<24)+(aChar3<<16)+(aChar2<<8)+aChar1;}

	/**
	Constructs a FourCC code with the given 8 bit desciptor where the descriptor contains the
	characters that make up the FourCC code.

	@param  aDes
	        The descriptor containing the characters from which to make the FourCC code.
	*/
	TFourCC(const TDesC8& aDes) {iFourCC =(aDes[3]<<24)+(aDes[2]<<16)+(aDes[1]<<8)+aDes[0];}

	/**
    Constructor with a TInt32 in little-endian format.

	@param  aFourCC
            The FourCC code.
	*/
	TFourCC(TInt32 aFourCC) {iFourCC = aFourCC;}

	/**
    Returns the FourCC code in little-endian format.

	@return The FourCC code in little-endian format.
	*/
	TUint32 FourCC() {return iFourCC;}
	inline void FourCC( TPtr8* aDes ) const ;

	/**
    Sets the FourCC code equal to the value in the supplied argument.

	@param  aFourCC
	        The required TFourCC containting the FourCC code.
	*/
	void Set(TFourCC aFourCC) {iFourCC = aFourCC.iFourCC;}

	/**
    Sets the FourCC code equal to the supplied argument.

	@param  aFourCC
	        The required FourCC code.
	*/
	void Set(TUint32 aFourCC) {iFourCC = aFourCC;}

	/**
	Equality operator.

	@param  aA
	        The TFourCC code that *this will be tested against.

	@return A boolean indicating if the two values are equal. ETrue if the two values are equal, 
	        otherwise false.
	*/
	TBool operator==(const TFourCC& aA) const {return (iFourCC == aA.iFourCC);}

	/**
    Equality operator.

	@param  aUint
	        The FourCC code that *this will be tested against.

	@return A boolean indicating if the two values are equal. ETrue if the two values are equal, 
	        otherwise false.
	*/
	TBool operator==(const TUint32& aUint) const {return (iFourCC == aUint);}

	/**
    Inequality operator.

	@param  aA
	        The TFourCC code that *this will be tested against.
	
	@return A boolean indicating if the two values are not equal. ETrue if the two values are 
	        unequal, otherwise false.
	*/
	TBool operator!=(const TFourCC& aA) const {return (iFourCC != aA.iFourCC);}

	/**
    Inequality operator.

	@param  aUint
	        The FourCC code that *this will be tested against.
	*/
	TBool operator!=(const TUint32& aUint) const {return (iFourCC != aUint);}

private:
	TUint32 iFourCC;
};


#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfhelper.h>
#endif


#include <mmf/common/mmfutilities.inl>

#endif

