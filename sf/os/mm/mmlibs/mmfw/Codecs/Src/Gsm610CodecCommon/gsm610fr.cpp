// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Polymorphic DLL implementation of GSM 6.10 full rate (FR) speech coder
// and decoder    objects.
// This codec is based on ANSI C simulation codes. For this implementation
// (polymorphic DLL) the original ANSI C codes have been modified slightly:
// - The original .c files were renamed to .cpp files.
// - All global varibles (codec's state) are now encoder's or decoder's
// private member variables.
// - Because codec's state is in the codec object, an extra variable -
// a pointer codec object - have been added to some original routines.
// - Global tables are now const C++ tables in tables.h header file.
// - VAD and DTX modules have been removed from the original routines.
// - Due to error in GNU tool chain all array indexes of type [i-1] in
// rpeltp.cpp have been removed and changed to [j] type.
// - multr, L_add, L_mac from basicop.cpp inlined
// INCLUDES
// 
//

#include "gsm610fr.h"
#include "codec.h"
#include <e32uid.h>


//  LOCAL FUNCTIONS
//===================================================================
/*
-----------------------------------------------------------------------------

    GSM610FR_Encoder

    ConstructL

    This function implements the second phase construction of GSM610FR_Encoder
    class. Function binds the encoder to given input and output streams and
    resets encoder.

    Parameters:     RReadStream aInStrm     Handle to input stream (16 bit
                                            PCM speech data)
                    RWriteStream aOutStrm   Handle to output stream (encoded
                                            speech data)

    Return Values:  none

-----------------------------------------------------------------------------
*/
EXPORT_C void CGSM610FR_Encoder::ConstructL()
    {
    ::reset_encoder(this);
    iOddFrame = TRUE;
    }


/*
-----------------------------------------------------------------------------

    GSM610FR_Encoder

    ~GSM610FR_Encoder

    Empty encoder destructor.

    Parameters:     none
    
    Return Values:  none

-----------------------------------------------------------------------------
*/
EXPORT_C CGSM610FR_Encoder::~CGSM610FR_Encoder()
    {
    }


/*
-----------------------------------------------------------------------------

    GSM610FR_Encoder

    StartL

    Start encoder object. Initialize codec status.

    Parameters:     none
    
    Return Values:  none

-----------------------------------------------------------------------------
*/
EXPORT_C void CGSM610FR_Encoder::StartL()
    {
    ::reset_encoder(this);
    iOddFrame = TRUE;
    }


/*
-----------------------------------------------------------------------------

    GSM610FR_Encoder

    ExecuteL

    Execute encoder object. Read one speech frame from the input stream,
    RPELTP encode it and write the encoded frame to output stream.

    Parameters:     none
    
    Return Values:  none

    Leave Handling:
    
    If the length of data available in the input stream is less than
    FRAMESIZE then the function leaves with KErrEof.

-----------------------------------------------------------------------------
*/
EXPORT_C void CGSM610FR_Encoder::ExecuteL(TUint8* aInBuf, TUint8* aOutBuf)
{
	TInt16* inBufPtr  = (TInt16*) aInBuf;


	for (TInt frame = 0; frame < 2; frame++)
	{
	    ::RPELTP_encoder(this, inBufPtr, &iCodeBuf);

		if ( iOddFrame )
		{
			PackFrame0 (&iCodeBuf, (TInt8*) aOutBuf);
		}
		else
		{
			PackFrame1 (&iCodeBuf, (TInt8*) aOutBuf);
		}

		iOddFrame = !iOddFrame;
		inBufPtr += FRAMESIZE;
	}

}
/*
-----------------------------------------------------------------------------

    GSM610FR_Encoder

    StopL

    Stop encoder object. Flush out last frames, if necessary.

    Parameters:     none
    
    Return Values:  none

-----------------------------------------------------------------------------
*/
EXPORT_C void CGSM610FR_Encoder::StopL()
    {
    }


