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

#ifndef __qSlicerAstroProfilesModuleWidget_h
#define __qSlicerAstroProfilesModuleWidget_h

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerAstroProfilesModuleExport.h"

// CTK includes
#include <ctkVTKObject.h>

class qSlicerAstroProfilesModuleWidgetPrivate;
class vtkMRMLAstroProfilesParametersNode;
class vtkMRMLNode;

/// \ingroup SlicerAstro_QtModules_AstroProfiles
class Q_SLICERASTRO_QTMODULES_ASTROPROFILES_EXPORT qSlicerAstroProfilesModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:

  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerAstroProfilesModuleWidget(QWidget *parent=0);
  virtual ~qSlicerAstroProfilesModuleWidget();

  virtual void enter();
  virtual void exit();

  /// Get vtkMRMLAstroProfilesParametersNode
  Q_INVOKABLE vtkMRMLAstroProfilesParametersNode* mrmlAstroProfilesParametersNode()const;

public slots:
  /// Slots to generate the profile.
  /// It creates an output volume and calls the logic
  void onCalculate();

protected:
  QScopedPointer<qSlicerAstroProfilesModuleWidgetPrivate> d_ptr;

  /// Initialization of the qvtk connections between MRML nodes
  virtual void setMRMLScene(vtkMRMLScene* scene);

  /// Initialization of MRML nodes
  void initializeNodes(bool forceNew = false);

  /// Initialization of MRML parameter node
  void initializeParameterNode(bool forceNew = false);

  /// Initialization of MRML plot nodes
  void initializePlotNodes(bool forceNew = false);

  /// Initialization of MRML segmentation nodes
  void initializeSegmentations(bool forceNew = false);

  /// Convert a segmentation to LabelMap volume (a mask).
  /// The LabelMap ID is stored in the MRML parameter node of the module
  /// \return Success flag
  bool convertSelectedSegmentToLabelMap();

protected slots:

  /// Set the MRML input node
  void onInputVolumeChanged(vtkMRMLNode* mrmlNode);

  /// Set the MRML profile output node
  void onProfileVolumeChanged(vtkMRMLNode* mrmlNode);

  /// Set the MRML segmentation editor node
  void onSegmentEditorNodeModified(vtkObject* sender);

  /// Update widget GUI from MRML input node
  void onInputVolumeModified();

  /// Update widget GUI from MRML parameter node
  void onMRMLAstroProfilesParametersNodeModified();

  /// Set the MRML parameter node
  void setMRMLAstroProfilesParametersNode(vtkMRMLNode*);

  void onEndCloseEvent();
  void onEndImportEvent();
  void onStartImportEvent();

  void onMaskActiveToggled(bool active);
  void onThresholdRangeChanged(double min, double max);
  void onUnitNodeIntensityChanged(vtkObject* sender);
  void onUnitNodeVelocityChanged(vtkObject* sender);
  void onVelocityRangeChanged(double min, double max);

  void onMRMLSelectionNodeModified(vtkObject* sender);
  void onMRMLSelectionNodeReferenceAdded(vtkObject* sender);
  void onMRMLSelectionNodeReferenceRemoved(vtkObject* sender);

  void onComputationStarted();
  void onComputationCancelled();
  void onComputationFinished();
  void updateProgress(int value);

private:
  Q_DECLARE_PRIVATE(qSlicerAstroProfilesModuleWidget);
  Q_DISABLE_COPY(qSlicerAstroProfilesModuleWidget);
};

#endif
