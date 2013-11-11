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
//

#include <ecom/ecom.h>
#include <mmf/plugin/mmfformatimplementationuids.hrh>
#include <mmf/common/mmffourcc.h>
#include <mmf/server/mmfaudioinput.h>
#include <mmf/server/mmffile.h>
#include <mmf/server/mmfdes.h>

#include "mmfAUformat.h"

#include "formatUtils.h"
#include "MMFFormatBaseClasses.h"


/**
*
* Read32
* @internalTechnology
* @param aPtr
* @return TUint32
* fast read for non-aligned BIG-endian 32-bit data
*/
LOCAL_C TUint32 Read32(const TUint8* aPtr)
    {
    TUint32 x = 0 ;
	x |= *aPtr++ <<24;
    x |= *aPtr++ << 16;
    x |= *aPtr++ << 8;
    x |= *aPtr++ ;
    return x;
    }

/**
*
* Write32
* @internalTechnology
* @param aPtr
* @param aValue
* fast write for non-aligned BIG-endian 32-bit data
*/
LOCAL_C void Write32(TUint8*& aPtr,TInt aValue)
   {
	*aPtr++ = (TUint8)(aValue >> 24);
	*aPtr++ = (TUint8)(aValue >> 16);
	*aPtr++ = (TUint8)(aValue >> 8);
	*aPtr++ = (TUint8)aValue;
    }

	
	
	
/*
 TMmfWavFormatPanics is an enumeration with the following entries:
 EPreconditionViolation indicates a precondition violation
 EPostConditionViolation indicates a post condition violation
*/
enum TMmfAuFormatPanics
	{
	EPostConditionViolation,
	EPreconditionViolation
	};
/**
@internalTechnology

This method generates a panic

@param aPanicCode
*/
void Panic(TInt aPanicCode)
	{
	_LIT(KMMFAuFormatPanicCategory, "MMFAuFormat");
	User::Panic(KMMFAuFormatPanicCategory, aPanicCode);
	}






template<class T>
void TMMFAuConsolidationMethods<T>::ProcessFormatHeaderL()
	{
	iThat.DoReadL( 0 ) ;
	
	// The header is in iBuffer
	const TUint8* pos = iThat.iBuffer->Data().Ptr() ;
	TUint8 offset = 0 ;
	iThat.iHeader.iMagicNumber = Read32( pos + offset ) ;
	
	offset+=sizeof( TUint32 ) ;
	iThat.iHeader.iHeaderSize = Read32( pos + offset ) ;
	iThat.iHeaderLength = iThat.iHeader.iHeaderSize;
	
	offset+=sizeof( TUint32 )  ;
	iThat.iHeader.iDataSize = Read32( pos + offset ) ;
	
	offset+=sizeof( TUint32 )  ;
	iThat.iHeader.iEncoding = Read32( pos + offset ) ;
	
	offset+=sizeof( TUint32 )  ;
	iThat.iHeader.iSampleFrequency = Read32( pos+ offset ) ;
	
	offset+=sizeof( TUint32 )  ;
	iThat.iHeader.iChannels = Read32( pos + offset ) ;
	
	//Extract the useful data
	iThat.iStartPosition = iThat.iHeaderLength ;
	iThat.iDataLength = iThat.iHeader.iDataSize ;
	iThat.iClipLength = iThat.iHeaderLength + iThat.iHeader.iDataSize ;
	iThat.iSampleRate = iThat.iHeader.iSampleFrequency ;
	iThat.iChannels = iThat.iHeader.iChannels ;
	
	//Set up the fourCC code
	switch ( iThat.iHeader.iEncoding )
		{
		case 1 : // 8 bit ISDN uLaw
			iThat.iFourCC = KMMFFourCCCodeMuLAW;
			iThat.iBitsPerSample = 8 ;
			break ;
		case 2 : // 8 bit linear PCM
			iThat.iFourCC = KMMFFourCCCodePCM8;
			iThat.iBitsPerSample = 8 ;
			break ;
		case 3 : // 16 bit linear PCM
			iThat.iFourCC = KMMFFourCCCodePCM16B;
			iThat.iBitsPerSample = 16 ;
			break ;
		case 27 : // 8 bit ISDN aLaw
			iThat.iFourCC = KMMFFourCCCodeALAW;
			iThat.iBitsPerSample = 8 ;
			break ;
			
		default :
			User::Leave( KErrNotSupported ) ;
			break ;
		}

	iThat.CalculateFrameSize();
	}


