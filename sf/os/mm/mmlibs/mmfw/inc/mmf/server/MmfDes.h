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

#ifndef __MMFDES_H_
#define __MMFDES_H_


#include <mmf/server/mmfclip.h>
#include <mmf/common/mmfutilities.h>


/**
@publishedAll
@released

Descriptor parameters.

Used by CMMFDescriptor::ConstructL().
*/
class TMMFDescriptorParams
	{
public:
	/**
    Address of TPr8 describing source descriptor.
	*/
	TAny* iDes ; // Address of TPr8 describing source Descriptor
	/**
	Descriptor thread.
	*/
	TThreadId iDesThreadId ;
private:
	/**
	This member is internal and not intended for use.
	*/
	TInt iReserved1;
	} ;

/**
@publishedAll
@released
*/
typedef TPckgBuf<TMMFDescriptorParams>  TMMFDescriptorConfig ;

/**
@publishedAll
@released

MultiMedia framework class.  Represents a client side descriptor.
May be source descriptor from which data is read or destination to which data is written
*/
class CMMFDescriptor : public CMMFClip
	{
public :

	// From MDataSource
	virtual TFourCC SourceDataTypeCode( TMediaId aMediaId) ;
	virtual void FillBufferL(CMMFBuffer* aBuffer, MDataSink* aConsumer,TMediaId aMediaId) ;//called by a MDataSink to request buffer fill
	virtual void BufferEmptiedL( CMMFBuffer* aBuffer ) ;
	virtual TBool CanCreateSourceBuffer() ;
	virtual CMMFBuffer* CreateSourceBufferL(TMediaId aMediaId, TBool &aReference) ;
	
	// From MDataSink
	virtual TFourCC SinkDataTypeCode(TMediaId aMediaId) ; //used by data path MDataSource/Sink for codec matching
	virtual void EmptyBufferL( CMMFBuffer* aBuffer, MDataSource* aSupplier, TMediaId aMediaId ) ;
	virtual void BufferFilledL( CMMFBuffer* aBuffer ) ;
	virtual TBool CanCreateSinkBuffer() ;
	virtual CMMFBuffer* CreateSinkBufferL( TMediaId aMediaId , TBool &aReference) ;

	// From CMMFClip
	virtual void ReadBufferL( TInt aLength, CMMFBuffer* aBuffer, TInt aPosition, MDataSink* aConsumer);
	virtual void WriteBufferL( TInt aLength, CMMFBuffer* aBuffer, TInt aPosition, MDataSource* aSupplier);
	virtual void ReadBufferL( CMMFBuffer* aBuffer, TInt aPosition, MDataSink* aConsumer) ;
	virtual void WriteBufferL( CMMFBuffer* aBuffer, TInt aPosition, MDataSource* aSupplier) ;
	virtual void ReadBufferL( CMMFBuffer* aBuffer, TInt aPosition) ;
	virtual void WriteBufferL( CMMFBuffer* aBuffer, TInt aPosition) ;
	virtual TInt64 BytesFree() ;  // amount of space available for the clip
	virtual TInt Size() ;  // length of clip
	virtual TInt SetSize( TInt aSize ); // sets the size of the clip (used mainly by crop)

	// CMMFDescriptor
	virtual ~CMMFDescriptor() ;
	static MDataSource* NewSourceL() ; 
	static MDataSink* NewSinkL() ; 

	void SetTransferBuffer (TBool aTBuffer) ;
	TBool CanUseTransferBuffer () ;

protected :
	virtual void ConstructSinkL( const TDesC8& aInitData ) ;
	virtual void ConstructSourceL(  const TDesC8& aInitData ) ;
	void ConstructL( const TDesC8& aInitData ) ;
	CMMFDescriptor( ) ;

private :
	// returns the Maximum length of the descriptor
	inline TInt MaxLength() { ASSERT( iDes ); return iDes->MaxLength(); }

	// Need to know about the thread that the descriptor is in.
	RThread iDesThread ;

	TDes8* iDes ;
	TInt iOffset ;

	TFourCC  iSinkFourCC ;
	TFourCC  iSourceFourCC ;

	TBool iUseTransferBuffer ;

	
	} ;


#endif
