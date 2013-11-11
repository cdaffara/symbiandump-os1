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

#include <3gplibrary/mp4config.h>
#include <3gplibrary/mp4lib.h>
#include "mp4atom.h"
#include "mp4memwrap.h"
#include "mp4file.h"
#include "mp4compose.h"
#include "mp4utils.h"

// MACROS
// Debug print macro
#ifdef _DEBUG
#include <e32svr.h>
#define PRINT(x) 
#else
#define PRINT(x)
#endif

#define STTSMAXENTRYCOUNT  100
#define STSZMAXSAMPLECOUNT 100
#define STSCMAXENTRYCOUNT    2
#define STCOMAXENTRYCOUNT  100
#define STSSMAXENTRYCOUNT  100
#define SDTPMAXENTRYCOUNT  100


extern EXPORT_C MP4Err MP4ComposeOpen(MP4Handle *apihandle,
                                                   MP4FileName filename,
                                                   mp4_u32 type)
{
  MP4Err error = MP4_OK;
  MP4HandleImp* handle = (MP4HandleStruct **)apihandle;

  if ( filename != NULL )
      {
      *handle = (MP4HandleImp)mp4malloc(sizeof(MP4HandleStruct));     
      if (*handle == NULL)
          {
          return MP4_OUT_OF_MEMORY;      
          }
      (*handle)->bufferWrite=MP4FALSE;
      }
  else
      {
      if ( *handle == NULL )
          {
          return MP4_ERROR;
          }
      }
  
  (*handle)->file32Duplicate = NULL;
  (*handle)->FileHandleFromOutside = EFalse;

  if(!(*handle)->bufferWrite)
  {
		if (!filename)
		    {
			error = MP4_FILE_ERROR;
		    }
		
		if ( error == MP4_OK )
		    {
    		if (saveFileName(filename, *handle) < 0)
	    		error = MP4_OUT_OF_MEMORY;
		    }
		
		if (error == MP4_OK)
		{
			if (createTmpFileName(filename, &((*handle)->tmpFileName)) == -1)
				error = MP4_OUT_OF_MEMORY;
		}
		
		if (error == MP4_OK)
		{
			if (initFileWrite(filename, *handle) == -1)
				error = MP4_FILE_ERROR;
		}
		
		if (error == MP4_OK)
		{
			if (initTmpFileWrite((*handle)->tmpFileName, *handle) == -1)
				error = MP4_FILE_ERROR;
		}
  }

  if (error == MP4_OK)
  {
    (*handle)->diskWriteBuf = (mp4_u8 *)mp4malloc(WRITEBUFSIZE);
    if ((*handle)->diskWriteBuf == NULL)
      error = MP4_OUT_OF_MEMORY;
  }

  if (error == MP4_OK)
  {
    (*handle)->audioSampleTable = (sampleTable *)mp4malloc(sizeof(sampleTable));
    if ((*handle)->audioSampleTable == NULL)
      error = MP4_OUT_OF_MEMORY;
  }

  if (error == MP4_OK)
  {
    (*handle)->audioSampleTable->sttsSampleCount = (mp4_u32 *)mp4malloc(sizeof(mp4_u32) * STTSMAXENTRYCOUNT);
    if ((*handle)->audioSampleTable->sttsSampleCount == NULL)
      error = MP4_OUT_OF_MEMORY;
  }

  if (error == MP4_OK)
  {
    (*handle)->audioSampleTable->sttsSampleDelta = (mp4_u32 *)mp4malloc(sizeof(mp4_u32) * STTSMAXENTRYCOUNT);
    if ((*handle)->audioSampleTable->sttsSampleDelta == NULL)
      error = MP4_OUT_OF_MEMORY;
  }

  if (error == MP4_OK)
  {
    (*handle)->audioSampleTable->stszEntrySize = (mp4_u32 *)mp4malloc(sizeof(mp4_u32) * STSZMAXSAMPLECOUNT);
    if ((*handle)->audioSampleTable->stszEntrySize == NULL)
      error = MP4_OUT_OF_MEMORY;
  }

  if (error == MP4_OK)
  {
    (*handle)->audioSampleTable->stscFirstChunk = (mp4_u32 *)mp4malloc(sizeof(mp4_u32) * STSCMAXENTRYCOUNT);
    if ((*handle)->audioSampleTable->stscFirstChunk == NULL)
      error = MP4_OUT_OF_MEMORY;
  }

  if (error == MP4_OK)
  {
    (*handle)->audioSampleTable->stscSamplesPerChunk = (mp4_u32 *)mp4malloc(sizeof(mp4_u32) * STSCMAXENTRYCOUNT);
    if ((*handle)->audioSampleTable->stscSamplesPerChunk == NULL)
      error = MP4_OUT_OF_MEMORY;
  }

  if (error == MP4_OK)
  {
    (*handle)->audioSampleTable->stscSampleDescriptionIndex = (mp4_u32 *)mp4malloc(sizeof(mp4_u32) * STSCMAXENTRYCOUNT);
    if ((*handle)->audioSampleTable->stscSampleDescriptionIndex == NULL)
      error = MP4_OUT_OF_MEMORY;
  }

  if (error == MP4_OK)
  {
    (*handle)->audioSampleTable->stcoChunkOffset = (mp4_u64 *)mp4malloc(sizeof(mp4_u64) * STCOMAXENTRYCOUNT);
    if ((*handle)->audioSampleTable->stcoChunkOffset == NULL)
      error = MP4_OUT_OF_MEMORY;
  }

  if (error == MP4_OK)
  {
    (*handle)->videoSampleTable = (sampleTable *)mp4malloc(sizeof(sampleTable));
    if ((*handle)->videoSampleTable == NULL)
      error = MP4_OUT_OF_MEMORY;
  }

  if (error == MP4_OK)
  {
    (*handle)->videoSampleTable->sttsSampleCount = (mp4_u32 *)mp4malloc(sizeof(mp4_u32) * STTSMAXENTRYCOUNT);
    if ((*handle)->videoSampleTable->sttsSampleCount == NULL)
      error = MP4_OUT_OF_MEMORY;
  }

  if (error == MP4_OK)
  {
    (*handle)->videoSampleTable->sttsSampleDelta = (mp4_u32 *)mp4malloc(sizeof(mp4_u32) * STTSMAXENTRYCOUNT);
    if ((*handle)->videoSampleTable->sttsSampleDelta == NULL)
      error = MP4_OUT_OF_MEMORY;
  }

  if (error == MP4_OK)
  {
    (*handle)->videoSampleTable->stszEntrySize = (mp4_u32 *)mp4malloc(sizeof(mp4_u32) * STSZMAXSAMPLECOUNT);
    if ((*handle)->videoSampleTable->stszEntrySize == NULL)
      error = MP4_OUT_OF_MEMORY;
  }

  if (error == MP4_OK)
  {
    (*handle)->videoSampleTable->stscFirstChunk = (mp4_u32 *)mp4malloc(sizeof(mp4_u32) * STSCMAXENTRYCOUNT);
    if ((*handle)->videoSampleTable->stscFirstChunk == NULL)
      error = MP4_OUT_OF_MEMORY;
  }

  if (error == MP4_OK)
  {
    (*handle)->videoSampleTable->stscSamplesPerChunk = (mp4_u32 *)mp4malloc(sizeof(mp4_u32) * STSCMAXENTRYCOUNT);
    if ((*handle)->videoSampleTable->stscSamplesPerChunk == NULL)
      error = MP4_OUT_OF_MEMORY;
  }

  if (error == MP4_OK)
  {
    (*handle)->videoSampleTable->stscSampleDescriptionIndex = (mp4_u32 *)mp4malloc(sizeof(mp4_u32) * STSCMAXENTRYCOUNT);
    if ((*handle)->videoSampleTable->stscSampleDescriptionIndex == NULL)
      error = MP4_OUT_OF_MEMORY;
  }

  if (error == MP4_OK)
  {
    (*handle)->videoSampleTable->stcoChunkOffset = (mp4_u64 *)mp4malloc(sizeof(mp4_u64) * STCOMAXENTRYCOUNT);
    if ((*handle)->videoSampleTable->stcoChunkOffset == NULL)
      error = MP4_OUT_OF_MEMORY;
  }

  if (error == MP4_OK)
  {
    (*handle)->videoSampleTable->stssSampleNumber = (mp4_u32 *)mp4malloc(sizeof(mp4_u32) * STSSMAXENTRYCOUNT);
    if ((*handle)->videoSampleTable->stssSampleNumber == NULL)
      error = MP4_OUT_OF_MEMORY;
  }
  
  if (error == MP4_OK)
  {
    (*handle)->videoSampleTable->sdtpSampleDependency = (mp4_u8 *)mp4malloc(sizeof(mp4_u8) * SDTPMAXENTRYCOUNT);
    if ((*handle)->videoSampleTable->sdtpSampleDependency == NULL)
      error = MP4_OUT_OF_MEMORY;
  }

  // register for stblib use 
  if (error == MP4_OK)
	  {
	  if (openStdlib() !=  MP4_OK)
		  {
		  error = MP4_ERROR;
		  }
	  }
  
  if (error != MP4_OK)
  {
    if(!(*handle)->bufferWrite)
    {
        closeFile(*handle);

        closeTmpFile(*handle);

        freeTmpFileName((*handle)->tmpFileName);
    }

    if ((*handle)->diskWriteBuf)
      mp4free((*handle)->diskWriteBuf);

    if ((*handle)->videoSampleTable)
    {
      if ((*handle)->videoSampleTable->stssSampleNumber)
        mp4free((*handle)->videoSampleTable->stssSampleNumber);

      if ((*handle)->videoSampleTable->stcoChunkOffset)
        mp4free((*handle)->videoSampleTable->stcoChunkOffset);

      if ((*handle)->videoSampleTable->stscSampleDescriptionIndex)
        mp4free((*handle)->videoSampleTable->stscSampleDescriptionIndex);

      if ((*handle)->videoSampleTable->stscSamplesPerChunk)
        mp4free((*handle)->videoSampleTable->stscSamplesPerChunk);

      if ((*handle)->videoSampleTable->stscFirstChunk)
        mp4free((*handle)->videoSampleTable->stscFirstChunk);

      if ((*handle)->videoSampleTable->stszEntrySize)
        mp4free((*handle)->videoSampleTable->stszEntrySize);

      if ((*handle)->videoSampleTable->sttsSampleCount)
        mp4free((*handle)->videoSampleTable->sttsSampleCount);

      if ((*handle)->videoSampleTable->sttsSampleDelta)
        mp4free((*handle)->videoSampleTable->sttsSampleDelta);
      
      if ((*handle)->videoSampleTable->sdtpSampleDependency)
        mp4free((*handle)->videoSampleTable->sdtpSampleDependency);

      mp4free((*handle)->videoSampleTable);
    }

    if ((*handle)->audioSampleTable)
    {
      if ((*handle)->audioSampleTable->stcoChunkOffset)
        mp4free((*handle)->audioSampleTable->stcoChunkOffset);

      if ((*handle)->audioSampleTable->stscSampleDescriptionIndex)
        mp4free((*handle)->audioSampleTable->stscSampleDescriptionIndex);

      if ((*handle)->audioSampleTable->stscSamplesPerChunk)
        mp4free((*handle)->audioSampleTable->stscSamplesPerChunk);

      if ((*handle)->audioSampleTable->stscFirstChunk)
        mp4free((*handle)->audioSampleTable->stscFirstChunk);

      if ((*handle)->audioSampleTable->stszEntrySize)
        mp4free((*handle)->audioSampleTable->stszEntrySize);

      if ((*handle)->audioSampleTable->sttsSampleDelta)
        mp4free((*handle)->audioSampleTable->sttsSampleDelta);

      if ((*handle)->audioSampleTable->sttsSampleCount)
        mp4free((*handle)->audioSampleTable->sttsSampleCount);

      mp4free((*handle)->audioSampleTable);
    }

    if ((*handle)->fileName)
      mp4free((*handle)->fileName);

    mp4free(*handle);
    *handle = NULL;

    return error;
  }


  (*handle)->audioSampleTable->sttsMaxEntryCount  = STTSMAXENTRYCOUNT;
  (*handle)->audioSampleTable->stszMaxSampleCount = STSZMAXSAMPLECOUNT;
  (*handle)->audioSampleTable->stscMaxEntryCount  = STSCMAXENTRYCOUNT;
  (*handle)->audioSampleTable->stcoMaxEntryCount  = STCOMAXENTRYCOUNT;
  (*handle)->audioSampleTable->stcoNeed64Bits     = EFalse;
  
  (*handle)->videoSampleTable->sttsMaxEntryCount  = STTSMAXENTRYCOUNT;
  (*handle)->videoSampleTable->stszMaxSampleCount = STSZMAXSAMPLECOUNT;
  (*handle)->videoSampleTable->stscMaxEntryCount  = STSCMAXENTRYCOUNT;
  (*handle)->videoSampleTable->stcoMaxEntryCount  = STCOMAXENTRYCOUNT;
  (*handle)->videoSampleTable->stssMaxEntryCount  = STSSMAXENTRYCOUNT;
  (*handle)->videoSampleTable->sdtpMaxEntryCount  = SDTPMAXENTRYCOUNT;
  (*handle)->videoSampleTable->stcoNeed64Bits     = EFalse;
  
  (*handle)->type = type;

  /* Check if a 3GPP2 codec is being used */
  if ((*handle)->type & MP4_TYPE_QCELP_13K)
      {
      (*handle)->generate3G2 = MP4TRUE;
      }

  return MP4_OK;
}

