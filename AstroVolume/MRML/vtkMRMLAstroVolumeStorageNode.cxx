// MRML includes
#include "vtkMRMLAstroVolumeStorageNode.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLVolumeNode.h"
#include "vtkMRMLAstroVolumeNode.h"
#include "vtkMRMLAstroVolumeDisplayNode.h"
#include "vtkMRMLAstroLabelMapVolumeNode.h"
#include "vtkMRMLAstroLabelMapVolumeDisplayNode.h"

//vtkFits includes
#include <vtkFITSReader.h>
#include <vtkFITSWriter.h>

// VTK includes
#include <vtkImageChangeInformation.h>
#include <vtkImageData.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkStringArray.h>
#include <vtkVersion.h>
#include <vtkType.h>

//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLAstroVolumeStorageNode);

//----------------------------------------------------------------------------
vtkMRMLAstroVolumeStorageNode::vtkMRMLAstroVolumeStorageNode()
{
  this->CenterImage = 0;
}

//----------------------------------------------------------------------------
vtkMRMLAstroVolumeStorageNode::~vtkMRMLAstroVolumeStorageNode()
{
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
}//end namespace

//----------------------------------------------------------------------------
void vtkMRMLAstroVolumeStorageNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);
  vtkIndent indent(nIndent);

  std::stringstream ss;
  ss << this->CenterImage;
  of << indent << " centerImage=\"" << ss.str() << "\"";
}

