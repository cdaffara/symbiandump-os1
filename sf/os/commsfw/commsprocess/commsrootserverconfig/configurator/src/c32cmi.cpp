/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#include <f32file.h>
#include "c32cmi.h"
#include <cfextras.h>
#include <e32base.h>
#include "c32cmiutils.h"

/** Implements the parser for .CMI configuration files.
@file
@publishedPartner
@released
*/

/** Name of application for logging
*/
__FLOG_STMT(_LIT8(KSubsys,"C32Start");)

/** Logging second tag
*/
__FLOG_STMT(_LIT8(KComponent,"Events");)

/** .CMI file section "[Loader]" contains the configuration of
the CPM and its bindings
*/
_LIT8(KSectionLoader,		"Loader");

/** .CMI optional file section "[IniData]" contains application specific
data.
*/
_LIT8(KSectionIniData,		"IniData");

/** Attribute which can be found in a .CMI file in the [loader] section.
*/
_LIT8(KAttrName,			"Name");

/** Attribute which can be found in a .CMI file in the [loader] section.
*/
_LIT8(KAttrFileName,		"FileName");

/** Attribute which can be found in a .CMI file in the [loader] section.
*/
_LIT8(KAttrIniDataFile,		"IniData");

/** Attribute which can be found in a .CMI file in the [loader] section.
*/
_LIT8(KAttrIsServer,		"IsServer");

/** Attribute which can be found in a .CMI file in the [loader] section.
*/
_LIT8(KAttrPriority,		"Priority");

/** Attribute which can be found in a .CMI file in the [loader] section.
*/
_LIT8(KAttrStackSize,		"StackSize");

/** Attribute which can be found in a .CMI file in the [loader] section.
*/
_LIT8(KAttrHeapOption,		"HeapOption");

/** Attribute which can be found in a .CMI file in the [loader] section.
*/
_LIT8(KAttrStartSequence,	"StartSequence");

/** Attribute which can be found in a .CMI file in the [loader] section.
*/
_LIT8(KAttrScaledStartupState,	"ScaledStartupState");

/** Attribute which can be found in a .CMI file in the [loader] section.
*/
_LIT8(KAttrMinHeapSize,		"MinHeapSize");

/** Attribute which can be found in a .CMI file in the [loader] section.
*/
_LIT8(KAttrMaxHeapSize,		"MaxHeapSize");

/** Attribute which can be found in a .CMI file in the [loader] section.
*/
_LIT8(KAttrSharedHeapName,	"SharedHeapName");

/** Attribute which can be found in a .CMI file in the [loader] section.
*/
_LIT8(KAttrThreadFuncOrdinal,"ThreadFunctionOrdinal");

/** Attribute which can be found in a .CMI file in the [loader] section.
*/
_LIT8(KAttrIsSticky,		"IsSticky");

/** Attribute which can be found in a .CMI file in the [loader] section.
*/
_LIT8(KAttrSystemCritical,	"SystemCritical");

/** Attribute which can be found in a .CMI file in the [loader] section.
*/
_LIT8(KAttrSystemCriticalAfterInit,	"SystemCriticalAfterInit");

/** Attribute which can be found in a .CMI file in the [loader] section.
*/
_LIT8(KAttrControlFlags,	"ControlFlags");

/**Attribute which can be found in a .CMI file in the [loader] section.
*/
_LIT8(KAttrOnDemand, "OnDemand");

/**Attribute which can be found in a .CMI file in the [loader] section.
*/
_LIT8(KAttrGroupName, "Group");

/** PANICs which can occur during .CMI file parsing.
*/
enum TIniPanic
	{
	/** The name of a section is too long.
	*/
	ESectionNameTooBig,
	/** The name of a variable is too big.
	*/
	EVarNameTooBig
	};

#ifdef _DEBUG
/** Function whch will PANIC the module, but only in debug mode.
*/
static void Panic(TIniPanic aPanic)
	{
	_LIT(KC32CmiData,"C32CmiData");
	User::Panic(KC32CmiData,aPanic);
	}
#endif



/////////////////////////////////////////////////////////////////////////////

