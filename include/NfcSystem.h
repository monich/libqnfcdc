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

#ifndef QNFCDC_SYSTEM_H
#define QNFCDC_SYSTEM_H

#include <QObject>

class QQmlEngine;
class QJSEngine;

class NfcSystem :
    public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(NfcSystem)
    Q_PROPERTY(bool valid READ valid NOTIFY validChanged)
    Q_PROPERTY(bool present READ present NOTIFY presentChanged)
    Q_PROPERTY(bool enabled READ enabled NOTIFY enabledChanged)
    Q_PROPERTY(int version READ version NOTIFY versionChanged)
    Q_PROPERTY(int mode READ mode NOTIFY modeChanged)
    Q_ENUMS(DaemonVersion)
    Q_ENUMS(Mode)

public:
    enum DaemonVersion {
        Version_1_0_26 = 0x0100001a, // Fixed ISO-DEP initialization
        Version_1_1_0 = 0x01001000,  // NFC-DEP (peer-to-peer) support
        Version_1_2_0 = 0x01002000   // Card Emulation support
    };

    enum Mode {
        None          = 0x00,
        P2PInitiator  = 0x01,
        ReaderWriter  = 0x02,
        P2PTarget     = 0x04,
        CardEmulation = 0x08
    };

    NfcSystem(QObject* aParent = Q_NULLPTR);
    ~NfcSystem();

    // Callback for qmlRegisterSingletonType<NfcSystem>
    static QObject* createSingleton(QQmlEngine*, QJSEngine*);

    bool valid() const;
    bool present() const;
    bool enabled() const;
    int version() const;
    int mode() const;

Q_SIGNALS:
    void validChanged();
    void presentChanged();
    void enabledChanged();
    void versionChanged();
    void modeChanged();

private:
    class Private;
    Private* iPrivate;
};

#endif // QNFCDC_SYSTEM_H