//----------------------------------------------------------------------------
void vtkMRMLAstroVolumeStorageNode::ReadXMLAttributes(const char** atts)
{
  int disabledModify = this->StartModify();

  Superclass::ReadXMLAttributes(atts);

  const char* attName;
  const char* attValue;
  while (*atts != NULL)
    {
    attName = *(atts++);
    attValue = *(atts++);
    if (!strcmp(attName, "centerImage"))
      {
      std::stringstream ss;
      ss << attValue;
      ss >> this->CenterImage;
      }
    }

  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, StorageID
void vtkMRMLAstroVolumeStorageNode::Copy(vtkMRMLNode *anode)
{
  int disabledModify = this->StartModify();

  Superclass::Copy(anode);
  vtkMRMLAstroVolumeStorageNode *node = (vtkMRMLAstroVolumeStorageNode *) anode;

  this->SetCenterImage(node->CenterImage);

  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void vtkMRMLAstroVolumeStorageNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMRMLStorageNode::PrintSelf(os,indent);
  os << indent << "CenterImage:   " << this->CenterImage << "\n";
}

//----------------------------------------------------------------------------
bool vtkMRMLAstroVolumeStorageNode::CanReadInReferenceNode(vtkMRMLNode *refNode)
{
  return refNode->IsA("vtkMRMLAstroVolumeNode") ||
         refNode->IsA("vtkMRMLAstroLabelMapVolumeNode");
}

//----------------------------------------------------------------------------
int vtkMRMLAstroVolumeStorageNode::ReadDataInternal(vtkMRMLNode *refNode)
{
  vtkMRMLAstroVolumeNode *volNode = NULL;
  vtkMRMLAstroLabelMapVolumeNode *labvolNode = NULL;
  vtkMRMLAstroVolumeDisplayNode *disNode = NULL;
  vtkMRMLAstroLabelMapVolumeDisplayNode *labdisNode = NULL;

  if ( refNode->IsA("vtkMRMLAstroVolumeNode") )
    {
    volNode = dynamic_cast <vtkMRMLAstroVolumeNode *> (refNode);
    disNode = volNode->GetAstroVolumeDisplayNode();
    }
  else if ( refNode->IsA("vtkMRMLAstroLabelMapVolumeNode") )
    {
    labvolNode = dynamic_cast <vtkMRMLAstroLabelMapVolumeNode *> (refNode);
    labdisNode = labvolNode->GetAstroLabelMapVolumeDisplayNode();
    }
  else
    {
    vtkErrorMacro(<< "Do not recognize node type " << refNode->GetClassName());
    return 0;
    }

  vtkNew<vtkFITSReader> reader;

  // Set Reader member variables
  if (this->CenterImage)
    {
    reader->SetUseNativeOriginOff();
    }
  else
    {
    reader->SetUseNativeOriginOn();
    }

  if ( refNode->IsA("vtkMRMLAstroVolumeNode") )
    {
    if (volNode->GetImageData())
      {
      volNode->SetAndObserveImageData (NULL);
      }
    }
  else if ( refNode->IsA("vtkMRMLAstroLabelMapVolumeNode") )
    {
    if (labvolNode->GetImageData())
      {
      labvolNode->SetAndObserveImageData (NULL);
      }
    }

  std::string fullName = this->GetFullNameFromFileName();

  if (fullName == std::string(""))
    {
    vtkErrorMacro("ReadData: File name not specified");
    return 0;
    }

  reader->SetFileName(fullName.c_str());

  // Check if this is a FITS file that we can read
  if (!reader->CanReadFile(fullName.c_str()))
    {
    vtkErrorMacro("ReadData: This is not a fits file");
    return 0;
    }

  // Read the header to see if the file corresponds to the MRML Node
  reader->UpdateInformation();

  if( refNode->IsA("vtkMRMLAstroVolumeNode") || refNode->IsA("vtkMRMLAstroLabelMapVolumeNode") )
    {
    if (reader->GetPointDataType() != vtkDataSetAttributes::SCALARS &&
         reader->GetNumberOfComponents() > 1 )
      {
      vtkErrorMacro("ReadData: MRMLVolumeNode does not match file kind");
      return 0;
      }
    }

  reader->Update();
  if ( refNode->IsA("vtkMRMLAstroVolumeNode") )
    {
    // set volume attributes
    vtkMatrix4x4* mat = reader->GetRasToIjkMatrix();
    volNode->SetRASToIJKMatrix(mat);

    //set WCSstruct
    volNode->SetWCSStruct(reader->GetWCSStruct());

    // parse WCS Status
    volNode->SetWCSStatus(reader->GetWCSStatus());

    // parse non-specific key-value pairs
    std::vector<std::string> keys = reader->GetHeaderKeysVector();
    for ( std::vector<std::string>::iterator kit = keys.begin();
          kit != keys.end(); ++kit)
      {
      volNode->SetAttribute((*kit).c_str(), reader->GetHeaderValue((*kit).c_str()));
      }

    // parse Space (WCS or IJK) to the display node
    if (disNode)
      {
      if (volNode->GetWCSStatus() != 0)
        {
        disNode->SetSpace("IJK");
        }
      if(!strcmp(reader->GetHeaderValue("SlicerAstro.CUNIT3"), "HZ"))
        {
        disNode->SetSpaceQuantity(2,"frequency");
        }
      }
    if (!strcmp(reader->GetHeaderValue("SlicerAstro.BUNIT"), "W.U."))
      {
      volNode->SetAttribute("SlicerAstro.BUNIT", "JY/BEAM");
      vtkWarningMacro("The flux unit of Volume "<<volNode->GetName()<<
                      " is in Westerbork Unit. It will be automatically converted in JY/BEAM"<<endl);
      }
    }
  else if ( refNode->IsA("vtkMRMLAstroLabelMapVolumeNode") )
    {
    // set volume attributes
    vtkMatrix4x4* mat = reader->GetRasToIjkMatrix();
    labvolNode->SetRASToIJKMatrix(mat);
    //set WCSstruct
    labvolNode->SetWCSStruct(reader->GetWCSStruct());
    // parse WCS Status
    labvolNode->SetWCSStatus(reader->GetWCSStatus());
    // parse non-specific key-value pairs
    std::vector<std::string> keys = reader->GetHeaderKeysVector();
    for ( std::vector<std::string>::iterator kit = keys.begin();
          kit != keys.end(); ++kit)
      {
      labvolNode->SetAttribute((*kit).c_str(), reader->GetHeaderValue((*kit).c_str()));
      }
      // parse Space (WCS or IJK) to the display node
    if (labdisNode)
      {
      if (labvolNode->GetWCSStatus() != 0)
        {
        labdisNode->SetSpace("IJK");
        }
      if(!strcmp(reader->GetHeaderValue("SlicerAstro.CUNIT3"), "HZ"))
        {
        labdisNode->SetSpaceQuantity(2,"frequency");
        }
      }
    if (!strcmp(reader->GetHeaderValue("SlicerAstro.BUNIT"), "W.U."))
      {
      labvolNode->SetAttribute("SlicerAstro.BUNIT", "JY/BEAM");
      vtkWarningMacro("THe flux unit of Volume "<<labvolNode->GetName()<<
                      " is in Westerbork Unit. It will be automatically converted in JY/BEAM"<<endl);
      }
    }
  // rescaling flux and calculating max and min
  if(!strcmp(reader->GetHeaderValue("SlicerAstro.DATAMAX"), "0.") ||
       !strcmp(reader->GetHeaderValue("SlicerAstro.DATAMIN"), "0.") ||
       !strcmp(reader->GetHeaderValue("SlicerAstro.BUNIT"), "W.U."))
    {
    vtkImageData *imageData = reader->GetOutput();
    int vtkType = reader->GetDataType();
    int *dims = imageData->GetDimensions();
    const int numComponents = imageData->GetNumberOfScalarComponents();
    double max = imageData->GetScalarTypeMin();
    double min = imageData->GetScalarTypeMax();
    const int numElements = dims[0] * dims[1] * dims[2] * numComponents;

    switch (vtkType)
      {
      case VTK_DOUBLE:
        double *dPixel;
        dPixel = static_cast<double*>(imageData->GetScalarPointer(0,0,0));
        if (!strcmp(reader->GetHeaderValue("SlicerAstro.BUNIT"), "W.U."))
          {
          for( int elemCnt = 0; elemCnt < numElements; elemCnt++)
            {
            *(dPixel+elemCnt) *= 0.005;
            }
          }

        for( int elemCnt = 0; elemCnt < numElements; elemCnt++ )
          {
          if(*(dPixel+elemCnt) > max)
            {
            max = *(dPixel+elemCnt);
            }
          if(*(dPixel+elemCnt) < min)
            {
            min = *(dPixel+elemCnt);
            }
          }
        break;
      case VTK_FLOAT:
        float *fPixel;
        fPixel = static_cast<float*>(imageData->GetScalarPointer(0,0,0));
        if (!strcmp(reader->GetHeaderValue("SlicerAstro.BUNIT"), "W.U."))
          {
          for( int elemCnt = 0; elemCnt < numElements; elemCnt++)
            {
            *(fPixel+elemCnt) *= 0.005;
            }
          }

        for( int elemCnt = 0; elemCnt < numElements; elemCnt++ )
          {
          if(*(fPixel+elemCnt) > max)
            {
            max = *(fPixel+elemCnt);
            }
          if(*(fPixel+elemCnt) < min)
            {
            min = *(fPixel+elemCnt);
            }
          }
        break;
      default:
        vtkErrorMacro("Could not get the data pointer.");
        return 0;
      }
    if ( refNode->IsA("vtkMRMLAstroVolumeNode") )
      {
      volNode->SetAttribute("SlicerAstro.DATAMAX", DoubleToString(max).c_str());
      volNode->SetAttribute("SlicerAstro.DATAMIN", DoubleToString(min).c_str());
      }
    else if ( refNode->IsA("vtkMRMLAstroLabelMapVolumeNode") )
      {
      labvolNode->SetAttribute("SlicerAstro.DATAMAX", DoubleToString(max).c_str());
      labvolNode->SetAttribute("SlicerAstro.DATAMIN", DoubleToString(min).c_str());
      }
    }

  vtkNew<vtkImageChangeInformation> ici;
#if (VTK_MAJOR_VERSION <= 5)
  ici->SetInput (reader->GetOutput());
#else
  ici->SetInputConnection(reader->GetOutputPort());
#endif
  ici->SetOutputSpacing( 1, 1, 1 );
  ici->SetOutputOrigin( 0, 0, 0 );
  ici->Update();

  if ( refNode->IsA("vtkMRMLAstroVolumeNode") )
    {
#if (VTK_MAJOR_VERSION <= 5)
  volNode->SetAndObserveImageData (ici->GetOutput());
#else
  volNode->SetImageDataConnection(ici->GetOutputPort());
#endif
    }
  else if ( refNode->IsA("vtkMRMLAstroLabelMapVolumeNode") )
    {
#if (VTK_MAJOR_VERSION <= 5)
  labvolNode->SetAndObserveImageData (ici->GetOutput());
#else
  labvolNode->SetImageDataConnection(ici->GetOutputPort());
#endif
    }

  return 1;
}

//----------------------------------------------------------------------------
int vtkMRMLAstroVolumeStorageNode::WriteDataInternal(vtkMRMLNode *refNode)
{
  vtkMRMLVolumeNode *volNode = NULL;

  if ( refNode->IsA("vtkMRMLAstroVolumeNode") )
    {
    volNode = vtkMRMLAstroVolumeNode::SafeDownCast(refNode);
    }
  else if ( refNode->IsA("vtkMRMLAstroLabelMapVolumeNode") )
    {
    volNode = vtkMRMLAstroLabelMapVolumeNode::SafeDownCast(refNode);
    }
  else if ( refNode->IsA("vtkMRMLVolumeNode") )
    {
    // Generic case used for any VolumeNode. Used when Extensions add
    // node types that are subclasses of VolumeNode.
    volNode = vtkMRMLVolumeNode::SafeDownCast(refNode);
    }
  else
    {
    vtkErrorMacro(<< "WriteData: Do not recognize node type " << refNode->GetClassName());
    return 0;
    }

  if (volNode->GetImageData() == NULL)
    {
    vtkErrorMacro("WriteData: Cannot write NULL ImageData");
    }

  std::string fullName = this->GetFullNameFromFileName();
  if (fullName == std::string(""))
    {
    vtkErrorMacro("WriteData: File name not specified");
    return 0;
    }

  // Use here the FITS Writer
  vtkNew<vtkFITSWriter> writer;
  writer->SetFileName(fullName.c_str());
#if (VTK_MAJOR_VERSION <= 5)
  writer->SetInput(volNode->GetImageData() );
#else
  writer->SetInputConnection(volNode->GetImageDataConnection());
#endif
  writer->SetUseCompression(this->GetUseCompression());

  // pass down all MRML attributes
  std::vector<std::string> attributeNames = volNode->GetAttributeNames();
  std::vector<std::string>::iterator ait = attributeNames.begin();
  for (; ait != attributeNames.end(); ++ait)
    {
    writer->SetAttribute((*ait), volNode->GetAttribute((*ait).c_str()));
    }

  writer->Write();
  int writeFlag = 1;
  if (writer->GetWriteError())
    {
    vtkErrorMacro("ERROR writing FITS file " << (writer->GetFileName() == NULL ? "null" : writer->GetFileName()));
    writeFlag = 0;
    }

  this->StageWriteData(refNode);

  return writeFlag;
}

//----------------------------------------------------------------------------
void vtkMRMLAstroVolumeStorageNode::InitializeSupportedReadFileTypes()
{
  this->SupportedReadFileTypes->InsertNextValue("FITS (.fits)");
}

//----------------------------------------------------------------------------
void vtkMRMLAstroVolumeStorageNode::InitializeSupportedWriteFileTypes()
{
  this->SupportedWriteFileTypes->InsertNextValue("FITS (.fits)");
}

//----------------------------------------------------------------------------
const char* vtkMRMLAstroVolumeStorageNode::GetDefaultWriteFileExtension()
{
  return "fits";
}

//----------------------------------------------------------------------------
void vtkMRMLAstroVolumeStorageNode::ConfigureForDataExchange()
{
  this->UseCompressionOff();
}
