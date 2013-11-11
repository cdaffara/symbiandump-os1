// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
// Accenture - adapted for fshell, much functionality added
//
// Description:
// Based on \SF\OS\KERNELHWSRV\USERLIBANDFILESERVER\FILESERVER\etshell\ts_deps.cpp
//

#include "chkdeps.h"

CCommandBase* CDllChecker::NewLC()
	{
	CDllChecker* self = new(ELeave) CDllChecker();
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return self;
	}

const TDesC& CDllChecker::Name() const
	{
	_LIT(KName, "chkdeps");
	return KName;
	}

void CDllChecker::DoRunL()
	{
#ifdef __WINS__
	PrintWarning(_L("chkdeps only works on E32 (ie ARM) format binaries, not WINS ones"));
#endif
	FsL();
	ConstructL();
	TBool exists = EFalse;
		{
		TEntry e;
		if (Fs().Entry(iFilename, e) == KErrNone) exists = ETrue;
		}

	if (!exists)
		{
		// Try and resolve
		iFilename = iFilename.NameAndExt(); // Is this going to work? Assigning to a descriptor a TPtrC that points to itself...?
		FindDll(iFilename, iFilename, _L("\\sys\\bin\\"));
		}

	TInt result;
	iStackBase = &result;
	iExeInfo = new(ELeave) CDllInfo;
	iExeInfo->iDllCaps.SetEmpty();
	iRequiredCaps = &iExeInfo->iDllCaps;
	TRAP(result, GetImportDataL(iFilename, *iExeInfo));
	if (result==KErrGeneral)
		{
		PrintError(result, _L("%S has no import data"), &iFilename);
		return;
		}
	else if (result)
		{
		PrintError(result, _L("Couldn't read %S"), &iFilename);
		User::Leave(result);
		}
	ListArray();	//	Print out the results of DllCheck
	//Stdin().ReadKey();
	}

void CDllChecker::ArgumentsL(RCommandArgumentList& aArguments)
	{
	aArguments.AppendFileNameL(iFilename, _L("file_name"));
	}

void CDllChecker::OptionsL(RCommandOptionList& aOptions)
	{
	aOptions.AppendBoolL(iVerbose, _L("verbose"));
	aOptions.AppendBoolL(iDebug, _L("debug"));
	}

EXE_BOILER_PLATE(CDllChecker)

///////////////////////////////////////////////////////////////////////

#define __PRINT(t) { if (iDebug) Printf(t);}
#define __PRINT1(t,a) { if (iDebug) Printf(t,a);}
#define __PRINT2(t,a,b) { if (iDebug) Printf(t,a,b);}
#define __PRINTWAIT(t) { if (iDebug) { Printf(t); Stdin().ReadKey();}}

/*
  
CDllChecker::GetImportDataL(aFilename) reads the Image Header, Import Section 
and all import data for aFilename.  If aFilename is a ROM dll, and thus has no 
import data, or if the file contains no import data for some other reason the 
function leaves with KErrGeneral. If a file is compressed, function calls 
appropriate decompression routine to inflate the import data.
The function then calls GetDllTableL function which 
reads the first import block and enters a "for" loop. The Dll's name and Uid3 are obtained 
from CDllChecker::GetFileNameAndUid().  If the Dll name does not occur in the 
array of previously checked Dlls, CDllChecker::FindDll() is called.  If the Dll 
is found,the function then calls CDllChecker::GetImportDataL on the filename acquired by 
GetFileNameAndUid()(recursive call). 

If the Dll contains no import data or cannot be found, or if the Uid is invalid,
the next import is checked.

The Uid3 value is checked by calling CDllChecker::CheckUid.  This compares the 
Uid3 value found in the image header of the file, with that found by 
GetFileNameAndUid().  If there are any discrepancies,these are noted.

Each potential import is added to the array to 
indicate its import status.

CDllChecker::ListArray() lists the contents of the array when all imports
have been checked.
*/


void CDllChecker::ConstructL()
//
//	Creates an array to hold DLLs referenced by this executable	
//
	{
	}

CDllChecker::CDllChecker()
	{
	}

