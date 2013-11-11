// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <gdi.h>
#include <f32file.h>
#include <s32file.h>
#include <bautils.h>
#include <barsc.h>
#include <barsread.h>
#include "GDIPANIC.h"

_LIT(KPdrExtension,"*.PDR"); // must be capitalized
_LIT(KPdlExtension,".PDL"); // must be capitalized
_LIT(KUdlExtension,".UDL"); // must be capitalized
_LIT(KRscExtension,".RSC");
_LIT(KGdiPrintPanic,"GDI - PRINT");

_LIT(KGDIPanicDesc1, "GDI Pdr internal Panic %S, in file %S @ line %i");
_LIT(KGDIPanicDesc2, "Assert condition = \"%S\"");

enum TPdrStorePanic
	{
	EPdrModelIndexOutOfRange,
	EPdrModelUidNotFound,
	EPdrDirectoryIndexOutOfRange,
	EPdrFileIndexOutOfRange,
	EPdrPrinterDeviceExists,
	EPdrPrinterDeviceDoesNotExist,
	};

void Panic(TPdrStorePanic aPanic)
	{
	User::Panic(KGdiPrintPanic,aPanic);
	}
	
void PanicWithCondAndInfo(TPdrStorePanic aError, const TDesC& aCondition, const TDesC& aFileName, const TDesC& aPanicName, TInt aLine)
	{
	TBuf<256> buf;
	buf.Format(KGDIPanicDesc1, &aPanicName, &aFileName, aLine);
	RDebug::Print(buf);

	buf.Format(KGDIPanicDesc2, &aCondition);
	RDebug::Print(buf);
	Panic(aError);
	}

//
// TPrinterModelEntry
//


EXPORT_C void TPrinterModelEntry::InternalizeL(RReadStream& aStream)

/** Internalises a printer model entry from a read stream. 

The presence of this function means that the standard templated stream operator>>(), 
defined in s32strm.h, is available to internalise objects of this class.

@param aStream The read stream. */
    {
	aStream >> iModelName;
	iRequiresPrinterPort=aStream.ReadUint8L();
	aStream >> iUid;
	}


EXPORT_C void TPrinterModelEntry::ExternalizeL(RWriteStream& aStream) const
/** Externalises the printer model entry to a write stream.

The presence of this function means that the standard templated stream operator<<(), 
defined in s32strm.h, is available to externalise objects of this class.

@param aStream The write stream. */
	{
	aStream << iModelName;
	aStream.WriteUint8L((TUint8) iRequiresPrinterPort);
	aStream << iUid;
	}

//
// TPrinterModelHeader
//


EXPORT_C void TPrinterModelHeader::InternalizeL(RReadStream& aStream)
/** Internalises a printer model header from a read stream. 

The presence of this function means that the standard templated stream operator>>(), 
defined in s32strm.h, is available to internalise objects of this class.

@param aStream The read stream. */
 	{
	aStream >> iEntry;
	aStream >> iModelDataStreamId;
	}


EXPORT_C void TPrinterModelHeader::ExternalizeL(RWriteStream& aStream) const
 
/** Externalises the printer model header to a write stream.

The presence of this function means that the standard templated stream operator<<(), 
defined in s32strm.h, is available to externalise objects of this class.
@param aStream The write stream. */
    {
	aStream << iEntry;
	aStream << iModelDataStreamId;
	}

//
// CPrinterDevice
//

EXPORT_C CPrinterDevice::CPrinterDevice():
	CGraphicsDevice(),
	iCurrentPageSpecInTwips()
/** Standard constructor. */
	{}


EXPORT_C CPrinterDevice::~CPrinterDevice()

/** Destructor.
It frees all resources owned by the object, prior to its destruction. */
	{
	delete iControl;
	}


EXPORT_C void CPrinterDevice::SelectPageSpecInTwips(const TPageSpec& aPageSpecInTwips)
/** Sets the page specification in twips.
@param  aPageSpec The page specification in twips. */	
    {
	iCurrentPageSpecInTwips=aPageSpecInTwips;
	}


EXPORT_C TRect CPrinterDevice::PrintablePageInPixels() const
/** Gets the dimensions of the area to which the printer device can print.

These dimensions are normally less than those returned by TPageSpec::OrientedPageSize() 
because a margin exists between the boundary of the printable page and the 
absolute extent of the page.

@return The dimensions of the printer device area in pixels, respecting the 
page orientation */
	{
	return TRect(SizeInPixels());
	}


