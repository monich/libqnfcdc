/*
 * Copyright (C) 2024-2025 Slava Monich <slava@monich.com>
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer
 *     in the documentation and/or other materials provided with the
 *     distribution.
 *
 *  3. Neither the names of the copyright holders nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
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

#include "NfcTech.h"

// This requires libgnfcdc 1.1.0 or newer
#ifdef NFCDC_VERSION_1_1_0

// ==========================================================================
// NfcTech::Private
// ==========================================================================

class NfcTech::Private
{
public:
    Private();
    ~Private();

    bool needRequest() const;
    void updateRequest();

public:
    NfcDaemonClient* iDaemon;
    NfcTechRequest* iRequest;
    NFC_TECH iAllowTechs;
    NFC_TECH iDisallowTechs;
    bool iActive;
};

NfcTech::Private::Private() :
    iDaemon(nfc_daemon_client_new()),
    iRequest(Q_NULLPTR),
    iAllowTechs(NFC_TECH_NONE),
    iDisallowTechs(NFC_TECH_NONE),
    iActive(false)
{
}

NfcTech::Private::~Private()
{
    nfc_tech_request_free(iRequest);
    nfc_daemon_client_unref(iDaemon);
}

inline
bool
NfcTech::Private::needRequest() const
{
    return iActive && (iAllowTechs || iDisallowTechs);
}

void
NfcTech::Private::updateRequest()
{
    if (needRequest()) {
        // Create new request before disposing of the old one, so that
        // RequestTechs D-Bus call gets issued before ReleaseTechs.
        // Under certain circumstances, it could be more efficient on
        // the nfcd side.
        NfcTechRequest* req = nfc_tech_request_new(iDaemon, iAllowTechs,
            iDisallowTechs);
        nfc_tech_request_free(iRequest);
        iRequest = req;
    } else if (iRequest) {
        nfc_tech_request_free(iRequest);
        iRequest = Q_NULLPTR;
    }
}

// ==========================================================================
// NfcTechRequest
// ==========================================================================

NfcTech::NfcTech(
    QObject* aParent) :
    QObject(aParent),
    iPrivate(new Private)
{
}

NfcTech::~NfcTech()
{
    delete iPrivate;
}

bool
NfcTech::active() const
{
    return iPrivate->iActive;
}

void
NfcTech::setActive(
    bool aActive)
{
    if (iPrivate->iActive != aActive) {
        iPrivate->iActive = aActive;
        iPrivate->updateRequest();
        Q_EMIT activeChanged();
    }
}

NfcSystem::Tech
NfcTech::allowTechs() const
{
    return (NfcSystem::Tech) iPrivate->iAllowTechs;
}

void
NfcTech::setAllowTechs(
    int aTechs)
{
    if (iPrivate->iAllowTechs != (NFC_TECH) aTechs) {
        iPrivate->iAllowTechs = (NFC_TECH) aTechs;
        iPrivate->updateRequest();
        Q_EMIT allowTechsChanged();
    }
}

NfcSystem::Tech
NfcTech::disallowTechs() const
{
    return (NfcSystem::Tech) iPrivate->iDisallowTechs;
}

void
NfcTech::setDisallowTechs(
    int aTechs)
{
    if (iPrivate->iDisallowTechs != (NFC_TECH) aTechs) {
        iPrivate->iDisallowTechs = (NFC_TECH) aTechs;
        iPrivate->updateRequest();
        Q_EMIT disallowTechsChanged();
    }
}

#else // NFCDC_VERSION_1_1_0
#pragma message("Please use libgnfcdc 1.1.0 or newer")
#endif
