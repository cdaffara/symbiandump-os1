// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <s32mem.h>
#include <mdf/codecapiresolverdata.h>
// The biggest descriptor that will be built when internalizing the object
const TInt KMaxDescriptorLength = 128;	

/**
 Default constructor.
 */
CCodecApiResolverData::CCodecApiResolverData()
	{
	}


/**
 Destructor.
 */
CCodecApiResolverData::~CCodecApiResolverData()
	{
	delete iInputDataFormat;
	delete iOutputDataFormat;
	}


/**
 Creates a new CodedApiResolverStandard object.
 @return A pointer to the newly constructed object.
 */
EXPORT_C CCodecApiResolverData* CCodecApiResolverData::NewL()
	{
	CCodecApiResolverData* self = new(ELeave) CCodecApiResolverData;
	return self;
	}

	
/**
 Creates a new CodedApiResolverStandard object.
 @param  aPackage
 	   	 A reference to a descriptor created using <code>PackL()</code>
       	 used to initialize the object.
 @return A pointer to the newly constructed object.
 */
EXPORT_C CCodecApiResolverData* CCodecApiResolverData::NewL(const TDesC8& aPackage)
	{
	CCodecApiResolverData* self = CCodecApiResolverData::NewLC(aPackage);
	CleanupStack::Pop(self);
	return self;
	}


/**
 Creates a new CodedApiResolverStandard object and leaves a pointer to it on the cleanup stack.
 @return A pointer to the newly constructed object.
 */
EXPORT_C CCodecApiResolverData* CCodecApiResolverData::NewLC()
	{
	CCodecApiResolverData* self = CCodecApiResolverData::NewL();
	CleanupStack::PushL(self); 
    return self;
	}


/**
 Creates a new CodedApiResolverStandard object and leaves a pointer to it on the cleanup stack.
 @param	aPackage
 		A reference to a descriptor created using <code>PackL()</code>
 		used to initialize the object.
 @return A pointer to the newly constructed match data object.
 */
EXPORT_C CCodecApiResolverData* CCodecApiResolverData::NewLC(const TDesC8& aPackage)
	{
	CCodecApiResolverData* self = new(ELeave) CCodecApiResolverData;
	CleanupStack::PushL(self);
	self->ConstructL(aPackage);
    return self;
	}


/**
 Sets up the data inside the class by calling <code>UnPackL()</code>.
 @param	aPackage
 		A reference to the data that will be contained by this class.
 */
void CCodecApiResolverData::ConstructL(const TDesC8& aPackage)
	{
	UnPackL(aPackage);
	}


/**
 Externalizes the object to a stream.
 All the member variables will be written to the stream.
 @param	aStream
 		A reference to the stream to which the member variables will
 		be written.
 */
 void CCodecApiResolverData::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt32L(iMatchType);
	aStream.WriteInt32L(iImplementationType.iUid);

	aStream << InputDataFormat();
	aStream << OutputDataFormat();
	}
	

/**
 Internalizes the object from a stream.
 All the member variables will be read from the streamed.
 @param	aStream
 		A reference to the stream from which data will be read to
 		setup the member variables.
 */
void CCodecApiResolverData::InternalizeL(RReadStream& aStream)
	{
	iMatchType = static_cast<TCodecApiResolverMatchType>(aStream.ReadInt32L());
	iImplementationType.iUid = aStream.ReadInt32L();

	delete iInputDataFormat; 
	iInputDataFormat = NULL;
	iInputDataFormat = HBufC8::NewL(aStream,KMaxDescriptorLength); //KErrOverflow

	delete iOutputDataFormat; 
	iOutputDataFormat = NULL;
	iOutputDataFormat = HBufC8::NewL(aStream,KMaxDescriptorLength); //KErrOverflow
	}


/**
 Sets the match type to match against.
 @param	aMatchType
 		The type of match requested.
 */
EXPORT_C void CCodecApiResolverData::SetMatchType(const TCodecApiResolverMatchType& aMatchType)
	{
	iMatchType = aMatchType;
	}


/**
 Sets the implementation type to match against.
 @param	aImplementationType
 		The implementation uid of a specific codec to match against.
 */
EXPORT_C void CCodecApiResolverData::SetImplementationType(const TUid& aImplementationType)
	{
	iImplementationType = aImplementationType;
	}


/**
 Sets the input data type to match against.
 @param	aDataType
 		The input data type of a codec to match against.
 */
EXPORT_C void CCodecApiResolverData::SetInputDataL(const TDesC8& aDataType)
	{
	delete iInputDataFormat;
	iInputDataFormat = NULL;
	iInputDataFormat = aDataType.AllocL();
	}


/**
 Sets the output data type to match against.
 @param	aDataType
 		The output data type of a codec to match against.
 */
EXPORT_C void CCodecApiResolverData::SetOutputDataL(const TDesC8& aDataType)
	{
	delete iOutputDataFormat;
	iOutputDataFormat = NULL;
	iOutputDataFormat = aDataType.AllocL();	
	}


/**
 Retrieves the match type to match against.
 @return The type of match requested.
 */
EXPORT_C TCodecApiResolverMatchType CCodecApiResolverData::MatchType() const
	{
	return iMatchType;
	}


/**
 Retrieves the implementation type to match against.
 @return The implementation of a specific codec to match against.
 */
EXPORT_C TUid CCodecApiResolverData::ImplementationType() const
	{
	return iImplementationType;
	}



/**
 Retrieves the input data format string to match against. If no string is set
 a null descriptor is returned.
 @return The string to match against.
 */
EXPORT_C const TPtrC8 CCodecApiResolverData::InputDataFormat() const
	{
	TPtrC8 result;
	if(iInputDataFormat)
		{
		result.Set(*iInputDataFormat);
		}		
	else
		{
		result.Set(KNullDesC8);
		}		
	return result;
	}


/**
 Retrieves the output data format string to match against. If no string is set
 a null descriptor is returned.
 @return The string to match against.
 */
EXPORT_C const TPtrC8 CCodecApiResolverData::OutputDataFormat() const
	{
	TPtrC8 result;
	if(iOutputDataFormat)
		{
		result.Set(*iOutputDataFormat);
		}		
	else
		{
		result.Set(KNullDesC8);
		}		
	return result;
	}




/** 
 Packages the object up into a descriptor.
 @return A pointer to a desctriptor with the object packed in it.
 */
EXPORT_C HBufC8* CCodecApiResolverData::NewPackLC() const
	{
	//Calculate the size necessary size for the descriptor to pack the object
	TInt size = 0;
	size = sizeof(iMatchType) + sizeof(iImplementationType.iUid) + 2*sizeof(TInt32); 
	// the operator << write first the length of the string so we need to add it to the total size
	if(iInputDataFormat)
		{
		size += iInputDataFormat->Size();
		}		
	if(iOutputDataFormat)
		{
		size += iOutputDataFormat->Size();
		}		

	HBufC8* package = HBufC8::NewLC(size);
	TPtr8   packageDes = package->Des();
	RDesWriteStream stream(packageDes);
	CleanupClosePushL(stream);
	ExternalizeL(stream);
	CleanupStack::PopAndDestroy(&stream);
	return package;
	}



/** 
 Unpacks a packed descriptor.
 @param	aPackage
 		A reference to a desctriptor created.
 */
EXPORT_C void CCodecApiResolverData::UnPackL(const TDesC8& aPackage)
	{
	RDesReadStream stream(aPackage);
	CleanupClosePushL(stream);
	InternalizeL(stream);
	CleanupStack::PopAndDestroy(&stream);
	}