CDllChecker::~CDllChecker()
//
//	Destructor
//
	{
    iDllArray.ResetAndDestroy();
	delete iExeInfo;
	}

void FileCleanup(TAny* aPtr)
	{
	TFileInput* f=(TFileInput*)aPtr;
	f->Cancel();
	delete f;
	}

void CDllChecker::LoadFileInflateL(const E32ImageHeaderComp* aHeader, RBuf8& aCode, RBuf8& aRestOfFile)
	{
	TInt pos = aHeader->TotalSize();
	User::LeaveIfError(iFile.Seek(ESeekStart,pos)); // Start at beginning of compressed data

	TFileInput* file = new (ELeave) TFileInput(iFile);
	CleanupStack::PushL(TCleanupItem(&FileCleanup,file));
	CInflater* inflater=CInflater::NewLC(*file);
	
	if (aHeader->iCodeSize)
		{
		aCode.CreateL(aHeader->iCodeSize);
		TInt count = inflater->ReadL((TUint8*)aCode.Ptr(), aHeader->iCodeSize, &Mem::Move);
		if(count != aHeader->iCodeSize)
			User::Leave(KErrCorrupt);
		aCode.SetLength(count);
		}
	
	// Compressed executable
	// iCodeOffset	= header size for format V or above
	//				= sizeof(E32ImageHeader) for format J
	TInt restOfFileSize = aHeader->UncompressedFileSize() - aHeader->iCodeOffset - aHeader->iCodeSize; // the size of the exe less header & code

	if (restOfFileSize)
		{
		aRestOfFile.CreateL(restOfFileSize);
		TUint32 count = inflater->ReadL((TUint8*)aRestOfFile.Ptr(), restOfFileSize, &Mem::Move);
		if(count != restOfFileSize)
			User::Leave(KErrCorrupt);
		aRestOfFile.SetLength(count);
		}
	CleanupStack::PopAndDestroy(2,file);
	}

void CDllChecker::LoadFileNoCompressL(const E32ImageHeaderComp* aHeader, RBuf8& aCode, RBuf8& aRestOfFile)
	{
	TInt codepos = aHeader->TotalSize();
	if (aHeader->iCodeSize)
		{
		aCode.CreateL(aHeader->iCodeSize);
		User::LeaveIfError(iFile.Read(codepos, aCode, aHeader->iCodeSize));
		}

	TInt restpos = aHeader->TotalSize() + aHeader->iCodeSize;
	TInt fileSize;
	User::LeaveIfError(iFile.Size(fileSize));
	TInt restOfFileSize = fileSize - aHeader->TotalSize() - aHeader->iCodeSize; // the size of the exe less header & code

	if (restOfFileSize)
		{
		aRestOfFile.CreateL(restOfFileSize);
		User::LeaveIfError(iFile.Read(restpos, aRestOfFile, restOfFileSize));	
		}
	}

void CDllChecker::LoadFileBytePairL(const E32ImageHeaderComp* aHeader, RBuf8& aCode, RBuf8& aRestOfFile)
	{
	TInt pos = aHeader->TotalSize();
	User::LeaveIfError(iFile.Seek(ESeekStart,pos)); // Start at beginning of compressed data

	CBytePairReader* reader = CBytePairFileReader::NewLC(iFile);

	if (aHeader->iCodeSize)
		{
		aCode.CreateL(aHeader->iCodeSize);
		TUint32 bytes = reader->DecompressPagesL((TUint8*)aCode.Ptr(), aHeader->iCodeSize, &Mem::Move);
		if((TInt)bytes != aHeader->iCodeSize)
			User::Leave(KErrCorrupt);
		aCode.SetLength(bytes);
		}

	TInt restOfFileSize = ((E32ImageHeaderV*)aHeader)->iUncompressedSize - aHeader->iCodeOffset - aHeader->iCodeSize;

	if (restOfFileSize)
		{
		aRestOfFile.CreateL(restOfFileSize);
		TUint32 count = reader->DecompressPagesL((TUint8*)aRestOfFile.Ptr(), restOfFileSize, &Mem::Move);
		if(count != restOfFileSize)
			User::Leave(KErrCorrupt);
		aRestOfFile.SetLength(count);
		}

	CleanupStack::PopAndDestroy(reader);
	}

