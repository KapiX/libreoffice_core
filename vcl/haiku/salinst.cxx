/*
 * Copyright 2015 Your Name <your@email.address>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#include "haiku/salinst.h"

#include <stdlib.h>
#include <unistd.h>

#define LOGI

void InitSalData()   {}
void DeInitSalData() {}
void InitSalMain()   {}

void SalAbort( const OUString& rErrorText, bool bDumpCore )
{
    OUString aError( rErrorText );
    if( aError.isEmpty() )
        aError = "Unknown application error";
    LOGI("%s", OUStringToOString(rErrorText, osl_getThreadTextEncoding()).getStr() );

    LOGI("SalAbort: '%s'",
         OUStringToOString(aError, RTL_TEXTENCODING_ASCII_US).getStr());
    if( bDumpCore )
        abort();
    else
        _exit(1);
}

const OUString& SalGetDesktopEnvironment()
{
    static OUString aEnv( "haiku" );
    return aEnv;
}

// This is our main entry point:
SalInstance *CreateSalInstance()
{
    /*LOGI("Android: CreateSalInstance!");
    AndroidSalInstance* pInstance = new AndroidSalInstance( new SalYieldMutex() );
    new AndroidSalData( pInstance );
    pInstance->AcquireYieldMutex(1);
    return pInstance;*/
    return NULL;
}

void DestroySalInstance( SalInstance *pInst )
{
    /*pInst->ReleaseYieldMutex();
    delete pInst;*/
}
