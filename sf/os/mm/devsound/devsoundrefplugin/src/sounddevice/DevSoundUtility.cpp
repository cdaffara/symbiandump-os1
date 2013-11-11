// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Class that provides API to list ECOM plugin implementation IDs
// 
//

#include <e32std.h>
#include <barsc.h>
#include <barsread.h>
#include <mmfbase.h>
#include <mmfplugininterfaceuids.hrh>
#include <fixedsequence.rsg>
#include "DevSoundUtility.h"
#include <mmf/common/mmfcontroller.h> //needed for CleanupResetAndDestroyPushL()
#include <mm/mmpluginutils.h>
#ifdef SYMBIAN_MULTIMEDIA_CODEC_API
#include <mdf/codecapiresolverutils.h>
#include <mdf/codecapiresolverdata.h>
#include <mdf/codecapiuids.hrh>
#include <mdf/codecapiresolver.hrh>
#endif // SYMBIAN_MULTIMEDIA_CODEC_API

_LIT(KFixedSequenceResourceFile, "Z:\\Resource\\DevSound\\FixedSequence.rsc"); 
const TInt KFourCCStringLength = 9;

inline TMMFRawPackage::TMMFRawPackage(TInt aDerivedSize)
#pragma warning( disable : 4355 )	// 'this' : used in base member initializer list
: iThis((TUint8*)this,aDerivedSize,aDerivedSize) 
#pragma warning( default : 4355 )
	{ 
	}

inline TPtr8& TMMFRawPackage::Package()
	{ 
	((TMMFRawPackage*)this)->iThis.Set((TUint8*)this,iThis.Length(),iThis.MaxLength()); 
	return iThis; 
	}

inline const TPtr8& TMMFRawPackage::Package() const
	{ 
	((TMMFRawPackage*)this)->iThis.Set((TUint8*)this,iThis.Length(),iThis.MaxLength()); 
	return iThis; 
	}

inline void TMMFRawPackage::SetSize(TInt aDerivedSize)
	{ 
	iThis.Set((TUint8*)this,aDerivedSize,aDerivedSize);
	}

inline TMMFToneFixedSequenceNames::TMMFToneFixedSequenceNames() :
TMMFRawPackage(sizeof(TMMFToneFixedSequenceNames)) {}

#ifdef _UNICODE
class TNameBuf : public TBufCBase16
#else
class TNameBuf : public TBufCBase8
#endif
	{
	friend class HMMFToneFixedSequenceNames;
	};

HMMFToneFixedSequenceNames::HMMFToneFixedSequenceNames()
	{
	iCount = 0;
	}

HMMFToneFixedSequenceNames* HMMFToneFixedSequenceNames::AddNameL(const TDesC& aName)
// Append a copy of the supplied descriptor to the end of the 
// current heap cell. This will involve a realloc that will normally
// result in the object moving
	{
	TInt size = Package().Length();
	TInt desSize = aName.Size() + sizeof(TInt);
	if (desSize&3) 
		desSize = ((desSize+4)&(~3)); // Must round up to word boundary to keep aligned
	HMMFToneFixedSequenceNames* self = REINTERPRET_CAST(HMMFToneFixedSequenceNames*,User::ReAllocL(STATIC_CAST(TAny*,this),size + desSize));
	TUint8* newDesPtr = REINTERPRET_CAST(TUint8*,self) + size;
	Mem::FillZ(newDesPtr,desSize);
	TNameBuf* newDes = REINTERPRET_CAST(TNameBuf*,newDesPtr);
	newDes->Copy(aName,aName.Length());
	self->SetSize(size+desSize);
	self->iCount++;
	return self;
	}


/******************************************************************
 *				CMMFDevSoundUtility
 ******************************************************************/
CMMFDevSoundUtility::CMMFDevSoundUtility()
	{
	// No default implementation
	}


CMMFDevSoundUtility::~CMMFDevSoundUtility()
	{
	delete iInfo;
	delete iFixedSequenceNames;
	}

CMMFDevSoundUtility* CMMFDevSoundUtility::NewL()
	{
	CMMFDevSoundUtility* self = NewLC();
	CleanupStack::Pop();
	return self;
	}

