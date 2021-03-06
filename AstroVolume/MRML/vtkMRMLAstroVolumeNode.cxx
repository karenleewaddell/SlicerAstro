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

// STD includes
#include <string>
#include <cstdlib>
#include <math.h>

// VTK includes
#include <vtkImageData.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>

// AstroVolume includes
#include <vtkSlicerAstroConfigure.h>

// MRML includes
#include <vtkMRMLAnnotationROINode.h>
#include <vtkMRMLAstroLabelMapVolumeNode.h>
#include <vtkMRMLAstroVolumeDisplayNode.h>
#include <vtkMRMLAstroVolumeNode.h>
#include <vtkMRMLAstroVolumeStorageNode.h>
#include <vtkMRMLVolumeNode.h>
#include <vtkMRMLTransformNode.h>
#include <vtkMRMLVolumePropertyNode.h>
#include <vtkMRMLVolumeRenderingDisplayNode.h>

// OpenMP includes
#ifdef VTK_SLICER_ASTRO_SUPPORT_OPENMP
#include <omp.h>
#endif

//------------------------------------------------------------------------------
const char* vtkMRMLAstroVolumeNode::PRESET_REFERENCE_ROLE = "preset";
const char* vtkMRMLAstroVolumeNode::VOLUMEPROPERTY_REFERENCE_ROLE = "volumeProperty";
const char* vtkMRMLAstroVolumeNode::ROI_REFERENCE_ROLE = "ROI";
const char* vtkMRMLAstroVolumeNode::ROI_ALIGNMENTTRANSFORM_REFERENCE_ROLE = "roiAlignmentTransform";

//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLAstroVolumeNode);

//----------------------------------------------------------------------------
vtkMRMLAstroVolumeNode::vtkMRMLAstroVolumeNode()
{
}

//----------------------------------------------------------------------------
vtkMRMLAstroVolumeNode::~vtkMRMLAstroVolumeNode()
{
}

//----------------------------------------------------------------------------
const char *vtkMRMLAstroVolumeNode::GetPresetNodeReferenceRole()
{
  return vtkMRMLAstroVolumeNode::PRESET_REFERENCE_ROLE;
}

//----------------------------------------------------------------------------
const char *vtkMRMLAstroVolumeNode::GetVolumePropertyNodeReferenceRole()
{
  return vtkMRMLAstroVolumeNode::VOLUMEPROPERTY_REFERENCE_ROLE;
}

//----------------------------------------------------------------------------
const char *vtkMRMLAstroVolumeNode::GetROINodeReferenceRole()
{
  return vtkMRMLAstroVolumeNode::ROI_REFERENCE_ROLE;
}

//----------------------------------------------------------------------------
const char *vtkMRMLAstroVolumeNode::GetROIAlignmentTransformNodeReferenceRole()
{
  return vtkMRMLAstroVolumeNode::ROI_ALIGNMENTTRANSFORM_REFERENCE_ROLE;
}

namespace
{
//----------------------------------------------------------------------------
template <typename T> std::string NumberToString(T V)
{
  std::string stringValue;
  std::stringstream strstream;
  strstream << V;
  strstream >> stringValue;
  return stringValue;
}

//----------------------------------------------------------------------------
std::string DoubleToString(double Value)
{
  return NumberToString<double>(Value);
}

//----------------------------------------------------------------------------
template <typename T> T StringToNumber(const char* num)
{
  std::stringstream ss;
  ss << num;
  T result;
  return ss >> result ? result : 0;
}

//----------------------------------------------------------------------------
int StringToInt(const char* str)
{
  return StringToNumber<int>(str);
}

//----------------------------------------------------------------------------
double StringToDouble(const char* str)
{
  return StringToNumber<double>(str);
}

//----------------------------------------------------------------------------
template <typename T> bool isNaN(T value)
{
  return value != value;
}

//----------------------------------------------------------------------------
bool DoubleIsNaN(double Value)
{
  return isNaN<double>(Value);
}

//----------------------------------------------------------------------------
bool FloatIsNaN(float Value)
{
  return isNaN<float>(Value);
}

//----------------------------------------------------------------------------
bool ShortIsNaN(short Value)
{
  return isNaN<short>(Value);
}

}// end namespace

