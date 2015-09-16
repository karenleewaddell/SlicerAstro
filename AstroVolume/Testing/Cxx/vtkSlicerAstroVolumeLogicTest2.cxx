// AstroVolume logic
#include "vtkSlicerVolumesLogic.h"

// MRML includes
#include <vtkMRMLVectorVolumeNode.h>
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkAlgorithmOutput.h>
#include <vtkDataSetAttributes.h>
#include <vtkImageData.h>
#include <vtkNew.h>
#include <vtkStringArray.h>
#include <vtkSmartPointer.h>
#include <vtkTrivialProducer.h>
#include <vtkStreamingDemandDrivenPipeline.h>

// ITK includes
#include "itkImage.h"
#include "itkRGBPixel.h"
#include "itkImageFileWriter.h"

// STD includes
#include <sstream>
#include <vector>
#include <string>

// ITK includes
#include <itkConfigure.h>
#include <itkFactoryRegistration.h>

//-----------------------------------------------------------------------------
#if VTK_MAJOR_VERSION <= 5
bool isImageDataValid(vtkImageData* imageData)
{
  if (!imageData)
    {
    return false;
    }
  imageData->GetProducerPort();
  vtkInformation* info = imageData->GetPipelineInformation();
  info = imageData->GetPipelineInformation();
#else
bool isImageDataValid(vtkAlgorithmOutput* imageDataConnection)
{
  if (!imageDataConnection ||
      !imageDataConnection->GetProducer())
    {
    std::cout << "No image data port" << std::endl;
    return false;
    }
  imageDataConnection->GetProducer()->Update();
  vtkInformation* info =
    imageDataConnection->GetProducer()->GetOutputInformation(0);
#endif
  if (!info)
    {
    std::cout << "No output information" << std::endl;
    return false;
    }
  vtkInformation *scalarInfo = vtkDataObject::GetActiveFieldInformation(info,
    vtkDataObject::FIELD_ASSOCIATION_POINTS, vtkDataSetAttributes::SCALARS);
  if (!scalarInfo)
    {
    std::cout << "No scalar information" << std::endl;
    return false;
    }
  return true;
}

//-----------------------------------------------------------------------------
int main( int argc, char * argv[] )
{
  itk::itkFactoryRegistration();

  if(argc<2)
    {
    std::cerr << "Usage: <testName> <temporary dir path>" << std::endl;
    return EXIT_FAILURE;
    }

  typedef itk::Image<itk::RGBPixel<unsigned char>,3 > ImageType;
  typedef itk::ImageFileWriter<ImageType> WriterType;

  vtkSmartPointer<vtkStringArray> fileNameList = vtkSmartPointer<vtkStringArray>::New();
  // create two RGB images and save as PNG
  for(int i=0;i<5;i++)
    {

    std::ostringstream sstr;
    sstr << argv[argc-1] << "/rgb_test_image" << i << ".png";
    fileNameList->InsertNextValue(sstr.str().c_str());
    ImageType::Pointer im1 = ImageType::New();

    ImageType::SizeType size;
    ImageType::RegionType region;
    size[0] = 10; size[1] = 10; size[2] = 10;

    region.SetSize(size);
    im1->SetRegions(region);
    im1->Allocate();

    WriterType::Pointer w = WriterType::New();
    w->SetInput(im1);
    w->SetFileName(sstr.str().c_str());
    w->Update();
    }



  vtkNew<vtkMRMLScene> scene;
  vtkNew<vtkSlicerVolumesLogic> logic;

  logic->SetMRMLScene(scene.GetPointer());

  vtkSmartPointer<vtkMRMLVectorVolumeNode> vectorVolume =
    vtkMRMLVectorVolumeNode::SafeDownCast(
      logic->AddArchetypeVolume(fileNameList->GetValue(0), "rgbVolume", 0, fileNameList));

  if (!vectorVolume ||
#if VTK_MAJOR_VERSION <= 5
      !isImageDataValid(vectorVolume->GetImageData()))
#else
      !isImageDataValid(vectorVolume->GetImageDataConnection()))
#endif
    {
    std::cerr << "Failed to load RGB image." << std::endl;
    return EXIT_FAILURE;
    }

  if (vectorVolume && vectorVolume->GetImageData())
    {
    vectorVolume->GetImageData()->Print(std::cerr);
    }

  return EXIT_SUCCESS;
}