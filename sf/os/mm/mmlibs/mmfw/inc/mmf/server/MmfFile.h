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
//

#ifndef __MMFFILE_H_
#define __MMFFILE_H_

#include <f32file.h>

#include <mmf/server/mmfclip.h>
#include <mmf/server/mmfdatabuffer.h>
#include <mmf/common/mmcaf.h>

class CReadWriteRequest;
class CTransferBufferCopy;
class MGenericFile;

/**
@publishedAll
@released

Provides the filename and path of file. 

Used by CMMFFile::ConstructL().
*/
class TMMFFileParams
	{
public:
	/**
	Filename and path of file. 
	*/
	TFileName iPath ; // Filename and path of file.
private:
	/**
	This member is internal and not intended for use.
	*/
	TInt iReserved1;
	};
	
/**
@publishedAll	
*/
const TUid KFileHandleUid = {0x101FFA19};
const TUid KMMFileSourceUid = {0x101F7DA9};
const TUid KMMFileHandleSourceUid = {0x101F7DAA};
	
/**
@publishedAll
@released

Provides a pointer to the file handle 

Used by CMMFFile::ConstructL().
*/
class TMMFFileHandleParams
	{
public:
	TMMFFileHandleParams(RFile* aFile) : iUid(KFileHandleUid), iFile(aFile) {}
	TMMFFileHandleParams() : iUid(KFileHandleUid) {}
		
	TUid iUid;
	/**
	Handle of file 
	*/
	RFile* iFile;
private:
	/**
	This member is internal and not intended for use.
	*/
	TInt iReserved1;
	};
	
	
	

/**
@publishedAll
@released
*/
typedef TPckgBuf<TMMFFileParams>  TMMFFileConfig ;

/**
@publishedAll
@released
*/
typedef TPckgBuf<TMMFFileHandleParams>  TMMFFileHandleConfig ;


/**
@publishedAll
@released

This details how big the iTransferBufferCopies array of CMMFFile may
become before we start looking to free entries.
*/
const TInt KAcceptableTransferBufferCopiesSize = 5;

/**
@publishedAll
@released

This details how big the iTransferBufferCopies array of CMMFFile may
become before we start looking to free entries.
*/
const TInt KMaximumTransferBufferCopiesSize = 100;