//function loads file's import information calling decompression routine if needed
void CDllChecker::LoadFileL(const E32ImageHeaderComp* aHeader, RBuf8& aCode, RBuf8& aRestOfFile)
	{
	TUint32 aCompression = aHeader->CompressionType();

	if(aCompression==KFormatNotCompressed)
		{
		LoadFileNoCompressL(aHeader, aCode, aRestOfFile);
		}
	else if(aCompression==KUidCompressionDeflate)
		{
		LoadFileInflateL(aHeader, aCode, aRestOfFile);
		}
	/*TOMSCI TODO doesn't seem to work...
	else if (aCompression == KUidCompressionBytePair)
		{
		LoadFileBytePairL(aHeader, aCode, aRestOfFile);
		}*/
	else
		{
		LeaveIfErr(KErrNotSupported, _L("Compression type 0x%x is not supported"), aCompression);
		}
	}


TBool CDllChecker::CompareDllInfo(const CDllInfo& aDllInfo1, const CDllInfo& aDllInfo2)
	{
	return (aDllInfo1.iDllName.CompareF(aDllInfo2.iDllName) == 0);
	}

								
//function iterates through the list of libraries the current executable depends on
//for each dependency in the list the function checks whether the .dll being checked is already added to the array of dependencies
//if not, the function adds dependency being checked to the array of dependencies and calls GetImportDataL function recursively
//void CDllChecker::GetDllTableL(TUint8* aImportData, TInt aDllRefTableCount,TUint aFlags)
//void CDllChecker::GetDllTableL(const TDesC& aFileName, E32ImageHeader* aImageHeader, TUint8* aRestOfFile)
void CDllChecker::GetDllTableL(const TDesC& aFileName, const E32ImageHeader* aImageHeader, const TUint8* aCode, const TUint8* aImportData)
	{
	const TInt aDllRefTableCount = aImageHeader->iDllRefTableCount;
	const TUint aFlags = aImageHeader->iFlags;

	const E32ImportBlock* block = (const E32ImportBlock*)(aImportData+sizeof(E32ImportSection));

	for (TInt i=0; i<aDllRefTableCount; i++)	
		{
		CDllInfo* dllInfo = new(ELeave) CDllInfo;
		CleanupStack::PushL(dllInfo);
		dllInfo->iResult = EPending;
		dllInfo->iNumExports = 0;
		const TText8* dllName=(aImportData+block->iOffsetOfDllName);
		TPtrC8 dllNamePtr(dllName, User::StringLength(dllName));
		GetFileNameAndUid(*dllInfo,dllNamePtr);
		__PRINT1(_L("Considering \"%S\"...\r\n"),&dllInfo->iDllName);
		TInt r=iDllArray.Find(dllInfo,TIdentityRelation<CDllInfo>(CompareDllInfo));
		if (r >= 0)
			{
			__PRINT(_L("\tAlready checked\r\n"));
			CDllInfo* dllInfoPtr = iDllArray[r];
			if ((dllInfoPtr->iResult!=EPending) && (dllInfoPtr->iUid!=dllInfo->iUid))	
				{
				__PRINT2(_L(" Uid3 [%08x] for %S is different from that noted previously\r\n"),dllInfo->iUid,&dllInfo->iDllName);
				dllInfoPtr->iResult=EUidDifference;
				}
			CleanupStack::PopAndDestroy(dllInfo);
			dllInfo = dllInfoPtr;
			}
		else
			{
			__PRINT(_L("\tNot previously checked\r\n"));
			iDllArray.AppendL(dllInfo);
			CleanupStack::Pop(dllInfo);
		
			TFileName fileName;
			r=FindDll(dllInfo->iDllName,fileName,Env().Pwd());
			
			if (r==KErrNotFound)	//	Could not find Dll
				{
				dllInfo->iResult=ENotFound;
				}//	Run to the end of the "for" loop for this i value

			else	//	File was located 
				{
				//	Go recursive.  Call GetImportDataL on the new dll, if it imports anything.
				//	ROM dlls have no import data so this is never called for ROM dlls.
				//	This *will* terminate.  It is only called on "new" dlls not in the array.
				iCalls++;
				
				TRAP(r,GetImportDataL(fileName, *dllInfo));				
				switch(r)		
					{
					case(KErrGeneral):	//	No import data
						{
						dllInfo->iResult=ENoImportData;
						if (!dllInfo->iDllCaps.HasCapabilities(*iRequiredCaps)) dllInfo->iResult = ERequiredCapabilitiesMissing;
						break;
						}
					case(EUidNotSupported):
					case(EVersionMismatch):
						{
						dllInfo->iResult=(CDllChecker::TResultCheck)r;
						break;
						}
					case(KErrNone):	//	Import data was read
						{
						dllInfo->iResult=EFileFoundAndUidSupported;
						if (!dllInfo->iDllCaps.HasCapabilities(*iRequiredCaps)) dllInfo->iResult = ERequiredCapabilitiesMissing;
						break;
						}
					case(KErrInUse):
						{
						__PRINT2(_L("\t\"%S\" is already open\r\n"),&fileName,r);
						dllInfo->iResult=EAlreadyOpen;
						break;
						}
					case(KErrCorrupt):
						{
						__PRINT2(_L("\t\"%S\" has unexpected format\r\n"),&fileName,r);
						dllInfo->iResult=EAlreadyOpen;
						break;
						}
					default:
						{
						__PRINT1(_L("\t\"%S\" could not be opened \r\n"),&fileName);
						dllInfo->iResult=ECouldNotOpenFile;
						break;
						}
					}	
				}
			}

		if (dllInfo->iResult == EFileFoundAndUidSupported)
			{
			// Check all the ordinals we import actually exist
			for (TInt i = 0; i < block->iNumberOfImports; i++)
				{
				TInt ordinal = 0;
				//Printf(_L("we import %d from %S\r\n"), block->Imports()[i], &dllInfo->iDllName);
				if (aImageHeader->ImportFormat() == KImageImpFmt_ELF)
					{
					TUint impd_offset = block->Imports()[i];
					TUint impd = *(TUint*)(aCode + impd_offset);
					ordinal = impd & 0xffff;
					//TUint offset = impd >> 16;
					}
				else
					{
					ordinal = block->Imports()[i];
					}
				//Printf(_L("We import %d (numExports=%d)\r\n"), ordinal, dllInfo->iNumExports);
				if (ordinal > dllInfo->iNumExports && dllInfo->iResult != ENotFound)
					{
					Printf(_L("\tOrdinal %d is missing from \"%S\" (used by \"%S\")\r\n"), ordinal, &dllInfo->iDllName, &aFileName);
					dllInfo->iResult = EOrdinalMissing;
					}
				}
			}
		block = (E32ImportBlock*)block->NextBlock(E32ImageHeader::ImpFmtFromFlags(aFlags));
		}
	}