C32ParseIniFile* C32ParseIniFile::NewL(const TDesC& aFileName, TAutoClose<RFs>& aFileServer)
	{
	C32ParseIniFile* self = new(ELeave) C32ParseIniFile;
	CleanupStack::PushL(self);
	self->ConstructL(aFileName, aFileServer);
	CleanupStack::Pop();
	return self;
	}

void C32ParseIniFile::ConstructL(const TDesC& aFileName, TAutoClose<RFs>& aFileServer)
	{
	__FLOG_OPEN(KSubsys,KComponent);
    iToken = HBufC8::NewL(KTokenSize+2);	// 2 extra chars for [tokenName]
	ReadFileL(aFileName, aFileServer);
	}

C32ParseIniFile::~C32ParseIniFile()
	{
	delete iCmiData;
	delete iToken;
	__FLOG_CLOSE;
	}

TBool C32ParseIniFile::FindVar(const TDesC8 &aSection, const TDesC8 &aVarName, TPtrC8 &aResult, TInt aEnumerator)
/** Find a variable's value given a section name and a var name.
@param aSection The section to search.
@param aVarName The name that will have aEnumerator appended to it to generate the key that will be searched for.
@param aResult The result.
@param aEnumerator Appended to the var name to generate a the key that will be searched for.
@return Success or fail.
*/
	{
	__ASSERT_DEBUG(aSection.Length()<=KTokenSize,Panic(ESectionNameTooBig));
	__ASSERT_DEBUG(aVarName.Length()<=KTokenSize,Panic(EVarNameTooBig));

	// append enumeration to the key name
	TBuf8<256> aKey(aVarName);
	aKey.AppendNum(aEnumerator);

	return CommsFW::GetVarFromIniData(*iCmiData, aSection, aKey, aResult);
	}

TBool C32ParseIniFile::FindVar(const TDesC8 &aSection, const TDesC8 &aVarName, TPtrC8 &aResult)
/** Find a variable's value given a section name and a var name.
@param aSection The section to search.
@param aVarName The name to find.
@param aResult The result.
@return Success or fail.
*/
	{
	__ASSERT_DEBUG(aSection.Length()<=KTokenSize,Panic(ESectionNameTooBig));
	__ASSERT_DEBUG(aVarName.Length()<=KTokenSize,Panic(EVarNameTooBig));
	return CommsFW::GetVarFromIniData(*iCmiData, aSection, aVarName, aResult);
	}


TBool C32ParseIniFile::FindVar(const TDesC8 &aSection, const TDesC8 &aVarName, TInt &aResult)
/**
@param aSection The section to search.
@param aVarName The name to find.
@param aResult The result.
@return Success or fail.
*/
	{
	TPtrC8 ptr(NULL,0);
	if (FindVar(aSection,aVarName,ptr))
		{
		TRadix radix;
		_LIT8(KHexPrefix, "0x");
		if(ptr.Left(2).CompareF(KHexPrefix) == 0)
			{
			ptr.Set(ptr.Mid(2));
			radix = EHex;
			}
		else
			{
			radix = EDecimal;
			}
		TLex8 lex(ptr);
		TUint32 result;
		if (lex.Val(result, radix)==KErrNone)
			{
			aResult = result;
			return(ETrue);
			}
		}
	return(EFalse);
	}

void C32ParseIniFile::ReadFileL(const TDesC& aFileName, TAutoClose<RFs>& aFileServer)
/** Reads the file content into a narrow-char buffer. By design the
configurator only supports ASCII text, as its purpose is invisible
system configuration.
@param aFileName The name of the file to be read.
*/
	{
	// Open session to the FileServer
	//TAutoClose<RFs> fs;
	TInt result;
	//check if the session is already open
	if(aFileServer.iObj.Handle() == 0)
		{
		if(KErrNone!=(result=aFileServer.iObj.Connect()))
			{
			__FLOG( _L( "Unable to connect to the File Server" ) );
			User::Leave(result);
			}
		}
	//fs.PushL();

	// Open file
	TAutoClose<RFile> rfile;
	result = rfile.iObj.Open(aFileServer.iObj, aFileName, EFileShareReadersOnly);
	if(KErrNone==result)
		{
		rfile.PushL();

		// Get filesize for allocating the buffer for the data
		TInt size;
		result = rfile.iObj.Size(size);
		if(KErrNone!=result)
			{
			__FLOG_1(_L("Couldn't get size of file %S"), &aFileName);
			User::Leave(result);
			}

		if(size<=0)
			{
			__FLOG_1(_L("File %S has no contents"), &aFileName);
			User::Leave(KErrCorrupt);
			}

		// Allocate a buffer that can contain the file data
		iCmiData = HBufC8::NewL(size);
		TPtr8 fileDataPtr = iCmiData->Des();

		// Read the contents of the file into the buffer
		result=rfile.iObj.Read(fileDataPtr);
		if(KErrNone!=result)
			{
			__FLOG_1(_L("Couldn't read contents of file %S"), &aFileName);
			User::Leave(result);
			}
		rfile.Pop();
		}
	//fs.Pop();
	}

