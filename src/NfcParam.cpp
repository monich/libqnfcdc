/*
 * Copyright (C) 2025 Slava Monich <slava@monich.com>
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

#include <nfcdc_default_adapter.h>

#include "NfcParam.h"

// This requires libgnfcdc 1.2.0 or newer
#ifdef NFCDC_VERSION_1_2_0

// ==========================================================================
// NfcParam::Private
// ==========================================================================

class NfcParam::Private
{
public:
    Private();
    ~Private();

    bool needRequest() const;
    void updateRequest();

public:
    NfcDefaultAdapter* iAdapter;
    NfcDefaultAdapterParamReq* iRequest;
    NfcAdapterParam* iT4Ndef;
    NfcAdapterParam* iLaNfcid1;
    QByteArray iLaNfcid1Bytes;
    bool iReset;
    bool iActive;
};

NfcParam::Private::Private() :
    iAdapter(nfc_default_adapter_new()),
    iRequest(Q_NULLPTR),
    iT4Ndef(Q_NULLPTR),
    iLaNfcid1(Q_NULLPTR),
    iReset(false),
    iActive(false)
{
}

NfcParam::Private::~Private()
{
    delete iT4Ndef;
    delete iLaNfcid1;
    nfc_default_adapter_param_req_free(iRequest);
    nfc_default_adapter_unref(iAdapter);
}

inline
bool
NfcParam::Private::needRequest() const
{
    return iActive && (iT4Ndef || iLaNfcid1);
}

void
NfcParam::Private::updateRequest()
{
    if (needRequest()) {
        int i = 0;
        NfcAdapterParamPtrC params[3];

        if (iT4Ndef) params[i++] = iT4Ndef;
        if (iLaNfcid1) params[i++] = iLaNfcid1;
        params[i] = Q_NULLPTR;
        nfc_default_adapter_param_req_free(iRequest);
        iRequest = nfc_default_adapter_param_req_new(iAdapter, iReset, params);
    } else if (iRequest) {
        nfc_default_adapter_param_req_free(iRequest);
        iRequest = Q_NULLPTR;
    }
}

// ==========================================================================
// NfcParam
// ==========================================================================

NfcParam::NfcParam(
    QObject* aParent) :
    QObject(aParent),
    iPrivate(new Private)
{
}

NfcParam::~NfcParam()
{
    delete iPrivate;
}

bool
NfcParam::active() const
{
    return iPrivate->iActive;
}

void
NfcParam::setActive(
    bool aActive)
{
    if (iPrivate->iActive != aActive) {
        iPrivate->iActive = aActive;
        iPrivate->updateRequest();
        Q_EMIT activeChanged();
    }
}

bool
NfcParam::reset() const
{
    return iPrivate->iReset;
}

void
NfcParam::setReset(
    bool aReset)
{
    if (iPrivate->iReset != aReset) {
        iPrivate->iReset = aReset;
        iPrivate->updateRequest();
        Q_EMIT resetChanged();
    }
}

bool
NfcParam::t4Ndef() const
{
    return iPrivate->iT4Ndef ? iPrivate->iT4Ndef->value.b : true;
}

void
NfcParam::setT4Ndef(
    bool aT4Ndef)
{
    if (!iPrivate->iT4Ndef || iPrivate->iT4Ndef->value.b != aT4Ndef) {
        if (!iPrivate->iT4Ndef) {
            iPrivate->iT4Ndef = new NfcAdapterParam;
            memset(iPrivate->iT4Ndef, 0, sizeof(NfcAdapterParam));
            iPrivate->iT4Ndef->key = NFC_ADAPTER_PARAM_KEY_T4_NDEF;
        }
        iPrivate->iT4Ndef->value.b = aT4Ndef;
        iPrivate->updateRequest();
        Q_EMIT t4NdefChanged();
    }
}

QString
NfcParam::laNfcid1() const
{
    if (iPrivate->iLaNfcid1) {
        const GUtilData* nfcid1 = &iPrivate->iLaNfcid1->value.data;
        return QByteArray((char*)nfcid1->bytes, nfcid1->size).toHex();
    } else {
        return QString();
    }
}

void
NfcParam::setLaNfcid1(
    QString aNfcid1)
{
    const QByteArray bytes(QByteArray::fromHex(aNfcid1.toLatin1()));

    if (!iPrivate->iLaNfcid1 || bytes != iPrivate->iLaNfcid1Bytes) {
        if (!iPrivate->iLaNfcid1) {
            iPrivate->iLaNfcid1 = new NfcAdapterParam;
            memset(iPrivate->iLaNfcid1, 0, sizeof(NfcAdapterParam));
            iPrivate->iLaNfcid1->key = NFC_ADAPTER_PARAM_KEY_LA_NFCID1;
        }
        iPrivate->iLaNfcid1Bytes = bytes;
        iPrivate->iLaNfcid1->value.data.size = bytes.size();
        iPrivate->iLaNfcid1->value.data.bytes = (uchar*)
            iPrivate->iLaNfcid1Bytes.constData();
        iPrivate->updateRequest();
        Q_EMIT laNfcid1Changed();
    }
}

#else // NFCDC_VERSION_1_2_0
#pragma message("Please use libgnfcdc 1.2.0 or newer")
#endif