EXPORT_C void CPrinterDevice::DeleteControl()
 /** Deletes the printer control owned by this object.

The function sets the iControl member to NULL. */
    {
	delete iControl;
	iControl=NULL;
	}


EXPORT_C void CPrinterDevice::RestorePropertiesL()
/** Restores printer properties. */	
    {
	_LIT(KSystemIniFileNameSpec,"Z:\\System\\System.ini");

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));
	TParse systemIniFileName;
	systemIniFileName.Set(drive.Name(), &KSystemIniFileNameSpec, NULL);
	
	TUint atts = 0;
	TInt ret = fs.Att(systemIniFileName.FullName(), atts);
	if (ret == KErrNone)
		{
		CDictionaryStore* dictionarystore = NULL;
		TRAPD(err,dictionarystore = CDictionaryFileStore::SystemL(fs));
		if (err == KErrNone)
			{
			CleanupStack::PushL(dictionarystore);
			if (dictionarystore->IsPresentL(Model().iUid))
				{
				RDictionaryReadStream stream;
				stream.OpenLC(*dictionarystore,Model().iUid);
				InternalizePropertiesL(stream);
				CleanupStack::PopAndDestroy(); // stream
				}
			CleanupStack::PopAndDestroy(); // dictionarystore
			}
		}
	CleanupStack::PopAndDestroy(); // fs
	}


EXPORT_C void CPrinterDevice::StorePropertiesL() const
/**  Stores the printer properties. */
    {
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	CDictionaryStore* dictionarystore = CDictionaryFileStore::SystemLC(fs);
	RDictionaryWriteStream stream;
	stream.AssignLC(*dictionarystore,Model().iUid);
	ExternalizePropertiesL(stream);
	stream.CommitL();
	CleanupStack::PopAndDestroy(); // stream
	dictionarystore->CommitL();
	CleanupStack::PopAndDestroy(2); // dictionarystore, fs
	}

//
// CPrinterControl
//


EXPORT_C CPrinterControl::~CPrinterControl()
/**  Destructor.

It frees all resources owned by the object, prior to its destruction. */
 {
	delete iPrinterPort;
	}

EXPORT_C CPrinterControl::CPrinterControl(CPrinterPort* aPrinterPort):
	CBase(),
	iState(ENotPrinting),
	iPrinterPort(aPrinterPort)
	{}

//
// TPageSpec
//


EXPORT_C TPageSpec::TPageSpec():
	iPortraitPageSize(TSize(0,0)),
	iOrientation(EPortrait)
/** Default constructor. 

Initialises the page orientation to portrait and the page height and width 
to zero. */
	{}


EXPORT_C TPageSpec::TPageSpec(TPageOrientation anOrientation,const TSize& aSize):
	iPortraitPageSize(aSize),
	iOrientation(anOrientation)
/** Constructor with page orientation and size. 

@param aOrientation Specifies the page orientation. 
@param aSize Specifies the page size. */
 	{}


EXPORT_C void TPageSpec::InternalizeL(RReadStream& aStream)
/** Internalises a page specification object from a read stream. 

The presence of this function means that the standard templated stream operator>>(), 
defined in s32strm.h, is available to internalise objects of this class.

@param aStream The read stream. */
 	{
	iPortraitPageSize.iWidth = aStream.ReadInt32L();
	iPortraitPageSize.iHeight = aStream.ReadInt32L();
	iOrientation=(TPageSpec::TPageOrientation)aStream.ReadInt8L();
	}


EXPORT_C void TPageSpec::ExternalizeL(RWriteStream& aStream) const
/** Externalises the page specification object to a write stream.

The presence of this function means that the standard templated stream operator<<(), 
defined in s32strm.h, is available to externalise objects of this class.
@param aStream The write stream. */
 	{
	aStream.WriteInt32L(iPortraitPageSize.iWidth);
	aStream.WriteInt32L(iPortraitPageSize.iHeight);
	aStream.WriteInt8L(iOrientation);
	}


EXPORT_C TSize TPageSpec::OrientedPageSize()const
/** Gets the oriented page size.

The oriented page size is the absolute width and height of the page, respecting 
the page orientation.
@return The oriented page size (in pixels or twips). */
  	{
	if(iOrientation==EPortrait)
		return(iPortraitPageSize);
	return(TSize(iPortraitPageSize.iHeight,iPortraitPageSize.iWidth));
	}


