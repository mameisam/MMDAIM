/* ----------------------------------------------------------------- */
/*                                                                   */
/*  Copyright (c) 2009-2011  Nagoya Institute of Technology          */
/*                           Department of Computer Science          */
/*                2010-2011  hkrn                                    */
/*                                                                   */
/* All rights reserved.                                              */
/*                                                                   */
/* Redistribution and use in source and binary forms, with or        */
/* without modification, are permitted provided that the following   */
/* conditions are met:                                               */
/*                                                                   */
/* - Redistributions of source code must retain the above copyright  */
/*   notice, this list of conditions and the following disclaimer.   */
/* - Redistributions in binary form must reproduce the above         */
/*   copyright notice, this list of conditions and the following     */
/*   disclaimer in the documentation and/or other materials provided */
/*   with the distribution.                                          */
/* - Neither the name of the MMDAI project team nor the names of     */
/*   its contributors may be used to endorse or promote products     */
/*   derived from this software without specific prior written       */
/*   permission.                                                     */
/*                                                                   */
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND            */
/* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,       */
/* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF          */
/* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE          */
/* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS */
/* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,          */
/* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED   */
/* TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     */
/* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON */
/* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,   */
/* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY    */
/* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE           */
/* POSSIBILITY OF SUCH DAMAGE.                                       */
/* ----------------------------------------------------------------- */

#ifndef QMAVISCRIPT_H
#define QMAVISCRIPT_H

#include <QtCore/QLinkedList>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>

struct QMAVIScriptArgument {
    QString type;
    QStringList arguments;
    QMAVIScriptArgument()
        : type(QString()), arguments(QStringList()) {
    }
    QMAVIScriptArgument(const QMAVIScriptArgument &value)
        : type(value.type), arguments(value.arguments) {
    }
    QMAVIScriptArgument(const QString &t, const QStringList &args)
        : type(t), arguments(args) {
    }
    void operator =(const QMAVIScriptArgument &value) {
        this->type = value.type;
        this->arguments = value.arguments;
    }
};

typedef struct QMAVIScriptState QMAVIScriptState;

class QMAVIScript
{
public:
    static const QString kEPS;

    QMAVIScript();
    ~QMAVIScript();

    bool load(QTextStream &stream);
    bool setTransition(const QMAVIScriptArgument &input,
                       QMAVIScriptArgument &output);

private:
    void addScriptArc(int from,
                      int to,
                      const QMAVIScriptArgument &input,
                      const QMAVIScriptArgument &output);
    QMAVIScriptState *newScriptState(quint32 index);

    QLinkedList<QMAVIScriptState *> m_states;
    QMAVIScriptState *m_currentState;
};

#endif // QMAVISCRIPT_H