CMMFDevSoundUtility* CMMFDevSoundUtility::NewLC()
	{
	CMMFDevSoundUtility* self = new(ELeave) CMMFDevSoundUtility();
	CleanupStack::PushL(self);
	self->ConstructL();
	// Leave it on Cleanupstack
	return self;
	}

void CMMFDevSoundUtility::ConstructL()
	{
	iFixedSequenceNames = new (ELeave) HMMFToneFixedSequenceNames;
	}

void CMMFDevSoundUtility::SeekUsingFourCCL(TUid aInterfaceUid, RImplInfoPtrArray& aPlugInArray, const TFourCC& aSrcDataType, const TFourCC& aDstDataType, const TDesC& aPreferredSupplier)
	{

	// Create a match string using the two FourCC codes.
	_LIT8(KEmptyFourCCString, "    ,    ");
	TBufC8<KFourCCStringLength> fourCCString(KEmptyFourCCString);
	TPtr8 fourCCPtr = fourCCString.Des();
	TPtr8 fourCCPtr1(&fourCCPtr[0], 4);
	TPtr8 fourCCPtr2(&fourCCPtr[5], 4 );
	aSrcDataType.FourCC(&fourCCPtr1);
	aDstDataType.FourCC(&fourCCPtr2);

	MmPluginUtils::FindImplementationsL(aInterfaceUid, aPlugInArray, fourCCPtr);

	// If more than one match.  Narrow the search by preferred supplier
	if((aPlugInArray.Count() > 1) && aPreferredSupplier.Length())
		{
		SelectByPreference( aPlugInArray, aPreferredSupplier );
		}		

	// If there are no plugins, return failure
	if(aPlugInArray.Count() == 0)
		{
		User::Leave( KErrNotFound );
		}	
	}
	
	
#ifdef SYMBIAN_MULTIMEDIA_CODEC_API
void CMMFDevSoundUtility::FindHwDeviceAdapterL(TUid aInterfaceUid, RImplInfoPtrArray& aPlugInArray)
	{
	// Create a match string using the two FourCC codes.
	_LIT8(KAdapterMatch, "*");

	MmPluginUtils::FindImplementationsL(aInterfaceUid, aPlugInArray, KAdapterMatch);

	// If there are no plugins, return failure

	if(aPlugInArray.Count() == 0)
		{
		User::Leave( KErrNotFound );
		}		
	}
	
void CMMFDevSoundUtility::SeekCodecPluginsL(RArray<TFourCC>& aSupportedDataTypes, TMMFState aState, TBool aAppend)	
	{
	_LIT8(KPCM16FourCCString, " P16");
	//check argument precondition for aState
	if ((aState != EMMFStatePlaying) && (aState != EMMFStateRecording))
		{
		User::Leave(KErrArgument);		
		}		

	if (!aAppend)
		{
		aSupportedDataTypes.Reset(); //clear any existing data in aSupportedDataTypes array if not appending
		}
	
	CCodecApiResolverData* customMatchData = CCodecApiResolverData::NewLC();
	if (aState == EMMFStatePlaying)
		{
		customMatchData->SetMatchType(EMatchOutputDataFormat);
		customMatchData->SetOutputDataL(KPCM16FourCCString);
		}
	else
		{
		customMatchData->SetMatchType(EMatchInputDataFormat);
		customMatchData->SetInputDataL(KPCM16FourCCString);
		}

	customMatchData->SetImplementationType(TUid::Uid(KUidAudioCodec));
	
	HBufC8* package = customMatchData->NewPackLC();	
	
	RImplInfoPtrArray ecomArray;
	CleanupResetAndDestroyPushL(ecomArray);

	TInt err = KErrNone;
	MmPluginUtils::FindImplementationsL(TUid::Uid(KUidMdfProcessingUnit), ecomArray, *package, TUid::Uid(KUidCodecApiResolverImplementation));

	for (TInt i=0 ; i < ecomArray.Count() ; i++)
		{
		CCodecApiOpaqueData* data = NULL;
		TRAP(err, data =  CCodecApiOpaqueData::NewL(ecomArray[i]->OpaqueData()));
		// simply ignore plugins that we can't parse the opaque data. They should not cause other plugins
		// to fall over
		if (err == KErrNone)
			{
			const TDesC8* dataType;
			if (aState == EMMFStatePlaying)
				{
				dataType = &data->InputDataType();	
				}
			else
				{
				dataType = &data->OutputDataType();	
				}
			TFourCC fourCC(*dataType);
			delete data;
			aSupportedDataTypes.AppendL(fourCC);
			}		
		}
	CleanupStack::PopAndDestroy(3, customMatchData);
	}