EXPORT_C TBool TPageSpec::operator==(const TPageSpec& aPageSpec) const
/** Equality operator. 

This operator compares page specifications for equality. Two page specifications 
are equal if both their orientations and portrait page sizes are equal.

@param aPageSpec Page specification to be compared.
@return ETrue, if the page specifications are equal; EFalse, otherwise. */
   {
	return(iPortraitPageSize==aPageSpec.iPortraitPageSize &&
		iOrientation==aPageSpec.iOrientation);
	}

 
EXPORT_C TBool TPageSpec::operator!=(const TPageSpec& aPageSpec) const
/** Inequality operator.

This operator compares two page specifications for inequality. Two page specifications 
are unequal if one or both of their orientations and portrait page sizes differ.

@param aPageSpec Page specification to be compared.
@return ETrue, if the page specifications differ; EFalse, otherwise. */
	{
	return(!(*this==aPageSpec));
	}


// TMargins

EXPORT_C void TMargins::InternalizeL(RReadStream& aStream)
/** Internalises a set of margins from a read stream.

The presence of this function means that the standard templated stream operator>>() 
is available to internalise objects of this class.

@param aStream Stream from which the object is internalised. */
	{
	iLeft = aStream.ReadInt32L();
	iRight = aStream.ReadInt32L();
	iTop = aStream.ReadInt32L();
	iBottom = aStream.ReadInt32L();
	}

EXPORT_C void TMargins::ExternalizeL(RWriteStream& aStream) const
/** Externalises a set of margins to a write stream.

The presence of this function means that the standard templated stream operator<<() 
is available to externalise objects of this class.

@param aStream Stream to which the object is externalised. */
	{
	aStream.WriteInt32L(iLeft);
	aStream.WriteInt32L(iRight);
	aStream.WriteInt32L(iTop);
	aStream.WriteInt32L(iBottom);
	}

EXPORT_C TBool TMargins::operator==(const TMargins& aMargins) const
/** Tests margins for equality.

@param aMargins The margin to be compared with this margin. 
@return ETrue, if the margins are equal; EFalse, otherwise. */
	{
	return(iLeft==aMargins.iLeft && iRight==aMargins.iRight &&
		iTop==aMargins.iTop && iBottom==aMargins.iBottom);
	}

EXPORT_C TBool TMargins::operator!=(const TMargins& aMargins) const
/** Tests margins for inequality.

@param aMargins The margin to be compared with this margin. 
@return ETrue, if the margins are unequal; EFalse, otherwise. */
	{
	return(!(*this==aMargins));
	}

//
// CPrinterDriverUI
//

EXPORT_C CPrinterDriverUI::CPrinterDriverUI()
	{
	__DECLARE_NAME(_S("CPrinterDriverUI"));
	}


EXPORT_C TBool CPrinterDriverUI::BeforePrintL()
 /** Provides an opportunity for a dialog to be put up before printing begins.

@return ETrue, if printing is to continue; EFalse, if printing is to be cancelled. 
The default implementation returns ETrue. */
  	{
	return ETrue;
	}


EXPORT_C void CPrinterDriverUI::AfterPrintL()
 /** Provides an opportunity for a dialog to be put up after printing is complete.
The default implementation is empty. */
	{
	}


EXPORT_C void CPrinterDriverUI::SetPropertiesL()
/** Provides an opportunity for a dialog to be put up to capture or change printer 
properties.
The default implementation is empty. */
	{
	}


EXPORT_C TBool CPrinterDriverUI::CanSetProperties()
/** Tests whether printer properties can be set.

@return ETrue, if printer properties can be set; EFalse, otherwise. The default 
implementation returns EFalse. */
    {
	return EFalse;
	}

//
// CPrinterDriver
//


EXPORT_C CPrinterDriver* CPrinterDriver::NewL()
/** Constructs, and returns a pointer to a new instance for accessing a printer 
specification data store.

@return Pointer to the object for accessing a printer specification data store. */
	{
	CPrinterDriver* printerdriver=new(ELeave) CPrinterDriver;
	CleanupStack::PushL(printerdriver);
	User::LeaveIfError(printerdriver->iFs.Connect());
	CleanupStack::Pop();
	return printerdriver;
	}


