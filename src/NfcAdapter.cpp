/*
 * Copyright (C) 2019-2024 Slava Monich <slava@monich.com>
 * Copyright (C) 2019-2021 Jolla Ltd.
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

#include "NfcAdapter.h"

#include "Debug.h"

// ==========================================================================
// NfcAdapter::Private
// ==========================================================================

class NfcAdapter::Private
{
public:
    Private(NfcAdapter* aParent);
    ~Private();

    static const char* SIGNAL_NAME[];
    static void propertyChanged(NfcDefaultAdapter*, NFC_DEFAULT_ADAPTER_PROPERTY, void*);

public:
    NfcDefaultAdapter* iAdapter;
    gulong iAdapterEventId[11];  // Must not be less than the number of non-NULLs:
};

const char* NfcAdapter::Private::SIGNAL_NAME[] = {
    Q_NULLPTR,                // NFC_DEFAULT_ADAPTER_PROPERTY_ANY
    "presentChanged",         // NFC_DEFAULT_ADAPTER_PROPERTY_ADAPTER
    "enabledChanged",         // NFC_DEFAULT_ADAPTER_PROPERTY_ENABLED
    "poweredChanged",         // NFC_DEFAULT_ADAPTER_PROPERTY_POWERED
    "supportedModesChanged",  // NFC_DEFAULT_ADAPTER_PROPERTY_SUPPORTED_MODES
    "modeChanged",            // NFC_DEFAULT_ADAPTER_PROPERTY_MODE
    "targetPresentChanged",   // NFC_DEFAULT_ADAPTER_PROPERTY_TARGET_PRESENT
    "tagPathChanged",         // NFC_DEFAULT_ADAPTER_PROPERTY_TAGS
    "validChanged",           // NFC_DEFAULT_ADAPTER_PROPERTY_VALID
    "peerPathChanged",        // NFC_DEFAULT_ADAPTER_PROPERTY_PEERS
#ifdef NFCDC_VERSION_1_1_0
    "hostPathChanged",        // NFC_DEFAULT_ADAPTER_PROPERTY_HOSTS
    "supportedTechsChanged",  // NFC_DEFAULT_ADAPTER_PROPERTY_SUPPORTED_TECHS
#endif
    // Remember to update iAdapterEventId count when adding new handlers!
};

NfcAdapter::Private::Private(
    NfcAdapter* aParent) :
    iAdapter(nfc_default_adapter_new())
{
    Q_STATIC_ASSERT(G_N_ELEMENTS(SIGNAL_NAME) ==
        NFC_DEFAULT_ADAPTER_PROPERTY_COUNT);
    uint k = 0;
    for (int i = 0; i < NFC_DEFAULT_ADAPTER_PROPERTY_COUNT; i++) {
        if (SIGNAL_NAME[i]) {
            iAdapterEventId[k++] =
                nfc_default_adapter_add_property_handler(iAdapter,
                    (NFC_DEFAULT_ADAPTER_PROPERTY)i, propertyChanged,
                    aParent);
        }
    }
    HASSERT(k <= G_N_ELEMENTS(iAdapterEventId));
    while (k < G_N_ELEMENTS(iAdapterEventId)) {
        iAdapterEventId[k++] = 0;
    }
}

NfcAdapter::Private::~Private()
{
    nfc_default_adapter_remove_all_handlers(iAdapter, iAdapterEventId);
    nfc_default_adapter_unref(iAdapter);
}

/* static */
void
NfcAdapter::Private::propertyChanged(
    NfcDefaultAdapter*,
    NFC_DEFAULT_ADAPTER_PROPERTY aProperty,
    void* aTarget)
{
    // Qt signals should be signalled from the Qt event loop
    // See https://bugreports.qt.io/browse/QTBUG-18434 for details
    QMetaObject::invokeMethod((QObject*)aTarget, SIGNAL_NAME[aProperty],
        Qt::QueuedConnection);
}

// ==========================================================================
// NfcAdapter
// ==========================================================================

NfcAdapter::NfcAdapter(
    QObject* aParent) :
    QObject(aParent),
    iPrivate(new Private(this))
{
}

NfcAdapter::~NfcAdapter()
{
    delete iPrivate;
}

/* static */
QObject*
NfcAdapter::createSingleton(
    QQmlEngine*,
    QJSEngine*)
{
    // Callback for qmlRegisterSingletonType<NfcAdapter>
    return new NfcAdapter;
}

bool
NfcAdapter::valid() const
{
    return iPrivate->iAdapter->valid;
}

bool
NfcAdapter::present() const
{
    return iPrivate->iAdapter->adapter != Q_NULLPTR;
}

bool
NfcAdapter::enabled() const
{
    return iPrivate->iAdapter->enabled;
}

bool
NfcAdapter::powered() const
{
    return iPrivate->iAdapter->powered;
}

bool
NfcAdapter::targetPresent() const
{
    return iPrivate->iAdapter->target_present;
}

int
NfcAdapter::supportedModes() const
{
    return iPrivate->iAdapter->supported_modes;
}

int
NfcAdapter::mode() const
{
    return iPrivate->iAdapter->mode;
}

QString
NfcAdapter::tagPath() const
{
    const char* tag = iPrivate->iAdapter->tags[0];
    return (tag && tag[0]) ? QString(tag) : QString();
}

QString
NfcAdapter::peerPath() const
{
    const char* peer = iPrivate->iAdapter->peers[0];
    return (peer && peer[0]) ? QString(peer) : QString();
}

QString
NfcAdapter::hostPath() const
{
#ifdef NFCDC_VERSION_1_1_0
    const char* host = iPrivate->iAdapter->hosts[0];
    return (host && host[0]) ? QString(host) : QString();
#else
#pragma message("Please use libgnfcdc 1.1.0 or newer")
    return QString();
#endif
}

int
NfcAdapter::supportedTechs() const
{
#ifdef NFCDC_VERSION_1_1_0
    return iPrivate->iAdapter->supported_techs;
#else
#pragma message("Please use libgnfcdc 1.1.0 or newer")
    return 0;
#endif
}