/*
-----------------------------------------------------------------------------

    GSM610FR_Encoder

    PackFrame0

    Pack the codewords of the even frame into pack buffer.

    Packing as in MS gsm610 encoder.

    Parameters:     struct codes* aCodeBuf    Code words for one speech frame.
    
    Return Values:  none


-----------------------------------------------------------------------------
*/
void CGSM610FR_Encoder::PackFrame0(struct codes* aCodeBuf, TInt8* pbuf)
    {
    TInt16* LAR = aCodeBuf->LARc;
    
    // pack the LARc[0..7] into the first 4.5 bytes
    *pbuf++ = (TUint8)(((LAR[0]     ) & 0x3F) | ((LAR[1] << 6) & 0xC0));
    *pbuf++ = (TUint8)(((LAR[1] >> 2) & 0x0F) | ((LAR[2] << 4) & 0xF0));
    *pbuf++ = (TUint8)(((LAR[2] >> 4) & 0x01) | ((LAR[3] << 1) & 0x3E) | ((LAR[4] << 6) & 0xC0));
    *pbuf++ = (TUint8)(((LAR[4] >> 2) & 0x03) | ((LAR[5] << 2) & 0x3C) | ((LAR[6] << 6) & 0xC0));
    *pbuf   = (TUint8)(((LAR[6] >> 2) & 0x01) | ((LAR[7] << 1) & 0x0E));
    
    // pack Nc, bc, Mc, xmaxc, and xMc for each of the 4 sub-frames
    for(TInt i = 0; i < 4; i++)
        {
        struct sfcodes& c = aCodeBuf->sfc[i];
        *pbuf++ |= ((c.Nc << 4) & 0xF0);
        *pbuf++ = (TUint8)(((c.Nc >> 4) & 0x07) | ((c.bc << 3) & 0x18) | ((c.Mc << 5) & 0x60) | ((c.xmaxc << 7) & 0x80));
        *pbuf++ = (TUint8)(((c.xmaxc >> 1) & 0x1F) | ((c.xMc[0] << 5) & 0xE0));
        *pbuf++ = (TUint8)((c.xMc[1] & 0x07) | ((c.xMc[2] << 3) & 0x38) | ((c.xMc[3] << 6) & 0xC0));
        *pbuf++ = (TUint8)(((c.xMc[3] >> 2) & 0x01) | ((c.xMc[4] << 1) & 0x0E) | ((c.xMc[5] << 4) & 0x70) | ((c.xMc[6] << 7) & 0x80));
        *pbuf++ = (TUint8)(((c.xMc[6] >> 1) & 0x03) | ((c.xMc[7] << 2) & 0x1C) | ((c.xMc[8] << 5) & 0xE0));
        *pbuf++ = (TUint8)((c.xMc[9] & 0x07) | ((c.xMc[10] << 3) & 0x38) | ((c.xMc[11] << 6) & 0xC0));
        *pbuf   = (TUint8)(((c.xMc[11] >> 2) & 0x01) | ((c.xMc[12] << 1) & 0x0E));
        }
    }