EXPORT_C CPrinterDriver::~CPrinterDriver()
/** Destructor.

It frees all resources owned by the object, prior to its destruction. In particular, 
it closes the printer specification data store and any open session with the file server.  */	
    {
	Close();
	iFs.Close();
	}


EXPORT_C void CPrinterDriver::OpenPdrL(const TDesC &aName)
/**  Opens the specified printer specification data store.

@return  The name of the printer specification data store. This must be a 
valid printer specification data store,otherwise the function leaves with
KErrNotSupported. */	
    {
	Close();
	TRAPD(ret,DoOpenPdrL(aName));
	if (ret!=KErrNone)
		{
		Close();
		User::Leave(ret);
		}
	}

EXPORT_C void CPrinterDriver::Close()
/** Closes the printer specification data store and frees resources.

An open session with the file server remains open. */
	{
	delete iPdrStore,
	iPdrStore=NULL;
	iNumModels=0;
	delete[] iModelList;
	iModelList=NULL;
	DeletePrinterDevice();
	}


EXPORT_C TInt CPrinterDriver::NumModels() const
 /** Gets the number of printer models defined by the printer specification.

@return The number of printer models. */
 	{
	return iNumModels;
	}


EXPORT_C TPrinterModelEntry CPrinterDriver::Model(TInt aNum) const
/** Gets the specified printer model.

@param aNum An index into the list of printer models defined in the printer 
specification data.
@return Model specific information. */
 	{
	GDI_ASSERT_DEBUG(aNum>=0,EPdrModelIndexOutOfRange);
	GDI_ASSERT_DEBUG(aNum<iNumModels,EPdrModelIndexOutOfRange);
	return iModelList[aNum].iEntry;
	}


EXPORT_C CPrinterDevice* CPrinterDriver::CreatePrinterDeviceL(TUid aModelUid)
/** Creates the physical graphics device to be used for printing.

@param aModelUid The UID of a specific model which is defined in the printer 
specification data.
@return The physical graphics device to be used for printing.  */	
    {
	GDI_ASSERT_DEBUG(!iPrinterDevice,EPdrPrinterDeviceExists);
	TRAPD(ret,DoCreatePrinterDeviceL(aModelUid));
	if (ret!=KErrNone)
		{
		DeletePrinterDevice();
		User::Leave(ret);
		}
	return iPrinterDevice;
	}

void CPrinterDriver::LoadLibraryL(RLibrary& aLibrary,const TDesC& aExt,TUid aUid2)
	{
	TFileName filename=iPdlName;
	filename.Append(aExt);

	User::LeaveIfError(aLibrary.Load(filename));
	TUidType type=aLibrary.Type();
	if (type[1]!=aUid2 && type[2]!=iPdlUid)
		{
		aLibrary.Close();
		User::Leave(KErrNotSupported);
		}
	if (type[1]!=aUid2)
		{
		aLibrary.Close();
		User::Leave(KErrNotFound);
		}
	}


EXPORT_C CPrinterDriverUI* CPrinterDriver::CreatePrinterDriverUIL()
/** Constructs a printer specific user interface.

The user interface object is optional, but if it exists, it is implemented 
as part of a UDL (i.e. a UI DLL).

@return A pointer to the printer specific user interface, or NULL if there is 
none. */
    {
	GDI_ASSERT_DEBUG(iPrinterDevice,EPdrPrinterDeviceDoesNotExist);
	if (iUdlLibrary.Handle()==0)
		{
		TRAPD(ret,LoadLibraryL(iUdlLibrary,KUdlExtension,TUid::Uid(KUdlUidVal)));
		if (ret==KErrNotFound)
			return NULL;
		else
			User::LeaveIfError(ret);
		}
	TLibraryFunction f = iUdlLibrary.Lookup(1);
	CPrinterDriverUI* printerdriverui=(CPrinterDriverUI*)((*f)());
	CleanupStack::PushL(printerdriverui);
	User::LeaveIfError(printerdriverui->SetPrinterDevice(iPrinterDevice));
	CleanupStack::Pop();
	return printerdriverui;
	}

CPrinterDriver::CPrinterDriver()
	{}

void CPrinterDriver::DeletePrinterDevice()
	{
	iUdlLibrary.Close();
	delete iPrinterDevice;
	iPrinterDevice=NULL;
	iPdlLibrary.Close();
	}

