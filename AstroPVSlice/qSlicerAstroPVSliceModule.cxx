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

// Qt includes
#include <QDebug>
#include <QtPlugin>

// Slicer includes
#include <qSlicerCoreApplication.h>
#include <qSlicerModuleManager.h>

// Logic includes
#include <vtkSlicerAstroMomentMapsLogic.h>
#include <vtkSlicerAstroVolumeLogic.h>
#include <vtkSlicerAstroPVSliceLogic.h>

// AstroPVSlice includes
#include "qSlicerAstroPVSliceModule.h"
#include "qSlicerAstroPVSliceModuleWidget.h"

//-----------------------------------------------------------------------------
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
#include <QtPlugin>
Q_EXPORT_PLUGIN2(qSlicerAstroPVSliceModule, qSlicerAstroPVSliceModule);
#endif

//-----------------------------------------------------------------------------
/// \ingroup SlicerAstro_QtModules_AstroPVSlice
class qSlicerAstroPVSliceModulePrivate
{
public:
  qSlicerAstroPVSliceModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerAstroPVSliceModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerAstroPVSliceModulePrivate::qSlicerAstroPVSliceModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerAstroPVSliceModule methods

//-----------------------------------------------------------------------------
qSlicerAstroPVSliceModule::qSlicerAstroPVSliceModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerAstroPVSliceModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerAstroPVSliceModule::~qSlicerAstroPVSliceModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerAstroPVSliceModule::helpText()const
{
  return "The AstroPVSlice module shows Position Velocity (PV) Slices by providing "
         "customized controls over the GUI. This module provides fast inspection "
         "tool with a PVSlice linked both with the 2D and 3D views.<br>"
         "For creating a PV Diagram (i.e., a curvilinear PV and able to save it as "
         "fits file), please navigate to the AstroPVDiagram module.";
}

//-----------------------------------------------------------------------------
QString qSlicerAstroPVSliceModule::acknowledgementText()const
{
  return "This module was developed by Davide Punzo. <br>"
         "This work was supported by ERC grant nr. 291531, "
         "and Slicer community. <br>";
}

//-----------------------------------------------------------------------------
QStringList qSlicerAstroPVSliceModule::contributors()const
{
  QStringList moduleContributors;
  moduleContributors << QString("Davide Punzo (Kapteyn Astronomical Institute)");
  moduleContributors << QString("Thijs van der Hulst (Kapteyn Astronomical Institute)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerAstroPVSliceModule::icon()const
{
  return QIcon(":/Icons/AstroPVSlice.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerAstroPVSliceModule::categories()const
{
  return QStringList() << "Astronomy" << "Registration";
}

//-----------------------------------------------------------------------------
QStringList qSlicerAstroPVSliceModule::associatedNodeTypes() const
{
  return QStringList() << "vtkMRMLAstroPVSliceParametersNode";
}

//-----------------------------------------------------------------------------
QStringList qSlicerAstroPVSliceModule::dependencies()const
{
  return QStringList() << "AstroVolume" << "AstroMomentMaps" ;
}

//-----------------------------------------------------------------------------
void qSlicerAstroPVSliceModule::setup()
{
  this->Superclass::setup();

  // Register logic
  vtkSlicerAstroPVSliceLogic* AstroPVSliceLogic =
    vtkSlicerAstroPVSliceLogic::SafeDownCast(this->logic());

  // Register AstroVolume module logic
  qSlicerAbstractCoreModule* astroVolumeModule =
    qSlicerCoreApplication::application()->moduleManager()->module("AstroVolume");
  if (!astroVolumeModule)
    {
    qCritical() << "AstroVolume module is not found";
    return;
    }

  vtkSlicerAstroVolumeLogic* astroVolumeLogic =
    vtkSlicerAstroVolumeLogic::SafeDownCast(astroVolumeModule->logic());
  if (!astroVolumeLogic)
    {
    qCritical() << "AstroVolume logic is not found";
    return;
    }

  AstroPVSliceLogic->SetAstroVolumeLogic(astroVolumeLogic);

  // Register AstroMomentMaps module logic
  qSlicerAbstractCoreModule* astroMomentMapsModule =
    qSlicerCoreApplication::application()->moduleManager()->module("AstroMomentMaps");
  if (!astroMomentMapsModule)
    {
    qCritical() << "astroMomentMaps module is not found";
    return;
    }

  vtkSlicerAstroMomentMapsLogic* astroMomentMapsLogic =
    vtkSlicerAstroMomentMapsLogic::SafeDownCast(astroMomentMapsModule->logic());
  if (!astroMomentMapsLogic)
    {
    qCritical() << "AstroMomentMaps logic is not found";
    return;
    }

  AstroPVSliceLogic->SetAstroMomentMapsLogic(astroMomentMapsLogic);
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSlicerAstroPVSliceModule::createWidgetRepresentation()
{
  return new qSlicerAstroPVSliceModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerAstroPVSliceModule::createLogic()
{
  return vtkSlicerAstroPVSliceLogic::New();
}
