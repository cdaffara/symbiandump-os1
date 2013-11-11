// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedPartner
*/

#include <ecom/ecomresolverparams.h>
#include <ecom/implementationinformation.h>
#include <ecom/ecom.h>

#include <bluetooth/hci/hciutil.h>
#include <bluetooth/hci/hciinifile.h>
#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_HCIUTIL);
#endif

const TInt KTokenSize=256;

/**
Creates an instance of CHciUtil for working with a HCI config
file for a particular component.
@param aComponentName The name of the component using the instance.
@return A new instance of CHciUtil.
*/
EXPORT_C CHciUtil* CHciUtil::NewL(const TDesC& aComponentName)
	{
	LOG_STATIC_FUNC
	LOG1(_L("aComponentName = %S"), &aComponentName);

	CHciUtil* self = new (ELeave) CHciUtil();
	CleanupStack::PushL(self);
	self->ConstructL(aComponentName);
	CleanupStack::Pop(self);
	return self;	
	}
	
void CHciUtil::ConstructL(const TDesC& aComponentName)
	{
	LOG_FUNC
	
	if (aComponentName.Length() > KMaxHciUtilComponentName)
		{
		LEAVEIFERRORL(KErrArgument);
		}
	
	iCompName = aComponentName;
	}

CHciUtil::CHciUtil()
	{
	LOG_FUNC
	}

EXPORT_C CHciUtil::~CHciUtil()
	{
	LOG_FUNC
	
	// ensure any ini file is closed
	CloseIniFile();
	}

/**
Opens an ini file based on the name of the component (provided when the CHciUtil
instance was created.)  It looks for the file in the default path.
@panic KHciUtilPanic EAlreadyAnInFileOpen If the CHciUtil instance already has an ini file opened.
*/
EXPORT_C void CHciUtil::OpenIniFileL()
	{
	LOG_FUNC

	__ASSERT_ALWAYS(!iIniFile, PANIC(KHciUtilPanic, EAlreadyAnIniFileOpen));

	TName name(KDefaultIniFilePath());
	name.Append(iCompName);
	name.Append(_L(".ini"));
	
	iIniFile = CIniFileData::NewL(name);
	}

/**
Opens an ini file with a particular name in a given path location.
@param aPath The path the ini file is located.
@param aFile The name of the ini file to be opened.
@panic KHciUtilPanic EAlreadyAnIniFileOpen If the CHciUtil instance already has an ini file opened.
*/
EXPORT_C void CHciUtil::OpenIniFileL(const TDesC& aPath, const TDesC& aFile)
	{
	LOG_FUNC
	
	__ASSERT_ALWAYS(!iIniFile, PANIC(KHciUtilPanic, EAlreadyAnIniFileOpen));

	TName name = aPath;
	name.Append(aFile);
	
	iIniFile = CIniFileData::NewL(name);
	}

/**
Closes any ini file currently open.
*/
EXPORT_C void CHciUtil::CloseIniFile()
	{
	LOG_FUNC
	
	delete iIniFile;
	iIniFile = NULL;
	}

TUid CHciUtil::FormatUidL(TPtrC& aDes)
	{
	LOG_FUNC
	
	aDes.Set(aDes.Right(aDes.Length() - 2)); // skip '0x'
	TLex uidLex(aDes.Left(8));
	TUint tmp = 0;
	
	if (uidLex.Val(tmp, EHex) != KErrNone)
		{
		LEAVEIFERRORL(KErrCorrupt);
		}

	TUid uid;
	uid.iUid = tmp;
	return uid;
	}

TUint CHciUtil::FormatValueL(TPtrC& aDes)
	{
	LOG_FUNC

	TLex uidLex(aDes);
	TUint ret = 0;
	
	if (uidLex.Val(ret) != KErrNone)
		{
		LEAVEIFERRORL(KErrCorrupt);
		}

	return ret;
	}

void CHciUtil::GetFromFileL(const TDesC& aSection, const TDesC& aTag, TPtrC& aRetText)
	{
	LOG_FUNC
	if (!iIniFile)
		{
		// File isn't open - so we aren't ready to get values.
		LEAVEIFERRORL(KErrNotReady);
		}

	if (!iIniFile->FindVar(aSection, aTag, aRetText))
		{
		LEAVEIFERRORL(KErrNotFound);
		}
	}

