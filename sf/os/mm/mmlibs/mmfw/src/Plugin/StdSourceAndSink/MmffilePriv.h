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


#ifndef __MMFFILEPRIV_H_
#define __MMFFILEPRIV_H_

#include <f32file.h>

#include <mmf/server/mmfclip.h>
#include <mmf/server/mmfdatabuffer.h>


/**
 *  @publishedAll
 *  @released
 *
 *  Represents a copy of a KUidMmfTransferBuffer used for reading/writting to the file server
 */

class CTransferBufferCopy : public CBase
	{
public:
	static CTransferBufferCopy* NewL(TInt aMaxLength);

	virtual ~CTransferBufferCopy()
		{delete iBuffer;}


	TDes8& Des() {return iBufferDes;}

	TInt MaxLength() {return iBufferDes.MaxLength();}

	void ReUse(TInt aMaxLength) {iBufferDes.Set(iBuffer,0, Min(aMaxLength, iMaxLength));}

	TBool InUse() {return iInUse;}

	void SetInUse(TBool aInUse) {iInUse=aInUse;}

private:
	CTransferBufferCopy(TInt aMaxLength) : CBase(), iMaxLength(aMaxLength), iBufferDes(0,0,0), iInUse(EFalse){}

	void ConstructL();

private:
	TUint8*	iBuffer;
	
	//Holds the original MaxLength when class constructed. 
	//May be larger than MaxLength of iBufferDes
	TInt	iMaxLength;

	TPtr8	iBufferDes;

	TBool	iInUse;
	};




/**
 * @internalComponent
 *
 * A request is created when an external object requests or supplies data.  Calls to the File Server are
 * made asynchronously and a CReadWriteRequest created to notify the caller on completion.
 *
 * CReadWriteRequest is an abstract class.  Concrete instances are of CReadRequest & CWriteRequest.
 * Concrete instances need to know whether to call MDataSink::BufferFilledL() or MDataSource::BufferEmptiedL()
 */
class CReadWriteRequest : public CActive
	{
public:
	CReadWriteRequest(TAny* aSinkOrSource, CMMFBuffer* aBuffer, MAsyncEventHandler* aEventHandler)
	: CActive(EPriorityStandard),
	iSinkOrSource(aSinkOrSource), 
	iBuffer(aBuffer),
	iEventHandler(aEventHandler)
		{
		CActiveScheduler::Add( this );
		}

	CReadWriteRequest(TAny* aSinkOrSource, CMMFBuffer* aBuffer, CTransferBufferCopy* aOptionalDataBuffer, MAsyncEventHandler* aEventHandler)
	: CActive(EPriorityStandard),
	iSinkOrSource(aSinkOrSource), 
	iBuffer(aBuffer),
	iTransferBufferCopy(aOptionalDataBuffer),
	iEventHandler(aEventHandler)
		{
		CActiveScheduler::Add( this );
		iTransferBufferCopy->SetInUse(ETrue);
		}
		
	CReadWriteRequest(CReadWriteRequest& aRequest)
	: CActive(EPriorityStandard),
	iSinkOrSource(aRequest.iSinkOrSource),
	iBuffer(aRequest.iBuffer),
	iTransferBufferCopy(aRequest.iTransferBufferCopy),
	iEventHandler(aRequest.iEventHandler)
		{
		CActiveScheduler::Add( this );
		iTransferBufferCopy->SetInUse(ETrue);
		}

		
	TBool Completed() ;
	TDes8& BufferDes() ;
	const TDesC8& BufferDesC() ;

	~CReadWriteRequest() ;


	// CActive functions.
	// 
	void SetActive() ;
	void DoCancel() ;
	virtual void RunL() = 0 ;
	virtual TInt RunError( TInt aError ) ;
	

protected :
	TAny* iSinkOrSource;
	CMMFBuffer* iBuffer;
	CTransferBufferCopy* iTransferBufferCopy;

	MAsyncEventHandler* iEventHandler;
	TBool iCompleted ;
	TDes8* iBufferDes ;
	TInt iError ;
	TBool iUseTransferBuffer ;
	void SetTransferBuffer (TBool aTBuffer) ;
	TBool CanUseTransferBuffer () ;
	} ;

/**
 * @internalComponent
 */
class CReadRequest : public CReadWriteRequest
	{
public :
	CReadRequest(TAny* aSinkOrSource, CMMFBuffer* aBuffer, TUint aPosition, TUint aFileSize, MAsyncEventHandler* aEventHandler)
	: CReadWriteRequest(aSinkOrSource, aBuffer, aEventHandler),
	iPosition(aPosition), iFileSize(aFileSize)
	{ } 

	CReadRequest(TAny* aSinkOrSource, CMMFBuffer* aBuffer, CTransferBufferCopy* aOptionalDataBuffer, TUint aPosition, TUint aFileSize, MAsyncEventHandler* aEventHandler)
	: CReadWriteRequest(aSinkOrSource, aBuffer, aOptionalDataBuffer, aEventHandler),
	iPosition(aPosition), iFileSize(aFileSize)
	{ } 

	void RunL();
private:
	TUint iPosition;
	TUint iFileSize;
	};

/**
 * @internalComponent
 */
class CWriteRequest : public CReadWriteRequest
	{
public :
	CWriteRequest(TAny* aSinkOrSource, CMMFBuffer* aBuffer, MAsyncEventHandler* aEventHandler)
	: CReadWriteRequest(aSinkOrSource, aBuffer, aEventHandler)
	{ }

	CWriteRequest(TAny* aSinkOrSource, CMMFBuffer* aBuffer, CTransferBufferCopy* aOptionalDataBuffer, MAsyncEventHandler* aEventHandler)
	: CReadWriteRequest(aSinkOrSource, aBuffer, aOptionalDataBuffer, aEventHandler)
	{ }

	void RunL();
	};


#endif