void CPrinterDriver::DoOpenPdrL(const TDesC &aName)
	{
	Close();
	iPdrStore=CDirectFileStore::OpenL(iFs,aName,EFileStream|EFileRead|EFileShareReadersOnly);
	if (iPdrStore->Type()[1]!=TUid::Uid(KPdrStoreFileUidVal))
		User::Leave(KErrNotSupported);
	TStreamId headerid = iPdrStore->Root();
	RStoreReadStream stream;
	stream.OpenLC(*iPdrStore,headerid);
		stream >> iPdlName;
	stream >> iPdlUid;
	iNumModels = stream.ReadInt32L();
	iModelList = new(ELeave) TPrinterModelHeader[iNumModels];
	for (TInt i=0; i<iNumModels; i++)
		iModelList[i].InternalizeL(stream);
	CleanupStack::PopAndDestroy();
	}

void CPrinterDriver::DoCreatePrinterDeviceL(TUid aModelUid)
	{
	if (!iPdlName.Length())
		User::Leave(KErrGeneral); // !! find a better error number
	LoadLibraryL(iPdlLibrary,KPdlExtension,TUid::Uid(KPdlUidVal));
	TLibraryFunction f = iPdlLibrary.Lookup(1);
	iPrinterDevice=(CPrinterDevice*)((*f)());
	TInt i;
	for (i=0; (i<iNumModels) && (aModelUid!=iModelList[i].iEntry.iUid); i++)
		{
		}
	GDI_ASSERT_DEBUG(i<iNumModels,EPdrModelUidNotFound);
	User::LeaveIfError(iPrinterDevice->SetModel(iModelList[i],*iPdrStore));
	iPrinterDevice->RestorePropertiesL();
	}

//
// CPdrModelList
//


EXPORT_C CPdrModelList* CPdrModelList::NewL()
/** Constructs, and returns a pointer to a new instance of the printer model 
list interface.
@return Pointer to the new printer model list interface object. */
    {
	CPdrModelList* modellist=new(ELeave) CPdrModelList();
	CleanupStack::PushL(modellist);
	modellist->ConstructL();
	CleanupStack::Pop();
	return modellist;
	}


EXPORT_C CPdrModelList::~CPdrModelList()
/** Virtual destructor.
Frees resources owned by the object, prior to its destruction. */
	{
	delete iModelArray;
	delete iFileArray;
	if (iDirectoryArray)
		{// delete all the HBufC's
		for (TInt i=iDirectoryArray->Count()-1 ; i>=0 ; i--)
			delete (*iDirectoryArray)[i];
		delete iDirectoryArray;
		}
	iFileServer.Close();
	}


EXPORT_C TInt CPdrModelList::ModelCount() const
/** Gets the number of printer models in the printer model list.
@return The number of printer models. */
    {
	return iModelArray->Count();
	}


EXPORT_C const TPrinterModelEntry CPdrModelList::operator [] (TInt anIndex)
/** Gets printer model name.

This is the name of the printer model at the specified index within the list 
of printer models.

@param anIndex The index of the printer model within the array of printer 
models. Note that this number must be between zero and ModelCount(). 

@return Name of printer model, up to 32 characters long */
 	{
	GDI_ASSERT_DEBUG(anIndex>=0,EPdrModelIndexOutOfRange);
	GDI_ASSERT_DEBUG(anIndex<iModelArray->Count(),EPdrModelIndexOutOfRange);

	return (*iModelArray)[anIndex].iEntry;
	}


EXPORT_C TInt CPdrModelList::UidToNum(TUid aModelUid) const
/** Gets a printer model's index within the model list from its UID.

@param aModelUid The UID of the printer model.
@return The index of the printer model within the array of printer models if 
found; KErrNotFound, otherwise. */
    {
	TInt i,count=iModelArray->Count();
	for (i=0; (i<count) && (aModelUid!=(*iModelArray)[i].iEntry.iUid); i++)
		{
		}

	if (i==count)
		i=KErrNotFound;

	return i;
	}


EXPORT_C void CPdrModelList::AddDirectoryL(const TDesC& aDir)
/** Adds a directory to the list of directories to be scanned for printer models.

@param aDir The directory to be added to the list. */
	{
	HBufC* buf = HBufC::NewL(aDir.Length());
	CleanupStack::PushL(buf);
	*buf = aDir;
	iDirectoryArray->AppendL(buf);
	CleanupStack::Pop(); //buf
	}