void CDllChecker::GetImportDataL(const TDesC& aFileName, CDllInfo& aInfo)
	{
	TInt cleanupCount=0;
	//	Check that the file is not a ROM dll.  These have no import data	
	if (Fs().IsFileInRom(aFileName))
		{
		// Still need to fill in caps and export count
		TRomImageHeader* romHeader = (TRomImageHeader*)Fs().IsFileInRom(aFileName);
		aInfo.iDllCaps.SetEmpty();
		SCapabilitySet* caps = &romHeader->iS.iCaps;
		aInfo.iDllCaps = *(TCapabilitySet*)caps;
		aInfo.iNumExports = romHeader->iExportDirCount;
		User::Leave(KErrGeneral);
		}
	
	//open file for reading and push it to autoclose stack
	TAutoClose<RFile> autoFile;
	User::LeaveIfError(autoFile.iObj.Open(Fs(),aFileName,EFileShareReadersOnly));
	autoFile.PushL();
	cleanupCount++;
	iFile=autoFile.iObj;
		
	//Create a pointer to an Image Header
	//reserve enough memory for compressed file header because we don't know whether the file is compressed or not 
	// In fact, allow for a E32ImageHeaderV so we can get the capability info too
	E32ImageHeaderV* imageHeader=new(ELeave)E32ImageHeaderV;
	CleanupStack::PushL(imageHeader);
	cleanupCount++;
	
	//read file header
	TPtr8 ptrToImageHeader((TText8*)(imageHeader),sizeof(E32ImageHeaderV),sizeof(E32ImageHeaderV));
	User::LeaveIfError(iFile.Read(ptrToImageHeader,sizeof(E32ImageHeaderV)));
	

	aInfo.iDllCaps.SetEmpty();
	if (imageHeader->HeaderFormat() >= KImageHdrFmt_V)
		{
		SCapabilitySet* caps = &imageHeader->iS.iCaps;
		aInfo.iDllCaps = *(TCapabilitySet*)caps;
		}
	aInfo.iNumExports = imageHeader->iExportDirCount;

	if (imageHeader->iImportOffset==0) // File contains no import data (ROM files never have import data)
		{	
		User::Leave(KErrGeneral);
		}

	RBuf8 code, rest;
	CleanupClosePushL(code);
	CleanupClosePushL(rest);
	cleanupCount += 2;
	LoadFileL(imageHeader, code, rest); // Read import information in
			
	TInt32 uid3=imageHeader->iUid3;
	if(iCalls!=0)	//	Only check Uid3 of dependencies (ie only after first 
		{			//	call of recursive function)
		TInt r=CheckUid3(uid3,aInfo.iUid);

		if (r!=KErrNone) //	Dll's Uid3 is not valid
			User::Leave(EUidNotSupported);

		TInt version = (imageHeader->iModuleVersion >> 16);
		if (version != aInfo.iMajorVersion)
			{
			Printf(_L("Version mismatch - %S version is %d, expected %d\r\n"), &aFileName, version, aInfo.iMajorVersion);
			User::Leave(EVersionMismatch);
			}
		}

	TInt importOffset = imageHeader->iImportOffset - (imageHeader->iCodeOffset + imageHeader->iCodeSize);
	if (TInt(importOffset + sizeof(E32ImportSection)) > rest.Size())
		User::Leave(KErrCorrupt);		
	//get the table of dependencies
	//GetDllTableL(restOfFileData+bufferOffset,imageHeader->iDllRefTableCount,imageHeader->iFlags);
	//GetDllTableL(aFileName, imageHeader, restOfFileData);
	GetDllTableL(aFileName, imageHeader, code.Ptr(), rest.Ptr() + importOffset);

	CleanupStack::PopAndDestroy(cleanupCount);	
	}