/**
*
* GetSupportedDataTypesL
* @param aMediaId
* @param aDataTypes 
*
*/
template<class T>
void TMMFAuConsolidationMethods<T>::GetSupportedDataTypesL(TMediaId aMediaId, RArray<TFourCC>& aDataTypes)
	{
	if (aMediaId.iMediaType != KUidMediaTypeAudio)
		User::Leave(KErrNotSupported);
	aDataTypes.Reset();
	User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodeMuLAW));
	User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodePCM8));
	User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodePCM16B));
	User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodeALAW));
	}

/**
*
* NewL
* @param aSource
* @return CMMFFormatDecode*
*
*/
CMMFFormatDecode* CMMFAuFormatRead::NewL(MDataSource* aSource)
	{
	if ((aSource->DataSourceType()==KUidMmfDescriptorSource)||
			(aSource->DataSourceType()==KUidMmfFileSource))
		{//currently only files and descriptor MDataSources are supported
		CMMFAuFormatRead* self = new(ELeave)CMMFAuFormatRead;
		CleanupStack::PushL(self);
		self->ConstructL(aSource);
		CleanupStack::Pop();
		return STATIC_CAST( CMMFFormatDecode*, self );
		}
	else 
		User::Leave(KErrNotSupported);
		return NULL;
	}


/**
*
* ConstructL
* @param aSource
* 
*/
void CMMFAuFormatRead::ConstructL(MDataSource* aSource)
	{
	iAuFormatConsolidator = new (ELeave) TMMFAuConsolidationMethods<CMMFAuFormatRead>(*this);

	CMMFFormatRead::ConstructL(aSource);
	}




/**
*
* ~CMMFAuFormatRead
*
*/
CMMFAuFormatRead::~CMMFAuFormatRead()
	{
	delete iAuFormatConsolidator;
	}	



/**
* @internalTechnology
* ProcessFormatHeaderL
*/
void CMMFAuFormatRead::ProcessFormatHeaderL()
	{
	__ASSERT_ALWAYS( iAuFormatConsolidator, Panic( EPreconditionViolation ) );
	iAuFormatConsolidator->ProcessFormatHeaderL();
	}





/**
*
* GetSupportedDataTypesL
* @param aMediaId
* @param aDataTypes
* 
*/
void CMMFAuFormatRead::GetSupportedDataTypesL(TMediaId aMediaId, RArray<TFourCC>& aDataTypes)
	{
	__ASSERT_ALWAYS( iAuFormatConsolidator, Panic( EPreconditionViolation ) );
	iAuFormatConsolidator->GetSupportedDataTypesL(aMediaId,aDataTypes);
	}

/***********************************************************************************************/

/**
*
* NewL
* @param aSink
*
*/
CMMFFormatEncode* CMMFAuFormatWrite::NewL(MDataSink* aSink)
	{
	if ((aSink->DataSinkType()==KUidMmfDescriptorSink)||
		(aSink->DataSinkType()==KUidMmfFileSink))
		{//currently only files and descriptor MDataSinks are supported
		CMMFAuFormatWrite* self = new( ELeave )CMMFAuFormatWrite;
		CleanupStack::PushL(self);
		self->ConstructL(aSink);
		CleanupStack::Pop();
		return STATIC_CAST( CMMFFormatEncode*, self);
		}
	else 
		User::Leave(KErrNotSupported);
	return NULL;
	}


/**
*
* ConstructL
* @param aSource
* 
*/
void CMMFAuFormatWrite::ConstructL(MDataSink* aSink)
	{
	iAuFormatConsolidator = new (ELeave) TMMFAuConsolidationMethods<CMMFAuFormatWrite>(*this);

	CMMFFormatWrite::ConstructL(aSink);
	}



/**
*
* ~CMMFAuFormatWrite
*
*/
CMMFAuFormatWrite::~CMMFAuFormatWrite()
	{
	delete iAuFormatConsolidator;
	}
	