//----------------------------------------------------------------------------
void vtkMRMLAstroVolumeNode::ReadXMLAttributes(const char** atts)
{
  this->Superclass::ReadXMLAttributes(atts);
}

//----------------------------------------------------------------------------
void vtkMRMLAstroVolumeNode::WriteXML(ostream& of, int nIndent)
{
  this->Superclass::WriteXML(of, nIndent);
}

//----------------------------------------------------------------------------
void vtkMRMLAstroVolumeNode::Copy(vtkMRMLNode *anode)
{
  vtkMRMLAstroVolumeNode *astroVolumeNode = vtkMRMLAstroVolumeNode::SafeDownCast(anode);
  if (!astroVolumeNode)
    {
    return;
    }

  this->Superclass::Copy(astroVolumeNode);
}

//----------------------------------------------------------------------------
void vtkMRMLAstroVolumeNode::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//---------------------------------------------------------------------------
vtkMRMLAstroVolumeDisplayNode* vtkMRMLAstroVolumeNode::GetAstroVolumeDisplayNode()
{
  return vtkMRMLAstroVolumeDisplayNode::SafeDownCast(this->GetDisplayNode());
}

//---------------------------------------------------------------------------
vtkMRMLVolumeRenderingDisplayNode *vtkMRMLAstroVolumeNode::GetAstroVolumeRenderingDisplayNode()
{  
  return vtkMRMLVolumeRenderingDisplayNode::SafeDownCast(this->GetNthDisplayNode(this->GetNumberOfDisplayNodes() - 1));
}

//---------------------------------------------------------------------------
bool vtkMRMLAstroVolumeNode::UpdateRangeAttributes()
{     
  if (!this->GetImageData())
   {
   return false;
   }

  this->GetImageData()->Modified();
  int *dims = this->GetImageData()->GetDimensions();
  int numElements = dims[0] * dims[1] * dims[2];
  const int DataType = this->GetImageData()->GetPointData()->GetScalars()->GetDataType();
  double max_val = this->GetImageData()->GetScalarTypeMin(), min_val = this->GetImageData()->GetScalarTypeMax();
  short *inSPixel = NULL;
  float *inFPixel = NULL;
  double *inDPixel = NULL;

  #ifdef VTK_SLICER_ASTRO_SUPPORT_OPENMP
  omp_set_num_threads(omp_get_num_procs());
  #endif // VTK_SLICER_ASTRO_SUPPORT_OPENMP

  switch (DataType)
    {
    case VTK_SHORT:
      inSPixel = static_cast<short*> (this->GetImageData()->GetScalarPointer());
      #ifdef VTK_SLICER_ASTRO_SUPPORT_OPENMP
      #pragma omp parallel for schedule(static) reduction(max : max_val), reduction(min : min_val)
      #endif // VTK_SLICER_ASTRO_SUPPORT_OPENMP
      for (int elementCnt = 0; elementCnt < numElements; elementCnt++)
        {
        if (ShortIsNaN(*(inSPixel + elementCnt)))
          {
          continue;
          }
        if (*(inSPixel + elementCnt) > max_val)
          {
          max_val = *(inSPixel + elementCnt);
          }
        if (*(inSPixel + elementCnt) < min_val)
          {
          min_val = *(inSPixel + elementCnt);
          }
        }
    break;
    case VTK_FLOAT:
      inFPixel = static_cast<float*> (this->GetImageData()->GetScalarPointer());
      #ifdef VTK_SLICER_ASTRO_SUPPORT_OPENMP
      #pragma omp parallel for schedule(static) reduction(max : max_val), reduction(min : min_val)
      #endif // VTK_SLICER_ASTRO_SUPPORT_OPENMP
      for (int elementCnt = 0; elementCnt < numElements; elementCnt++)
        {
        if (FloatIsNaN(*(inFPixel + elementCnt)))
          {
          continue;
          }
        if (*(inFPixel + elementCnt) > max_val)
          {
          max_val = *(inFPixel + elementCnt);
          }
        if (*(inFPixel + elementCnt) < min_val)
          {
          min_val = *(inFPixel + elementCnt);
          }
        }
      break;
    case VTK_DOUBLE:
      inDPixel = static_cast<double*> (this->GetImageData()->GetScalarPointer());
      #ifdef VTK_SLICER_ASTRO_SUPPORT_OPENMP
      #pragma omp parallel for schedule(static) reduction(max : max_val), reduction(min : min_val)
      #endif // VTK_SLICER_ASTRO_SUPPORT_OPENMP
      for (int elementCnt = 0; elementCnt < numElements; elementCnt++)
        {
        if (DoubleIsNaN(*(inDPixel + elementCnt)))
          {
          continue;
          }
        if (*(inDPixel + elementCnt) > max_val)
          {
          max_val = *(inDPixel + elementCnt);
          }
        if (*(inDPixel + elementCnt) < min_val)
          {
          min_val = *(inDPixel + elementCnt);
          }
        }
      break;
    default:
      vtkErrorMacro("vtkSlicerAstroVolumeLogic::UpdateRangeAttributes : "
                    "attempt to allocate scalars of type not allowed");
      return false;
    }

  int wasModifying = this->StartModify();
  this->SetAttribute("SlicerAstro.DATAMAX", DoubleToString(max_val).c_str());
  this->SetAttribute("SlicerAstro.DATAMIN", DoubleToString(min_val).c_str());

  vtkMRMLAstroVolumeDisplayNode* displayNode = this->GetAstroVolumeDisplayNode();
  if (displayNode)
    {
    double window = max_val - min_val;
    double level = 0.5 * (max_val + min_val);
    int disModify = displayNode->StartModify();
    displayNode->SetWindowLevel(window, level);
    displayNode->SetThreshold(min_val, max_val);
    displayNode->EndModify(disModify);
    }

  this->EndModify(wasModifying);

  inSPixel = NULL;
  inFPixel = NULL;
  inDPixel = NULL;
  delete inSPixel;
  delete inFPixel;
  delete inDPixel;

  return true;
}