LOCAL_C void DereferenceAndDeleteHBufC8(TAny* aPointerToPointerToHBufC8)
	{
	delete *STATIC_CAST(HBufC8**, aPointerToPointerToHBufC8);
	}


EXPORT_C CPrinterModelList* CPdrModelList::ScanForModelsL()
/** Scans through through the list of directories for all .pdr files and generates 
a list of printer models.

@return The list of model names. */
	{
	iModelArray->Reset();
	iFileArray->Reset();
	// check that there is at least one directory to parse?
	// get a list of *.pdr files in all directories specified (using AddDirectory())
	for (TInt index=iDirectoryArray->Count()-1 ; index>=0 ; index--)
		ScanDirectoryL(index);
 	// then go through the files one at a time, adding all models to the list
	TParse* parser = new(ELeave) TParse;
	CleanupStack::PushL(parser);
	TFileName* nameOfLoadedResourceFile=new(ELeave) TFileName;
	CleanupStack::PushL(nameOfLoadedResourceFile);
	TFileName* tempFileName=new(ELeave) TFileName;
	CleanupStack::PushL(tempFileName);
	RResourceFile resourceFile;
	CleanupClosePushL(resourceFile);
	HBufC8* resource=NULL;
	CleanupStack::PushL(TCleanupItem(DereferenceAndDeleteHBufC8, &resource));
	for (TInt fileNum=iFileArray->Count()-1 ; fileNum>=0 ; fileNum--)
		ListModelsL(fileNum, *parser, *nameOfLoadedResourceFile, *tempFileName, resourceFile, resource);
	CleanupStack::PopAndDestroy(5); // resource, resourceFile, tempFileName, nameOfLoadedResourceFile and parser
	// return a handle to the array of model names
	return this;
	}


EXPORT_C CPrinterDriver* CPdrModelList::CreatePrinterDriverL(TInt anIndex)
/** Creates an object for accessing the specified store that contains printer specification 
data.

@param anIndex An index into a list of files containing printer specification 
data. The files are the complete set of .pdr files found in the directories 
known to this object.
@return A pointer to the object representing the store containing the printer 
specification data. */
 	{
	GDI_ASSERT_DEBUG(anIndex>=0,EPdrModelIndexOutOfRange);
	GDI_ASSERT_DEBUG(anIndex<iModelArray->Count(),EPdrModelIndexOutOfRange);

	CPrinterDriver* driver = CPrinterDriver::NewL();
	CleanupStack::PushL(driver);
	HBufC* file = NewPathBufL(*((*iModelArray)[anIndex].iFile));
	CleanupStack::PushL(file);
	driver->OpenPdrL(*file);
	driver->CreatePrinterDeviceL((*iModelArray)[anIndex].iEntry.iUid);
	CleanupStack::PopAndDestroy();
	CleanupStack::Pop();
	return driver;
	}

 
 CPdrModelList::CPdrModelList():
	iModelArray(NULL),
	iFileArray(NULL),
	iDirectoryArray(NULL)
	{
	}
 
 void CPdrModelList::ConstructL()
	{
	__DECLARE_NAME(_S("CPdrModelList"));
	iModelArray = new(ELeave) CArrayFixSeg<TModelEntry>(5);
	iFileArray = new(ELeave) CArrayFixFlat<TFileEntry>(5);
	iDirectoryArray = new(ELeave) CArrayFixFlat<HBufC*>(1);
	User::LeaveIfError(iFileServer.Connect());
	}

void CPdrModelList::ScanDirectoryL(TInt aDirIndex)
/** Scans the given directory, parsing all files found
 If a file is of the correct type it is appended to the file list*/
	{
	GDI_ASSERT_DEBUG(aDirIndex>=0,EPdrDirectoryIndexOutOfRange);
	GDI_ASSERT_DEBUG(aDirIndex<iDirectoryArray->Count(),EPdrDirectoryIndexOutOfRange);

	TDesC* dir = (*iDirectoryArray)[aDirIndex];
	TParse path;
	path.Set(KPdrExtension,dir,NULL);
	CDir* fileList;
	TInt ret = iFileServer.GetDir(path.FullName(),KEntryAttNormal,ESortByName,fileList);
	if (ret == KErrNone)
		{
		CleanupStack::PushL(fileList);
		for (TInt i=fileList->Count()-1 ; i>=0 ; i--) 
			{
			TFileEntry& entry = iFileArray->ExtendL();
			entry.iFileName = (*fileList)[i].iName;
			entry.iDirectory = dir;
			}
		CleanupStack::PopAndDestroy(); // fileList
		}
	else if (ret == KErrNoMemory) // Ignore errors other than KErrNoMemory
		User::LeaveNoMemory();
	}