/**
Gets a TUid from an opened ini file.
@param aSection The section name under which the tag for the UID is to be found.
@param aTag The tag which marks up the UID that is required.
@return The UID found in the ini file under given section and tag.
*/
EXPORT_C TUid CHciUtil::GetUidFromFileL(const TDesC& aSection, const TDesC& aTag)
	{
	LOG_FUNC
	
	TPtrC uidText;
	
	// Get the uid from the ini file
	GetFromFileL(aSection, aTag, uidText);
	
	// uid found
	return (FormatUidL(uidText));
	}

/**
Gets a numeric value from an opened ini file.
@param aSection The section name under which the tag for the UID is to be found.
@param aTag The tag which marks up the UID that is required.
@return The value found in the ini file under given section and tag.
*/
EXPORT_C TUint CHciUtil::GetValueFromFileL(const TDesC& aSection, const TDesC& aTag)
	{
	LOG_FUNC
	
	TPtrC valText;
	
	// Get the value from the ini file
	GetFromFileL(aSection, aTag, valText);
	
	// value found
	return (FormatValueL(valText));
	}

void ResetAndDestroy(TAny* aPtr)
	{
	reinterpret_cast<RImplInfoPtrArray*>(aPtr)->ResetAndDestroy();
	}

void CleanupResetAndDestroyPushL(RImplInfoPtrArray& aArray)
	{
	TCleanupItem item(ResetAndDestroy, &aArray);
	CleanupStack::PushL(item);
	}

/**
Utility for loading HCI plugins.
Attempts to create a plugin instance with the appropriate interface.
@param aIfUid The ECom interface UID to create an instance of.
@param aKeyOffset The key offset.
@return A newly created instance of the given ECom interface UID.
@leave KErrNotFound If no ECom plugins supporting the supplied interface UID are available.
@leave KErrArgument If more than one ECom plugin implementations are providing the specified interface.
*/
EXPORT_C TAny* CHciUtil::LoadImplementationL(TUid aIfUid, TInt aKeyOffset)
	{
	LOG_STATIC_FUNC

	RImplInfoPtrArray implInfoArray;
	CleanupResetAndDestroyPushL(implInfoArray);

	REComSession::ListImplementationsL(aIfUid, implInfoArray);
	TUid implUid = TUid::Null();

	if (implInfoArray.Count() == 1)
		{
		// Return the UID of the one and only implementation
		implUid = implInfoArray[0]->ImplementationUid();
		}
	else
		{
		// Not a single implementation
		if (implInfoArray.Count() == 0)
			{
			// No implementations found
			LEAVEIFERRORL(KErrNotFound);
			}
		else
			{
			// More than one implementation found
			LEAVEIFERRORL(KErrArgument);
			}
		}

	// Load the implementation
	TAny* ret = REComSession::CreateImplementationL(implUid, aKeyOffset);
	CleanupStack::PopAndDestroy(); // cleanup implInfoArray
	return ret;
	}
	
// Used by Plugin base classes to load an implementation
EXPORT_C TAny* CHciUtil::LoadImplementationL(TUid aIfUid, TUid aImplUid, TInt aKeyOffset)
	{
	LOG_STATIC_FUNC

	RImplInfoPtrArray implInfoArray;
	CleanupResetAndDestroyPushL(implInfoArray);

	REComSession::ListImplementationsL(aIfUid, implInfoArray);

	// Check if the specified implementation is for the correct interface
	for (TInt implIndex = implInfoArray.Count() - 1; implIndex >= 0; implIndex--)
		{
		if (implInfoArray[implIndex]->ImplementationUid() == aImplUid)
			{
			// Found the implmentation for the interface so load it
			TAny* ret = REComSession::CreateImplementationL(aImplUid, aKeyOffset);
			CleanupStack::PopAndDestroy(); // cleanup implInfoArray
			return ret;
			}
		}
	
	// Implementation not found for the interface
	LEAVEIFERRORL(KErrNotFound);

	// Not Reached
	return NULL;
	}

/**
Indicates that a specific ECom instance should be destroyed.
*/
EXPORT_C void CHciUtil::DestroyedImplementation(TUid aKey)
	{
	LOG_STATIC_FUNC

	REComSession::DestroyedImplementation(aKey);
	}

/**
A Method that converts a HCI error code (as defined by the Bluetooth specification)
into a Symbian error code.
@param aErrorCode The HCI error code to be converted
@return The HCI error code provided as a value in the Symbian error code space.
*/
EXPORT_C /*static*/ TInt CHciUtil::SymbianErrorCode(THCIErrorCode aErrorCode)
	{
	LOG_STATIC_FUNC
	
	TInt rerr = KErrNone;
	if(aErrorCode != EOK)
		{
		rerr = KHCIErrorBase - aErrorCode;
		}
	return rerr;
	}