//---------------------------------------------------------------------------
bool vtkMRMLAstroVolumeNode::UpdateDisplayThresholdAttributes()
{
  if (!this->GetImageData())
   {
   return false;
   }

  // Calculate the noise as the std of 6 slices of the datacube.
  // The DisplayThreshold = noise
  // 3D color function starts from 3 times the value of DisplayThreshold.
  int *dims = this->GetImageData()->GetDimensions();
  const int DataType = this->GetImageData()->GetPointData()->GetScalars()->GetDataType();
  short *outSPixel = NULL;
  float *outFPixel = NULL;
  double *outDPixel = NULL;
  switch (DataType)
    {
    case VTK_SHORT:
      outSPixel = static_cast<short*> (this->GetImageData()->GetScalarPointer(0,0,0));
      break;
    case VTK_FLOAT:
      outFPixel = static_cast<float*> (this->GetImageData()->GetScalarPointer(0,0,0));
      break;
    case VTK_DOUBLE:
      outDPixel = static_cast<double*> (this->GetImageData()->GetScalarPointer(0,0,0));
      break;
    default:
      vtkErrorMacro("vtkMRMLAstroVolumeNode::UpdateDisplayThresholdAttributes : "
                    "attempt to allocate scalars of type not allowed");
      return false;
    }
  double sum = 0., noise1 = 0., noise2 = 0, noise = 0.;
  int lowBoundary;
  int highBoundary;

  if (StringToInt(this->GetAttribute("SlicerAstro.NAXIS")) == 3)
    {
    lowBoundary = dims[0] * dims[1] * 2;
    highBoundary = dims[0] * dims[1] * 4;
    }
  else if (StringToInt(this->GetAttribute("SlicerAstro.NAXIS")) == 2)
    {
    lowBoundary = dims[0] * 2;
    highBoundary = dims[0] * 4;
    }
  else
    {
    lowBoundary = 2;
    highBoundary = 4;
    }

  int cont = highBoundary - lowBoundary;

  #ifdef VTK_SLICER_ASTRO_SUPPORT_OPENMP
  omp_set_num_threads(omp_get_num_procs());
  #endif // VTK_SLICER_ASTRO_SUPPORT_OPENMP

  switch (DataType)
    {
    case VTK_SHORT:
      #ifdef VTK_SLICER_ASTRO_SUPPORT_OPENMP
      #pragma omp parallel for schedule(static) reduction(+:sum)
      #endif // VTK_SLICER_ASTRO_SUPPORT_OPENMP
      for (int elemCnt = lowBoundary; elemCnt <= highBoundary; elemCnt++)
        {
        if (ShortIsNaN(*(outSPixel + elemCnt)))
           {
           continue;
           }
        sum += *(outSPixel + elemCnt);
        }
      sum /= cont;

      #ifdef VTK_SLICER_ASTRO_SUPPORT_OPENMP
      #pragma omp parallel for schedule(static) reduction(+:noise1)
      #endif // VTK_SLICER_ASTRO_SUPPORT_OPENMP
      for (int elemCnt = lowBoundary; elemCnt <= highBoundary; elemCnt++)
        {
        if (ShortIsNaN(*(outSPixel + elemCnt)))
           {
           continue;
           }
        noise1 += (*(outSPixel + elemCnt) - sum) * (*(outSPixel + elemCnt) - sum);
        }
      noise1 = sqrt(noise1 / cont);
      break;
    case VTK_FLOAT:
      #ifdef VTK_SLICER_ASTRO_SUPPORT_OPENMP
      #pragma omp parallel for schedule(static) reduction(+:sum)
      #endif // VTK_SLICER_ASTRO_SUPPORT_OPENMP
      for (int elemCnt = lowBoundary; elemCnt <= highBoundary; elemCnt++)
        {
        if (FloatIsNaN(*(outFPixel + elemCnt)))
           {
           continue;
           }
        sum += *(outFPixel + elemCnt);
        }
      sum /= cont;

      #ifdef VTK_SLICER_ASTRO_SUPPORT_OPENMP
      #pragma omp parallel for schedule(static) reduction(+:noise1)
      #endif // VTK_SLICER_ASTRO_SUPPORT_OPENMP
      for (int elemCnt = lowBoundary; elemCnt <= highBoundary; elemCnt++)
        {
        if (FloatIsNaN(*(outFPixel + elemCnt)))
           {
           continue;
           }
        noise1 += (*(outFPixel + elemCnt) - sum) * (*(outFPixel + elemCnt) - sum);
        }
      noise1 = sqrt(noise1 / cont);
      break;
    case VTK_DOUBLE:
      #ifdef VTK_SLICER_ASTRO_SUPPORT_OPENMP
      #pragma omp parallel for schedule(static) reduction(+:sum)
      #endif // VTK_SLICER_ASTRO_SUPPORT_OPENMP
      for (int elemCnt = lowBoundary; elemCnt <= highBoundary; elemCnt++)
        {
        if (DoubleIsNaN(*(outDPixel + elemCnt)))
           {
           continue;
           }
        sum += *(outDPixel + elemCnt);
        }
      sum /= cont;

      #ifdef VTK_SLICER_ASTRO_SUPPORT_OPENMP
      #pragma omp parallel for schedule(static) reduction(+:noise1)
      #endif // VTK_SLICER_ASTRO_SUPPORT_OPENMP
      for (int elemCnt = lowBoundary; elemCnt <= highBoundary; elemCnt++)
        {
        if (DoubleIsNaN(*(outDPixel + elemCnt)))
           {
           continue;
           }
        noise1 += (*(outDPixel + elemCnt) - sum) * (*(outDPixel + elemCnt) - sum);
        }
      noise1 = sqrt(noise1 / cont);
      break;
    }

  if (StringToInt(this->GetAttribute("SlicerAstro.NAXIS")) == 3)
    {
    lowBoundary = dims[0] * dims[1] * (dims[2] - 4);
    highBoundary = dims[0] * dims[1] * (dims[2] - 2);
    }
  else if (StringToInt(this->GetAttribute("SlicerAstro.NAXIS")) == 2)
    {
    lowBoundary = dims[0] * (dims[1] - 4);
    highBoundary = dims[0] * (dims[1] - 2);
    }
  else
    {
    lowBoundary = dims[0] - 4;
    highBoundary = dims[0] - 2;
    }

  sum = 0.;

  switch (DataType)
    {
    case VTK_SHORT:
      #ifdef VTK_SLICER_ASTRO_SUPPORT_OPENMP
      #pragma omp parallel for schedule(static) reduction(+:sum)
      #endif // VTK_SLICER_ASTRO_SUPPORT_OPENMP
      for (int elemCnt = lowBoundary; elemCnt <= highBoundary; elemCnt++)
        {
        if (ShortIsNaN(*(outSPixel + elemCnt)))
           {
           continue;
           }
        sum += *(outSPixel + elemCnt);
        }
      sum /= cont;

      #ifdef VTK_SLICER_ASTRO_SUPPORT_OPENMP
      #pragma omp parallel for schedule(static) reduction(+:noise2)
      #endif // VTK_SLICER_ASTRO_SUPPORT_OPENMP
      for (int elemCnt = lowBoundary; elemCnt <= highBoundary; elemCnt++)
        {
        if (ShortIsNaN(*(outSPixel + elemCnt)))
           {
           continue;
           }
        noise2 += (*(outSPixel + elemCnt) - sum) * (*(outSPixel + elemCnt) - sum);
        }
      noise2 = sqrt(noise2 / cont);
      break;
    case VTK_FLOAT:
      #ifdef VTK_SLICER_ASTRO_SUPPORT_OPENMP
      #pragma omp parallel for schedule(static) reduction(+:sum)
      #endif // VTK_SLICER_ASTRO_SUPPORT_OPENMP
      for (int elemCnt = lowBoundary; elemCnt <= highBoundary; elemCnt++)
        {
        if (FloatIsNaN(*(outFPixel + elemCnt)))
           {
           continue;
           }
        sum += *(outFPixel + elemCnt);
        }
      sum /= cont;

      #ifdef VTK_SLICER_ASTRO_SUPPORT_OPENMP
      #pragma omp parallel for schedule(static) reduction(+:noise2)
      #endif // VTK_SLICER_ASTRO_SUPPORT_OPENMP
      for (int elemCnt = lowBoundary; elemCnt <= highBoundary; elemCnt++)
        {
        if (FloatIsNaN(*(outFPixel + elemCnt)))
           {
           continue;
           }
        noise2 += (*(outFPixel + elemCnt) - sum) * (*(outFPixel + elemCnt) - sum);
        }
      noise2 = sqrt(noise2 / cont);
      break;
    case VTK_DOUBLE:
      #ifdef VTK_SLICER_ASTRO_SUPPORT_OPENMP
      #pragma omp parallel for schedule(static) reduction(+:sum)
      #endif // VTK_SLICER_ASTRO_SUPPORT_OPENMP
      for (int elemCnt = lowBoundary; elemCnt <= highBoundary; elemCnt++)
        {
        if (DoubleIsNaN(*(outDPixel + elemCnt)))
           {
           continue;
           }
        sum += *(outDPixel + elemCnt);
        }
      sum /= cont;

      #ifdef VTK_SLICER_ASTRO_SUPPORT_OPENMP
      #pragma omp parallel for schedule(static) reduction(+:noise2)
      #endif // VTK_SLICER_ASTRO_SUPPORT_OPENMP
      for (int elemCnt = lowBoundary; elemCnt <= highBoundary; elemCnt++)
        {
        if (DoubleIsNaN(*(outDPixel + elemCnt)))
           {
           continue;
           }
        noise2 += (*(outDPixel + elemCnt) - sum) * (*(outDPixel + elemCnt) - sum);
        }
      noise2 = sqrt(noise2 / cont);
      break;
    }

  noise = (noise1 + noise2) * 0.5;

  outSPixel = NULL;
  outFPixel = NULL;
  outDPixel = NULL;
  delete outSPixel;
  delete outFPixel;
  delete outDPixel;

  if (noise < 1.E-6)
    {
    double MAX = StringToDouble(this->GetAttribute("SlicerAstro.DATAMAX"));
    double MIN = StringToDouble(this->GetAttribute("SlicerAstro.DATAMIN"));
    noise = (MAX - MIN) * 0.01;
    }

  this->SetDisplayThreshold(noise);

  return true;
}

