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

#ifndef __vtkMRMLAstroPVSliceParametersNode_h
#define __vtkMRMLAstroPVSliceParametersNode_h

// MRML includes
#include <vtkMRML.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLNode.h>

// Export includes
#include <vtkSlicerAstroVolumeModuleMRMLExport.h>

/// \brief MRML parameter node for the AstroPVSlice module.
///
/// \ingroup SlicerAstro_QtModules_AstroPVSlice
class VTK_MRML_ASTRO_EXPORT vtkMRMLAstroPVSliceParametersNode : public vtkMRMLNode
{
  public:

  static vtkMRMLAstroPVSliceParametersNode *New();
  vtkTypeMacro(vtkMRMLAstroPVSliceParametersNode,vtkMRMLNode);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  virtual vtkMRMLNode* CreateNodeInstance() VTK_OVERRIDE;

  /// Set node attributes
  virtual void ReadXMLAttributes( const char** atts) VTK_OVERRIDE;

  /// Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent) VTK_OVERRIDE;

  /// Copy the node's attributes to this object
  virtual void Copy(vtkMRMLNode *node) VTK_OVERRIDE;

  /// Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() VTK_OVERRIDE {return "AstroPVSliceParameters";};

  /// Set/Get the InputVolumeNodeID.
  /// \sa SetInputVolumeNodeID(), GetInputVolumeNodeID()
  vtkSetStringMacro(InputVolumeNodeID);
  vtkGetStringMacro(InputVolumeNodeID);

  /// Set/Get the MomentMapNodeID.
  /// \sa SetMomentMapNodeID(), GetMomentMapNodeID()
  vtkSetStringMacro(MomentMapNodeID);
  vtkGetStringMacro(MomentMapNodeID);

  /// Set/Get the RulerNodeID.
  /// \sa SetRulerNodeID(), GetRulerNodeID()
  vtkSetStringMacro(RulerNodeID);
  vtkGetStringMacro(RulerNodeID);

  /// Set/Get the Ruler angle value respect to the center of the slice.
  /// \sa SetRulerAngle(), GetRulerAngle()
  vtkSetMacro(RulerAngle,double);
  vtkGetMacro(RulerAngle,double);

  /// Set/Get the Ruler old angle value respect to the center of the slice.
  /// \sa SetRulerNodeID(), GetRulerNodeID()
  vtkSetMacro(RulerOldAngle,double);
  vtkGetMacro(RulerOldAngle,double);

  /// Set/Get the RulerShiftX value respect to the center of the slice.
  /// \sa SetRulerShiftX(), GetRulerShiftX()
  vtkSetMacro(RulerShiftX,double);
  vtkGetMacro(RulerShiftX,double);

  /// Set/Get the RulerOldShiftX old value respect to the center of the slice.
  /// \sa SetRulerOldShiftX(), GetRulerOldShiftX()
  vtkSetMacro(RulerOldShiftX,double);
  vtkGetMacro(RulerOldShiftX,double);

  /// Set/Get the RulerShiftY value respect to the center of the slice.
  /// \sa SetRulerShiftY(), GetRulerShiftY()
  vtkSetMacro(RulerShiftY,double);
  vtkGetMacro(RulerShiftY,double);

  /// Set/Get the RulerOldShiftY old value respect to the center of the slice.
  /// \sa SetRulerOldShiftY(), GetRulerOldShiftY()
  vtkSetMacro(RulerOldShiftY,double);
  vtkGetMacro(RulerOldShiftY,double);

  /// Get Ruler center in MRML node is in IJK coordinates
  vtkGetVector2Macro (RulerCenter, int);

  /// Set Ruler center. No modification event is fired
  virtual void SetRulerCenterRightAscension(int value);

  /// Set Ruler center. No modification event is fired
  virtual void SetRulerCenterDeclination(int value);

  /// Set Ruler center. No modification event is fired
  virtual void SetRulerCenter(int arg1, int arg2);

  /// Set Ruler center. No modification event is fired
  virtual void SetRulerCenter(int arg[2]);

  //
  enum
    {
    RulerCenterModifiedEvent = 78000
    };

protected:
  vtkMRMLAstroPVSliceParametersNode();
  ~vtkMRMLAstroPVSliceParametersNode();

  vtkMRMLAstroPVSliceParametersNode(const vtkMRMLAstroPVSliceParametersNode&);
  void operator=(const vtkMRMLAstroPVSliceParametersNode&);

  char *InputVolumeNodeID;
  char *MomentMapNodeID;
  char *RulerNodeID;

  double RulerAngle;
  double RulerOldAngle;

  double RulerShiftX;
  double RulerOldShiftX;

  double RulerShiftY;
  double RulerOldShiftY;

  int RulerCenter[2];
};

#endif