TPtrC8 C32ParseIniFile::RetrieveSectionStart(const TDesC8& aSectionName)
/** Returns a byte descriptor containing the data from the start of a section to the
end of the file.
*/
	{
	TPtrC8 section;
	TPtr8 sectionToken = iToken->Des();
	_LIT8(sectionTokenString,"[%S]");
	sectionToken.Format(sectionTokenString, &aSectionName);
	TInt sectionStart = iCmiData->Find(sectionToken);
	if (KErrNotFound==sectionStart)
		return section;

	section.Set(iCmiData->Mid(sectionStart));
	sectionStart += section.Find(TPtrC8(_S8("]")));
	if (KErrNotFound==sectionStart)
		{
		section.Set(KNullDesC8);
		}
	else
		{
		sectionStart++;
		section.Set(iCmiData->Mid(sectionStart, iCmiData->Length()-sectionStart));
		}
	return section;
	}


/////////////////////////////////////////////////////////////////////////////

C32CmiData::C32CmiData()
/** C'tor. Sets th default values for the attributes as well as
the default required attributes.
*/
	{
	__FLOG_OPEN(KSubsys,KComponent);
	iAttrPriority = EPriorityNormal;
//	iAttrThreadFuncOrdinal = 0; // Rootserver uses defaults ordinal when this is set to 0.
	iAttrStackSize = KDefaultStackSize * sizeof(TText); // Bigger stack if unicode
	iAttrRequired = (AttrName|AttrFileName);
	iAttrScaledStartupState = 0x3000;
//	iAttrIsSticky=EFalse;
//	iAttrSystemCritical=EFalse;
//	iAttrControlFlags=0;
	}

C32CmiData::~C32CmiData()
/** D'tor.
*/
	{
	delete iCmiFile;
	__FLOG_CLOSE;
	}

/*static*/ C32CmiData* C32CmiData::NewL(const TDesC& aFileName, TAutoClose<RFs>& aFileServer)
/**
@param aFileName The filename of the file to be read.
@return pointer to new C32CmiData object.
*/
	{
	C32CmiData* p = new(ELeave) C32CmiData;
	CleanupStack::PushL(p);
	p->ConstructL(aFileName, aFileServer);
	CleanupStack::Pop();
	return p;
	}

void C32CmiData::RetrieveAttribute(TInt aAttr, const TDesC8 &aVarName, TInt &aResult)
/**
@param aAttr The attibute set and returned.
@param aVarName The name to be found.
@param aResult The result.
*/
	{
	if(FindVar(KSectionLoader, aVarName, aResult))
		{
		SetAttr(aAttr);
		}
	}

void C32CmiData::RetrieveAttribute(TInt aAttr, const TDesC8 &aVarName, TPtrC8 &aResult)
/** Retrieves an option from the file data and
if it was found set the requested TPtrC to point to it.
@param aAttr The attibute set and returned.
@param aVarName The name to be found.
@param aResult The result.
*/
	{
	if(FindVar(KSectionLoader, aVarName, aResult))
		{
		SetAttr(aAttr);
		}
	}