//-----------------------------------------------------------
void vtkMRMLAstroVolumeNode::SetDisplayThreshold(double DisplayThreshold)
{
  this->SetAttribute("SlicerAstro.DisplayThreshold", DoubleToString(DisplayThreshold).c_str());
  this->InvokeCustomModifiedEvent(vtkMRMLAstroVolumeNode::DisplayThresholdModifiedEvent);
}

//----------------------------------------------------------------------------
double vtkMRMLAstroVolumeNode::GetDisplayThreshold()
{
  return StringToDouble(this->GetAttribute("SlicerAstro.DisplayThreshold"));
}

//----------------------------------------------------------------------------
void vtkMRMLAstroVolumeNode::SetROINode(vtkMRMLAnnotationROINode* node)
{
  this->SetNodeReferenceID(this->GetROINodeReferenceRole(), (node ? node->GetID() : NULL));
}

//----------------------------------------------------------------------------
vtkMRMLAnnotationROINode *vtkMRMLAstroVolumeNode::GetROINode()
{
  return vtkMRMLAnnotationROINode::SafeDownCast(this->GetNodeReference(this->GetROINodeReferenceRole()));
}

//----------------------------------------------------------------------------
void vtkMRMLAstroVolumeNode::SetROIAlignmentTransformNode(vtkMRMLTransformNode* node)
{
  this->SetNodeReferenceID(this->GetROIAlignmentTransformNodeReferenceRole(), (node ? node->GetID() : NULL));
}