/**
@publishedAll
@released

MultiMedia framework class.  Represents a physical file.

This may be source file from which data is read or destination to which data is written.

Intended to be used by controller plugin developers for source and sink plugins.
*/
class CMMFFile : public CMMFClip
	{
public:
	// From MDataSource
	virtual TFourCC SourceDataTypeCode(TMediaId aMediaId) ;
	virtual void FillBufferL( CMMFBuffer* aBuffer, MDataSink* aConsumer, TMediaId aMediaId ) ;
	virtual void BufferEmptiedL( CMMFBuffer* aBuffer ) ;
	virtual TBool CanCreateSourceBuffer() ;
	virtual CMMFBuffer* CreateSourceBufferL( TMediaId aMediaId, TBool &aReference ) ;
	virtual TInt SourceThreadLogon(MAsyncEventHandler& aEventHandler) ;
	virtual void SourceThreadLogoff();
	virtual void SourcePrimeL();
	virtual void SourceStopL();

	// From MDataSink
	virtual TFourCC SinkDataTypeCode(TMediaId aMediaId) ; //used by data path MDataSource/Sink for codec matching
	virtual void EmptyBufferL( CMMFBuffer* aBuffer, MDataSource* aSupplier, TMediaId aMediaId ) ;
	virtual void BufferFilledL( CMMFBuffer* aBuffer ) ;
	virtual TBool CanCreateSinkBuffer() ;
	virtual CMMFBuffer* CreateSinkBufferL( TMediaId aMediaId , TBool &aReference) ;
	virtual TInt SinkThreadLogon(MAsyncEventHandler& aEventHandler) ;
	virtual void SinkThreadLogoff();
	virtual void SinkPrimeL();
	virtual void SinkStopL();

	// From CMMFClip
	virtual void ReadBufferL( TInt aLength, CMMFBuffer* aBuffer, TInt aPosition, MDataSink* aConsumer);
	virtual void WriteBufferL( TInt aLength, CMMFBuffer* aBuffer, TInt aPosition, MDataSource* aSupplier);
	virtual void ReadBufferL( CMMFBuffer* aBuffer, TInt aPosition, MDataSink* aConsumer) ;
	virtual void WriteBufferL( CMMFBuffer* aBuffer, TInt aPosition, MDataSource* aSupplier) ;
	virtual void ReadBufferL( CMMFBuffer* aBuffer, TInt aPosition ) ;
	virtual void WriteBufferL( CMMFBuffer* aBuffer, TInt aPosition ) ;
	virtual TInt64 BytesFree() ;  // amount of space available for the clip
	virtual TInt Size() ;
	virtual TInt Delete();
	virtual TInt SetSize(TInt aSize);

	// New Functions
	virtual const TDesC& FileName() const ;
	virtual const TDesC& Extension() const ;
	virtual const TDesC& FilePath() const ;
	virtual const TDesC& FileDrive() const ;
	virtual const TFileName FullName() const;
	virtual RFile& FileL() ;

public :
	virtual ~CMMFFile() ;
	static MDataSource* NewSourceL() ;
	static MDataSink* NewSinkL() ;

	void SetTransferBuffer (TBool aTBuffer) ;
	TBool CanUseTransferBuffer () ;
	virtual TBool SinkStopped();

protected:
	/**
	Indicates for which purpose the object instance is being created
	*/
	enum TMMFileMode
		{
		/** The object is created for being a source */
		ESourceMode,
		/** The object is created for being a sink */
		ESinkMode
		};
	virtual void ConstructSourceL( const TDesC8& aInitData ) ;
	virtual void ConstructSinkL( const TDesC8& aInitData ) ;
	void ConstructL(const TDesC8& aInitData, TMMFileMode aFileMode) ;
	CMMFFile() ;

	void StoreRequestL( CReadWriteRequest* aRequest ) ;
	void CancelRequests();


	CTransferBufferCopy* ObtainCopyOfTransferBufferL(TInt aMaxLength);

public:
	// Additional virtuals for DRM Intent
	virtual TInt ExecuteIntent(ContentAccess::TIntent aIntent);
	virtual TInt EvaluateIntent(ContentAccess::TIntent aIntent) const;
	virtual TBool IsProtectedL() const;

	virtual TInt SetAgentProperty(ContentAccess::TAgentProperty aProperty, TInt aValue); 
	virtual const TDesC& UniqueId() const;

private:
	class CMMFFileAsyncEventHandler : public MAsyncEventHandler
		{
		public:
			CMMFFileAsyncEventHandler(CMMFFile* aParent);
			virtual ~CMMFFileAsyncEventHandler();
		public:
			virtual TInt SendEventToClient(const TMMFEvent& aEvent);
		private:
			CMMFFile* iParent;
		};
		
	class CCAFParameters : public CBase
		{
	public:
		CCAFParameters() : iUniqueId(NULL), iEnableUI(EFalse) {}
		~CCAFParameters() {	delete iUniqueId;	}
	public:
		HBufC* iUniqueId;
		TBool iEnableUI;
		};

public:
	virtual TInt Data(ContentAccess::CData*& aData);
	virtual void SourcePauseL();
	
private:
	MGenericFile* iFile;
	RFs iFsSession;
	
	TBool iFileHandle;
	RFile iHandle;
	TBool iReserved;

	TBool iSinkNotStopped;

	TFileName iFullFileName ;
	HBufC* iFileName ;
	HBufC* iFileExt ;
	HBufC* iFilePath ;
	HBufC* iFileDrive ;
	CCAFParameters* iCAFParameters;
	TInt iFileSize; //holds the cached file size

	//Keeps track of the read position within the file. Needed in order to enable passing of the
	//read position to the CReadRequest to manage the LastBuffer flag.
	//NB: Continuous reads once EOF has been reached will result in iPosition going > iFileSize,
	//but this causes no ill effects
	TUint iPosition;

	TFourCC  iSinkFourCC ;
	TFourCC  iSourceFourCC ;


	RPointerArray<CReadWriteRequest> iRequests;

	MAsyncEventHandler *iEventHandler;
	CMMFFileAsyncEventHandler* iMmfFileEventHandler;

	RPointerArray<CTransferBufferCopy> iTransferBufferCopies;
	friend class CMMFFileAsyncEventHandler;
	};

#endif