extern EXPORT_C MP4Err MP4ComposeOpenToBuffer(MP4Handle *apihandle,
                                                   mp4_u32 type,
                                                   mp4_u8* composeBuffer, 
                                                   mp4_u32 *composedSize)
{
  MP4HandleImp* handle = (MP4HandleStruct **)apihandle;

  *handle = (MP4HandleImp)mp4malloc(sizeof(MP4HandleStruct));
  if (*handle == NULL)              
    return MP4_OUT_OF_MEMORY;

  (*handle)->file32Duplicate = NULL;
  (*handle)->bufferWrite=MP4TRUE;
  (*handle)->FileHandleFromOutside = EFalse;

  if(composeBuffer == NULL)
	{
	return MP4_NO_OUTPUT_BUFFER; //This is to indicate that compose buffer has not been initialized
	}

  (*handle)->composeBuffer=composeBuffer;
  (*handle)->composedSize=composedSize;

  return MP4ComposeOpen( apihandle, NULL, type );
}

extern EXPORT_C MP4Err MP4ComposeOpenFileHandle(MP4Handle *apihandle, 
                                                             RFile *composedfile, 
                                                             TDriveNumber metadataTempDrive, 
                                                             mp4_u32 type)
{
  MP4Err err;
  RFile64 *f64 = new RFile64;
  if (f64 == NULL)
  {
    return MP4_OUT_OF_MEMORY;
  }
  if  (f64->Duplicate(*composedfile) != KErrNone)
  {
    delete f64;
    return MP4_ERROR;
  }
  err = MP4ComposeOpenFileHandle64(apihandle, f64, metadataTempDrive, type);
  if (err == MP4_OK)
  {    
    MP4HandleImp* handle = (MP4HandleStruct **)apihandle;
    (*handle)->file32Duplicate = (void*)f64;
  }
  return err;
}



