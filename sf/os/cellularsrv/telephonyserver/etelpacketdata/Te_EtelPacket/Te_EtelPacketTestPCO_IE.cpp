// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// EtelPacketTestAuthenticationAndDNS.cpp
// This is the cpp file for Packet Data Context Data Structure
// File created 03-11-2004
// 
//

// Symbian OS includes
#include <e32base.h>
#include <etelpckt.h>
#include <etelutils.h>
#include <pcktcs.h>


#include "Te_EtelPacketTestStepBase.h"
#include "testdef.h"
#include "Te_etelpckt_defs.h"
#include "Te_EtelPacketTestPCO_IE.h"


CEtelPacketTestPCO_IE::CEtelPacketTestPCO_IE()
{
	// store the name of this test case
	SetTestStepName(_L("PCO_IE"));
}

// destructor
CEtelPacketTestPCO_IE::~CEtelPacketTestPCO_IE()
{
}

enum TVerdict CEtelPacketTestPCO_IE::doTestStepL( void )
/**
 * Test step AuthorizationData
 * 
 */
{
	TInt ret;

	//	---------------------------------------------------
	//	TEST CASE 1 - Construction
	//	---------------------------------------------------
	const TInt KZero=0;
	const TInt KMinimumBufferSize = 1;
	const TInt KSmallBuffer=20;
	
	//	Create Item
	const TUint16 XId=1;
	const TUint16 YId=10;
	
	RPacketContext::TPcoId pcoIdX(XId);
	RPacketContext::TPcoId pcoIdY(YId);
	
	//	Create Item Buffers
	const TInt KAppendableBufferSize=3;

	//	BufferX
	TBuf8<KAppendableBufferSize> appendableBufferX(_L8("xxx"));	
	appendableBufferX.SetLength(KAppendableBufferSize);
	TPtr8 appendablePtrX(const_cast<TUint8*>(appendableBufferX.Ptr()),appendableBufferX.Length(),appendableBufferX.Length());

	//	BufferY
	TBuf8<KAppendableBufferSize> appendableBufferY(_L8("yyy"));	
	appendableBufferY.SetLength(KAppendableBufferSize);
	TPtr8 appendablePtrY(const_cast<TUint8*>(appendableBufferY.Ptr()),appendableBufferY.Length(),appendableBufferY.Length());
	
	//	Create Buffer for TLV struct
	TBuf8<KSmallBuffer> smallBuffer;		//	MaxLength = 20
	smallBuffer.SetLength(smallBuffer.MaxLength());
	smallBuffer.FillZ();
	TPtr8 smallPtr(smallBuffer.LeftTPtr(smallBuffer.MaxLength()));
	smallPtr.SetLength(0);

	//	Create Zero length buffer
	TPtr8 zeroSizePtr=smallPtr.LeftTPtr(KZero);
	TTlvStruct<RPacketContext::TPcoId,RPacketContext::TPcoItemDataLength> zeroTLV(zeroSizePtr,KZero);

	//	Create TLV struct with size=20
	TTlvStruct<RPacketContext::TPcoId,RPacketContext::TPcoItemDataLength> smallTLV(smallPtr,KZero);	

	//	---------------------------------------------------
	//	TEST CASE 2 - AppendItemL
	//	---------------------------------------------------
	
	//	Append an Item to zero length Tlv
	ret=zeroTLV.AppendItemL(pcoIdX,appendablePtrX);
	TEST(ret==KErrOverflow);
	
	//	Append a larger buffer 
	const TInt KLargeBuffer=30;
	TBuf8<KLargeBuffer> largeBuffer(_L8("xxxxxxxxx"));	
	largeBuffer.SetLength(KLargeBuffer);
	TPtr8 largePtr(const_cast<TUint8*>(largeBuffer.Ptr()),largeBuffer.Length(),largeBuffer.Length());

	//	Append
	ret=smallTLV.AppendItemL(pcoIdX,largePtr);
	TEST(ret==KErrOverflow);
	
	//	Append 3 Items	-  X + Y + X
	ret=smallTLV.AppendItemL(pcoIdX,appendablePtrX);
	TEST(ret==KErrNone);

	ret=smallTLV.AppendItemL(pcoIdY,appendablePtrY);
	TEST(ret==KErrNone);
	
	ret=smallTLV.AppendItemL(pcoIdX,appendablePtrX);
	TEST(ret==KErrNone);
	
	//	Overflow the buffer
	ret=smallTLV.AppendItemL(pcoIdY,appendablePtrY);
	TEST(ret==KErrOverflow);

	//	---------------------------------------------------
	//	TEST CASE 3 - NextItemL
	//	Buffer contains = XYX
	//	---------------------------------------------------
	//	Construct TPtr8 buffers for retrieval
	//	Buffer large enough to handle data
	const TInt KBigEnoughBuf=5;
	TBuf8<KBigEnoughBuf> bigEnoughBuf;
	bigEnoughBuf.SetLength(0);
	TPtr8 bigEnoughPtr(const_cast<TUint8*>(bigEnoughBuf.Ptr()),bigEnoughBuf.Length(),bigEnoughBuf.MaxLength());

	//	Buffer too small to hold data
	const TInt KTooSmallBuf=2;
	TBuf8<KTooSmallBuf> tooSmallBuf;
	tooSmallBuf.SetLength(0);
	TPtr8 tooSmallPtr(const_cast<TUint8*>(tooSmallBuf.Ptr()),tooSmallBuf.Length(),tooSmallBuf.MaxLength());

	//	Get Next Item from zero length TLV
	ret=zeroTLV.NextItemL(pcoIdY,bigEnoughPtr);
	TEST(ret==KErrNotFound);
	
	//	Create Item Z	
	smallTLV.ResetCursorPos();
	const TUint16 ZId=2;
	RPacketContext::TPcoId pcoIdZ(ZId);
			
	//	Get a non existant item	
	TInt err(KErrNone);
	TInt leaveErr(KErrNone);
	TRAP(leaveErr,err=smallTLV.NextItemL(pcoIdZ,bigEnoughPtr));
	TEST(leaveErr==KErrNone && err==KErrNotFound);

	
	//	Get an existing item in bigEnoughBuffer
	ret=smallTLV.NextItemL(pcoIdX,bigEnoughPtr);
	TEST(ret==KErrNone);
	TEST(bigEnoughPtr==appendablePtrX);
	
	//	Get last item 
	ret=smallTLV.NextItemL(pcoIdX,bigEnoughPtr);
	TEST(ret==KErrNone);
	TEST(bigEnoughPtr==appendablePtrX);
	
	//	Cursor should be at end, try to get item before cursor
	ret=smallTLV.NextItemL(pcoIdY,bigEnoughPtr);
	TEST(ret==KErrNotFound);
	
	//	reset cursor
	smallTLV.ResetCursorPos();
	
	//	get item
	ret=smallTLV.NextItemL(pcoIdY,bigEnoughPtr);
	TEST(ret==KErrNone);
	TEST(bigEnoughPtr==appendablePtrY);

	
	//	---------------------------------------------------
	//	TEST CASE 4 - RemoveNextItemL
	//	Buffer contains = XYX
	//	---------------------------------------------------
	//	remove item form zero length TLV
	ret=zeroTLV.RemoveNextItemL(pcoIdX);
	TEST(ret==KErrNotFound);
	
	//	reset cursor
	smallTLV.ResetCursorPos();

	//	remove pcoIdX
	ret=smallTLV.RemoveNextItemL(pcoIdX);
	TEST(ret==KErrNone);
	
	//	remove pcoIdX
	ret=smallTLV.RemoveNextItemL(pcoIdX);
	TEST(ret==KErrNone);
	
	ret=smallTLV.AppendItemL(pcoIdZ,appendablePtrX);
	TEST(ret==KErrNone);
	ret=smallTLV.AnyNextItemL(pcoIdY,bigEnoughPtr);
	TEST(ret==KErrNone);
	TEST(pcoIdY.Id()==YId);	
	TEST(bigEnoughPtr==appendablePtrY);
	
	ret=smallTLV.AnyNextItemL(pcoIdZ,bigEnoughPtr);
	TEST(ret==KErrNone);
	TEST(pcoIdZ.Id()==ZId);	
	TEST(bigEnoughPtr==appendablePtrX);
		
	// 	remove pcoIdY
	ret=smallTLV.RemoveNextItemL(pcoIdY);
	TEST(ret==KErrNotFound);

	//	reset cursor
	smallTLV.ResetCursorPos();
	ret=smallTLV.RemoveNextItemL(pcoIdX);
	TEST(ret==KErrNotFound);
	ret=smallTLV.RemoveNextItemL(pcoIdY);
	TEST(ret==KErrNone);
	ret=smallTLV.RemoveNextItemL(pcoIdZ);
	TEST(ret==KErrNone);


	//	---------------------------------------------------
	//	TEST CASE 4 - AnyNextItemL
	//	Buffer is Empty
	//	---------------------------------------------------
	//	Get any next item from zero length TLV
	ret=zeroTLV.AnyNextItemL(pcoIdY,bigEnoughPtr);
	TEST(ret==KErrNotFound);

	ret=smallTLV.AnyNextItemL(pcoIdY,tooSmallPtr);
	TEST(ret==KErrNotFound);

	//	Append Item
	ret=smallTLV.AppendItemL(pcoIdX,appendablePtrX);
	TEST(ret==KErrNone);
	
	//	Get item after appending an existing item
	ret=smallTLV.NextItemL(pcoIdX,bigEnoughPtr);
	TEST(ret==KErrNone);
	TEST(bigEnoughPtr==appendablePtrX);
			
	ret=smallTLV.AppendItemL(pcoIdY,appendablePtrY);
	TEST(ret==KErrNone);
	ret=smallTLV.AppendItemL(pcoIdX,appendablePtrX);
	TEST(ret==KErrNone);

	//	Searching for item while cursor at end of buffer
	ret=smallTLV.AnyNextItemL(pcoIdY,bigEnoughPtr);
	TEST(ret==KErrNone);
	TEST(pcoIdY.Id()==YId);	
	TEST(bigEnoughPtr==appendablePtrY);
	
	//	Reset cursor
	smallTLV.ResetCursorPos();
		
	//	Get item into tooSmallbuf
	ret=smallTLV.AnyNextItemL(pcoIdX,tooSmallPtr);
	TEST(ret==KErrNone);

	//	Reset cursor
	smallTLV.ResetCursorPos();
		
	//	Get 3 items
	ret=smallTLV.AnyNextItemL(pcoIdX,bigEnoughPtr);
	TEST(ret==KErrNone);
	TEST(pcoIdX.Id()==XId);	
	TEST(bigEnoughPtr==appendablePtrX);
	
	//	second item
	ret=smallTLV.AnyNextItemL(pcoIdY,bigEnoughPtr);
	TEST(ret==KErrNone);
	TEST(pcoIdY.Id()==YId);	
	TEST(bigEnoughPtr==appendablePtrY);
	
	//	third item
	ret=smallTLV.AnyNextItemL(pcoIdX,bigEnoughPtr);
	TEST(ret==KErrNone);
	TEST(pcoIdX.Id()==XId);	
	TEST(bigEnoughPtr==appendablePtrX);

	//	try get item at end of buffer
	ret=smallTLV.AnyNextItemL(pcoIdY,bigEnoughPtr);
	TEST(ret==KErrNotFound);

	//	---------------------------------------------------
	//	TEST CASE 5 - Random pattern of calls
	//	Buffer contains XYX
	//	---------------------------------------------------
	//	Clear Buffer
	//	reset cursor
	smallTLV.ResetCursorPos();
	//	remove pcoIdX

	ret=smallTLV.RemoveNextItemL(pcoIdX);
	TEST(ret== KErrNone);

	// 	remove pcoIdY
	ret=smallTLV.RemoveNextItemL(pcoIdY);
	TEST(ret== KErrNone);

	//	remove pcoIdX
	ret=smallTLV.RemoveNextItemL(pcoIdX);
	TEST(ret== KErrNone);	

	//  Buffer is Empty
	//	Append Item
	ret=smallTLV.AppendItemL(pcoIdX,appendablePtrX);
	TEST(ret==KErrNone);
		
	// 	remove pcoIdX after appending pcoIdX
	ret=smallTLV.RemoveNextItemL(pcoIdX);
	TEST(ret==KErrNone);
	
	//	Append Item
	ret=smallTLV.AppendItemL(pcoIdY,appendablePtrY);
	TEST(ret==KErrNone);

	//	try get any item after adding pcoIdY
	ret=smallTLV.AnyNextItemL(pcoIdY,bigEnoughPtr);
	TEST(ret==KErrNone);
	TEST(pcoIdY.Id()==YId);	
	TEST(bigEnoughPtr==appendablePtrY);
	
	//	reset cursor
	//	Buffer contains Y
	smallTLV.ResetCursorPos();

	//	get item
	ret=smallTLV.AnyNextItemL(pcoIdY,bigEnoughPtr);
	TEST(ret==KErrNone);
	TEST(pcoIdY.Id()==YId);
	
	//	Get item pcoIdY
	ret=smallTLV.NextItemL(pcoIdY,bigEnoughPtr);
	TEST(ret==KErrNotFound);

	// 	remove pcoIdX
	ret=smallTLV.RemoveNextItemL(pcoIdX);
	TEST(ret==KErrNotFound);
	
	
	//	---------------------------------------------------
	//	Test CASE 6 - TPcoId
	//	---------------------------------------------------
	const TUint16 KFirstPcoId=257;
	const TUint16 KSecondPcoId=0x103;
	const TUint16 KSecondPcoIdFlipped=0x301;
	const TInt KTwoByte=0xFFFE;
	const TInt KTwoByteFlipped=0xFEFF;
	const TInt16 KIDSize=sizeof(TUint16);
	
	//	Create  PCO_ID's
	RPacketContext::TPcoId firstId(KFirstPcoId);
	
	//	---------------------------------------------------
	//	-- ExternalizeL
	//	---------------------------------------------------
	//	-- to buffer large enough to hold value
	//	-- Memory to contain ID - 4 bytes
	TInt bigExternalizedBuff(KZero);
	
	//	Create TPtr to the first byte
	TPtr8 bigExternalizedPtr(reinterpret_cast<TUint8*>(&bigExternalizedBuff),sizeof(TInt),sizeof(TInt));

	firstId.ExternalizeL(bigExternalizedPtr);
	TEST(bigExternalizedBuff==KFirstPcoId);
		
	//	-- to buffer to small to hold value
	//	-- Memory to contain ID - 1 byte
	TUint8 smallExternalizedBuff(KZero);
	//	Create TPtr to the first byte
	TPtr8 smallExternalizedPtr(static_cast<TUint8*>(&smallExternalizedBuff),sizeof(TUint8),sizeof(TUint8));

	//	Externalize to a pointer with size less than needed.
	TRAP(err,firstId.ExternalizeL(smallExternalizedPtr));
	TEST(err==KErrOverflow);
	
	//	-- InternalizeL
	//	-- Try internalize a 
	TRAP(err,firstId.InternalizeL(smallExternalizedPtr));
	TEST(err==KErrUnderflow);
	
	bigExternalizedBuff=KTwoByte;
	firstId.InternalizeL(bigExternalizedPtr);
	TEST(firstId.Id()== KTwoByteFlipped);
	
	//	-- SerializedLength
	TUint16 size=firstId.SerializedLength();
	TEST(size==KIDSize);
	
	//	-- SetId	
	RPacketContext::TPcoId secondId(0);
	secondId.SetId(KSecondPcoId);
	TEST(secondId.Id()==KSecondPcoId);
	
	//	Check with Externalize
	secondId.ExternalizeL(bigExternalizedPtr);
	TEST(bigExternalizedBuff==KSecondPcoIdFlipped);
	
	//	-- IsEqual
	firstId.SetId(KSecondPcoId);
	secondId.SetId(KSecondPcoId);
	TBool result=firstId.IsEqual(secondId);
	TEST(result);
	
	secondId.SetId(KFirstPcoId);
	result=firstId.IsEqual(secondId);
	TEST(!result);
	
	//	---------------------------------------------------
	//	Test CASE 7 - NextItemL and RemoveNextItemL using a 
	//  0 length item
	//	Buffer is empty
	//	---------------------------------------------------
	
	// Create Buffer A with 0 length
	const TUint16 AId=3;
	RPacketContext::TPcoId pcoIdA(AId);
	TBuf8<KMinimumBufferSize> appendableBufferA;
	appendableBufferA.SetLength(KZero);
	TPtr8 appendablePtrA(const_cast<TUint8*>(appendableBufferA.Ptr()),appendableBufferA.Length(),appendableBufferA.Length());	

	// reset cursor
	smallTLV.ResetCursorPos();

	// Append 0 length item to TLV struct
	ret=smallTLV.AppendItemL(pcoIdA, appendablePtrA);
	TEST(ret==KErrNone);
	
	// Try to read back 0 length item
	// Will fail with KErrNotFound if unsuccessful
	ret=smallTLV.NextItemL(pcoIdA, bigEnoughPtr);
	TEST(ret==KErrNone);
	TEST(bigEnoughPtr==appendablePtrA);
	
	//	reset cursor
	smallTLV.ResetCursorPos();
	
	// Finally remove item with 0 length
	// Will fail with KErrNotFound if unsuccessful
	ret=smallTLV.RemoveNextItemL(pcoIdA);
	TEST(ret==KErrNone);

	return TestStepResult();
}