//----------------------------------------------------------------------------
void vtkMRMLAstroVolumeNode::DeleteROIAlignmentTransformNode()
{
  vtkMRMLTransformNode* transformNode = this->GetROIAlignmentTransformNode();
  if (transformNode)
    {
    this->SetROIAlignmentTransformNodeID(NULL);
    if (this->GetScene())
      {
      this->GetScene()->RemoveNode(transformNode);
      }
    }
}

//----------------------------------------------------------------------------
vtkMRMLTransformNode *vtkMRMLAstroVolumeNode::GetROIAlignmentTransformNode()
{
  return vtkMRMLTransformNode::SafeDownCast(this->GetNodeReference
                                           (this->GetROIAlignmentTransformNodeReferenceRole()));
}

//----------------------------------------------------------------------------
void vtkMRMLAstroVolumeNode::SetROIAlignmentTransformNodeID(const char *nodeID)
{
  this->SetNodeReferenceID(this->GetROIAlignmentTransformNodeReferenceRole(), nodeID);
}

//-----------------------------------------------------------
void vtkMRMLAstroVolumeNode::SetPresetNode(vtkMRMLVolumePropertyNode *node)
{
  this->SetNodeReferenceID(this->GetPresetNodeReferenceRole(), (node ? node->GetID() : NULL));
}