extern EXPORT_C MP4Err MP4ComposeOpenFileHandle64(MP4Handle *apihandle, 
                                                             RFile64 *composedfile, 
                                                             TDriveNumber metadataTempDrive, 
                                                             mp4_u32 type)
{
  MP4Err error = MP4_OK;
  MP4HandleImp* handle = (MP4HandleStruct **)apihandle;

  *handle = (MP4HandleImp)mp4malloc(sizeof(MP4HandleStruct));     
  if (*handle == NULL)
      {
      return MP4_OUT_OF_MEMORY;      
      }
  (*handle)->bufferWrite=MP4FALSE;
  (*handle)->file32Duplicate = NULL;
  // since file handle we can´t use temporary file for mediadata safely.
  (*handle)->flags |= MP4_FLAG_METADATALAST;
  (*handle)->FileHandleFromOutside = ETrue;
  (*handle)->fileHandleDrive = metadataTempDrive;

  RFs   *fs;
  fs = new(RFs);
  (*handle)->fs = (void *)fs;
  if (fs == NULL)
    error = MP4_FILE_ERROR;
  
  if (error == MP4_OK)
    {
    if (fs->Connect() != KErrNone)
        error = MP4_FILE_ERROR;
    }

  if (error == MP4_OK)
    {
    (*handle)->rfile = (void *)composedfile;
    if (composedfile == NULL)
        error = MP4_FILE_ERROR;
    
    (*handle)->file = (*handle)->rfile;    
    
    TRAPD(traperror, (*handle)->filewriter = CFileWriter::NewL( *composedfile ));
    if ( traperror != KErrNone )
        {
        error = MP4_FILE_ERROR;
        }
    }
	
  if (error == MP4_OK)
  {
    (*handle)->diskWriteBuf = (mp4_u8 *)mp4malloc(WRITEBUFSIZE);
    if ((*handle)->diskWriteBuf == NULL)
      error = MP4_OUT_OF_MEMORY;
  }

  if (error == MP4_OK)
  {
    (*handle)->audioSampleTable = (sampleTable *)mp4malloc(sizeof(sampleTable));
    if ((*handle)->audioSampleTable == NULL)
      error = MP4_OUT_OF_MEMORY;
  }

  if (error == MP4_OK)
  {
    (*handle)->audioSampleTable->sttsSampleCount = (mp4_u32 *)mp4malloc(sizeof(mp4_u32) * STTSMAXENTRYCOUNT);
    if ((*handle)->audioSampleTable->sttsSampleCount == NULL)
      error = MP4_OUT_OF_MEMORY;
  }

  if (error == MP4_OK)
  {
    (*handle)->audioSampleTable->sttsSampleDelta = (mp4_u32 *)mp4malloc(sizeof(mp4_u32) * STTSMAXENTRYCOUNT);
    if ((*handle)->audioSampleTable->sttsSampleDelta == NULL)
      error = MP4_OUT_OF_MEMORY;
  }

  if (error == MP4_OK)
  {
    (*handle)->audioSampleTable->stszEntrySize = (mp4_u32 *)mp4malloc(sizeof(mp4_u32) * STSZMAXSAMPLECOUNT);
    if ((*handle)->audioSampleTable->stszEntrySize == NULL)
      error = MP4_OUT_OF_MEMORY;
  }

  if (error == MP4_OK)
  {
    (*handle)->audioSampleTable->stscFirstChunk = (mp4_u32 *)mp4malloc(sizeof(mp4_u32) * STSCMAXENTRYCOUNT);
    if ((*handle)->audioSampleTable->stscFirstChunk == NULL)
      error = MP4_OUT_OF_MEMORY;
  }

  if (error == MP4_OK)
  {
    (*handle)->audioSampleTable->stscSamplesPerChunk = (mp4_u32 *)mp4malloc(sizeof(mp4_u32) * STSCMAXENTRYCOUNT);
    if ((*handle)->audioSampleTable->stscSamplesPerChunk == NULL)
      error = MP4_OUT_OF_MEMORY;
  }

  if (error == MP4_OK)
  {
    (*handle)->audioSampleTable->stscSampleDescriptionIndex = (mp4_u32 *)mp4malloc(sizeof(mp4_u32) * STSCMAXENTRYCOUNT);
    if ((*handle)->audioSampleTable->stscSampleDescriptionIndex == NULL)
      error = MP4_OUT_OF_MEMORY;
  }

  if (error == MP4_OK)
  {
    (*handle)->audioSampleTable->stcoChunkOffset = (mp4_u64 *)mp4malloc(sizeof(mp4_u64) * STCOMAXENTRYCOUNT);
    if ((*handle)->audioSampleTable->stcoChunkOffset == NULL)
      error = MP4_OUT_OF_MEMORY;
  }

  if (error == MP4_OK)
  {
    (*handle)->videoSampleTable = (sampleTable *)mp4malloc(sizeof(sampleTable));
    if ((*handle)->videoSampleTable == NULL)
      error = MP4_OUT_OF_MEMORY;
  }

  if (error == MP4_OK)
  {
    (*handle)->videoSampleTable->sttsSampleCount = (mp4_u32 *)mp4malloc(sizeof(mp4_u32) * STTSMAXENTRYCOUNT);
    if ((*handle)->videoSampleTable->sttsSampleCount == NULL)
      error = MP4_OUT_OF_MEMORY;
  }

  if (error == MP4_OK)
  {
    (*handle)->videoSampleTable->sttsSampleDelta = (mp4_u32 *)mp4malloc(sizeof(mp4_u32) * STTSMAXENTRYCOUNT);
    if ((*handle)->videoSampleTable->sttsSampleDelta == NULL)
      error = MP4_OUT_OF_MEMORY;
  }

  if (error == MP4_OK)
  {
    (*handle)->videoSampleTable->stszEntrySize = (mp4_u32 *)mp4malloc(sizeof(mp4_u32) * STSZMAXSAMPLECOUNT);
    if ((*handle)->videoSampleTable->stszEntrySize == NULL)
      error = MP4_OUT_OF_MEMORY;
  }

  if (error == MP4_OK)
  {
    (*handle)->videoSampleTable->stscFirstChunk = (mp4_u32 *)mp4malloc(sizeof(mp4_u32) * STSCMAXENTRYCOUNT);
    if ((*handle)->videoSampleTable->stscFirstChunk == NULL)
      error = MP4_OUT_OF_MEMORY;
  }

  if (error == MP4_OK)
  {
    (*handle)->videoSampleTable->stscSamplesPerChunk = (mp4_u32 *)mp4malloc(sizeof(mp4_u32) * STSCMAXENTRYCOUNT);
    if ((*handle)->videoSampleTable->stscSamplesPerChunk == NULL)
      error = MP4_OUT_OF_MEMORY;
  }

  if (error == MP4_OK)
  {
    (*handle)->videoSampleTable->stscSampleDescriptionIndex = (mp4_u32 *)mp4malloc(sizeof(mp4_u32) * STSCMAXENTRYCOUNT);
    if ((*handle)->videoSampleTable->stscSampleDescriptionIndex == NULL)
      error = MP4_OUT_OF_MEMORY;
  }

  if (error == MP4_OK)
  {
    (*handle)->videoSampleTable->stcoChunkOffset = (mp4_u64 *)mp4malloc(sizeof(mp4_u64) * STCOMAXENTRYCOUNT);
    if ((*handle)->videoSampleTable->stcoChunkOffset == NULL)
      error = MP4_OUT_OF_MEMORY;
  }

  if (error == MP4_OK)
  {
    (*handle)->videoSampleTable->stssSampleNumber = (mp4_u32 *)mp4malloc(sizeof(mp4_u32) * STSSMAXENTRYCOUNT);
    if ((*handle)->videoSampleTable->stssSampleNumber == NULL)
      error = MP4_OUT_OF_MEMORY;
  }
  
  if (error == MP4_OK)
  {
    (*handle)->videoSampleTable->sdtpSampleDependency = (mp4_u8 *)mp4malloc(sizeof(mp4_u8) * SDTPMAXENTRYCOUNT);
    if ((*handle)->videoSampleTable->sdtpSampleDependency == NULL)
      error = MP4_OUT_OF_MEMORY;
  }

  // register for stblib use 
  if (error == MP4_OK)
	  {
	  if (openStdlib() !=  MP4_OK)
		  {
		  error = MP4_ERROR;
		  }
	  }
  
  if (error != MP4_OK)
  {
    if(!(*handle)->bufferWrite)
    {
        closeFile(*handle);
    }

    if ((*handle)->diskWriteBuf)
      mp4free((*handle)->diskWriteBuf);

    if ((*handle)->videoSampleTable)
    {
      if ((*handle)->videoSampleTable->stssSampleNumber)
        mp4free((*handle)->videoSampleTable->stssSampleNumber);

      if ((*handle)->videoSampleTable->stcoChunkOffset)
        mp4free((*handle)->videoSampleTable->stcoChunkOffset);

      if ((*handle)->videoSampleTable->stscSampleDescriptionIndex)
        mp4free((*handle)->videoSampleTable->stscSampleDescriptionIndex);

      if ((*handle)->videoSampleTable->stscSamplesPerChunk)
        mp4free((*handle)->videoSampleTable->stscSamplesPerChunk);

      if ((*handle)->videoSampleTable->stscFirstChunk)
        mp4free((*handle)->videoSampleTable->stscFirstChunk);

      if ((*handle)->videoSampleTable->stszEntrySize)
        mp4free((*handle)->videoSampleTable->stszEntrySize);

      if ((*handle)->videoSampleTable->sttsSampleCount)
        mp4free((*handle)->videoSampleTable->sttsSampleCount);

      if ((*handle)->videoSampleTable->sttsSampleDelta)
        mp4free((*handle)->videoSampleTable->sttsSampleDelta);
      
      if ((*handle)->videoSampleTable->sdtpSampleDependency)
        mp4free((*handle)->videoSampleTable->sdtpSampleDependency);

      mp4free((*handle)->videoSampleTable);
    }

    if ((*handle)->audioSampleTable)
    {
      if ((*handle)->audioSampleTable->stcoChunkOffset)
        mp4free((*handle)->audioSampleTable->stcoChunkOffset);

      if ((*handle)->audioSampleTable->stscSampleDescriptionIndex)
        mp4free((*handle)->audioSampleTable->stscSampleDescriptionIndex);

      if ((*handle)->audioSampleTable->stscSamplesPerChunk)
        mp4free((*handle)->audioSampleTable->stscSamplesPerChunk);

      if ((*handle)->audioSampleTable->stscFirstChunk)
        mp4free((*handle)->audioSampleTable->stscFirstChunk);

      if ((*handle)->audioSampleTable->stszEntrySize)
        mp4free((*handle)->audioSampleTable->stszEntrySize);

      if ((*handle)->audioSampleTable->sttsSampleDelta)
        mp4free((*handle)->audioSampleTable->sttsSampleDelta);

      if ((*handle)->audioSampleTable->sttsSampleCount)
        mp4free((*handle)->audioSampleTable->sttsSampleCount);

      mp4free((*handle)->audioSampleTable);
    }

    if ((*handle)->fileName)
      mp4free((*handle)->fileName);

    mp4free(*handle);
    *handle = NULL;

    return error;
  }

  (*handle)->audioSampleTable->sttsMaxEntryCount  = STTSMAXENTRYCOUNT;
  (*handle)->audioSampleTable->stszMaxSampleCount = STSZMAXSAMPLECOUNT;
  (*handle)->audioSampleTable->stscMaxEntryCount  = STSCMAXENTRYCOUNT;
  (*handle)->audioSampleTable->stcoMaxEntryCount  = STCOMAXENTRYCOUNT;
  (*handle)->audioSampleTable->stcoNeed64Bits     = EFalse;

  (*handle)->videoSampleTable->sttsMaxEntryCount  = STTSMAXENTRYCOUNT;
  (*handle)->videoSampleTable->stszMaxSampleCount = STSZMAXSAMPLECOUNT;
  (*handle)->videoSampleTable->stscMaxEntryCount  = STSCMAXENTRYCOUNT;
  (*handle)->videoSampleTable->stcoMaxEntryCount  = STCOMAXENTRYCOUNT;
  (*handle)->videoSampleTable->stssMaxEntryCount  = STSSMAXENTRYCOUNT;
  (*handle)->videoSampleTable->sdtpMaxEntryCount  = SDTPMAXENTRYCOUNT;
  (*handle)->videoSampleTable->stcoNeed64Bits     = EFalse;

  (*handle)->type = type;

  /* Check if a 3GPP2 codec is being used */
  if ((*handle)->type & MP4_TYPE_QCELP_13K)
      {
      (*handle)->generate3G2 = MP4TRUE;
      }
  
  return MP4_OK;
}