TUint8* CDllChecker::NextBlock(TUint8* aBlock)
	{
	E32ImportBlock* block;	
	//	Advance the pointer to the next block	
	block=(E32ImportBlock*)aBlock;
	aBlock=(aBlock+sizeof(E32ImportBlock)+((block->iNumberOfImports)*sizeof(TUint)));		
	return (aBlock);
	}


TFileNameInfo::TFileNameInfo()
	{
	memclr(this, sizeof(TFileNameInfo));
	}

TInt TFileNameInfo::Set(const TDesC8& aFileName, TUint aFlags)
	{
	iUid = 0;
	iVersion = 0;
	iPathPos = 0;
	iName = aFileName.Ptr();
	iLen = aFileName.Length();
	iExtPos = aFileName.LocateReverse('.');
	if (iExtPos<0)
		iExtPos = iLen;
	TInt osq = aFileName.LocateReverse('[');
	TInt csq = aFileName.LocateReverse(']');
	if (!(aFlags & EAllowUid) && (osq>=0 || csq>=0))
		{
		return KErrBadName;
		}
	if (osq>=iExtPos || csq>=iExtPos)
		{
		return KErrBadName;
		}
	TInt p = iExtPos;
	if ((aFlags & EAllowUid) && p>=10 && iName[p-1]==']' && iName[p-10]=='[')
		{
		TPtrC8 uidstr(iName + p - 9, 8);
		TLex8 uidlex(uidstr);
		TUint32 uid = 0;
		TInt r = uidlex.Val(uid, EHex);
		if (r==KErrNone && uidlex.Eos())
			iUid = uid, p -= 10;
		}
	iUidPos = p;
	TInt ob = aFileName.LocateReverse('{');
	TInt cb = aFileName.LocateReverse('}');
	if (ob>=iUidPos || cb>=iUidPos)
		{
		return KErrBadName;
		}
	if (ob>=0 && cb>=0 && p-1==cb)
		{
		TPtrC8 p8(iName, p);
		TInt d = p8.LocateReverse('.');
		TPtrC8 verstr(iName+ob+1, p-ob-2);
		TLex8 verlex(verstr);
		if (ob==p-10 && d<ob)
			{
			TUint32 ver = 0;
			TInt r = verlex.Val(ver, EHex);
			if (r==KErrNone && verlex.Eos())
				iVersion = ver, p = ob;
			}
		else if (d>ob && p-1>d && (aFlags & EAllowDecimalVersion))
			{
			TUint32 maj = 0;
			TUint32 min = 0;
			TInt r = verlex.Val(maj, EDecimal);
			TUint c = (TUint)verlex.Get();
			TInt r2 = verlex.Val(min, EDecimal);
			if (r==KErrNone && c=='.' && r2==KErrNone && verlex.Eos() && maj<32768 && min<32768)
				iVersion = (maj << 16) | min, p = ob;
			}
		}
	iVerPos = p;
	if (iLen>=2 && iName[1]==':')
		{
		TUint c = iName[0];
		if (c!='?' || !(aFlags & EAllowPlaceholder))
			{
			c |= 0x20;
			if (c<'a' || c>'z')
				{
				return KErrBadName;
				}
			}
		iPathPos = 2;
		}
	TPtrC8 pathp(iName+iPathPos, iVerPos-iPathPos);
	if (pathp.Locate('[')>=0 || pathp.Locate(']')>=0 || pathp.Locate('{')>=0 || pathp.Locate('}')>=0 || pathp.Locate(':')>=0)
		{
		return KErrBadName;
		}
	iBasePos = pathp.LocateReverse('\\') + 1 + iPathPos;
	return KErrNone;
	}

