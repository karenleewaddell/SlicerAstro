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

#ifndef __qSlicerAstroVolumeModule_h
#define __qSlicerAstroVolumeModule_h

// CTK includes
#include <ctkVTKObject.h>

// SlicerQt includes
#include "qSlicerLoadableModule.h"

#include "qSlicerAstroVolumeModuleExport.h"

class qSlicerAbstractModuleWidget;
class qSlicerAstroVolumeModulePrivate;
class vtkMRMLSliceLogic;
class vtkObject;

/// \ingroup SlicerAstro_QtModules_AstroVolume
class Q_SLICERASTRO_QTMODULES_ASTROVOLUME_EXPORT qSlicerAstroVolumeModule :
  public qSlicerLoadableModule
{
  Q_OBJECT
  QVTK_OBJECT
#ifdef Slicer_HAVE_QT5
  Q_PLUGIN_METADATA(IID "org.slicer.modules.loadable.qSlicerLoadableModule/1.0");
#endif
  Q_INTERFACES(qSlicerLoadableModule);

public:

  typedef qSlicerLoadableModule Superclass;
  qSlicerAstroVolumeModule(QObject *parent=0);
  virtual ~qSlicerAstroVolumeModule();

  qSlicerGetTitleMacro(QTMODULE_TITLE);

  /// Help to use the module
  virtual QString helpText()const;

  /// Return acknowledgments
  virtual QString acknowledgementText()const;

  /// Return the authors of the module
  virtual QStringList contributors()const;

  /// Return module dependencies
  virtual QStringList dependencies()const;

  /// Return a custom icon for the module
  virtual QIcon icon()const;

  /// Return the categories for the module
  virtual QStringList categories()const;

  /// Define associated node types
  virtual QStringList associatedNodeTypes()const;

protected:
  /// Initialize the module. Register the AstroVolume reader/writer
  virtual void setup();

  /// Create and return the widget representation associated to this module
  virtual qSlicerAbstractModuleRepresentation* createWidgetRepresentation();

  /// Create and return the logic associated to this module
  virtual vtkMRMLAbstractLogic* createLogic();

  QScopedPointer<qSlicerAstroVolumeModulePrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerAstroVolumeModule);
  Q_DISABLE_COPY(qSlicerAstroVolumeModule);
};

#endif