void C32CmiData::RetrievePriorityL()
/** Retrieves a priority option from the file data. If present, checks it is a valid
TThreadPriority enumerator name, and sets the corresponding priority attribute.
@leave KErrCorrupt If the priority name is invalid
*/
	{
	TPtrC8 priorityName;
	if(FindVar(KSectionLoader, KAttrPriority, priorityName))
		{
		TThreadPriority priority;
		if(C32CmiUtils::ThreadPriorityNameToEnum(priorityName, priority) == KErrNone)
			{
			iAttrPriority = priority;
			SetAttr(AttrPriority);
			return;
			}
		__FLOG_1(_L8("Invalid Priority %S"), &priorityName);
		User::Leave(KErrCorrupt);
		}
	}

TBool C32CmiData::CompareHeapTypes(const TDesC8 &aType1, const TDesC8 &aType2, TRSHeapType aType3)
/** Helper function: Compares two strings and if there
is a match set the AttrHeapOption bit and save the requested value.
@param aType1 The first descriptor.
@param aType2 The second descriptor.
@param aType3 Enum type to be set on a positive match.
@return Success or fail.
*/
	{
	if(0==aType1.CompareF(aType2))
		{
		iAttrHeapOption=aType3;
		SetAttr(AttrHeapOption);
		return ETrue;
		}
	return EFalse;
	}

void C32CmiData::RetrieveHeapTypeL()
/** Determines the heap type required.
*/
	{
	TPtrC8 type;
	if(FindVar(KSectionLoader, KAttrHeapOption, type))
		{
		// List of possible heap types
		_LIT8(KEDefaultHeap, "EDefaultHeap");
		_LIT8(KEShareHeap, "EShareHeap");
		_LIT8(KENewHeap, "ENewHeap");
		if(CompareHeapTypes(type, KEDefaultHeap, EDefaultHeap))
			return;
		if(CompareHeapTypes(type, KEShareHeap, EShareHeap))
			{
			iAttrRequired |= AttrSharedHeapName;
			return;
			}
		if(CompareHeapTypes(type, KENewHeap, ENewHeap))
			{
			iAttrRequired |= (AttrMinHeapSize|AttrMaxHeapSize);
			return;
			}
		__FLOG_1(_L8("Invalid Heap Type %S"), &type);
		User::Leave(KErrCorrupt);
		}
	}

void C32CmiData::RetrieveIniDataSection()
/** Sets a narrow ptr to point from the start of
the inidata to the end of the file.
*/
	{
	TPtrC8 sectionStart = iCmiFile->RetrieveSectionStart(KSectionIniData);
	iAttrIniData.Set(sectionStart.Ptr(), sectionStart.Length());
	SetAttr(AttrIniData);
	}