void TFileNameInfo::GetName(TDes8& aName, TUint aFlags) const
	{
	if (aFlags & EIncludeDrive)
		aName.Append(Drive());
	if (aFlags & EIncludePath)
		{
		if (PathLen() && iName[iPathPos]!='\\')
			aName.Append('\\');
		aName.Append(Path());
		}
	if (aFlags & EIncludeBase)
		aName.Append(Base());
	if ((aFlags & EForceVer) || ((aFlags & EIncludeVer) && VerLen()) )
		{
		aName.Append('{');
		aName.AppendNumFixedWidth(iVersion, EHex, 8);
		aName.Append('}');		
		}
	if ((aFlags & EForceUid) || ((aFlags & EIncludeUid) && UidLen()) )
		{
		aName.Append('[');
		aName.AppendNumFixedWidth(iUid, EHex, 8);
		aName.Append(']');
		}
	if (aFlags & EIncludeExt)
		aName.Append(Ext());
	}


void CDllChecker::GetFileNameAndUid(CDllInfo &aDllInfo, const TDesC8 &aExportName)
//	
//	Gets filename and UID 
//
	{	
	TFileNameInfo filename;
	filename.Set(aExportName,TFileNameInfo::EAllowUid|TFileNameInfo::EAllowDecimalVersion);
	TBuf8<KMaxFileName> narrowDllName;
	filename.GetName(narrowDllName,TFileNameInfo::EIncludeBaseExt);
	aDllInfo.iDllName.Copy(narrowDllName);
	aDllInfo.iUid=TUid::Uid(filename.Uid());
	TUint32 version = filename.Version();
	aDllInfo.iMajorVersion = version >> 16;
	aDllInfo.iMinorVersion = version & 0xFFFF;
	}