#endif // SYMBIAN_MULTIMEDIA_CODEC_API

/*
 * local function to disable items which do not match the preferred supplier.
 * Note that at least one enabled item is returned (if there was an enabled item to begin with) which
 * may not match the preferred supplier.
 * 
 */
void CMMFDevSoundUtility::SelectByPreference( RImplInfoPtrArray& aPlugInArray, const TDesC& aPreferredSupplier ) 
	{

	// Use the Disabled flag to eliminated all currently enabled matches that
	// do not match the preferred supplier.
	TInt firstEnabled = -1 ; // to ensure that we return something valid
	TInt matchCount = 0 ;
	for ( TInt ii = 0 ; ii < aPlugInArray.Count() ; ii++ )
		{
		if ( !( aPlugInArray[ii]->Disabled() ) )
			{
			if ( firstEnabled == -1 )
				firstEnabled = ii ;
			if ( aPlugInArray[ii]->DisplayName().FindF( aPreferredSupplier ) == KErrNotFound )
				aPlugInArray[ii]->SetDisabled( ETrue ) ;
			else
				matchCount++ ;
			}
		}

	// If there are no matches then re-enable the first enabled
	if ( matchCount == 0 )
		aPlugInArray[firstEnabled]->SetDisabled( EFalse ) ;
	else if ( matchCount > 1 )
		{
		// find the latest version from more than one match
		TInt highestVersionIndex = -1 ;
		for ( TInt ii = 0 ; ii < aPlugInArray.Count() ; ii++ )
			{
			if ( !( aPlugInArray[ii]->Disabled() ) )  // only interested in enabled elements
				{
				if ( highestVersionIndex == -1 )
					{ // first match.  Store this.  Keep it enabled
					highestVersionIndex = ii ;
					}
				else if ( aPlugInArray[ii]->Version() > aPlugInArray[highestVersionIndex]->Version() )
					{ // a new leader.  Disable the previous leader.  Keep this one.
					aPlugInArray[highestVersionIndex]->SetDisabled( ETrue ) ;
					highestVersionIndex = ii ;
					}
				else  // we already have a higher version.
					aPlugInArray[ii]->SetDisabled( ETrue ) ;
				}
			}
		}
	}


/*
 *  SeekHwDevicePluginsL
 *  This method looks for hwDevicePlugins that support the state given in aState which
 *  must be either EMMFStatePlaying or EMMFStateRecording
 *  For each HwDevice plugin found the datatype as indicated by its fourCC code
 *  from the default_data field in the resource file is added to the array of aSupportedDataTypes
 *
 *  @internalComponent
 *
 *	@param	"RArray<TFourCC>& aSupportedDataTypes"
 *  an array of fourCC codes that has a fourCC code added to for each hardware device found
 *
 *	@param  "TMMFState aState"
 *  must be set to EMMFStatePlaying if seeking HwDevice plugins that support play and
 *  EMMFStateRecording if seeking HwDevice plugins that support record
 *	
 *  @leave KErrArgument if aState is not EMMFStatePlaying or EMMFStateRecording else leaves
 *  with standard symbian OS error code
 */