extern EXPORT_C MP4Err MP4ComposeClose(MP4Handle apihandle)
{
  PRINT((_L("e_composeclose 1")));

  mp4_i32  error;
  MP4HandleImp handle = (MP4HandleImp)apihandle;

  if (!handle)
    return MP4_ERROR;
  
  PRINT((_L("e_composeclose_writedatatofile 1")));

  error = writeDataToFile(handle);

  PRINT((_L("e_composeclose_writedatatofile 0")));

  if(handle->bufferWrite && handle->composedSize)
  {
	   *(handle->composedSize)=handle->bytesProgressed;
  }

  PRINT((_L("e_composeclose_free_decspecinfos 1")));
  if (handle->videoDecSpecificInfo)
    mp4free(handle->videoDecSpecificInfo);

  if (handle->audioDecSpecificInfo)
    mp4free(handle->audioDecSpecificInfo);
  
  PRINT((_L("e_composeclose_free_decspecinfos 0")));
  PRINT((_L("e_composeclose_free_audiosampletables 1")));
  
  if (handle->audioSampleTable && handle->audioSampleTable->sttsSampleCount)
    mp4free(handle->audioSampleTable->sttsSampleCount);

  if (handle->audioSampleTable && handle->audioSampleTable->sttsSampleDelta)
    mp4free(handle->audioSampleTable->sttsSampleDelta);

  if (handle->audioSampleTable && handle->audioSampleTable->stszEntrySize)
    mp4free(handle->audioSampleTable->stszEntrySize);

  if (handle->audioSampleTable && handle->audioSampleTable->stscFirstChunk)
    mp4free(handle->audioSampleTable->stscFirstChunk);

  if (handle->audioSampleTable && handle->audioSampleTable->stscSamplesPerChunk)
    mp4free(handle->audioSampleTable->stscSamplesPerChunk);

  if (handle->audioSampleTable && handle->audioSampleTable->stscSampleDescriptionIndex)
    mp4free(handle->audioSampleTable->stscSampleDescriptionIndex);

  if (handle->audioSampleTable && handle->audioSampleTable->stcoChunkOffset)
    mp4free(handle->audioSampleTable->stcoChunkOffset);

  if (handle->audioSampleTable)
    mp4free(handle->audioSampleTable);
  PRINT((_L("e_composeclose_free_audiosampletables 0")));
  PRINT((_L("e_composeclose_free_videosampletables 1")));

  if (handle->videoSampleTable && handle->videoSampleTable->sttsSampleCount)
    mp4free(handle->videoSampleTable->sttsSampleCount);

  if (handle->videoSampleTable && handle->videoSampleTable->sttsSampleDelta)
    mp4free(handle->videoSampleTable->sttsSampleDelta);

  if (handle->videoSampleTable && handle->videoSampleTable->stszEntrySize)
    mp4free(handle->videoSampleTable->stszEntrySize);

  if (handle->videoSampleTable && handle->videoSampleTable->stscFirstChunk)
    mp4free(handle->videoSampleTable->stscFirstChunk);

  if (handle->videoSampleTable && handle->videoSampleTable->stscSamplesPerChunk)
    mp4free(handle->videoSampleTable->stscSamplesPerChunk);

  if (handle->videoSampleTable && handle->videoSampleTable->stscSampleDescriptionIndex)
    mp4free(handle->videoSampleTable->stscSampleDescriptionIndex);

  if (handle->videoSampleTable && handle->videoSampleTable->stcoChunkOffset)
    mp4free(handle->videoSampleTable->stcoChunkOffset);

  if (handle->videoSampleTable && handle->videoSampleTable->stssSampleNumber)
    mp4free(handle->videoSampleTable->stssSampleNumber);
  
  if (handle->videoSampleTable && handle->videoSampleTable->sdtpSampleDependency)
    mp4free(handle->videoSampleTable->sdtpSampleDependency);

  if (handle->videoSampleTable)
    mp4free(handle->videoSampleTable);
  PRINT((_L("e_composeclose_free_videosampletables 0")));  
  PRINT((_L("e_composeclose_free_dskbuf_and_udta 1")));
  if (handle->diskWriteBuf)
    mp4free(handle->diskWriteBuf);

  if (handle->moovUDTA)
      {
      if ( handle->moovUDTA->contentdata )
          {
          mp4free(handle->moovUDTA->contentdata);
          }
      mp4free(handle->moovUDTA);
      }
  if (handle->audioUDTA)
      {
      if ( handle->audioUDTA->contentdata )
          {
          mp4free(handle->audioUDTA->contentdata);
          }
      mp4free(handle->audioUDTA);
      }
  if (handle->videoUDTA)
      {
      if ( handle->videoUDTA->contentdata )
          {
          mp4free(handle->videoUDTA->contentdata);
          }
      mp4free(handle->videoUDTA);
      }
  PRINT((_L("e_composeclose_free_dskbuf_and_udta 0")));

  if (!(handle->flags & MP4_FLAG_METADATALAST))
  {
    if (handle->tmpfile)
    {
      PRINT((_L("e_composeclose_close_temp_files 1")));
      closeTmpFile(handle);
      PRINT((_L("e_composeclose_close_temp_files 0")));
      PRINT((_L("e_composeclose_del_temp_files 1")));      
      deleteTmpFile(handle);
      PRINT((_L("e_composeclose_del_temp_files 0")));
    }
  }

  PRINT((_L("e_composeclose_free_temp_filename 1")));
  freeTmpFileName(handle->tmpFileName);
  PRINT((_L("e_composeclose_free_temp_filename 0")));

  if (handle->flags & MP4_FLAG_LONGCLIP)
  {
  	PRINT((_L("e_composeclose_close_metadata_files 1")));
    closeMetaDataFiles(handle);
    PRINT((_L("e_composeclose_close_metadata_files 0")));
	PRINT((_L("e_composeclose_del_metadata_files 1")));
    if (deleteMetaDataFiles(handle) < 0)
      error = -1;
    PRINT((_L("e_composeclose_del_metadata_files 0")));
  }

  PRINT((_L("e_composeclose_free_filename 1")));
  if (handle->fileName)
    mp4free(handle->fileName);
  PRINT((_L("e_composeclose_free_filename 0")));

  PRINT((_L("e_composeclose_close_file 1")));
  if (handle->file)
    closeFile(handle);
  PRINT((_L("e_composeclose_close_file 0")));

  PRINT((_L("e_composeclose_close_std_lib 1")));
  closeStdlib();
  PRINT((_L("e_composeclose_close_std_lib 0")));

  if (handle->file32Duplicate)
      ((RFile64*)handle->file32Duplicate)->Close();
  
  PRINT((_L("e_composeclose_free_handle 1")));
  if (handle)
    {
    mp4free(handle);
    handle = NULL;
    }
  PRINT((_L("e_composeclose_free_handle 0")));  

  if (error)
    return MP4_METADATA_ERROR;
  
  PRINT((_L("e_composeclose 0")));
  return MP4_OK;
}

