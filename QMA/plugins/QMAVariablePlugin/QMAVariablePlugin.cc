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

#include <QDateTime>

#include <MMDAI/MMDAI.h>
#include "QMAVariablePlugin.h"

const char *QMAVariablePlugin::kValueSet = "VALUE_SET";
const char *QMAVariablePlugin::kValueUnset = "VALUE_UNSET";
const char *QMAVariablePlugin::kValueEvaluate = "VALUE_EVAL";
const char *QMAVariablePlugin::kTimerStart = "TIMER_START";
const char *QMAVariablePlugin::kTimerStop = "TIMER_STOP";

const char *QMAVariablePlugin::kValueSetEvent = "VALUE_EVENT_SET";
const char *QMAVariablePlugin::kValueUnsetEvent = "VALUE_EVENT_UNSET";
const char *QMAVariablePlugin::kValueEvaluateEvent = "VALUE_EVENT_EVAL";
const char *QMAVariablePlugin::kTimerStartEvent = "TIMER_EVENT_START";
const char *QMAVariablePlugin::kTimerStopEvent = "TIMER_EVENT_STOP";

QMAVariablePlugin::QMAVariablePlugin()
{
  qsrand(QDateTime::currentDateTime().toTime_t());
}

QMAVariablePlugin::~QMAVariablePlugin()
{
}

void QMAVariablePlugin::initialize(MMDAI::SceneController *controller)
{
  Q_UNUSED(controller);
  /* do nothing */
}

void QMAVariablePlugin::start()
{
  /* do nothing */
}

void QMAVariablePlugin::stop()
{
  /* do nothing */
}

void QMAVariablePlugin::receiveCommand(const QString &command, const QStringList &arguments)
{
  int argc = arguments.count();
  if (command == kValueSet && argc >= 2) {
    const QString key = arguments.at(0);
    const QString value = arguments.at(1);
    QString value2 = "";
    if (argc >= 3)
      value2 = arguments.at(2);
    setValue(key, value, value2);
  }
  else if (command == kValueUnset && argc >= 1) {
    const QString key = arguments.at(0);
    deleteValue(key);
  }
  else if (command == kValueEvaluate && argc >= 3) {
    const QString key = arguments.at(0);
    const QString op = arguments.at(1);
    const QString value = arguments.at(2);
    evaluate(key, op, value);
  }
  else if (command == kTimerStart && argc >= 2) {
    const QString key = arguments.at(0);
    const QString value = arguments.at(1);
    startTimer0(key, value);
  }
  else if (command == kTimerStop && argc >= 1) {
    const QString key = arguments.at(0);
    stopTimer0(key);
  }
}

void QMAVariablePlugin::receiveEvent(const QString &type, const QStringList &arguments)
{
  Q_UNUSED(type);
  Q_UNUSED(arguments);
}

void QMAVariablePlugin::update(const QRect &rect, const QPoint &pos, const double delta)
{
  Q_UNUSED(rect);
  Q_UNUSED(pos);
  Q_UNUSED(delta);
}

void QMAVariablePlugin::prerender()
{
  /* do nothing */
}

void QMAVariablePlugin::postrender()
{
  /* do nothing */
}

void QMAVariablePlugin::setValue(const QString &key, const QString &value, const QString &value2)
{
  if (key.isEmpty()) {
    MMDAILogInfoString("specified key is empty");
    return;
  }
  if (value2.isNull()) {
    m_values[key] = value.toFloat();
  }
  else {
    float min = value.toFloat();
    float max = value2.toFloat();
    if (max < min) {
      float swap = max;
      max = min;
      min = swap;
    }
    const float random = min + (max - min) * qrand() * (1.0f / RAND_MAX);
    m_values[key] = random;
  }
  QStringList arguments;
  arguments << key;
  emit eventPost(kValueSetEvent, arguments);
}

void QMAVariablePlugin::deleteValue(const QString &key)
{
  if (m_values.contains(key)) {
    m_values.remove(key);
    QStringList arguments;
    arguments << key;
    emit eventPost(kValueUnsetEvent, arguments);
  }
}

void QMAVariablePlugin::evaluate(const QString &key, const QString &op, const QString &value)
{
  if (!m_values.contains(key)) {
    MMDAILogInfo("Evaluating %s is not found", key.toUtf8().constData());
    return;
  }
  const float v1 = value.toFloat();
  const float v2 = m_values[key];
  bool ret = false;
  if (op == "EQ") {
    ret = v1 == v2;
  }
  else if (op == "NE") {
    ret = v1 != v2;
  }
  else if (op == "LT") {
    ret = v1 > v2;
  }
  else if (op == "LE") {
    ret = v1 >= v2;
  }
  else if (op == "GT") {
    ret = v1 < v2;
  }
  else if (op == "GE") {
    ret = v1 <= v2;
  }
  else {
    MMDAILogInfo("Operation %s is invalid", op.toUtf8().constData());
  }
  QStringList arguments;
  arguments << key << op << value;
  arguments << (ret ? "TRUE" : "FALSE");
  emit eventPost(kValueEvaluateEvent, arguments);
}

void QMAVariablePlugin::startTimer0(const QString &key, const QString &value)
{
  if (key.isEmpty()) {
    MMDAILogInfoString("specified key is empty");
    return;
  }
  const int seconds = value.toInt();
  if (m_timers.contains(key)) {
    QBasicTimer *timer = m_timers.value(key);
    timer->stop();
    delete timer;
  }
  if (seconds > 0) {
    const int msec = seconds * 1000;
    QBasicTimer *timer = new QBasicTimer();
    m_timers.insert(key, timer);
    timer->start(msec, this);
    QStringList arguments;
    arguments << key;
    emit eventPost(kTimerStartEvent, arguments);
  }
  else {
    MMDAILogInfo("Invalid second: %s", value.toUtf8().constData());
  }
}

void QMAVariablePlugin::stopTimer0(const QString &key)
{
  if (m_timers.contains(key)) {
    QBasicTimer *timer = m_timers.value(key);
    m_timers.remove(key);
    timer->stop();
    delete timer;
    QStringList arguments;
    arguments << key;
    emit eventPost(kTimerStopEvent, arguments);
  }
}

void QMAVariablePlugin::timerEvent(QTimerEvent *event)
{
  QMapIterator<QString, QBasicTimer *> iterator(m_timers);
  QString key;
  QBasicTimer *timer = NULL;
  const int id = event->timerId();
  while (iterator.hasNext()) {
    iterator.next();
    timer = iterator.value();
    if (timer->timerId() == id) {
      key = iterator.key();
      break;
    }
  }
  if (!key.isNull()) {
    timer->stop();
    delete timer;
    m_timers.remove(key);
    QStringList arguments;
    arguments << key;
    emit eventPost(kTimerStopEvent, arguments);
  }
  else {
    MMDAILogWarn("%s seems deleted", key.toUtf8().constData());
  }
}

Q_EXPORT_PLUGIN2(qma_variable_plugin, QMAVariablePlugin);