/**
*
* NegotiateL
* @param aSource
*
*/
void CMMFAuFormatWrite::NegotiateL(MDataSource& aSource)
	{
	if (aSource.DataSourceType() == KUidMmfAudioInput)
		{
		CMMFDevSound& soundDevice = STATIC_CAST(MMMFAudioInput&, aSource).SoundDevice();
		GetAudioSettingsFromDevice(&soundDevice, iSourceSampleRate, iSourceChannels, iSourceFourCC);
		}
	else if (aSource.DataSourceType() == KUidMmfFormatDecode) 
		{//source is a clip so for now set sink settings to match source
		iSourceSampleRate = STATIC_CAST( CMMFFormatDecode&, aSource ).SampleRate() ;
		iSourceChannels = STATIC_CAST( CMMFFormatDecode&, aSource ).NumChannels() ;
		iSourceFourCC.Set( aSource.SourceDataTypeCode( TMediaId( KUidMediaTypeAudio ) ) ) ;
		
		iSourceWillSampleConvert = STATIC_CAST(CMMFFormatDecode&, aSource).SourceSampleConvert();
		//make the start position the end of the clip
		}
	else
		{
		return;
		}
	//set default sink parameters to be the same as the source
	if (!IsValidSampleRate( iSampleRate ))
		{
		if( IsValidSampleRate( iSourceSampleRate) )
			{
			iSampleRate = iSourceSampleRate; //might have already been set by custom command
			}
		// default to first valid sample rate
		else
			{
			iSampleRate = KMMFFormatDefaultSampleRate;
			}
		}

	if (!iChannels) 
		iChannels = iSourceChannels;
	if (!iBitsPerSample)
		{ 
		//iFourCC.Set( iSourceFourCC );
		TInt errorCode = SetSinkDataTypeCode( iSourceFourCC, KUidMediaTypeAudio);
		if( errorCode != KErrNone )
			{
			//[set the four cc to a default value ]
			User::LeaveIfError( SetSinkDataTypeCode( KMMFFourCCCodePCM16B, KUidMediaTypeAudio) );
			}

		if (iFourCC==KMMFFourCCCodePCM8 || iFourCC==KMMFFourCCCodeMuLAW || iFourCC==KMMFFourCCCodeALAW) 
			iBitsPerSample = 8;
		else 
			iBitsPerSample = 16; //default to 16
		}
	
	//now re-evaluate the frame  size & interval
	CalculateFrameSize();

	// if we're converting suggest a frame size to the source format
	if (aSource.DataSourceType() == KUidMmfFormatDecode) 
		STATIC_CAST( CMMFFormatDecode&, aSource ).SuggestSourceBufferSize( iFrameSize ); // for now suggest format src takes same buf size as sink??
	}



/**
*
* DetermineIfValidAuClip
* @param aClip
* @return TInt
*
*/
TInt CMMFAuFormatWrite::DetermineIfValidClip()
	{
	TRAPD(err,DoReadL(0 )) ; //read from the beginning of the clip
	if (err)
		{
		return err;
		}
	if ( iBuffer->Data().Size() > STATIC_CAST(TInt, iHeaderLength) )
		{//we have a clip header so get info
		// The header is in iBuffer
		const TUint8* pos = iBuffer->Data().Ptr() ;
		TUint8 offset = 0 ;
		iHeader.iMagicNumber = Read32( pos + offset ) ;
		if ( iHeader.iMagicNumber == KAuSignature ) 
			err = KErrNone 	; //
		else
			err = KErrUnknown ;  
		}
	else if (iBuffer->Data().Size() ==0)
		err = KErrNotFound; //no file present
	else
		err = KErrCorrupt;

	return err;
	}





/**
*
* SetDefaultHeaderParametersL
*
*/
void CMMFAuFormatWrite::SetDefaultHeaderParametersL()
	{
	//If this is a new file, set initial data parameters, else retain the existing settings.
	//This situation can occur if the file has been deleted 
	if(!iClipAlreadyExists)
		{
		//Need an initial frame size, so assume certain settings
		iChannels      = KMMFFormatDefaultChannels;		       // can't have stereo
		iSampleRate    = KMMFFormatDefaultSampleRate;  // assume 8KHz for now
		iBitsPerSample = KMMFFormatDefaultBitsPerSample; // default bits per sample
		CalculateFrameSize();

		//these values must be set via Negotiation or explicitly set via a custom command
		iChannels      = 0;
		iSampleRate    = 0;
		iBitsPerSample  =0;	

		iFourCC.Set(KMMFFourCCCodePCM16B); //16 bit PCM
		}

	iDataLength = 0;
	iStartPosition = iClipLength = iHeaderLength = KAuFileUncompressedDataHeaderSize;

	iState = EInitialised;
	}


