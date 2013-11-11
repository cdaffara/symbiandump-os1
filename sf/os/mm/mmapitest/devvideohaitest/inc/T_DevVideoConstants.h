/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef T_DEVVIDEOCONSTANTS_H_
#define T_DEVVIDEOCONSTANTS_H_

#include <devvideoplay.h>
#include <devvideobase.h> 

//Constant
// size constant declarations
const TInt KScreenNumber = 0;//check documentation for more information about screen number 
/**
 * Length in byte of start code/picture start code
 */
const TInt KScLength =3;
const TInt KVc1HeaderLength = 36;
/**
 * Min def. of Max byte size of a h263 coded picture in QCIF
 */
const TInt KH263MaxCodedSizeQCIF = (8192); //1024 bytes( 1 kibibyte)

const TInt KH263MaxCodedSizeCIF = (32768); //Min def. of Max byte size of a h263 coded picture in CIF

/**
 * from VBV buffering model
 * The Video Buffer Verifier (VBV) is a model hypothetical decoder buffer that will not overflow or underflow 
 * when fed a conforming MPEG bit stream.
 * 
 */
const TInt KMP4MaxCodedSizeQCIF = (10*16384/8);        
const TInt KMP4MaxCodedSizeCIF = (40*16384/8);     
const TInt KMP4MaxCodedSizeVGA = (80*16384/8);     // 2048 bytes / 8 bits
const TInt KMP4MaxCodedSizePAL = (112*16384/8);    
const TInt KMP4MaxCodedSize720P = (248*16384/8);   
const TInt KMaxCodedSize = KMP4MaxCodedSize720P;    // initialize value

/**
 * Buffer Options Const 
 * Min Number of Input buffer, used by decoding SetBufferOptions
 */
const TInt KNumOfInputBuffers = 2; 

//For Syncronization 
/**
 * Time interval for postproc sync test 15 fps
 */
const TInt64 KFrameTimeInterval = (TInt64)(1000000/15) ;
/**
 * Need to tune if playback timing does not match
 */
const TInt64 KStartOffset = 0;

const TInt KQVGAWidth = 240;
const TInt KQVGAHeight = 320;

const TInt KBufferInputPictures = 1;

enum TVideoCodec
	{
	ENoCodec,
	EH263,
	EMPEG4,
	EH264,
	EAVC,
    EVC1,
    EFLV,
    EVP6,
	ERV9	
	};

const TInt KNumPictures = 4;

const TReal32 KVEDSPH263TimerResolution = 30000 / (TReal32 )1001;

#endif /*T_DEVVIDEOCONSTANTS_H_*/