extern EXPORT_C MP4Err MP4ComposeAddVideoDescription(MP4Handle apihandle,
                                                                  mp4_u32 timescale,
                                                                  mp4_u16 width,
                                                                  mp4_u16 height,
                                                                  mp4_u32 maxbitrate,
                                                                  mp4_u32 avgbitrate)
{
  MP4HandleImp handle = (MP4HandleImp)apihandle;

  if (timescale)
    handle->videoTimeScale = timescale;
  else
    return MP4_ERROR;

  handle->videoWidth = width;
  handle->videoHeight = height;
  handle->videoMaxBitrate = maxbitrate;
  handle->videoAvgBitrate = avgbitrate;

  if ( handle->type & (MP4_TYPE_H263_PROFILE_0 | MP4_TYPE_H263_PROFILE_3) )
      {
      // default H.263 level is 10; it may be overwritten by MP4ComposeWriteVideoDecoderSpecificInfo
      handle->videoLevel = 10;
      }

  /* Write FTYP and media data size & type if meta data flag is set */
  if (handle->flags & MP4_FLAG_METADATALAST)
  {
    if (handle->ftypWritten != MP4TRUE)
    {
      if (writeFTYPAndMDATToFile(handle) < 0)
        return MP4_ERROR;
    }
  }

  return MP4_OK;
}