//-----------------------------------------------------------
void vtkMRMLAstroVolumeNode::SetPresetNode(vtkMRMLNode *node)
{
  this->SetPresetNode(vtkMRMLVolumePropertyNode::SafeDownCast(node));
}

//-----------------------------------------------------------
vtkMRMLNode *vtkMRMLAstroVolumeNode::GetPresetNode()
{
  return this->GetNodeReference(this->GetPresetNodeReferenceRole());
}

//-----------------------------------------------------------
int vtkMRMLAstroVolumeNode::GetPresetIndex()
{
  vtkMRMLNode* presetNode = this->GetPresetNode();
  if (!presetNode)
    {
    return vtkMRMLAstroVolumeNode::LowConstantOpacityPreset;
    }

  std::string presetName = presetNode->GetName();

  if (presetName.find("LowConstantOpacity") != std::string::npos)
    {
    return vtkMRMLAstroVolumeNode::LowConstantOpacityPreset;
    }
  else if (presetName.find("MediumConstantOpacity") != std::string::npos)
    {
    return vtkMRMLAstroVolumeNode::MediumConstantOpacityPreset;
    }
  else if (presetName.find("HighConstantOpacity") != std::string::npos)
    {
    return vtkMRMLAstroVolumeNode::HighConstantOpacityPreset;
    }
  else if (presetName.find("OneSurfaceGreen") != std::string::npos)
    {
    return vtkMRMLAstroVolumeNode::OneSurfaceGreenPreset;
    }
  else if (presetName.find("OneSurfaceWhite") != std::string::npos)
    {
    return vtkMRMLAstroVolumeNode::OneSurfaceWhitePreset;
    }
  else if (presetName.find("TwoSurfaces") != std::string::npos)
    {
    return vtkMRMLAstroVolumeNode::TwoSurfacesPreset;
    }
  else if (presetName.find("ThreeSurfaces") != std::string::npos)
    {
    return vtkMRMLAstroVolumeNode::ThreeSurfacesPreset;
    }
  else if (presetName.find("BrightSurface") != std::string::npos)
    {
    return vtkMRMLAstroVolumeNode::BrightSurface;
    }
  else
    {
    return vtkMRMLAstroVolumeNode::LowConstantOpacityPreset;
    }
}