void C32CmiData::RetrieveAttributesL()
/** Controls extraction of data.
*/
	{
	RetrieveAttribute(AttrName, KAttrName, iAttrName);

	RetrieveAttribute(AttrFileName, KAttrFileName, iAttrFileName);
	RetrieveAttribute(AttrIniDataFile, KAttrIniDataFile, iAttrIniDataFile);
	RetrieveAttribute(AttrIsServer, KAttrIsServer, iAttrIsServer);

	RetrievePriorityL();

	RetrieveAttribute(AttrStackSize, KAttrStackSize, iAttrStackSize);
	RetrieveHeapTypeL();
	TInt scaledStartupState = (TInt) iAttrScaledStartupState;
	RetrieveAttribute(AttrScaledStartupState, KAttrScaledStartupState, scaledStartupState);
	iAttrScaledStartupState = (TUint32) scaledStartupState;
	// The original start sequence scheme is remapped to the scaled startup states
	TInt attrStartSequence = KC32LowStartSequenceCeiling;
	RetrieveAttribute(AttrStartSequence, KAttrStartSequence, attrStartSequence);
	if(AttrIsSet(AttrStartSequence))
		{
		if(AttrIsSet(AttrScaledStartupState))
			{
			__FLOG(_L("ERROR: Both StartSequence and ScaledStartupState given"));
			User::Leave(KErrCorrupt);
			}
		if(attrStartSequence < KC32LowStartSequenceCeiling)
			{
			iAttrScaledStartupState = KC32LowStartSequenceScaleBase + attrStartSequence;
			}
		else if(attrStartSequence < KC32MidStartSequenceCeiling)
			{
			iAttrScaledStartupState = KC32MidStartSequenceScaleBase + attrStartSequence;
			}
		else
			{
			iAttrScaledStartupState = KC32HighStartSequenceScaleBase + attrStartSequence;
			}
		__FLOG_2(_L("...StartSequence %d converted to ScaledStartupState 0x%x"), attrStartSequence, iAttrScaledStartupState);
		SetAttr(AttrScaledStartupState);
		}
	else
		{
		__FLOG_1(_L("...ScaledStartupState 0x%x"), iAttrScaledStartupState);
		}
	RetrieveAttribute(AttrMinHeapSize, KAttrMinHeapSize, iAttrMinHeapSize);
	RetrieveAttribute(AttrMaxHeapSize, KAttrMaxHeapSize, iAttrMaxHeapSize);
	RetrieveAttribute(AttrSharedHeapName, KAttrSharedHeapName, iAttrSharedHeapName);
	RetrieveAttribute(AttrThreadFuncOrdinal, KAttrThreadFuncOrdinal, iAttrThreadFuncOrdinal);
	RetrieveAttribute(AttrIsSticky, KAttrIsSticky, iAttrIsSticky);
	RetrieveAttribute(AttrSystemCritical, KAttrSystemCritical, iAttrSystemCritical);
	RetrieveAttribute(AttrSystemCriticalAfterInit, KAttrSystemCriticalAfterInit, iAttrSystemCriticalAfterInit);

	//Added for on demand loading
	RetrieveAttribute(AttrOnDemand, KAttrOnDemand, iAttrOnDemand);
	RetrieveAttribute(AttrGroupName, KAttrGroupName, iAttrGroupName);

	TInt controlFlags = 0;
	RetrieveAttribute(AttrControlFlags, KAttrControlFlags, controlFlags);
	iAttrControlFlags = (TUint32) controlFlags;

 	RetrieveIniDataSection();

	// Check that the required attributes were set
	if(!AttrIsSet(iAttrRequired))
		{
		__FLOG(_L("Required attributes not set!"));
		User::Leave(KErrCorrupt);
		}
	}

void C32CmiData::ConstructL(const TDesC& aFileName, TAutoClose<RFs>& aFileServer)
/** Second stage Constructor. Will cause file to be read and content to be parsed.
@param aFileName The filename.
*/
	{
	iCmiFile = C32ParseIniFile::NewL(aFileName, aFileServer);
	RetrieveAttributesL();
	}


TBool C32CmiData::FindVar(const TDesC8 &aSection,const TDesC8 &aVarName,TPtrC8 &aResult)
/** Find a variable's value given a section name and a var name.
@param aSection The section to search.
@param aVarName The name to find.
@param aResult The result.
@return Success or fail.
*/
	{
	return iCmiFile->FindVar(aSection, aVarName, aResult);
	}

TBool C32CmiData::FindVar(const TDesC8 &aSection,const TDesC8 &aVarName,TInt &aResult)
/**
@param aSection The section to search.
@param aVarName The name to find.
@param aResult The result.
@return Success or fail.
*/
	{
	return iCmiFile->FindVar(aSection, aVarName, aResult);
	}