/*
-----------------------------------------------------------------------------

    GSM610FR_Encoder

    PackFrame1

    Pack the codewords of the odd frame into pack buffer.

    Packing as in MS gsm610 encoder.

    Parameters:     struct codes* aCodeBuf    Code words for one speech frame.
    
    Return Values:  none


-----------------------------------------------------------------------------
*/
void CGSM610FR_Encoder::PackFrame1(struct codes* aCodeBuf, TInt8* pbuf)
    {
    TInt16* LAR = aCodeBuf->LARc;
    
	pbuf += (PACKSIZE / 2);
	TInt8 pbufZero = pbuf[0];

    // pack the LARc[0..7] into the first 4.5 bytes, starting with the msb of the first byte
    *pbuf++ = (TUint8) (pbufZero | ((LAR[0] << 4) & 0xF0));
    *pbuf++ = (TUint8)(((LAR[0] >> 4) & 0x03) | ((LAR[1] << 2) & 0xFC));
    *pbuf++ = (TUint8)(((LAR[2]     ) & 0x1F) | ((LAR[3] << 5) & 0xE0));
    *pbuf++ = (TUint8)(((LAR[3] >> 3) & 0x03) | ((LAR[4] << 2) & 0x3C) | ((LAR[5] << 6) & 0xC0));
    *pbuf++ = (TUint8)(((LAR[5] >> 2) & 0x03) | ((LAR[6] << 2) & 0x1C) | ((LAR[7] << 5) & 0xE0));
    
    // pack Nc, bc, Mc, xmaxc, and xMc for each of the 4 sub-frames
    for(TInt i = 0; i < 4; i++)
        {
        struct sfcodes& c = aCodeBuf->sfc[i];
        *pbuf++ = (TUint8)((c.Nc & 0x7F) | ((c.bc << 7) & 0x80));
        *pbuf++ = (TUint8)(((c.bc >> 1) & 0x01) | ((c.Mc << 1) & 0x06) | ((c.xmaxc << 3) & 0xF8));
        *pbuf++ = (TUint8)(((c.xmaxc >> 5) & 0x01) | ((c.xMc[0] << 1) & 0x0E) | ((c.xMc[1] << 4) & 0x70) | ((c.xMc[2] << 7) & 0x80));
        *pbuf++ = (TUint8)(((c.xMc[2] >> 1) & 0x03) | ((c.xMc[3] << 2) & 0x1C) | ((c.xMc[4] << 5) & 0xE0));
        *pbuf++ = (TUint8)(((c.xMc[5]) & 0x07) | ((c.xMc[6] << 3) & 0x38) | ((c.xMc[7] << 6) & 0xC0));
        *pbuf++ = (TUint8)(((c.xMc[7] >> 2) & 0x01) | ((c.xMc[8] << 1) & 0x0E) | ((c.xMc[9] << 4) & 0x70) | ((c.xMc[10] << 7) & 0x80));
        *pbuf++ = (TUint8)(((c.xMc[10] >> 1) & 0x03) | ((c.xMc[11] << 2) & 0x1C) | ((c.xMc[12] << 5) & 0xE0));
        }
    }

/*
-----------------------------------------------------------------------------

    GSM610FR_Decoder

    ConstructL

    This function implements the second phase construction of GSM610FR_Decoder
    class. Function binds the decoder to given input and output streams and
    resets decoder.

    Parameters:     RReadStream aInStrm     Handle to input stream (encoded
                                            speech data)
                                            
                    RWriteStream aOutStrm   Handle to output stream (16 bit
                                            PCM speech data)

    Return Values:  none

-----------------------------------------------------------------------------
*/
EXPORT_C void CGSM610FR_Decoder::ConstructL()
    {
    ::reset_decoder(this);
    iOddFrame = TRUE;
    }


/*
-----------------------------------------------------------------------------

    GSM610FR_Decoder

    ~GSM610FR_Decoder

    Empty decoder destructor.

    Parameters:     none
    
    Return Values:  none

-----------------------------------------------------------------------------
*/
EXPORT_C CGSM610FR_Decoder::~CGSM610FR_Decoder()
    {
    }


/*
-----------------------------------------------------------------------------

    GSM610FR_Decoder

    StartL

    Start decoder object. Initialize codec status.

    Parameters:     none
    
    Return Values:  none

-----------------------------------------------------------------------------
*/
EXPORT_C void CGSM610FR_Decoder::StartL()
    {
    ::reset_decoder(this);
    iOddFrame = TRUE;
    }


/*
-----------------------------------------------------------------------------

    GSM610FR_Decoder

    ExecuteL

    Execute decoder object. Read one encoded speech frame from the input
    stream RPELTP decode it and write the decoded frame to output stream.

    Parameters:     none
    
    Return Values:  none

    Leave Handling:
    
    If the length of data available in the input stream is less than size
    of one encoded speech frame then the function leaves with KErrEof.

-----------------------------------------------------------------------------
*/
EXPORT_C void CGSM610FR_Decoder::ExecuteL(TUint8* aInBuf, TUint8* aOutBuf)
{

	TInt16* outBufPtr = (TInt16*) aOutBuf;


	// Process both odd and even frames

	for (TInt frame = 0; frame < 2; frame++)
	{
		if ( iOddFrame )
		{
			UnpackFrame0(&iCodeBuf, (TInt8*) aInBuf);
        }
	   else
        {
			UnpackFrame1(&iCodeBuf, (TInt8*) aInBuf);
		}


		::RPELTP_decoder(this, &iCodeBuf, outBufPtr);

		iOddFrame = !iOddFrame;
		outBufPtr += FRAMESIZE;

	}

}