void CMMFDevSoundUtility::SeekHwDevicePluginsL(RArray<TFourCC>& aSupportedDataTypes, TMMFState aState)
	{	
	//check argument precondition for aState
	if ((aState != EMMFStatePlaying) && (aState != EMMFStateRecording))
		{
		User::Leave(KErrArgument);
		}

	aSupportedDataTypes.Reset(); //clear any existing data in aSupportedDataTypes array

	RImplInfoPtrArray plugInArray ; // Array to return hw device plugin resource info
	CleanupResetAndDestroyPushL(plugInArray);

	TUid KUidMmfHWPluginInterfaceCodec = {KMmfUidPluginInterfaceHwDevice};

	MmPluginUtils::FindImplementationsL(KUidMmfHWPluginInterfaceCodec, plugInArray);

	TUint numberOfHwDevicePlugins = plugInArray.Count();

	//if have hwdevice plugin resource entries then scan entries
	//matching on a datatype of pcm16 as the destination datatype for play and the 
	//source datatype for record
	//if a match is found and isn't already in list of supported data types
	//then add it to the list
	if (numberOfHwDevicePlugins)
		{					
		CImplementationInformation* hwDeviceResourceEntry = NULL;
		_LIT8(KPCM16FourCCString, " P16");
		TBufC8<KFourCCLength> fourCCStringPCM16(KPCM16FourCCString);
		TPtr8 fourCCPtrPCM16 = fourCCStringPCM16.Des();
		TUint entryNumber = 0;

		//check each resource entry for dst 4CC = P16 for play and src 4CC = P16 for record
		for (TUint hwDeviceEntry = 0; hwDeviceEntry < numberOfHwDevicePlugins; hwDeviceEntry++)
			{
			hwDeviceResourceEntry = plugInArray[hwDeviceEntry];
			if (IsDataTypeMatch(hwDeviceResourceEntry, fourCCPtrPCM16, aState))
				{//resource entry data field has dest/src datatype ' P16' ie pcm16 for play/record
				TPtrC8 fourCCPtr(0,0);
				if (aState == EMMFStatePlaying)//then datatype supported 4CC is left 4 chars
					{
					fourCCPtr.Set(hwDeviceResourceEntry->DataType().Left(KFourCCLength));
					}
				else if (aState == EMMFStateRecording) //then datatype supported 4CC is right 4 chars
					{
					fourCCPtr.Set(hwDeviceResourceEntry->DataType().Right(KFourCCLength));
					}
				TFourCC fourCCEntry(fourCCPtr);
				//need to check if entry already exists to prevent duplicate entries
				entryNumber = aSupportedDataTypes.Count();
				TBool alreadyExists = EFalse;
				for (TUint fourCCEntryNumber = 0; fourCCEntryNumber < entryNumber; fourCCEntryNumber++)
					{
					if (aSupportedDataTypes[fourCCEntryNumber]==fourCCEntry)
						{
						alreadyExists = ETrue;//we already have this 4CC in the supported data types
						break;
						}
					}
				if (!alreadyExists)
					{
					TInt err = aSupportedDataTypes.Append(fourCCEntry);
					if (err)
						{//note we don't destroy array because we don't own it
						//but we do reset it as it is incomplete
						aSupportedDataTypes.Reset();
						User::Leave(err);
						}
					}
				}//if (IsDataTypeMatch(hwDeviceResourceEntry, fourCCPtrPCM16, aState))
			}//for (TUint hwDeviceEntry = 0; hwDeviceEntry < numberOfHwDevicePlugins; hwDeviceEntry++)
		}//if (numberOfHwDevicePlugins)

	CleanupStack::PopAndDestroy(&plugInArray);
	}


/*
 *	IsDataTypeMatch
 *  This method takes a given resource entry from a hardware device and determines 
 *  whether the hwdevice plugin is a data type match for playing or recording
 *  depending on the setting of aState
 *  The method matchs the default_data field from the hw device resource entry matching
 *  it with the aHwMatchFourCC code.
 *
 *  @internalComponent
 *
 *	@param	"CImplementationInformation aHwDeviceResourceEntry"
 *  the hw device resource entry that is to be checked 
 *  whether it can be used to play or record
 *
 *	@param  "TDesC8& aHwMatchFourCC
 *	the data type fourCC code to match to that the hardware device that must convert to for
 *	playing and convert from for recording - for the reference DevSound this is always ' P16' ie pcm16
 *
 *  @param "TMMFState aState"
 *  this determines whether the match is for playing or recording and should take 
 *  either the values EMMFStatePlaying or EMMFStateRecording
 *
 *  @return ETrue if a match for play or record else EFalse
 */