HBufC8* C32CmiData::IniDataL()
/**
@return Pointer to the ini data.
*/
	{

	if((!AttrIsSet(AttrIniDataFile)) && (!AttrIsSet(AttrIniData)))
		{
		return NULL;
		}

	// If there was a inidata file specified we read that one, ignoring any inidata section
	if(AttrIsSet(AttrIniDataFile))
		{

		// Open fileserver sesion
		TInt result=KErrNone;
		TAutoClose<RFs> fs;
		if(KErrNone!=(result=fs.iObj.Connect()))
			{
			__FLOG( _L( "Unable to connect to the File Server" ) );
			User::Leave(result);
			}
		fs.PushL();

		// Open File and read content
		TInt size = 0;
		TAutoClose<RFile> rfile;
		TFileName fileName;
		fileName.Copy(iAttrIniDataFile);
		result = rfile.iObj.Open(fs.iObj, fileName, EFileShareReadersOnly);

		if(KErrNone!=result)
			{
			__FLOG_1(_L("Couldn't open inidata file %S"), &iAttrIniDataFile);
			User::Leave(result);
			}
		else
			{
			rfile.PushL();
			// Get filesize for allocating the buffer
			result=rfile.iObj.Size(size);

			if(size<=0)
				{
				__FLOG_1(_L("Inidata file %S has no contents"), &iAttrIniDataFile);
				User::Leave(KErrCorrupt);
				}

			// Allocate buffer and read data
			HBufC8* iniData = HBufC8::NewL(size);
			CleanupStack::PushL(iniData);
			TPtr8 fileDataPtr = iniData->Des();
			result = rfile.iObj.Read(fileDataPtr);

			if(KErrNone!=result)
				{
				__FLOG_1(_L("Couldn't read Inidata file %S"), &iAttrIniDataFile);
				User::Leave(result);
				}

			rfile.Pop();
			CleanupStack::Pop(iniData);
			return iniData;
			}
		}
	// else the inidata is embedded in the file
	// If inidata section is empty just return NULL, this is non-critical
	if(iAttrIniData.Length()<=0)
		{
		return NULL;
		}

	// Allocate heap buffer with room for data and return it
	// IniData is required to be Narrow...
	HBufC8* iniData = iAttrIniData.AllocL();
	return iniData;
	}

void C32CmiData::SplitBindingL(TPtrC8 aSource, TPtrC8& aAddr1, TPtrC8& aAddr2,
     TPtrC8& aType, TInt& aForwardQLength, TInt& aReverseQLength)
/** Identifies the various components of a line specifying a binding.
@param aSource The line to parse.
@param aAddr1 Pointer to name.
@param aAddr2 Pointer to name.
@param aType Pointer to binding type.
@leave KErrCorrupt
*/
	{

	TPtrC8 temp;
	TPtrC8 tempNum;
	TLex8 lex;
	TInt pos = aSource.Locate(',');
    //Set aAddr1
	if((pos==KErrNotFound) || (pos==0))
		{
		__FLOG(_L("Binding string corrupt"));
		User::Leave(KErrCorrupt);
		}
	aAddr1.Set(aSource.Ptr(), pos);

	//Set aAddr2
	if (pos < aSource.Length())
		{
		temp.Set(aSource.Ptr() + pos + 1, aSource.Length() - pos -1);
	    pos = temp.Locate(',');
		}
	else
		{
		pos = KErrNotFound;
		}
	if((pos==KErrNotFound) || (pos==0))
		{
		__FLOG(_L("Binding string corrupt"));
		User::Leave(KErrCorrupt);
		}
	aAddr2.Set(temp.Ptr(), pos);

	//Set aType
	if (pos < temp.Length())
		{
		temp.Set(temp.Ptr() + pos + 1,temp.Length() - pos - 1);
		pos = temp.Locate(',');
		}
	else
		{
		pos = KErrNotFound;
		}
	if((pos==KErrNotFound) || (pos==0))
		{
		__FLOG(_L("Binding string corrupt"));
		User::Leave(KErrCorrupt);
		}
	aType.Set(temp.Ptr(), pos);

	//Set aForwardQLength
	if (pos < temp.Length())
		{
		temp.Set(temp.Ptr() + pos + 1,temp.Length() - pos - 1);
		pos = temp.Locate(',');
		}
	else
		{
		pos = KErrNotFound;
		}
	if((pos==KErrNotFound) || (pos==0))
		{
		__FLOG(_L("Binding - no Forward queue length specified set to default of 1"));
		aForwardQLength = 1;
		aReverseQLength = 1;
		return;
		}
	tempNum.Set(temp.Ptr(),pos);
	lex.Assign(tempNum);
	if (lex.Val(aForwardQLength) != KErrNone)
		{
		__FLOG(_L("Binding string corrupt - ForwardQLength Invalid"));
		User::Leave(KErrCorrupt);
		}

	//Set aReverseQLength
	if (pos < temp.Length())
		{
		temp.Set(temp.Ptr() + pos + 1,temp.Length() - pos - 1);
		pos = temp.Length();
		}
	else
		{
		pos = KErrNotFound;
		__FLOG(_L("Binding - no Reverse queue length specified set to default of 1"));
		aReverseQLength = 1;
		return;
		}
	tempNum.Set(temp.Ptr(),pos);
	lex.Assign(tempNum);
	if (lex.Val(aReverseQLength) != KErrNone)
		{
		__FLOG(_L("Binding string corrupt - ForwardQLength Invalid"));
		User::Leave(KErrCorrupt);
		}

	}