void CPdrModelList::ListModelsL(TInt aFileIndex, TParse& aParser, TFileName& aNameOfLoadedResourceFile, TFileName& aTempFileName, RResourceFile& aResourceFile, HBufC8*& aResource)
/** given a pdr file list all the models it contains in the model array */
	{
	GDI_ASSERT_DEBUG(aFileIndex>=0,EPdrFileIndexOutOfRange);
	GDI_ASSERT_DEBUG(aFileIndex<iFileArray->Count(),EPdrFileIndexOutOfRange);

	CPrinterDriver* driver = CPrinterDriver::NewL() ;
	CleanupStack::PushL(driver);
	// open the file
	HBufC* fullPath = NewPathBufL((*iFileArray)[aFileIndex]);
	TRAPD(ret,driver->OpenPdrL(*fullPath));
	delete fullPath;
	if ((ret!=KErrNone) && (ret!=KErrNotSupported))
		User::Leave(ret);
	// get info on the models one by one and insert them into the array
	if (ret==KErrNone)
		{
		TModelEntry modelentry;
		modelentry.iFile = &(*iFileArray)[aFileIndex]; // set the file pointer for the entry
		const TInt numberOfModels = driver->NumModels();
		for (TInt i=0 ; i<numberOfModels ; i++)
			{
			modelentry.iEntry=driver->Model(i);
			if (UidToNum(modelentry.iEntry.iUid)==KErrNotFound)
				{
				User::LeaveIfError(aParser.SetNoWild(KRscExtension, &modelentry.iFile->iFileName, modelentry.iFile->iDirectory));
				aTempFileName=aParser.FullName();
				BaflUtils::NearestLanguageFile(iFileServer, aTempFileName);
				if (aNameOfLoadedResourceFile.CompareF(aTempFileName)!=0)
					{
					if (!BaflUtils::FileExists(iFileServer, aTempFileName))
						{
						iModelArray->AppendL(modelentry); // no resource file found, so reverting to old behaviour (i.e. where the model-name is set from the PDR file)
						continue;
						}
					aNameOfLoadedResourceFile=KNullDesC;
					aResourceFile.OpenL(iFileServer, aTempFileName);
					HBufC8* resource=aResourceFile.AllocReadL(aResourceFile.Offset()+2); // read the first resource after the RSS_SIGNATURE resource
					delete aResource;
					aResource=resource;
					aNameOfLoadedResourceFile=aTempFileName;
					}
				TResourceReader resourceReader;
				resourceReader.SetBuffer(aResource);
				for (TInt j=resourceReader.ReadUint16()-1; ; --j)
					{
					if (j<0)
						{
						iModelArray->AppendL(modelentry); // no matching uid found in the resource file, so reverting to old behaviour (i.e. where the model-name is set from the PDR file)
						break;
						}
					TInt uid=resourceReader.ReadInt32();
					TPtrC name=resourceReader.ReadTPtrC();
					if (uid==modelentry.iEntry.iUid.iUid)
						{
						if (name.Length()>0)
							{
							modelentry.iEntry.iModelName=name;
							iModelArray->AppendL(modelentry);
							}
						break;
						}
					}
				}
			}
		}
	CleanupStack::PopAndDestroy(); // driver
	}


HBufC* CPdrModelList::NewPathBufL(const TFileEntry& aFileEntry)  
/** Create a buf of the right length and... 
 set its contents to the full filename of model aModel */
	{
	// Create a buf of the right length
	HBufC* buf = HBufC::NewL(aFileEntry.iFileName.Length()+aFileEntry.iDirectory->Length());
	// Insert the file path
	TPtr filePtr = buf->Des();
	filePtr.Append(*aFileEntry.iDirectory);
	filePtr.Append(aFileEntry.iFileName);
	return buf;
	}

// !!!!
// Retained for binary compatibility only: remove if we make a binary incompatible release
//

IMPORT_C void GDI_Reserved();
EXPORT_C void GDI_Reserved()
	{}