/*
-----------------------------------------------------------------------------

    GSM610FR_Decoder

    StopL

    Stop decoder object. Flush out last frames, if necessary.

    Parameters:     none
    
    Return Values:  none

-----------------------------------------------------------------------------
*/
EXPORT_C void CGSM610FR_Decoder::StopL()
    {
    }


/*
-----------------------------------------------------------------------------

    GSM610FR_Decoder

    UnpackFrame0

    Unpack the codewords of the even frame from pack buffer.

    Packing as in MS gsm610 encoder.

    Parameters:     struct codes* aCodeBuf    Code words for one speech frame
                                              are unpacked into this structure.
    
    Return Values:  none


-----------------------------------------------------------------------------
*/
void CGSM610FR_Decoder::UnpackFrame0(struct codes* aCodeBuf,  TInt8* pbuf)
    {
    TInt16* LAR = aCodeBuf->LARc;

    // unpack the LAR[0..7] from the first 4.5 bytes
    LAR[0] = (TInt16)((pbuf[0] & 0x3F));
    LAR[1] = (TInt16)(((pbuf[0] & 0xC0) >> 6) | ((pbuf[1] & 0x0F) << 2));
    LAR[2] = (TInt16)(((pbuf[1] & 0xF0) >> 4) | ((pbuf[2] & 0x01) << 4));
    LAR[3] = (TInt16)(((pbuf[2] & 0x3E) >> 1));
    LAR[4] = (TInt16)(((pbuf[2] & 0xC0) >> 6) | ((pbuf[3] & 0x03) << 2));
    LAR[5] = (TInt16)(((pbuf[3] & 0x3C) >> 2));
    LAR[6] = (TInt16)(((pbuf[3] & 0xC0) >> 6) | ((pbuf[4] & 0x01) << 2));
    LAR[7] = (TInt16)(((pbuf[4] & 0x0E) >> 1));

    // unpack Nc, bc, Mc, xmaxc, and xMc for each of the 4 sub-frames
    for(TInt i = 0; i < 4; i++)
        {
        struct sfcodes& c = aCodeBuf->sfc[i];
#define sfb(x) (pbuf[4+i*7+x])
        c.Nc = (TInt16)(((sfb(0) & 0xF0) >> 4) | ((sfb(1) & 0x07) << 4));
        c.bc = (TInt16)(((sfb(1) & 0x18) >> 3));
        c.Mc = (TInt16)(((sfb(1) & 0x60) >> 5));
        c.xmaxc = (TInt16)(((sfb(1) & 0x80) >> 7) | ((sfb(2) & 0x1F) << 1));
        c.xMc[0] = (TInt16)(((sfb(2) & 0xE0) >> 5));
        c.xMc[1] = (TInt16)((sfb(3) & 0x07));
        c.xMc[2] = (TInt16)(((sfb(3) & 0x3C) >> 3));
        c.xMc[3] = (TInt16)(((sfb(3) & 0xC0) >> 6) | ((sfb(4) & 0x01) << 2));
        c.xMc[4] = (TInt16)(((sfb(4) & 0x0E) >> 1));
        c.xMc[5] = (TInt16)(((sfb(4) & 0x70) >> 4));
        c.xMc[6] = (TInt16)(((sfb(4) & 0x80) >> 7) | ((sfb(5) & 0x03) << 1));
        c.xMc[7] = (TInt16)(((sfb(5) & 0x1C) >> 2));
        c.xMc[8] = (TInt16)(((sfb(5) & 0xE0) >> 5));
        c.xMc[9] = (TInt16)((sfb(6) & 0x07));
        c.xMc[10] = (TInt16)(((sfb(6) & 0x38) >> 3));
        c.xMc[11] = (TInt16)(((sfb(6) & 0xC0) >> 6) | ((sfb(7) & 0x01) << 2));
        c.xMc[12] = (TInt16)(((sfb(7) & 0x0E) >> 1));
#undef sfb
        }
    }