void C32CmiData::MakeAddressL(const TPtrC8& aAddress, TCFSubModuleAddress& aSubModuleAddress)
/** Generate an aAddress from a string.
@param aAddress Pointer to string name.
@param aSubModuleAddress The value to be filled and returned.
@leave KErrCorrupt
*/
	{
	TCFModuleNameF name;
	TCFModuleNameF subName;
	TInt pos = aAddress.Locate(':');
	if((pos==KErrNotFound) || (pos==0))
		{
		__FLOG(_L("Binding string corrupt"));
		User::Leave(KErrCorrupt);
		}
	TPtrC8 tempptr(aAddress.Ptr(), pos);
	name.Copy(tempptr);
	name.TrimLeft();
	name.TrimRight();
	if(name.Length()<=0)
		{
		__FLOG(_L("Binding string corrupt"));
		User::Leave(KErrCorrupt);
		}
	pos++;
	if(aAddress.Length()<=pos)
		{
		__FLOG(_L("Binding string corrupt"));
		User::Leave(KErrCorrupt);
		}
	tempptr.Set(aAddress.Ptr()+pos, aAddress.Length()-pos);
	subName.Copy(tempptr);
	subName.TrimLeft();
	subName.TrimRight();
	aSubModuleAddress.SetModule(name);
	aSubModuleAddress.SetSubModule(subName);
	}

TRSBindType C32CmiData::MakeBindingTypeL(const TDesC8& aTxtType)
/** Generate a bindtype object from a string.
@param aTxtType The type of binding required in text form.
@return Type of binding.
@leave KErrCorrupt
*/
	{
	TBuf8<32> buf=aTxtType;
	buf.TrimLeft();
	buf.TrimRight();
	buf.LowerCase();

    _LIT8(KEHierarchical, "ehierarchical");
    _LIT8(KECustom, "ecustom");

	if(0==buf.CompareF(KEHierarchical))
		{
		return EHierarchical;
		}

	if(0==buf.CompareF(KECustom))
		{
		return ECustom;
		}

	__FLOG(_L("Binding type string corrupt"));
	User::Leave(KErrCorrupt);
	return ECustom;		//lint !e527 // LINT knows that we can't get here, but the compiler doesn't
	}

TBool C32CmiData::NextBindingL(TRSBindingInfo& aBinding, TBool aReset/*=EFalse*/)
/** Find the next binding.
@param aBinding The bindinfo struct to be filled with a found binding.
@param TBool aReset Indicates sequence of call to this function - first
has aReset=ETrue.
@return Found or not.
*/
	{

	if(aReset)
		{
		iNextBinding=0;
		}

	TBuf8<C32ParseIniFile::KTokenSize> buf;
	_LIT8(bindingTgt,"Binding%d");
	buf.Format(bindingTgt, iNextBinding);
	TPtrC8 ptr(NULL, 0);
	TBool found;

	// Get next BindingX string
	if((found = FindVar(KSectionLoader, buf, ptr)) != EFalse)
		{
		__FLOG_2(_L8("Found binding: %S=%S"), &buf, &ptr);
		++iNextBinding;
		TPtrC8 addr1;
		TPtrC8 addr2;
		TPtrC8 type;
		TInt forwardQLength;
		TInt reverseQLength;
		SplitBindingL(ptr, addr1, addr2, type,
		forwardQLength,reverseQLength); // Split into sections between ','

		TCFSubModuleAddress address;
		MakeAddressL(addr1, address);
		aBinding.iParams.iAddress1=address;

		MakeAddressL(addr2, address);
		aBinding.iParams.iAddress2=address;

		aBinding.iParams.iType=MakeBindingTypeL(type);

		aBinding.iParams.iForwardQLength = forwardQLength;
		aBinding.iParams.iReverseQLength = reverseQLength;
		}
	return found;
	}


