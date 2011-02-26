/* ----------------------------------------------------------------- */
/*                                                                   */
/*  Copyright (c) 2009-2010  Nagoya Institute of Technology          */
/*                           Department of Computer Science          */
/*                2010-2011  hkrn (libMMDAI)                         */
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
/* - Neither the name of the MMDAgent project team nor the names of  */
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

#include <QtConcurrentRun>
#include <QBuffer>
#include <QByteArray>
#include <QDir>
#include <QDebug>
#include <QFuture>

#include "QMAOpenJTalkPlugin.h"

const QString kOpenJTalkStartCommand = "SYNTH_START";
const QString kOpenJTalkStopCommand =  "SYNTH_STOP";
const QByteArray kOpenJTalkCodecName = "Shift-JIS";

QMAOpenJTalkPlugin::QMAOpenJTalkPlugin(QObject *parent)
  : QMAPlugin(parent),
  m_audioOutput(0),
  m_buffer(0)
{
  m_format.setFrequency(48000);
  m_format.setChannels(1);
  m_format.setSampleSize(16);
  m_format.setCodec("audio/pcm");
  m_format.setByteOrder(QAudioFormat::LittleEndian);
  m_format.setSampleType(QAudioFormat::SignedInt);
  QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
  if (!info.isFormatSupported(m_format)) {
    qWarning() << "Default format not supported - trying to use nearest";
    m_format = info.nearestFormat(m_format);
  }
  m_audioOutput = new QAudioOutput(m_format, this);
  connect(m_audioOutput, SIGNAL(stateChanged(QAudio::State)), this, SLOT(stateChanged(QAudio::State)));
  connect(&m_watcher, SIGNAL(finished()), this, SLOT(play()));
}

QMAOpenJTalkPlugin::~QMAOpenJTalkPlugin()
{
  foreach (QMAOpenJTalkModel *model, m_models)
    delete model;
  delete m_audioOutput;
  delete m_buffer;
}

void QMAOpenJTalkPlugin::initialize(MMDAI::SceneController *controller)
{
  Q_UNUSED(controller);
  m_base = QDir::searchPaths("mmdai").at(0);
  m_dir = m_base + "/AppData/Open_JTalk";
  m_config = QFile("mmdai:MMDAI.ojt").fileName();
}

void QMAOpenJTalkPlugin::start()
{
  /* do nothing */
}

void QMAOpenJTalkPlugin::stop()
{
  /* do nothing */
}

void QMAOpenJTalkPlugin::receiveCommand(const QString &command, const QStringList &arguments)
{
  if (command == kOpenJTalkStartCommand && arguments.count() == 3) {
    QString name = arguments[0];
    QString style = arguments[1];
    QString text = arguments[2];
    m_watcher.setFuture(QtConcurrent::run(this, &QMAOpenJTalkPlugin::run, name, style, text));
  }
  else if (command == kOpenJTalkStopCommand && arguments.count() == 1) {
    QString name = arguments[0];
    Q_UNUSED(name);
  }
}

void QMAOpenJTalkPlugin::receiveEvent(const QString &type, const QStringList &arguments)
{
  Q_UNUSED(type);
  Q_UNUSED(arguments);
  /* do nothing */
}

void QMAOpenJTalkPlugin::update(const QRect &rect, const QPoint &pos, const double delta)
{
  Q_UNUSED(rect);
  Q_UNUSED(pos);
  Q_UNUSED(delta);
  /* do nothing */
}

void QMAOpenJTalkPlugin::render()
{
  /* do nothing */
}

void QMAOpenJTalkPlugin::stateChanged(QAudio::State state)
{
  qDebug() << "stateChanged:" << state;
  if (state == QAudio::IdleState) {
    m_buffer->close();
    m_audioOutput->stop();
  }
  else if (state == QAudio::StoppedState) {
    QMAOpenJTalkModelData result = m_watcher.future();
    QStringList arguments;
    arguments << result.name;
    eventPost("SYNTH_EVENT_STOP", arguments);
  }
}

void QMAOpenJTalkPlugin::play()
{
  QMAOpenJTalkModelData result = m_watcher.future();
  QString sequence = result.sequence;
  m_bytes = result.bytes;
  delete m_buffer;
  m_buffer = new QBuffer(&m_bytes);
  m_buffer->open(QBuffer::ReadOnly);
  m_audioOutput->start(m_buffer);
  QStringList arguments;
  QString name = result.name;
  arguments << name;
  eventPost("SYNTH_EVENT_START", arguments);
  arguments.clear();
  arguments << name << sequence;
  commandPost("LIPSYNC_START", arguments);
}

struct QMAOpenJTalkModelData QMAOpenJTalkPlugin::run(const QString &name,
                                                     const QString &style,
                                                     const QString &text)
{
  QMAOpenJTalkModel *model;
  if (m_models.contains(name)) {
    model = m_models[name];
  }
  else {
    model = new QMAOpenJTalkModel(this);
    model->loadSetting(m_base, m_config);
    model->loadDictionary(m_dir);
    m_models[name] = model;
  }
  model->setStyle(style);
  model->setText(text);
  struct QMAOpenJTalkModelData data;
  data.name = name;
  data.sequence = model->getPhonemeSequence();
  data.duration = model->getDuration();
  data.bytes = model->finalize();
  return data;
}

Q_EXPORT_PLUGIN2(qma_openjtalk_plugin, QMAOpenJTalkPlugin)