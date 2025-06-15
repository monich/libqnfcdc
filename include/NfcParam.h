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

#ifndef QNFCDC_PARAM_H
#define QNFCDC_PARAM_H

#include <QObject>

class NfcParam :
    public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(bool reset READ reset WRITE setReset NOTIFY resetChanged)
    Q_PROPERTY(bool t4Ndef READ t4Ndef WRITE setT4Ndef NOTIFY t4NdefChanged)
    Q_PROPERTY(QString laNfcid1 READ laNfcid1 WRITE setLaNfcid1 NOTIFY laNfcid1Changed)

public:
    NfcParam(QObject* aParent = Q_NULLPTR);
    ~NfcParam();

    bool active() const;
    void setActive(bool);

    bool reset() const;
    void setReset(bool);

    bool t4Ndef() const;
    void setT4Ndef(bool);

    QString laNfcid1() const;
    void setLaNfcid1(QString);

Q_SIGNALS:
    void activeChanged();
    void resetChanged();
    void t4NdefChanged();
    void laNfcid1Changed();

private:
    class Private;
    Private* iPrivate;
};

#endif // QNFCDC_PARAM_H
