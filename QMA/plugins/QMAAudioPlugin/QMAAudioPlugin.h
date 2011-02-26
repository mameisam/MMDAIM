/* ----------------------------------------------------------------- */
/*                                                                   */
/*  Copyright (c) 2010-2011  hkrn (libMMDAI)                         */
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

#ifndef QMAAUDIOPLUGIN_H
#define QMAAUDIOPLUGIN_H

#include <QDir>
#include <QMap>
#include <phonon/AudioOutput>
#include <phonon/MediaObject>

#include "QMAPlugin.h"

class QMAAudioPlugin : public QMAPlugin
{
  Q_OBJECT
  Q_INTERFACES(QMAPlugin)

public:
  QMAAudioPlugin(QObject *parent = 0);
  ~QMAAudioPlugin();

public slots:
  void initialize(MMDAI::SceneController *controller);
  void start();
  void stop();
  void receiveCommand(const QString &command, const QStringList &arguments);
  void receiveEvent(const QString &type, const QStringList &arguments);
  void update(const QRect &rect, const QPoint &pos, const double delta);
  void render();

private slots:
  void aboutToFinish();
  void changeCurrentSource(Phonon::MediaSource source);
  void changeState(Phonon::State newState, Phonon::State oldState);

signals:
  void commandPost(const QString &command, const QStringList &arguments);
  void eventPost(const QString &type, const QStringList &arguments);

private:
  Phonon::AudioOutput *m_audioOutput;
  Phonon::MediaObject *m_audioObject;
  QMap<QString, Phonon::MediaSource> m_audioSources;
};

#endif // QMAAUDIOPLUGIN_H