/**
*
* SetSinkDataTypeCode
* @param aSinkFourCC
* @param aMediaId
* @return Tint
*
*/
TInt CMMFAuFormatWrite::SetSinkDataTypeCode(TFourCC aSinkFourCC, TMediaId aMediaId)
	{
	if (aMediaId.iMediaType != KUidMediaTypeAudio) 
		return KErrNotSupported;
	if (aSinkFourCC == iFourCC)
		return KErrNone;
	else if (iClipAlreadyExists)
		return KErrAlreadyExists; //can't set fourCC if clip exists - it already has a code
	else
		{
		if ((aSinkFourCC == KMMFFourCCCodePCM8) ||
			(aSinkFourCC == KMMFFourCCCodeMuLAW) ||
			(aSinkFourCC == KMMFFourCCCodeALAW))
			{
			iBitsPerSample = 8;
			iFourCC = aSinkFourCC;
			}
		else if (aSinkFourCC == KMMFFourCCCodePCM16B)
			{
			iBitsPerSample = 16;
			iFourCC = aSinkFourCC;
			}
		else
			return KErrNotSupported;
		}
	return KErrNone;
	}


/**
*
* ProcessFormatHeaderL
* @internalTechnology
*/
void CMMFAuFormatWrite::ProcessFormatHeaderL()
	{
	__ASSERT_ALWAYS( iAuFormatConsolidator, Panic( EPreconditionViolation ) );
	iAuFormatConsolidator->ProcessFormatHeaderL();
	}


/**
*
* WriteHeaderL
* @internalTechnology
*/
void CMMFAuFormatWrite::WriteHeaderL()
	{
	if (iClipAlreadyExists) 
		iDataLength = iClipLength - iHeader.iHeaderSize;
	else 
		iDataLength = iClipLength - iHeaderLength;
		
	if (iClipAlreadyExists)
		{
		iBuffer->Data().SetLength( sizeof( TUint32 ) ) ; 
		TUint8* bufPtr = &iBuffer->Data()[0]; // Needn't be word aligned
		iClipLength = iDataLength + iHeader.iHeaderSize ;
		Write32(bufPtr, iDataLength);
		DoWriteL( KAuDataSizeOffset ); // offset of size data in header
		}
	else //clip does not already exist - need to write the entire header
		{		
		iBuffer->Data().SetLength(iHeaderLength); 
		TUint8* bufPtr = &iBuffer->Data()[0]; // Needn't be word aligned
		iClipLength = iDataLength + iHeaderLength;
		Write32( bufPtr, KAuSignature) ;
		Write32( bufPtr, iHeaderLength);
		Write32( bufPtr, iDataLength);
		
		if ( iFourCC == KMMFFourCCCodeMuLAW)
			Write32( bufPtr, 1 ) ; // 8 bit ISDN uLaw
		else if ( iFourCC == KMMFFourCCCodePCM8)  // 8 bit linear PCM
			Write32( bufPtr, 2 ) ;
		else if ( iFourCC == KMMFFourCCCodePCM16B)
			Write32( bufPtr, 3 ) ; // 16 bit linear PCM
		else if ( iFourCC == KMMFFourCCCodeALAW)
			Write32( bufPtr, 27 ) ; // 8 bit ISDN aLaw
		else 
			User::Leave( KErrNotSupported ) ;
		
		Write32( bufPtr, iSampleRate ) ;
		Write32( bufPtr, iChannels ) ;
		DoWriteL(0); //write the header at the beginning of the clip
		}
	iFileHasChanged = ETrue;
	iHeaderUpdated = ETrue;
	}




/**
*
* GetSupportedDataTypesL
* @param aMediaId
* @param aDataTypes
*
*/
void CMMFAuFormatWrite::GetSupportedDataTypesL(TMediaId aMediaId, RArray<TFourCC>& aDataTypes)
	{
	__ASSERT_ALWAYS( iAuFormatConsolidator, Panic( EPreconditionViolation ) );
	iAuFormatConsolidator->GetSupportedDataTypesL(aMediaId,aDataTypes);
	}


// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs

const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidFormatAURead,	CMMFAuFormatRead::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KMmfUidFormatAUWrite, CMMFAuFormatWrite::NewL)
	};

/**
ImplementationGroupProxy

*/
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}


