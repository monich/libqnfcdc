/*
 * Copyright (C) 2021-2024 Slava Monich <slava@monich.com>
 * Copyright (C) 2021 Jolla Ltd.
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

#include <nfcdc_peer.h>

#include "NfcPeer.h"

#include "Debug.h"

// ==========================================================================
// NfcPeer::Private
// ==========================================================================

class NfcPeer::Private
{
public:
    Private(NfcPeer* aParent);
    ~Private();

    void setPath(const char*);
    void emitPropertySignal(NFC_PEER_PROPERTY);

    static const char* SIGNAL_NAME[];
    static void propertyChanged(NfcPeerClient*, NFC_PEER_PROPERTY, void*);

public:
    NfcPeer* iParent;
    NfcPeerClient* iPeer;
    gulong iPeerEventId[3]; // Must match number of non-NULLs below:
};

const char* NfcPeer::Private::SIGNAL_NAME[] = {
    Q_NULLPTR,          // NFC_PEER_PROPERTY_ANY
    "validChanged",     // NFC_PEER_PROPERTY_VALID
    "presentChanged",   // NFC_PEER_PROPERTY_PRESENT
    "wksChanged",       // NFC_PEER_PROPERTY_WKS
    // Remember to update iPeerEventId count when adding new handlers!
};

NfcPeer::Private::Private(
    NfcPeer* aParent) :
    iParent(aParent),
    iPeer(Q_NULLPTR)
{
    memset(iPeerEventId, 0, sizeof(iPeerEventId));
    Q_STATIC_ASSERT(G_N_ELEMENTS(SIGNAL_NAME) == NFC_PEER_PROPERTY_COUNT);
}

NfcPeer::Private::~Private()
{
    nfc_peer_client_remove_all_handlers(iPeer, iPeerEventId);
    nfc_peer_client_unref(iPeer);
}

inline
void
NfcPeer::Private::emitPropertySignal(
    NFC_PEER_PROPERTY aProperty)
{
    // Qt signals should be signalled from the Qt event loop
    // See https://bugreports.qt.io/browse/QTBUG-18434 for details
    QMetaObject::invokeMethod(iParent, SIGNAL_NAME[aProperty],
        Qt::QueuedConnection);
}

void
NfcPeer::Private::setPath(
    const char* aPath)
{
    bool changed[NFC_PEER_PROPERTY_COUNT];
    NFC_PEER_PROPERTY p;
    gboolean valid = FALSE;
    gboolean present = FALSE;
    guint wks = 0;

    memset(changed, 0, sizeof(changed));
    if (iPeer) {
        valid = iPeer->valid;
        present = iPeer->present;
        wks = iPeer->wks;
        nfc_peer_client_remove_all_handlers(iPeer, iPeerEventId);
        nfc_peer_client_unref(iPeer);
        iPeer = Q_NULLPTR;
    }

    if (aPath) {
        int k;

        iPeer = nfc_peer_client_new(aPath);
        for (p = NFC_PEER_PROPERTY_VALID, k = 0;
             p < NFC_PEER_PROPERTY_COUNT;
             p = NFC_PEER_PROPERTY(p+1)) {
            if (SIGNAL_NAME[p]) {
                iPeerEventId[k++] =
                    nfc_peer_client_add_property_handler(iPeer, p,
                        propertyChanged, this);
            }
        }
        HASSERT(k == G_N_ELEMENTS(iPeerEventId));
        // Signal the changes
        changed[NFC_PEER_PROPERTY_VALID] = (valid != iPeer->valid);
        changed[NFC_PEER_PROPERTY_PRESENT] = (present != iPeer->present);
        changed[NFC_PEER_PROPERTY_WKS] = (wks != iPeer->wks);
    } else {
        changed[NFC_PEER_PROPERTY_VALID] = (valid != FALSE);
        changed[NFC_PEER_PROPERTY_PRESENT] = (present != FALSE);
        changed[NFC_PEER_PROPERTY_WKS] = (wks != 0);
    }

    for (p = NFC_PEER_PROPERTY_VALID;
         p < NFC_PEER_PROPERTY_COUNT;
         p = NFC_PEER_PROPERTY(p+1)) {
        if (changed[p]) {
            emitPropertySignal(p);
        }
    }
}

/* static */
void
NfcPeer::Private::propertyChanged(
    NfcPeerClient*,
    NFC_PEER_PROPERTY aProperty,
    void* aPrivate)
{
    ((Private*) aPrivate)->emitPropertySignal(aProperty);
}

// ==========================================================================
// NfcPeer
// ==========================================================================

NfcPeer::NfcPeer(
    QObject* aParent) :
    QObject(aParent),
    iPrivate(new Private(this))
{
}

NfcPeer::~NfcPeer()
{
    delete iPrivate;
}

void
NfcPeer::setPath(
    QString aPath)
{
    const QString currentPath(path());

    if (currentPath != aPath) {
        HDEBUG(aPath);
        if (aPath.isEmpty()) {
            iPrivate->setPath(Q_NULLPTR);
        } else {
            QByteArray bytes(aPath.toLatin1());
            iPrivate->setPath(bytes.constData());
        }
        Q_EMIT pathChanged();
    }
}

QString
NfcPeer::path() const
{
    return iPrivate->iPeer ? QString(iPrivate->iPeer->path) : QString();
}

bool
NfcPeer::valid() const
{
    return iPrivate->iPeer && iPrivate->iPeer->valid;
}

bool
NfcPeer::present() const
{
    return iPrivate->iPeer && iPrivate->iPeer->present;
}

uint
NfcPeer::wks() const
{
    return iPrivate->iPeer ? iPrivate->iPeer->wks : 0;
}