TBool CMMFDevSoundUtility::IsDataTypeMatch(CImplementationInformation* aHwDeviceResourceEntry,const TDesC8& aHwMatchFourCC, TMMFState aState)
	{
	TBool match = EFalse;
	// extra length safety check to remove adapter plugins and incorrect ones
	if (aHwDeviceResourceEntry->DataType().Length()>=KFourCCStringLength)
		{
		if (aState == EMMFStatePlaying)
			{//play need to match with the right four characters
			match =  (!(aHwMatchFourCC.Match(aHwDeviceResourceEntry->DataType().Right(KFourCCLength))==KErrNotFound));
			}
		else if (aState == EMMFStateRecording)
			{//record need to match with the left four characters
			match = (!(aHwMatchFourCC.Match(aHwDeviceResourceEntry->DataType().Left(KFourCCLength))==KErrNotFound));
			}
		}
	return match;
	}


/**
 *	Populate fixed sequences
 *
 */
void CMMFDevSoundUtility::InitializeFixedSequenceL(CPtrC8Array** aFixedSequences)
	{

	RFs fsSession;
	User::LeaveIfError(fsSession.Connect());
	CleanupClosePushL(fsSession);

	// Open the resource file
	RResourceFile resourceFile;
	resourceFile.OpenL(fsSession, KFixedSequenceResourceFile);
	CleanupClosePushL(resourceFile);

	// Allocate buffer to hold resource data in binary format
	iInfo = resourceFile.AllocReadL(FIXED_TONE_SEQUENCE);

	TResourceReader reader;
	reader.SetBuffer(iInfo);

	// Create array to hold fixed sequences data
	CPtrC8Array* tempSequences = new(ELeave) CPtrC8Array(8); //  granularity
	CleanupStack::PushL(tempSequences);

	// First word gives number of entries
	TInt numberOfEntries = reader.ReadUint16(); 
	ASSERT(!(numberOfEntries&1)); // Should have atleast one entry

	// There must be an even number entries as each sequence structure
	// is made of a name string and a data string (SEQUENCE_NAME and SEQUENCE_DATA)

	HMMFToneFixedSequenceNames* names = new (ELeave) HMMFToneFixedSequenceNames;
	CleanupStack::PushL(names);
	for (TInt i=0;i<numberOfEntries;i+=2)
		{
		// Copy name from resource array to returnable array
		HMMFToneFixedSequenceNames* newNames = names->AddNameL(reader.ReadTPtrC());
		if (names != newNames)
			{ // May have moved so fixup cleanupstack reference
			CleanupStack::Pop();
			names = newNames;
			CleanupStack::PushL(names);
			}
		TInt len = reader.ReadUint16();
		TPtrC8 tempTPtrC8(REINTERPRET_CAST(const TUint8*,reader.Ptr()),len<<1);
		tempSequences->AppendL(tempTPtrC8);
		reader.Advance(len<<1);
		}
	CleanupStack::Pop(); // names

	// Delete the old fixed sequence names
	delete iFixedSequenceNames;
	iFixedSequenceNames = NULL;
	iFixedSequenceNames = names;

	*aFixedSequences = tempSequences;
	CleanupStack::Pop(tempSequences);
	CleanupStack::PopAndDestroy(2);	// resourceFile, fsSession
	}

TBool CMMFDevSoundUtility::RecognizeSequence(const TDesC8& aData)
	{
	// Reference plug-in only supports its own sequence format
	_LIT8(KSequenceSignature,"SQNC");
	if (aData.Length() > 4)
		{
		if (aData.Left(4) == KSequenceSignature)
			return ETrue;
		}
	// Didn't recognise
	return EFalse;
	}

const TDesC& CMMFDevSoundUtility::FixedSequenceName(TInt aSequenceNumber)
	{
	ASSERT(iFixedSequenceNames); // Defect if this not true when previous was true
	ASSERT((aSequenceNumber>=0)&&(aSequenceNumber<iFixedSequenceNames->iCount));

	// Ptr to first descriptor
	TUint8* ptr = REINTERPRET_CAST(TUint8*,&(iFixedSequenceNames->iCount))+sizeof(TInt);
	TDesC* desPtr = REINTERPRET_CAST(TDesC*,ptr); // First des
	while (aSequenceNumber--)
		{
		TInt size = desPtr->Size();
		if (size&3)
			size = ((size+4)&(~3));
		ptr += sizeof(TInt) + size;
		desPtr = REINTERPRET_CAST(TDesC*,ptr); // Next des
		}
	return *desPtr;
	}
