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

#include <nfcdc_daemon.h>

#include "NfcSystem.h"

#include "Debug.h"

Q_STATIC_ASSERT(NfcSystem::Version_1_0_26 == NFC_DAEMON_VERSION(1,0,26));
Q_STATIC_ASSERT(NfcSystem::Version_1_1_0 == NFC_DAEMON_VERSION(1,1,0));

Q_STATIC_ASSERT((int)NfcSystem::None == (int)NFC_MODE_NONE);
Q_STATIC_ASSERT((int)NfcSystem::P2PInitiator == (int)NFC_MODE_P2P_INITIATOR);
Q_STATIC_ASSERT((int)NfcSystem::ReaderWriter == (int)NFC_MODE_READER_WRITER);
Q_STATIC_ASSERT((int)NfcSystem::P2PTarget == (int)NFC_MODE_P2P_TARGET);
Q_STATIC_ASSERT((int)NfcSystem::CardEmulation == (int)NFC_MODE_CARD_EMILATION);

// ==========================================================================
// NfcSystem::Private
// ==========================================================================

class NfcSystem::Private
{
public:
    Private(NfcSystem*);
    ~Private();

    static const char* SIGNAL_NAME[];
    static void propertyChanged(NfcDaemonClient*, NFC_DAEMON_PROPERTY, void*);

public:
    NfcDaemonClient* iDaemon;
    gulong iDaemonEventId[6]; // Must not be less than the number of non-NULLs:
};

const char* NfcSystem::Private::SIGNAL_NAME[] = {
    Q_NULLPTR,          // NFC_DAEMON_PROPERTY_ANY
    "validChanged",     // NFC_DAEMON_PROPERTY_VALID
    "presentChanged",   // NFC_DAEMON_PROPERTY_PRESENT
    Q_NULLPTR,          // NFC_DAEMON_PROPERTY_ERROR
    "enabledChanged",   // NFC_DAEMON_PROPERTY_ENABLED
    Q_NULLPTR,          // NFC_DAEMON_PROPERTY_ADAPTERS
    "versionChanged",   // NFC_DAEMON_PROPERTY_VERSION
    "modeChanged",      // NFC_DAEMON_PROPERTY_MODE
#ifdef NFCDC_VERSION_1_1_0
    "techsChanged"       // NFC_DAEMON_PROPERTY_MODE
#endif
    // Remember to update iDaemonEventId count when adding new handlers!
};

NfcSystem::Private::Private(
    NfcSystem* aParent) :
    iDaemon(nfc_daemon_client_new())
{
    Q_STATIC_ASSERT(G_N_ELEMENTS(NfcSystem::Private::SIGNAL_NAME) ==
        NFC_DAEMON_PROPERTY_COUNT);
    uint k = 0;
    for (uint i = 0; i < NFC_DAEMON_PROPERTY_COUNT; i++) {
        if (SIGNAL_NAME[i]) {
            iDaemonEventId[k++] =
                nfc_daemon_client_add_property_handler(iDaemon,
                    (NFC_DAEMON_PROPERTY)i, propertyChanged, aParent);
        }
    }
    HASSERT(k <= G_N_ELEMENTS(iDaemonEventId));
    while (k < G_N_ELEMENTS(iDaemonEventId)) {
        iDaemonEventId[k++] = 0;
    }
}

NfcSystem::Private::~Private()
{
    nfc_daemon_client_remove_all_handlers(iDaemon, iDaemonEventId);
    nfc_daemon_client_unref(iDaemon);
}

/* static */
void
NfcSystem::Private::propertyChanged(
    NfcDaemonClient*,
    NFC_DAEMON_PROPERTY aProperty,
    void* aTarget)
{
    // Qt signals should be signalled from the Qt event loop
    // See https://bugreports.qt.io/browse/QTBUG-18434 for details
    QMetaObject::invokeMethod((QObject*)aTarget, SIGNAL_NAME[aProperty],
        Qt::QueuedConnection);
}

// ==========================================================================
// NfcSystem
// ==========================================================================

NfcSystem::NfcSystem(
    QObject* aParent) :
    QObject(aParent),
    iPrivate(new Private(this))
{
}

NfcSystem::~NfcSystem()
{
    delete iPrivate;
}

/* static */
QObject*
NfcSystem::createSingleton(
    QQmlEngine*,
    QJSEngine*)
{
    // Callback for qmlRegisterSingletonType<NfcSystem>
    return new NfcSystem;
}

bool
NfcSystem::valid() const
{
    return iPrivate->iDaemon->valid;
}

bool
NfcSystem::present() const
{
    return iPrivate->iDaemon->present;
}

bool
NfcSystem::enabled() const
{
    return iPrivate->iDaemon->enabled;
}

int
NfcSystem::version() const
{
    return iPrivate->iDaemon->version;
}

int
NfcSystem::mode() const
{
    return iPrivate->iDaemon->mode;
}

int
NfcSystem::techs() const
{
#ifdef NFCDC_VERSION_1_1_0
    return iPrivate->iDaemon->techs;
#else
#pragma message("Please use libgnfcdc 1.1.0 or newer")
    return 0;
#endif
}