//
//
// Find a key's value given a section name and a key name
//


CIniFileData::CIniFileData() 
	: iPtr(NULL,0)
/** Constructor

*/
	{
	__DECLARE_NAME(_S("CIniFileData"));
	}


CIniFileData::~CIniFileData()
/** Destructor.
    
    Frees the resources located in second-phase constructor

*/
	{

	delete (TText*)iPtr.Ptr();
	delete iToken;
	delete iName;
	}


CIniFileData* CIniFileData::NewL(const TDesC& aName)
/**
 Creates, and returns a pointer to CIniFileData object, leave on failure

 @return A pointer to the CIniFileData object
*/
	{
	CIniFileData* p=new(ELeave) CIniFileData;
	CleanupStack::PushL(p);
	p->ConstructL(aName);
	CleanupStack::Pop();
	return p;
	}


void CIniFileData::ConstructL(const TDesC& aName)
/**
 Second-phase constructor.

 The function attempts to allocate a buffer and Read file's contents into iPtr
 
 @param aName the name of the file which contains the ini data

 @leave One of the system-wide error codes
*/
	{
 	// Allocate space for token
	iToken=HBufC::NewL(KTokenSize+2);	// 2 extra chars for [tokenName]

	// Connect to file server
	TAutoClose<RFs> fs;
	User::LeaveIfError(fs.iObj.Connect());
	fs.PushL();

	// Find file, given name
	TFindFile ff(fs.iObj);
	User::LeaveIfError(ff.FindByDir(aName, KDefaultIniFilePath));
	iName=ff.File().AllocL();

	// Open file
	TAutoClose<RFile> file;
	TInt size;
	User::LeaveIfError(file.iObj.Open(fs.iObj,*iName,EFileStreamText|EFileShareReadersOrWriters));
	file.PushL();

	// Get file size and read in
	User::LeaveIfError(file.iObj.Size(size));
	TText* data=(TText*)User::AllocL(size);
	iPtr.Set(data, size/sizeof(TText), size/sizeof(TText));
	TPtr8 dest((TUint8*)data, 0, size);
	User::LeaveIfError(file.iObj.Read(dest));
	TUint8* ptr = (TUint8*)data;

	//
	// This is orderred as FEFF assuming the processor is Little Endian
	// The data in the file is FFFE.		PRR 28/9/98
	//
	if(size>=(TInt)sizeof(TText) && iPtr[0]==0xFEFF)
	{
		// UNICODE Text file so lose the FFFE
		Mem::Copy(ptr, ptr+sizeof(TText), size-sizeof(TText));
		iPtr.Set(data, size/sizeof(TText)-1, size/sizeof(TText)-1);
	}
	else if(size)
	{
		// NON-UNICODE so convert to UNICODE
		TText* newdata = (TText*)User::AllocL(size*sizeof(TText));
		iPtr.Set(newdata, size, size);
		TInt i;
		for(i=0 ; i<size ; ++i)
			iPtr[i]=ptr[i];
		delete data;
	}

	file.Pop();
	fs.Pop();
}