extern EXPORT_C MP4Err MP4ComposeAddAudioDescription(MP4Handle apihandle,
                                                                  mp4_u32 timescale,
                                                                  mp4_u8 audioFramesPerSample,
                                                                  mp4_u16 modeSet)
{
  MP4HandleImp handle = (MP4HandleImp)apihandle;
  
  PRINT((_L("3GPMP4Lib::MP4ComposeAddAudioDescription in. TimeScale=%d, AudioFrames=%d, ModeSet=%d "), timescale, audioFramesPerSample, modeSet));  
  if (timescale)
  {
    if (timescale > (mp4_u32)0xffff)
    	{
      	return MP4_ERROR;    	
    	}
    handle->audioTimeScale = timescale;
    PRINT((_L("3GPMP4Lib::MP4ComposeAddAudioDescription in. TimeScale set to = %d"), handle->audioTimeScale ));  
  }
  else
    return MP4_ERROR;


  if ((handle->type & MP4_TYPE_AMR_NB) ||
      (handle->type & MP4_TYPE_AMR_WB)) /* Audio is AMR */
  {
    if (audioFramesPerSample)
      handle->audioFramesPerSample = audioFramesPerSample;
    else
      return MP4_ERROR;

    if (modeSet)
      handle->audioModeSet = modeSet;
    else
      return MP4_ERROR;
  }
  else if (handle->type & MP4_TYPE_QCELP_13K) /* Audio is QCELP 13K */
  {
    if (audioFramesPerSample)
      handle->audioFramesPerSample = audioFramesPerSample;
    else
      return MP4_ERROR;
  }
  else /* MPEG AAC audio */
  {
    handle->audioFramesPerSample = 1;
  }

  /* Write FTYP and media data size & type if meta data flag is set */
  if (handle->flags & MP4_FLAG_METADATALAST)
  {
    if (handle->ftypWritten != MP4TRUE)
    {
      if (writeFTYPAndMDATToFile(handle) < 0)
        return MP4_ERROR;
    }
  }
  
  PRINT((_L("3GPMP4Lib::MP4ComposeAddAudioDescription out")));  
  return MP4_OK;
}

extern EXPORT_C MP4Err MP4ComposeWriteVideoFrame(MP4Handle apihandle,
                                                              mp4_u8 *buffer,
                                                              mp4_u32 framesize,
                                                              mp4_u32 duration,
                                                              mp4_bool keyframe)
{
  MP4HandleImp handle = (MP4HandleImp)apihandle;
  PRINT((_L("3GPMP4Lib::MP4ComposeWriteVideoFrame in")));
  if (handle->videoTimeScale == 0)
    return MP4_TIMESCALE_NOT_SET;

  if (framesize == 0)
    return MP4_ERROR;

  handle->videoDuration += duration;
  handle->videoSampleNum++;

  if (updateVideoMetaData(handle, framesize, duration, keyframe) < 0)
    return MP4_ERROR;

  if (handle->flags & MP4_FLAG_METADATALAST)
  {
    if (writeFile(handle, buffer, framesize) < 0)
      return MP4_ERROR;

    handle->mediaDataBytes += framesize;
  }
  else
  {
    if (writeTmpFile(handle, buffer, framesize) < 0)
      return MP4_ERROR;
  }

  PRINT((_L("3GPMP4Lib::MP4ComposeWriteVideoFrame out")));
  return MP4_OK;
}