//-----------------------------------------------------------
void vtkMRMLAstroVolumeNode::SetVolumePropertyNode(vtkMRMLVolumePropertyNode *node)
{
  this->SetNodeReferenceID(this->GetVolumePropertyNodeReferenceRole(), (node ? node->GetID() : NULL));
}

//-----------------------------------------------------------
void vtkMRMLAstroVolumeNode::SetVolumePropertyNode(vtkMRMLNode *node)
{
  this->SetVolumePropertyNode(vtkMRMLVolumePropertyNode::SafeDownCast(node));
}

//-----------------------------------------------------------
vtkMRMLNode *vtkMRMLAstroVolumeNode::GetVolumePropertyNode()
{
  return this->GetNodeReference(this->GetVolumePropertyNodeReferenceRole());
}

//-----------------------------------------------------------
void vtkMRMLAstroVolumeNode::CreateNoneNode(vtkMRMLScene *scene)
{
  // Create a None volume RGBA of 0, 0, 0 so the filters won't complain
  // about missing input
  vtkNew<vtkImageData> id;
  id->SetDimensions(1, 1, 1);
  id->AllocateScalars(VTK_DOUBLE, 4);
  id->GetPointData()->GetScalars()->FillComponent(0, 0.0);
  id->GetPointData()->GetScalars()->FillComponent(1, 125.0);
  id->GetPointData()->GetScalars()->FillComponent(2, 0.0);
  id->GetPointData()->GetScalars()->FillComponent(3, 0.0);

  vtkNew<vtkMRMLAstroVolumeNode> n;
  n->SetName("None");
  // the scene will set the id
  n->SetAndObserveImageData(id.GetPointer());
  scene->AddNode(n.GetPointer());
}

//---------------------------------------------------------------------------
vtkMRMLStorageNode* vtkMRMLAstroVolumeNode::CreateDefaultStorageNode()
{
  return vtkMRMLAstroVolumeStorageNode::New();
}

//---------------------------------------------------------------------------
void vtkMRMLAstroVolumeNode::CreateDefaultDisplayNodes()
{
  vtkMRMLAstroVolumeDisplayNode *displayNode = 
    vtkMRMLAstroVolumeDisplayNode::SafeDownCast(this->GetDisplayNode());
  if(displayNode == NULL)
    {
    displayNode = vtkMRMLAstroVolumeDisplayNode::New();
    if(this->GetScene())
      {
      displayNode->SetScene(this->GetScene());
      this->GetScene()->AddNode(displayNode);
      displayNode->SetDefaultColorMap();
      displayNode->Delete();

      this->SetAndObserveDisplayNodeID(displayNode->GetID());
      vtkWarningMacro("Display node set and observed" << endl);
      }
    }
}
