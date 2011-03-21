/*
 *
 * QMAARToolKit is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * QMAARToolKit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with QMAARToolKit; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "QMAARToolKitPlugin.h"

#include <QtCore>

QMAARToolKitPlugin::QMAARToolKitPlugin()
  : m_controller(NULL),
  m_image(NULL),
  m_settings(NULL),
  m_patternID(0),
  m_threshold(100),
  m_patternWidth(80.0),
  m_enabled(false)
{
  m_patternCenter[0] = 0.0f;
  m_patternCenter[1] = 0.0f;
}

QMAARToolKitPlugin::~QMAARToolKitPlugin()
{
}

void QMAARToolKitPlugin::initialize(MMDAI::SceneController *controller)
{
  QDir dir = QDir::searchPaths("mmdai").at(0) + "/ARToolKit/Data";
  QString confingFile = "";
  QString cameraParamFile = dir.absoluteFilePath("camera_para.dat");
  QString patternFile = dir.absoluteFilePath("patt.hiro");
  m_controller = controller;
  if (arVideoOpen(confingFile.toUtf8().data()) < 0) {
    MMDAILogWarnString("Failed loading configuration");
    return;
  }
  int width = 0, height = 0;
  if (arVideoInqSize(&width, &height) < 0) {
    MMDAILogWarnString("Failed getting window size");
    return;
  }
  MMDAILogDebug("Grabbed size: (%d, %d)", width, height);
  ARParam wparam;
  if (arParamLoad(cameraParamFile.toUtf8().constData(), 1, &wparam)) {
    MMDAILogWarn("Failed loading a camera parameter file: %s", cameraParamFile.toUtf8().constData());
    return;
  }
  arParamChangeSize(&wparam, width, height, &m_cameraParam);
  arInitCparam(&m_cameraParam);
  arParamDisp(&m_cameraParam);
  if ((m_patternID = arLoadPatt(patternFile.toUtf8().constData())) < 0) {
    MMDAILogWarn("Failed loading a pattern file: %s", patternFile.toUtf8().constData());
    return;
  }
  if ((m_settings = arglSetupForCurrentContext()) == NULL) {
    MMDAILogWarnString("Failed initializing ARGL");
    return;
  }
  MMDAILogDebugString("Initialized ARToolKit settings successfully");
  m_enabled = true;
}

void QMAARToolKitPlugin::start()
{
  if (m_enabled)
    arVideoCapStart();
}

void QMAARToolKitPlugin::stop()
{
  if (m_enabled) {
    arVideoCapStop();
    arVideoClose();
    arglCleanup(m_settings);
  }
}

void QMAARToolKitPlugin::receiveCommand(const QString &command, const QStringList &arguments)
{
  Q_UNUSED(command);
  Q_UNUSED(arguments);
  /* do nothing */
}

void QMAARToolKitPlugin::receiveEvent(const QString &type, const QStringList &arguments)
{
  Q_UNUSED(type);
  Q_UNUSED(arguments);
  /* do nothing */
}

void QMAARToolKitPlugin::update(const QRect &rect, const QPoint &pos, const double delta)
{
  Q_UNUSED(rect);
  Q_UNUSED(pos);
  Q_UNUSED(delta);
  /* do nothing */
}

void QMAARToolKitPlugin::prerender()
{
  if (m_enabled) {
    ARUint8 *ptr = NULL;
    if ((ptr = static_cast<ARUint8 *>(arVideoGetImage())) == NULL) {
      ptr = m_image;
    }
    else {
      m_image = ptr;
    }
    arglDispImage(ptr, &m_cameraParam, 1.0, m_settings);
    arVideoCapNext();
    ARMarkerInfo *markerInfo = NULL;
    int nmarkers = 0;
    if (arDetectMarker(ptr, m_threshold, &markerInfo, &nmarkers) < 0) {
      MMDAILogWarnString("Failed calling arDetectMarker");
      return;
    }
    ptr = NULL;
    int found = -1;
    for (int i = 0; i < nmarkers; i++) {
      if (m_patternID == markerInfo[i].id) {
        if (found == -1)
          found = i;
        else if (markerInfo[found].cf < markerInfo[i].cf)
          found = i;
      }
    }
    double arProjection[16];
    float projection[16];
    arglCameraFrustumRH(&m_cameraParam, 0.1, 100, arProjection);
    for (int i = 0; i < 16; i++) {
      projection[i] = arProjection[i];
    }
    m_controller->setProjectionMatrix(projection);
    if (found == -1) {
      return;
    }
    else {
      double arModelView[16], arPatternTransform[3][4];
      float modelView[16];
      arGetTransMat(&markerInfo[found], m_patternCenter, m_patternWidth, arPatternTransform);
      arglCameraViewRH(arPatternTransform, arModelView, 0.2);
      for (int i = 0; i < 16; i++) {
        modelView[i] = arModelView[i];
      }
      btTransform transform;
      transform.setFromOpenGLMatrix(modelView);
      transform.setRotation(btQuaternion(btVector3(0.0, 0.1, 0.0), 90.0));
      transform.getOpenGLMatrix(modelView);
      m_controller->setModelViewMatrix(modelView);
    }
  }
}

void QMAARToolKitPlugin::postrender()
{
  /* do nothing */
}

Q_EXPORT_PLUGIN2(qma_artoolkit_plugin, QMAARToolKitPlugin);