extern EXPORT_C MP4Err MP4ComposeWriteAudioFrames(MP4Handle apihandle,
                                                               mp4_u8 *buffer,
                                                               mp4_u32 bytestowrite,
                                                               mp4_u32 numberofframes,
                                                               mp4_u32 duration)
{
  MP4HandleImp handle = (MP4HandleImp)apihandle;
  PRINT((_L("3GPMP4Lib::MP4ComposeWriteAudioFrames in")));

  if ( handle->audioTimeScale == 0)	
  	  {
  	  PRINT((_L("3GPMP4Lib::MP4ComposeWriteAudioFrames Error 14 - AudioTimeScale is 0")));
      return MP4_TIMESCALE_NOT_SET;
	  }
	  
  PRINT((_L("3GPMP4Lib::MP4ComposeWriteAudioFrames Audio Timescale ok")));	  

  if (bytestowrite == 0)
    return MP4_ERROR;

  handle->audioDuration += duration;
  handle->audioFrameCount += numberofframes;
  handle->audioMediaDataSize += bytestowrite;

  PRINT((_L("3GPMP4Lib::MP4ComposeWriteAudioFrames updating audio metadata")));	  
  
  if (updateAudioMetaData(handle, bytestowrite, duration) < 0)
    return MP4_ERROR;

  if (handle->flags & MP4_FLAG_METADATALAST)
  {
	PRINT((_L("3GPMP4Lib::MP4ComposeWriteAudioFrames writing to file")));	    
    if (writeFile(handle, buffer, bytestowrite) < 0)
      return MP4_ERROR;

    handle->mediaDataBytes += bytestowrite;
  }
  else
  {
	PRINT((_L("3GPMP4Lib::MP4ComposeWriteAudioFrames writing to temp file")));	      
    if (writeTmpFile(handle, buffer, bytestowrite) < 0)
      return MP4_ERROR;
  }
  
  PRINT((_L("3GPMP4Lib::MP4ComposeWriteAudioFrames out")));
  return MP4_OK;
}

extern EXPORT_C MP4Err MP4ComposeWriteVideoDecoderSpecificInfo(MP4Handle apihandle,
                                                                            mp4_u8 *info,
                                                                            mp4_u32 infosize)
{
  MP4HandleImp handle = (MP4HandleImp)apihandle;

  if ( (handle->type & MP4_TYPE_MPEG4_VIDEO) || containsAvcVideo( handle->type ) )
      {
      if ((handle->videoDecSpecificInfo = (mp4_u8 *)mp4malloc(infosize)) == NULL)
        return MP4_ERROR;

      mp4memcpy(handle->videoDecSpecificInfo, info, infosize);
      handle->videoDecSpecificInfoSize = infosize;
      }
  else
      {
      // H.263, save level only
      if ( *info >= 10 && *info < 100 )
          {
          handle->videoLevel = *info;
          }
      }
  return MP4_OK;
}

extern EXPORT_C MP4Err MP4ComposeWriteAudioDecoderSpecificInfo(MP4Handle apihandle,
                                                                            mp4_u8 *info,
                                                                            mp4_u32 infosize)
{
  MP4HandleImp handle = (MP4HandleImp)apihandle;

  if ((handle->audioDecSpecificInfo = (mp4_u8 *)mp4malloc(infosize)) == NULL)
    return MP4_ERROR;

  mp4memcpy(handle->audioDecSpecificInfo, info, infosize);
  handle->audioDecSpecificInfoSize = infosize;

  return MP4_OK;
}

extern EXPORT_C MP4Err MP4ComposeSetFlags(MP4Handle apihandle,
                                                       mp4_u32 flags)
{
  MP4HandleImp handle = (MP4HandleImp)apihandle;
  handle->flags |= flags;

  if (handle->flags & MP4_FLAG_METADATALAST)
  {
    if (handle->tmpfile)
    {
      closeTmpFile(handle);
      deleteTmpFile(handle);
    }
  }

  if (handle->flags & MP4_FLAG_LONGCLIP)
  {
    // Open temporary files
    MP4Err error = MP4_OK;
    error = initMetaDataFiles(handle);
    if ( error == MP4_OUT_OF_MEMORY )
	    {
	    return MP4_OUT_OF_MEMORY;
	    }
 	else if ( error != MP4_OK )
	 	{
		return MP4_ERROR;
	 	}
  }

  handle->generate3G2 = MP4FALSE;
  handle->generateMP4 = MP4FALSE;   

  if (handle->flags & MP4_FLAG_GENERATE_3G2) // 3G2
  {
    // Generate 3G2 file.
    handle->generate3G2 = MP4TRUE;
    handle->generateMP4 = MP4FALSE;      
  }
  else if ( handle->flags & MP4_FLAG_GENERATE_MP4 ) // MP4
	  {
	  // if at least ONE audio/video codec is specified
	  if (handle->type != MP4_TYPE_NONE)
		  {
	      /* Check if a 3GPP2 codec is being used */
	      if ( handle->type & MP4_TYPE_QCELP_13K )
	    	  {
	    	  handle->generate3G2 = MP4TRUE;
	    	  handle->generateMP4 = MP4FALSE;   
	    	  }
	      else 
	    	  {
			  // types other than MPEG-4 AUDIO & VIDEO  
			  mp4_u32 type = handle->type >> 2;	
		  
			  // Check if a MPEG4 codec is being used
			  if (type == MP4_TYPE_NONE)
				  {
				  // if ONLY MPEG4 Video and/or Audio codec is used, generate MP4 file.
				  handle->generateMP4 = MP4TRUE;      
				  handle->generate3G2 = MP4FALSE;
				  }	  
			  else
				  {
				  // ignoring both MPEG-4 audio and video, check again if only AVC codecs are 
				  // used
				  type <<= 2;			  
				  if ( isAvcVideo(type) )
					  {
					  // generate MP4 file
					  handle->generateMP4 = MP4TRUE;      
					  handle->generate3G2 = MP4FALSE;
					  }
				  }
	    	  }
		  }
	  }
  else // 3GP
  {
      /* Check if a 3GPP2 codec is being used */
      if ( handle->type & MP4_TYPE_QCELP_13K )
      {
        handle->generate3G2 = MP4TRUE;
        handle->generateMP4 = MP4FALSE;   
      }
      // use defaults -> 3GP
  }

  if ( (handle->flags & MP4_FLAG_LARGEFILEBUFFER) && !(handle->bufferWrite) )
      {
      TInt bufferSizeError = KErrNone;
      bufferSizeError = handle->filewriter->SetOutputBufferSize( CFileWriter::EBufferSizeLarge, apihandle );
      if ( bufferSizeError == KErrNoMemory )
      	{
      	return MP4_OUT_OF_MEMORY;
      	}
	  else if ( bufferSizeError != KErrNone )
        {
        return MP4_ERROR;
        }
      }
  return MP4_OK;
}

extern EXPORT_C MP4Err MP4ComposeSetQCELPStorageMode(MP4Handle apihandle, mp4_u8 qcelpStorageMode)
{
    MP4HandleImp handle = (MP4HandleImp)apihandle;

    if (qcelpStorageMode)
      handle->qcelpStoredAsMPEGAudio = MP4TRUE;
    else
      handle->qcelpStoredAsMPEGAudio = MP4FALSE;

    return MP4_OK;

}

