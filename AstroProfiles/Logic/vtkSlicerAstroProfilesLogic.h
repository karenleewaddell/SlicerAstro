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

#ifndef __vtkSlicerAstroProfilesLogic_h
#define __vtkSlicerAstroProfilesLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"
class vtkMRMLVolumeNode;
class vtkSlicerAstroVolumeLogic;

// AstroProfiless includes
#include "vtkSlicerAstroProfilesModuleLogicExport.h"
class vtkMRMLAstroProfilesParametersNode;

/// \ingroup Slicer_QtModules_AstroProfiles
class VTK_SLICER_ASTROPROFILES_MODULE_LOGIC_EXPORT vtkSlicerAstroProfilesLogic
  : public vtkSlicerModuleLogic
{
public:

  static vtkSlicerAstroProfilesLogic *New();
  vtkTypeMacro(vtkSlicerAstroProfilesLogic,vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  void SetAstroVolumeLogic(vtkSlicerAstroVolumeLogic* logic);
  vtkSlicerAstroVolumeLogic* GetAstroVolumeLogic();

  virtual void RegisterNodes() VTK_OVERRIDE;

  bool CalculateProfile(vtkMRMLAstroProfilesParametersNode *pnode);

protected:
  vtkSlicerAstroProfilesLogic();
  virtual ~vtkSlicerAstroProfilesLogic();

private:
  vtkSlicerAstroProfilesLogic(const vtkSlicerAstroProfilesLogic&); // Not implemented
  void operator=(const vtkSlicerAstroProfilesLogic&);           // Not implemented

  class vtkInternal;
  vtkInternal* Internal;
};

#endif

