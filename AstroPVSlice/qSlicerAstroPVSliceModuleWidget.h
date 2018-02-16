/*==============================================================================

  Copyright (c) Kapteyn Astronomical Institute
  University of Groningen, Groningen, Netherlands. All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Davide Punzo, Kapteyn Astronomical Institute,
  and was supported through the European Research Council grant nr. 291531.

==============================================================================*/

#ifndef __qSlicerAstroPVSliceModuleWidget_h
#define __qSlicerAstroPVSliceModuleWidget_h

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerAstroPVSliceModuleExport.h"

// CTK includes
#include <ctkVTKObject.h>

class qSlicerAstroPVSliceModuleWidgetPrivate;
class vtkMRMLAstroPVSliceParametersNode;
class vtkMRMLNode;

/// \ingroup Slicer_QtModules_AstroPVSlice
class Q_SLICER_QTMODULES_ASTROPVSLICE_EXPORT qSlicerAstroPVSliceModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:

  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerAstroPVSliceModuleWidget(QWidget *parent=0);
  virtual ~qSlicerAstroPVSliceModuleWidget();

  virtual void enter();
  virtual void exit();

  /// Get vtkMRMLAstroPVSliceParametersNode
  Q_INVOKABLE vtkMRMLAstroPVSliceParametersNode* mrmlAstroPVSliceParametersNode()const;

protected:
  QScopedPointer<qSlicerAstroPVSliceModuleWidgetPrivate> d_ptr;

  virtual void setMRMLScene(vtkMRMLScene*);
  void initializeNodes(bool forceNew = false);
  void initializeParameterNode(bool forceNew = false);
  void initializeMomentMapNode(bool forceNew = false);
  void initializeRulerNode(bool forceNew = false, bool InitRulerPositions = true);

protected slots:
  void on3DViewParallel();
  void on3DViewPerpendicular();
  void onEndCloseEvent();
  void onEndImportEvent();
  void onInputVolumeChanged(vtkMRMLNode* mrmlNode);
  void onMRMLAstroPVSliceParametersNodeModified();
  void onMRMLPVSliceRulerNodeModified();
  void onMRMLSelectionNodeModified(vtkObject* sender);
  void onMomentMapChanged(vtkMRMLNode* mrmlNode);
  void onRotateRulerChanged(double theta);
  void onRulerCenterRightAscensionChanged(double value);
  void onRulerCenterDeclinationChanged(double value);
  void onRulerChanged(vtkMRMLNode* mrmlNode);
  void onShiftXRulerChanged(double shiftX);
  void onShiftYRulerChanged(double shiftY);
  void onStartImportEvent();
  void setMRMLAstroPVSliceParametersNode(vtkMRMLNode*);

private:
  Q_DECLARE_PRIVATE(qSlicerAstroPVSliceModuleWidget);
  Q_DISABLE_COPY(qSlicerAstroPVSliceModuleWidget);
};

#endif
