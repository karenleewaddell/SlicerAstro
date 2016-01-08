#ifndef __qSlicerScalarVolumeDisplayWidget_h
#define __qSlicerScalarVolumeDisplayWidget_h

// Qt includes
#include <QWidget>

// CTK includes
#include <ctkVTKObject.h>

// SlicerQt includes
#include <qSlicerWidget.h>

#include "qSlicerVolumesModuleWidgetsExport.h"

class vtkMRMLNode;
class vtkMRMLAstroVolumeDisplayNode;
class vtkMRMLAstroVolumeNode;
class qSlicerScalarVolumeDisplayWidgetPrivate;
class vtkImageData;

/// \ingroup Slicer_QtModules_Volumes
class Q_SLICER_QTMODULES_VOLUMES_WIDGETS_EXPORT qSlicerScalarVolumeDisplayWidget
  : public qSlicerWidget
{
  Q_OBJECT
  QVTK_OBJECT
  Q_PROPERTY(bool enableColorTableComboBox READ isColorTableComboBoxEnabled WRITE setColorTableComboBoxEnabled )
  Q_PROPERTY(bool enableMRMLWindowLevelWidget READ isMRMLWindowLevelWidgetEnabled WRITE setMRMLWindowLevelWidgetEnabled )
public:
  /// Constructors
  typedef qSlicerWidget Superclass;
  explicit qSlicerScalarVolumeDisplayWidget(QWidget* parent);
  virtual ~qSlicerScalarVolumeDisplayWidget();

  vtkMRMLAstroVolumeNode* volumeNode()const;
  vtkMRMLAstroVolumeDisplayNode* volumeDisplayNode()const;
  vtkImageData* volumeImageData()const;

  bool isColorTableComboBoxEnabled()const;
  void setColorTableComboBoxEnabled(bool);

  bool isMRMLWindowLevelWidgetEnabled()const;
  void setMRMLWindowLevelWidgetEnabled(bool);

public slots:

  ///
  /// Set the MRML node of interest
  void setMRMLVolumeNode(vtkMRMLAstroVolumeNode* volumeNode);
  void setMRMLVolumeNode(vtkMRMLNode* node);

  void setInterpolate(bool interpolate);
  void setColorNode(vtkMRMLNode* colorNode);

protected slots:
  void updateWidgetFromMRML();
  void updateColorFunctionFromMRML();
  void updateTransferFunction();

protected:
  void showEvent(QShowEvent * event);
protected:
  QScopedPointer<qSlicerScalarVolumeDisplayWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerScalarVolumeDisplayWidget);
  Q_DISABLE_COPY(qSlicerScalarVolumeDisplayWidget);
};

#endif
