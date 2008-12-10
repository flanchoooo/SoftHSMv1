/* $Id$ */

/*
 * Copyright (c) 2008 .SE (The Internet Infrastructure Foundation).
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/************************************************************
*
* This class defines a session
* It holds the current state of the session
*
************************************************************/

#include "main.h"

SoftSession::SoftSession(int rwSession) {
  pApplication = NULL_PTR;
  Notify = NULL_PTR;

  if(rwSession == CKF_RW_SESSION) {
    readWrite = true;
  } else {
    readWrite = false;
  }

  findAnchor = NULL_PTR;
  findCurrent = NULL_PTR;
  findInitialized = false;

  digestPipe = NULL_PTR;
  digestSize = 0;
  digestInitialized = false;

  pkSigner = NULL_PTR;
  signSinglePart = false;
  signSize = 0;
  signInitialized = false;

  pkVerifier = NULL_PTR;
  verifySinglePart = false;
  verifySize = 0;
  verifyInitialized = false;

  keyStore = new SoftKeyStore();

  rng = new AutoSeeded_RNG();

  db = new SoftDatabase();
}

SoftSession::~SoftSession() {
  pApplication = NULL_PTR;
  Notify = NULL_PTR;

  if(findAnchor != NULL_PTR) {
    delete findAnchor;
    findAnchor = NULL_PTR;
  }

  findCurrent = NULL_PTR;

  if(digestPipe != NULL_PTR) {
    delete digestPipe;
    digestPipe = NULL_PTR;
  }

  if(pkSigner != NULL_PTR) {
    delete pkSigner;
    pkSigner = NULL_PTR;
  }

  if(pkVerifier != NULL_PTR) {
    delete pkVerifier;
    pkVerifier = NULL_PTR;
  }

  if(keyStore != NULL_PTR) {
    delete keyStore;
    keyStore = NULL_PTR;
  }

  if(rng != NULL_PTR) {
    delete rng;
  }

  if(db != NULL_PTR) {
    delete db;
    db = NULL_PTR;
  }
}

bool SoftSession::isReadWrite() {
  return readWrite;
}

Private_Key* SoftSession::getKey(SoftObject *object, CK_OBJECT_HANDLE hKey) {
  Private_Key* tmpKey = keyStore->getKey(hKey);

  if(tmpKey == NULL_PTR) {
    // Clone the key
    IF_Scheme_PrivateKey *ifKeyPriv = dynamic_cast<IF_Scheme_PrivateKey*>(object->key);
    tmpKey = new RSA_PrivateKey(*rng, ifKeyPriv->get_p(),
       ifKeyPriv->get_q(), ifKeyPriv->get_e(), ifKeyPriv->get_d(), ifKeyPriv->get_n());
    keyStore->addKey(hKey, tmpKey);
  }

  return tmpKey;
}
