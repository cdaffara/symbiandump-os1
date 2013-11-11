//a3ffourcclookup.cpp

// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <a3f/a3ffourcclookup.h>
#include "a3ffourcclookup.hrh"
#include <a3f/a3f_trace_utils.h>
#include <mm/mmcleanup.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <a3f/a3f_trace_ctxt_def.h>
#endif
_LIT8(KHexPrefix, "0x");
_LIT8(KHexPrefix2, "0X");
_LIT8(uTag,"<u>");
_LIT8(mTag,"<m>");
_LIT8(fTag,"<f>");

const TInt KUidLength = 10;
const TInt KTagLength = 3;
const TInt KFourCCLength = 4;

// ---------------------------------------------------------------------------
// Function to compute Hash value
// ---------------------------------------------------------------------------
TUint32 TIntegerHash(const TUint32& aInt)
	{
	return DefaultHash::Integer(aInt);
	}
	
// ---------------------------------------------------------------------------
// Function to Check similar identities from Hash Table
// ---------------------------------------------------------------------------
TBool TIntegerIdent(const TUint32& /*aL*/, const TUint32& /*aR*/)
	{
	return ETrue;
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
CFourCCConvertor::~CFourCCConvertor()
	{
	if(iHashFourCCToFormat)
		{
		iHashFourCCToFormat->Close();
		delete iHashFourCCToFormat;		
		iHashFourCCToFormat = NULL;
		}
	if(iHashFormatToFourCC)
		{
		iHashFormatToFourCC->Close();	
		delete iHashFormatToFourCC;
		iHashFormatToFourCC = NULL;
		}	
	}
	
// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
CFourCCConvertor::CFourCCConvertor()
	{
	TRACE_CREATE();
	DP_CONTEXT(CFourCCConvertor::CFourCCConvertor *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CFourCCConvertor::NewL
// ---------------------------------------------------------------------------
EXPORT_C CFourCCConvertor* CFourCCConvertor::NewL()
	{
	DP_STATIC_CONTEXT(CFourCCConvertor::NewL *CD0*, CtxDevSound, DPLOCAL);
	DP_IN();
	CFourCCConvertor* self = new(ELeave) CFourCCConvertor();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	DP0_RET(self, "0x%x");
	}
		
// ---------------------------------------------------------------------------
// Second Phase Constructor
// ---------------------------------------------------------------------------
void CFourCCConvertor::ConstructL()
	{
	DP_CONTEXT(CCFourCCConvertor::ConstructL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iHashFourCCToFormat = new(ELeave) RHashMap<TUint32,TUint32>(&TIntegerHash,&TIntegerIdent);
	iHashFormatToFourCC = new(ELeave) RHashMap<TUint32,TUint32>(&TIntegerHash,&TIntegerIdent);
	LoadL();
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CFourCCConvertor::LoadL
// ---------------------------------------------------------------------------
void CFourCCConvertor::LoadL()
	{
	DP_CONTEXT(CCFourCCConvertor::LoadL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TUid KFourCCInterfaceDefinitionUid = {KUidA3fFourCCConvertorPlugin};

	RImplInfoPtrArray ecomArray;
	REComSession::ListImplementationsL(KFourCCInterfaceDefinitionUid, ecomArray);
	CleanupResetAndDestroyPushL(ecomArray);

	ASSERT(iHashFourCCToFormat);
	ASSERT(iHashFormatToFourCC);
	
	TInt index;
	index = ecomArray.Count();

	if(index == 0)
		{
		User::Leave(KErrNotFound);
		}
	TInt tagPosition=0;
	TUint32 formatIdValue = 0;
	
	// Construct the hash map
	for (TInt i=0; i<index; i++)
		{
		// Get opaque data field
		const TPtrC8 opaquedata(ecomArray[i]->OpaqueData());
		TInt length = opaquedata.Length();
				
		// Extracting format uid
		tagPosition = opaquedata.Find(uTag);
		
		if( (tagPosition >= 0) && ( (length-tagPosition) >= (KUidLength + KTagLength) ) )
			{
			// Format Uid
			if(((((opaquedata).Mid(tagPosition + KTagLength, KUidLength) ).Length()) == KUidLength) &&
				((((opaquedata).Mid(tagPosition + KTagLength, KUidLength)).Find(KHexPrefix)) ==0) ||
				((((opaquedata).Mid(tagPosition + KTagLength, KUidLength)).Find(KHexPrefix2)) ==0))
				{
				TLex8 lexFormatUid(((opaquedata).Mid(tagPosition + KTagLength, KUidLength)).Right(8));
				User::LeaveIfError(lexFormatUid.Val(formatIdValue,EHex));
				}
			// Check if this is master FourCC
			TInt tagMasterFourCC=0;
			tagMasterFourCC= (opaquedata).Find(mTag);

			// Extract fourCC
			tagPosition = (opaquedata).Find(fTag);
			if((tagPosition >=0) && (length-tagPosition>=(KFourCCLength + KTagLength)))
				{
				TFourCC tempFourCC((opaquedata).Mid(tagPosition + KTagLength, KFourCCLength));
				if(tempFourCC.FourCC() != 0)
					{
					iHashFourCCToFormat->InsertL(tempFourCC.FourCC(),formatIdValue); 
					}
				// Only fourCC master should be inserted
				if(tagMasterFourCC >= 0)
					{
					iHashFormatToFourCC->InsertL(formatIdValue, tempFourCC.FourCC());
					}
				}
			else
				{
				DP0(DLINFO,"CCFourCCConvertor::<f> tag not found or <f> tag should have four characters entry in resource file ");	
				}
			}
		else
			{
			DP0(DLINFO,"CCFourCCConvertor::<u> tag not found or <u> tag should have ten characters e.g. 0X12122121 entry in resource file");
			}
		}
		
	CleanupStack::PopAndDestroy(&ecomArray);
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CFourCCConvertor::FormatToFourCC
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFourCCConvertor::FormatToFourCC(TUid aFormat,TFourCC &aFourCC)
	{
	DP_CONTEXT(CCFourCCConvertor::FormatToFourCC *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	ASSERT(iHashFormatToFourCC);

	TInt err = KErrNone;
	TUint32 *mime = iHashFormatToFourCC->Find(aFormat.iUid);
	if(mime == NULL)
		{
		err = KErrNotFound;
		}
	else
		{
		aFourCC.Set(*mime);
		}
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// CFourCCConvertor::FourCCToFormat
// ---------------------------------------------------------------------------
EXPORT_C TInt CFourCCConvertor::FourCCToFormat(TFourCC aFourCC, TUid &aFormat) const
	{
	DP_CONTEXT(CCFourCCConvertor::FourCCToFormat *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	ASSERT(iHashFourCCToFormat);

	TInt err = KErrNone;
	TUint32 *mime= iHashFourCCToFormat->Find(aFourCC.FourCC());
	if(mime == NULL)
		{
		err = KErrNotFound;
		}
	else
		{
		aFormat.iUid = *mime;
		}
	DP0_RET(err, "%d");
	}
