/*
 * Copyright (C) 2021 Jolla Ltd.
 * Copyright (C) 2021 Slava Monich <slava@monich.com>
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *   3. Neither the names of the copyright holders nor the names of its
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation
 * are those of the authors and should not be interpreted as representing
 * any official policies, either expressed or implied.
 */

#include <nfcdc_daemon.h>

#include "NfcMode.h"

#include "Debug.h"

// ==========================================================================
// NfcMode::Private
// ==========================================================================

class NfcMode::Private {
public:
    Private();
    ~Private();

    bool needRequest() const;
    void updateRequest();

public:
    NfcDaemonClient* iDaemon;
    NfcModeRequest* iRequest;
    NfcSystem::Mode iEnableModes;
    NfcSystem::Mode iDisableModes;
    bool iActive;
};

NfcMode::Private::Private() :
    iDaemon(nfc_daemon_client_new()),
    iRequest(Q_NULLPTR),
    iEnableModes(NfcSystem::None),
    iDisableModes(NfcSystem::None),
    iActive(false)
{
}

NfcMode::Private::~Private()
{
    nfc_mode_request_free(iRequest);
    nfc_daemon_client_unref(iDaemon);
}

inline bool NfcMode::Private::needRequest() const
{
    return iActive && (iEnableModes || iDisableModes);
}

void NfcMode::Private::updateRequest()
{
    if (needRequest()) {
        nfc_mode_request_free(iRequest);
        iRequest = nfc_mode_request_new(iDaemon, (NFC_MODE)iEnableModes,
            (NFC_MODE)iDisableModes);
    } else if (iRequest) {
        nfc_mode_request_free(iRequest);
        iRequest = Q_NULLPTR;
    }
}

// ==========================================================================
// NfcModeRequest
// ==========================================================================

NfcMode::NfcMode(QObject* aParent) :
    QObject(aParent),
    iPrivate(new Private)
{
}

NfcMode::~NfcMode()
{
    delete iPrivate;
}

bool NfcMode::active() const
{
    return iPrivate->iActive;
}

void NfcMode::setActive(bool aActive)
{
    if (iPrivate->iActive != aActive) {
        iPrivate->iActive = aActive;
        iPrivate->updateRequest();
        Q_EMIT activeChanged();
    }
}

NfcSystem::Mode NfcMode::enableModes() const
{
    return iPrivate->iEnableModes;
}

void NfcMode::setEnableModes(int aModes)
{
    if (iPrivate->iEnableModes != (NfcSystem::Mode)aModes) {
        const bool didNeedRequest = iPrivate->needRequest();
        iPrivate->iEnableModes = (NfcSystem::Mode)aModes;
        if (didNeedRequest != iPrivate->needRequest()) {
            iPrivate->updateRequest();
        }
        Q_EMIT enableModesChanged();
    }
}

NfcSystem::Mode NfcMode::disableModes() const
{
    return iPrivate->iDisableModes;
}

void NfcMode::setDisableModes(int aModes)
{
    if (iPrivate->iDisableModes != (NfcSystem::Mode)aModes) {
        const bool didNeedRequest = iPrivate->needRequest();
        iPrivate->iDisableModes = (NfcSystem::Mode)aModes;
        if (didNeedRequest != iPrivate->needRequest()) {
            iPrivate->updateRequest();
        }
        Q_EMIT disableModesChanged();
    }
}