extern EXPORT_C MP4Err MP4ComposeSetVideoClipProperties(MP4Handle apihandle, const TVideoClipProperties& aVideoClipProperties)
{
    MP4HandleImp handle = (MP4HandleImp)apihandle;

    if ( !handle )
    {
        return MP4_ERROR;
    }
    else
    {
        if ( aVideoClipProperties.iH263Level )
        {
            handle->videoLevel = aVideoClipProperties.iH263Level;
        }
        return MP4_OK;
    }
}

extern EXPORT_C MP4Err MP4ComposeSetUserDataAtom(MP4Handle apihandle, 
                                                            mp4_u8& udtaLocation,
                                                            mp4_u8* buffer,
                                                            mp4_u32& bufferSize )
    {
    userDataAtom* udta = NULL;
    MP4HandleImp handle = (MP4HandleImp)apihandle;

    if (!handle)
        return MP4_ERROR;

    if ( buffer == NULL )
        {
        return MP4_ERROR;
        }
    if ( !bufferSize )
        {
        return MP4_ERROR;
        }

    // Check which UDTA atom to use
    switch ( udtaLocation )
        {
        case MP4_UDTA_MOOV:
            {
            if ( handle->moovUDTA == NULL )
                {
                handle->moovUDTA = (userDataAtom *)mp4malloc(sizeof(userDataAtom));
                }
            udta = handle->moovUDTA;
            break;
            }
        case MP4_UDTA_VIDEOTRAK:
            {
            if ( handle->videoUDTA == NULL )
                {
                handle->videoUDTA = (userDataAtom *)mp4malloc(sizeof(userDataAtom));
                }
            udta = handle->videoUDTA;
            break;
            }
        case MP4_UDTA_AUDIOTRAK:
            {
            if ( handle->audioUDTA == NULL )
                {
                handle->audioUDTA = (userDataAtom *)mp4malloc(sizeof(userDataAtom));
                }
            udta = handle->audioUDTA;
            break;
            }
       default:
            {
            return MP4_INVALID_TYPE;
            }
        }
        
    if ( udta == NULL )
        {
        return MP4_OUT_OF_MEMORY;
        }

    // CHECK if there is old data in UDTA
    if ( udta->contentdata && udta->atomcontentsize )
        {
        mp4_u8* temp;
        if ((temp = (mp4_u8 *)mp4malloc(bufferSize + udta->atomcontentsize)) == NULL)
            {
            return MP4_OUT_OF_MEMORY;
            }
        mp4memcpy(temp, udta->contentdata, udta->atomcontentsize);
        mp4memcpy(temp+udta->atomcontentsize, buffer, bufferSize);
        mp4free(udta->contentdata);
        udta->contentdata = temp;
        udta->atomcontentsize += bufferSize;
        }
    else
        {
        if ((udta->contentdata = (mp4_u8 *)mp4malloc(bufferSize)) == NULL)
            return MP4_OUT_OF_MEMORY;        
        // Copy data from buffer to atom contentdata        
        mp4memcpy(udta->contentdata, buffer, bufferSize);                
        udta->atomcontentsize = bufferSize;
        }

    return MP4_OK;
    }
	
	
extern EXPORT_C MP4Err MP4SetCustomFileBufferSizes( MP4Handle apihandle, 
                                                               mp4_u32 mediaWriteBufferSize,
                                                               mp4_u32 writeBufferMaxCount,
                                                               mp4_u32 readBufferSize )
    {
    MP4HandleImp handle = (MP4HandleImp)apihandle;

    if (!handle)
        return MP4_ERROR;
    
    // If no specific file size is given we try to use an 'optimal' buffer size.
    if (readBufferSize == 0)
    	{
    	readBufferSize = RecommendedBufferSize(handle);
		}

	if (readBufferSize > handle->readBufferSize)
        {
        handle->readBufferSize = readBufferSize;
        if (handle->diskReadBuf)
            {
            mp4free(handle->diskReadBuf);
            handle->diskReadBuf = NULL;
            if ((handle->diskReadBuf = (mp4_u8 *)mp4malloc(handle->readBufferSize)) == NULL)
                {
                return MP4_OUT_OF_MEMORY;
                }
            }
        }
        
    // Media Write buffer size
    if ( (mediaWriteBufferSize) &&  
         (mediaWriteBufferSize != handle->mediaWriteBufferSize) &&
         (!(handle->bufferWrite)) )
        {
        handle->mediaWriteBufferSize = mediaWriteBufferSize;
        if ( (handle->filewriter) )
          {
          if ( handle->filewriter->SetOutputBufferSize( CFileWriter::EBufferSizeCustom, apihandle ) < 0 )
              {
              return MP4_ERROR;
              }
          }
        }
        
    // Write Buffer Max Count change
    if ( (writeBufferMaxCount) &&  
         (writeBufferMaxCount != handle->writeBufferMaxCount) &&
         !(handle->bufferWrite) )
        {
        if ( writeBufferMaxCount >= 6 ) // min number of buffers is 4, +1 for soft limit, +1 for hardlimit = 6
            {
            handle->writeBufferMaxCount = writeBufferMaxCount;
            if ( (handle->filewriter) )
              {
              handle->filewriter->SetOutputBufferCount( apihandle );
              }
            }
        else
            {
            return MP4_ERROR;
            }
        }
    return MP4_OK;
    }

extern EXPORT_C MP4Err MP4ComposeWriteNextVideoFrameDependencies(MP4Handle apihandle, mp4_u8 aDependsOn, mp4_u8 aIsDependentOn, mp4_u8 aHasRedundancy)
{
	MP4HandleImp handle = (MP4HandleImp)apihandle;
	
	if( (aDependsOn > 2) || (aIsDependentOn > 2) || (aHasRedundancy > 2) )
	{
		return MP4_ERROR;
	}
	
	if (updateVideoDependencyMetaData(handle, aDependsOn, aIsDependentOn, aHasRedundancy) < 0)
	{
		return MP4_ERROR;
	}
	
	return MP4_OK;
}

extern EXPORT_C MP4Err MP4ComposeSetTempFileRemoverObserver(
                                        MP4Handle *apihandle,
                                        M3GPMP4LibAsyncTempFileRemoverObserver *aObserver)
{
  PRINT((_L("3GPMP4Lib::MP4ComposeSetTempFileRemoverObserver in")));

  MP4Err error = MP4_OK;
  MP4HandleImp* handle = (MP4HandleStruct **)apihandle;

  if ( handle != NULL && *handle != NULL )
      {
      (*handle)->tempFileRemoverObserver = aObserver;
      }
  else
      {
      error = MP4_ERROR;
      }


  PRINT((_L("3GPMP4Lib::MP4ComposeSetTempFileRemoverObserver out error=%d"), error));
  return error;
}
// End of File