/*
-----------------------------------------------------------------------------

    GSM610FR_Decoder

    UnpackFrame1

    Unpack the codewords of the odd frame from pack buffer.

    Packing as in MS gsm610 encoder.

    Parameters:     struct codes* aCodeBuf    Code words for one speech frame
                                              are unpacked into this structure.
    
    Return Values:  none


-----------------------------------------------------------------------------
*/
void CGSM610FR_Decoder::UnpackFrame1(struct codes* aCodeBuf, TInt8* pbuf)
{
    TInt16* LAR = aCodeBuf->LARc;

    // unpack the LAR[0..7] from the first 4.5 bytes
    LAR[0] = (TInt16)(((pbuf[32] & 0xF0) >> 4) | ((pbuf[33] & 0x03) << 4));
    LAR[1] = (TInt16)(((pbuf[33] & 0xFC) >> 2));
    LAR[2] = (TInt16)(((pbuf[34] & 0x1F)));
    LAR[3] = (TInt16)(((pbuf[34] & 0xE0) >> 5) | ((pbuf[35] & 0x03) << 3));
    LAR[4] = (TInt16)(((pbuf[35] & 0x3C) >> 2));
    LAR[5] = (TInt16)(((pbuf[35] & 0xC0) >> 6) | ((pbuf[36] & 0x03) << 2));
    LAR[6] = (TInt16)(((pbuf[36] & 0x1C) >> 2));
    LAR[7] = (TInt16)(((pbuf[36] & 0xE0) >> 5));

    // unpack Nc, bc, Mc, xmaxc, and xMc for each of the 4 sub-frames
    for(TInt i = 0; i < 4; i++)
        {
        struct sfcodes& c = aCodeBuf->sfc[i];
#define sfb(x) (pbuf[37+i*7+x])
        c.Nc = (TInt16)(sfb(0) & 0x7F);
        c.bc = (TInt16)(((sfb(0) & 0x80) >> 7) | ((sfb(1) & 0x01) << 1));
        c.Mc = (TInt16)(((sfb(1) & 0x06) >> 1));
        c.xmaxc = (TInt16)(((sfb(1) & 0xF8) >> 3) | ((sfb(2) & 0x01) << 5));
        c.xMc[0] = (TInt16)(((sfb(2) & 0x0E) >> 1));
        c.xMc[1] = (TInt16)(((sfb(2) & 0x70) >> 4));
        c.xMc[2] = (TInt16)(((sfb(2) & 0x80) >> 7) | ((sfb(3) & 0x03) << 1));
        c.xMc[3] = (TInt16)(((sfb(3) & 0x1C) >> 2));
        c.xMc[4] = (TInt16)(((sfb(3) & 0xE0) >> 5));
        c.xMc[5] = (TInt16)(((sfb(4) & 0x07)));
        c.xMc[6] = (TInt16)(((sfb(4) & 0x38) >> 3));
        c.xMc[7] = (TInt16)(((sfb(4) & 0xC0) >> 6) | ((sfb(5) & 0x01) << 2));
        c.xMc[8] = (TInt16)(((sfb(5) & 0x0E) >> 1));
        c.xMc[9] = (TInt16)(((sfb(5) & 0x70) >> 4));
        c.xMc[10] = (TInt16)(((sfb(5) & 0x80) >> 7) | ((sfb(6) & 0x03) << 1));
        c.xMc[11] = (TInt16)(((sfb(6) & 0x1C) >> 2));
        c.xMc[12] = (TInt16)(((sfb(6) & 0xE0) >> 5));

#undef sfb
        }
    }


//-----------------------------------------------------------------------------
//  End of File
//-----------------------------------------------------------------------------