TInt CDllChecker::CheckUid3(TInt32 aUid3,TUid aUid)
//
//	Check that Uid3 is the same in the iDllName and as noted by the Image Header
//	aUid3 is the value found by the image header
//	aUid is the value found by parsing the result of block->dllname 
//	using GetFileNameAndUid()
	{

	if ((aUid.iUid)==aUid3)
		{

		__PRINT(_L("\tUid3 is valid\r\n"));
		return KErrNone;
		}
	else
		{

		__PRINT(_L("\tUid3 value is not supported\r\n"));
		return (EUidNotSupported);
		}
	}



TInt CDllChecker::FindDll(TDes& aDllName,TFileName& aFileName, const TDesC& aPath)
//
// Search for a dll in the following sequence ...
// 1. Supplied path parameter
// 2. System directories on all drives
//
	{
	TFindFile findFile(Fs());
	TInt r=findFile.FindByPath(aDllName,&aPath);
	if (r==KErrNone)
		{
		aFileName=findFile.File();	

		__PRINT1(_L("\t\"%S\" was found (supplied path)\r\n"),&aFileName);

		return(r);
		}

	r=findFile.FindByDir(aDllName,_L("Y:\\Sys\\Bin\\"));
	if (r==KErrNone)
		{
		aFileName=findFile.File();	

		__PRINT1(_L("\t\"%S\" was found (system directory)\r\n"),&aFileName);

		return(r);
		}
	/*
	if(!PlatSec::ConfigSetting(PlatSec::EPlatSecEnforceSysBin))
		{
		r=findFile.FindByDir(aDllName,_L("\\System\\Bin\\"));
		if (r==KErrNone)
			{
			aFileName=findFile.File();	

			__PRINT1(_L("\t\"%S\" was found (system directory)\r\n"),&aFileName);

			return(r);
			}
		}
	*/
	__PRINT1(_L("\t\"%S\" was not found\r\n"),&aDllName);

	return(KErrNotFound);
	}

void CDllChecker::ListArray()
	{
	const TInt elements=iDllArray.Count();
	
	Printf(_L("Number of dependencies checked = %d\r\n"),elements);

	for (TInt i=0;i<elements; i++)
		{
		CDllInfo& info = *(iDllArray[i]);
		TResultCheck res = info.iResult;
		TBool print = iVerbose || (res != EFileFoundAndUidSupported && res != ENoImportData && res != EAlreadyOpen);
		if (print) Printf(_L("% 2d: %-15S  Uid3: [%08x] "),(i+1),&info.iDllName,(info.iUid));
		if (!print) continue;
		switch(res)
			{
		case(ENoImportData):
			Printf(_L("--- No import data\r\n"));
			break;

		case(EUidNotSupported):
			Printf(_L("--- Uid3 is not supported\r\n"));
			break;

		case(ENotFound):
			Printf(_L("--- File was not found\r\n"));
			break;

		case(ECouldNotOpenFile):
			Printf(_L("--- File could not be opened\r\n"));
			break;

		case(EUidDifference):
			Printf(_L("--- File already noted with different Uid\r\n"));
			break;

		case(EAlreadyOpen):
			Printf(_L("--- File already open\r\n"));
			break;
		
		case(EFileFoundAndUidSupported):
			Printf(_L("--- File was found, Uid3 is supported\r\n"));
			break;

		case ERequiredCapabilitiesMissing:
			{
			Printf(_L("--- Dll is missing capabilities:"));
			for (TInt i = 0; i < ECapability_Limit; i++)
				{
				TCapability c = (TCapability)i;
				if (iRequiredCaps->HasCapability(c) && !info.iDllCaps.HasCapability(c))
					{
					Printf(_L8(" %s"), CapabilityNames[i]);
					}
				}
			Printf(_L("\r\n"));
			break;
			}
		case EOrdinalMissing:
			Printf(_L("--- Required ordinal(s) missing\r\n"));
			break;
		case EVersionMismatch:
			Printf(_L("--- Version mismatch\r\n"));
			break;
		default:	//	Will never reach here
			Printf(_L("--- Undefined\r\n"));
			break;
			}
		}
	}