//
//
// Find a key's value given a section name and a key name
//
TBool CIniFileData::FindVar(const TDesC &aSectName,const TDesC &aKeyName,TPtrC &aResult)
/** Find a text value from given aKeyName and aSecName in the ini data file
 
@param aSectName Section containing key
@param aKeyName Key being searched for in aSectName
@param aResult On return, contains the text result 

@return ETrue if found, otherwise EFalse
*/
     {
     __ASSERT_DEBUG(aSectName.Length()<=KTokenSize,PANIC(KHciUtilPanic, ESectionNameTooBig));
     __ASSERT_DEBUG(aKeyName.Length()<=KTokenSize,PANIC(KHciUtilPanic, EKeyNameTooBig));
 
     TInt posStartOfSection(0);
     TInt posEndOfSection(iPtr.Length()); // Default to the entire length of the ini data
     TPtrC SearchBuf;

     // If we have a section, set pos to section start
     TInt posI(0);   // Position in internal data Buffer
     if( aSectName.Length() > 0 )
         {
         TBool FoundSection(false);
         while ( ! FoundSection )
             {
             // Move search buffer to next area of interest
             SearchBuf.Set(iPtr.Mid(posI));
 
             // Make up token "[SECTIONNAME]", to search for
             TPtr sectionToken=iToken->Des();
             _LIT(sectionTokenFmtString,"[%S]");
             sectionToken.Format(sectionTokenFmtString,&aSectName);
 
             // Search for next occurrence of aSectName
             TInt posSB = SearchBuf.Find(sectionToken);
 
             if (posSB==KErrNotFound)
                 return(EFalse);
 
             // Check this is at beginning of line (ie. non-commented)
             // ie. Check preceding char was LF
             if(posSB>0)
                 {
                 // Create a Buffer, starting one char before found subBuf
                 TPtrC CharBefore(SearchBuf.Right(SearchBuf.Length()-posSB+1));
                 // Check first char is end of prev
                 if(CharBefore[0] == '\n')
                     {
                     FoundSection = ETrue;       // found
                     posI = posI + posSB;
                     }
                 else
                     {
                     posI = posI + posSB + 1;    // try again
                     }
                 }
             else
                 {
                 FoundSection = ETrue;
                 }
 
             }   // while ( ! FoundSection ) 
 
         // Set start of section, after section name, (incl '[' and ']')
         posStartOfSection = posI + aSectName.Length() + 2;
 
         // Set end of section, by finding begin of next section or end
         SearchBuf.Set(iPtr.Mid(posI));
         _LIT(nextSectionBuf,"\n[");
         TInt posSB = SearchBuf.Find(nextSectionBuf);
         if(posSB != KErrNotFound)
             {
             posEndOfSection = posI + posSB;
             }
         else
             {
             posEndOfSection = iPtr.Length();
             }
 
         }   // if( aSectName.Length() > 0 )
 
     // Look for key in ini file data Buffer
     posI = posStartOfSection;
     TBool FoundKey(false);
     while ( ! FoundKey )
         {
         // Search for next occurrence of aKeyName
         SearchBuf.Set(iPtr.Mid(posI,posEndOfSection-posI));
         TInt posSB = SearchBuf.Find(aKeyName);
 
         // If not found, return
         if (posSB==KErrNotFound)
             return(EFalse);
 
         // Check this is at beginning of line (ie. non-commented)
         // ie. Check preceding char was CR or LF
         if(posSB>0)
             {
             // Create a Buffer, starting one char before found subBuf
             TPtrC CharBefore(SearchBuf.Right(SearchBuf.Length()-posSB+1));
             // Check if the first char is end of prev and also check 
             // if the token found is not a substring of another string  
             TBool beginningOK = ((CharBefore[0] == '\n') || (CharBefore[0] == ' ') || (CharBefore[0] == '\t'));
             TBool endingOK = ((CharBefore[aKeyName.Length()+1] == '=') || (CharBefore[aKeyName.Length()+1] == ' ') || (CharBefore[aKeyName.Length()+1] == '\t'));
             if (beginningOK && endingOK)
                 {
                 FoundKey = ETrue;
                 posI = posI + posSB;
                 }
             else
                 {
                 posI = posI + posSB + 1;
                 }
             }
         else
             {
             FoundKey = ETrue;
             }
         }   // while ( ! FoundKey )
 
     // Set pos, to just after '=' sign
     SearchBuf.Set(iPtr.Mid(posI));
     TInt posSB = SearchBuf.Locate('=');
     if(posSB==KErrNotFound)     // Illegal format, should flag this...
         return(EFalse);
 
     // Identify start and end of data (EOL or EOF)
     posI = posI + posSB + 1;    // 1 char after '='
     TInt posValStart = posI;
     TInt posValEnd;
     SearchBuf.Set(iPtr.Mid(posI));
     posSB = SearchBuf.Locate('\r');
     if(posSB!=KErrNotFound)
         {
         posValEnd = posI + posSB;
         }
     else
         {
         posValEnd = iPtr.Length();
         }
 
     // Check we are still in the section requested
     if( aSectName.Length() > 0 )
         {
         if( posValEnd > posEndOfSection )
             {
             return(EFalse);
             }
         }
     // Parse Buffer from posn of key
     // Start one space after '='
     TLex lex(iPtr.Mid(posValStart, posValEnd-posValStart));
     lex.SkipSpaceAndMark();     // Should be at the start of the data
     aResult.Set(lex.MarkedToken().Ptr(),posValEnd-posValStart - lex.Offset() );
     return(ETrue);
     }
 





