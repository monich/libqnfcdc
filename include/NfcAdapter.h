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

#ifndef QNFCDC_ADAPTER_H
#define QNFCDC_ADAPTER_H

#include <QObject>

class QQmlEngine;
class QJSEngine;

class NfcAdapter :
    public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(NfcAdapter)
    Q_PROPERTY(bool valid READ valid NOTIFY validChanged)
    Q_PROPERTY(bool present READ present NOTIFY presentChanged)
    Q_PROPERTY(bool enabled READ enabled NOTIFY enabledChanged)
    Q_PROPERTY(bool powered READ powered NOTIFY poweredChanged)
    Q_PROPERTY(bool targetPresent READ targetPresent NOTIFY targetPresentChanged)
    Q_PROPERTY(int supportedModes READ supportedModes NOTIFY supportedModesChanged)
    Q_PROPERTY(int supportedTechs READ supportedTechs NOTIFY supportedTechsChanged)
    Q_PROPERTY(int mode READ mode NOTIFY modeChanged)
    Q_PROPERTY(QString tagPath READ tagPath NOTIFY tagPathChanged)
    Q_PROPERTY(QString peerPath READ peerPath NOTIFY peerPathChanged)
    Q_PROPERTY(QString hostPath READ hostPath NOTIFY hostPathChanged)

public:
    NfcAdapter(QObject* aParent = Q_NULLPTR);
    ~NfcAdapter();

    // Callback for qmlRegisterSingletonType<NfcAdapter>
    static QObject* createSingleton(QQmlEngine*, QJSEngine*);

    bool valid() const;
    bool present() const;
    bool enabled() const;
    bool powered() const;
    bool targetPresent() const;
    int supportedModes() const;
    int supportedTechs() const;
    int mode() const;
    QString tagPath() const;
    QString peerPath() const;
    QString hostPath() const;

Q_SIGNALS:
    void validChanged();
    void presentChanged();
    void enabledChanged();
    void poweredChanged();
    void targetPresentChanged();
    void supportedModesChanged();
    void supportedTechsChanged();
    void modeChanged();
    void tagPathChanged();
    void peerPathChanged();
    void hostPathChanged();

private:
    class Private;
    Private* iPrivate;
};

#endif // QNFCDC_ADAPTER_H
