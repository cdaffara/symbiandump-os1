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
//

#ifndef __SBCCONST_H__
#define __SBCCONST_H__

/**
SBC Syncword, used in the begin of each sbc frame to identify sbc frame, 
it's value is always 0x9c
*/
const TUint8 KSBCFrameSyncWord = 0x9c;

/**
This table is used to calculate bit allocation for 4 subbands sbc frame 
with Loudness allocation method
*/
const TInt8 KSBCOffset4[4][4] =	
	{
		{-1, 0, 0, 0}, 	// fs = 16000
		{-2, 0, 0, 1}, 	// fs = 32000
		{-2, 0, 0, 1}, 	// fs = 44100
		{-2, 0, 0, 1}	// fs = 48000
	};
		
/**
This table is used to calculate bit allocation for 8 subbands sbc frame 
with Loudness allocation method
*/
const TInt8 KSBCOffset8[4][8] =	
	{
		{-2, 0, 0, 0, 0, 0, 0, 1}, 	// fs = 16000
		{-3, 0, 0, 0, 0, 0, 1, 2}, 	// fs = 32000
		{-4, 0, 0, 0, 0, 0, 1, 2}, 	// fs = 44100
		{-4, 0, 0, 0, 0, 0, 1, 2}	// fs = 48000
	};

/**
2^32, scale value of Table KSBCProto4 and KSBCProto8.
*/
const TUint8 KSBCProtoBitsShift = 32;

/**
Table KSBCProto4 is derived form Table Proto_4_40 (see A2DP spec for detail) 
by scaling up by 2^32, it is for the Analysis process for 4 subbands sbc frames, 
we are using integer numbers for high speed, after the calculation the results 
will be scaled down by the same value.  
*/
const TInt32 KSBCProto4[40] = 
	{
	           0,     2304460,     6407591,    11741191, 
	    16480657,    16716234,     8013623,   -13143128, 
	    46874251,    87782726,   124020356,   138271872, 
	   111139929,    26338680,  -123788377,  -333488531, 
	   582368677,   837466400,  1059296397,  1210442915, 
	  1264074726,  1210442915,  1059296397,   837466400, 
	  -582368677,  -333488531,  -123788377,    26338680, 
	   111139929,   138271872,   124020356,    87782726, 
	   -46874251,   -13143128,     8013623,    16716234, 
	    16480657,    11741191,     6407591,     2304460
	};

/**
Table KSBCProto8 is derived form Table Proto_8_80 (see A2DP spec for detail) 
by scaling up by 2^32, it is for the Analysis process for 8 subbands sbc frames, 
we are using integer numbers for high speed, after the calculation the results 
will be scaled down by the same value.  
*/
const TInt32 KSBCProto8[80] = 
	{
	           0,      672486,     1474275,     2382076, 
	     3538707,     4895941,     6341097,     7661007, 
	     8640724,     9035408,     8566508,     6943084, 
	     3874724,     -767963,    -7085541,   -15020250, 
	    24307345,    34486060,    44918676,    54748951, 
	    62932121,    68309566,    69668008,    65792073, 
	    55564768,    38042997,    12558847,   -21113115, 
	   -62880072,  -112141060,  -167826440,  -228437728, 
	   292053237,   356416821,   419083117,   477586143, 
	   529417202,   572366304,   604531701,   624444638, 
	   631167211,   624444638,   604531701,   572366304, 
	   529417202,   477586143,   419083117,   356416821, 
	  -292053237,  -228437728,  -167826440,  -112141060, 
	   -62880072,   -21113115,    12558847,    38042997, 
	    55564768,    65792073,    69668008,    68309566, 
	    62932121,    54748951,    44918676,    34486060, 
	   -24307345,   -15020250,    -7085541,     -767963, 
	     3874724,     6943084,     8566508,     9035408, 
	     8640724,     7661007,     6341097,     4895941, 
	     3538707,     2382076,     1474275,      672486
	};


/**
2^30, scale value of Table KSBCAnalysisMatrix4 and KSBCAnalysisMatrix8.
*/
const TUint8 KSBCAnalysisMatrixBitsShift = 30;

/**
Table of cos((i + 0.5) * (k - 2) * PI / 4) * 2^30
This table is used in Analysis process for 4 subbands, here we are using 
integer values instead of real values for high speed, the values are scaled 
up by 2^30, after calculation the results will be scaled down by the same value.  
*/
const TInt32 KSBCAnalysisMatrix4[4][8] =
	{
		{  759250125,   992008094,  1073741824,   992008094, 
		   759250125,   410903207,           0,  -410903207 },
		   
		{ -759250125,   410903207,  1073741824,   410903207, 
		  -759250125,  -992008094,           0,   992008094 },
		  
		{ -759250125,  -410903207,  1073741824,  -410903207, 
		  -759250125,   992008094,           0,  -992008094 },
		  
		{  759250125,  -992008094,  1073741824,  -992008094, 
		   759250125,  -410903207,           0,   410903207 }
	};

/**
Table of cos((i + 0.5) * (k - 4) * PI / 8) * 2^30
This table is used in Analysis process for 8 subbands, here we are using 
integer values instead of real values for high speed, the values are scaled 
up by 2^30, after calculation the results will be scaled down by the same value.  
*/
const TInt32 KSBCAnalysisMatrix8[8][16] =
	{
		{  759250125,   892783698,   992008094,  1053110176, 
		  1073741824,  1053110176,   992008094,   892783698, 
		   759250125,   596538995,   410903207,   209476638, 
		           0,  -209476638,  -410903207,  -596538995 },
		           
		{ -759250125,  -209476638,   410903207,   892783698, 
		  1073741824,   892783698,   410903207,  -209476638, 
		  -759250125, -1053110176,  -992008094,  -596538995, 
		           0,   596538995,   992008094,  1053110176 },
		          
		{ -759250125, -1053110176,  -410903207,   596538995, 
		  1073741824,   596538995,  -410903207, -1053110176, 
		  -759250125,   209476638,   992008094,   892783698, 
		           0,  -892783698,  -992008094,  -209476638 },
		           
		{  759250125,  -596538995,  -992008094,   209476638, 
		  1073741824,   209476638,  -992008094,  -596538995, 
		   759250125,   892783698,  -410903207, -1053110176, 
		           0,  1053110176,   410903207,  -892783698 },
		          
		{  759250125,   596538995,  -992008094,  -209476638, 
		  1073741824,  -209476638,  -992008094,   596538995, 
		   759250125,  -892783698,  -410903207,  1053110176, 
		           0, -1053110176,   410903207,   892783698 },
		           
		{ -759250125,  1053110176,  -410903207,  -596538995, 
		  1073741824,  -596538995,  -410903207,  1053110176, 
		  -759250125,  -209476638,   992008094,  -892783698, 
		           0,   892783698,  -992008094,   209476638 },
		          
		{ -759250125,   209476638,   410903207,  -892783698, 
		  1073741824,  -892783698,   410903207,   209476638, 
		  -759250125,  1053110176,  -992008094,   596538995, 
		           0,  -596538995,   992008094, -1053110176 },
		          
		{  759250125,  -892783698,   992008094, -1053110176, 
		  1073741824, -1053110176,   992008094,  -892783698, 
		   759250125,  -596538995,   410903207,  -209476638, 
		           0,   209476638,  -410903207,   596538995 }
	};
		
#endif // __SBCCONST_H__
