#include <QtGui>

#include <math.h>
#include <stdlib.h>

#include "ITKFastMarching.h"
#include "V3DITKFilterSingleImage.h"

// ITK Header Files
#include "itkFastMarchingImageFilter.h"


// Q_EXPORT_PLUGIN2 ( PluginName, ClassName )
// The value of PluginName should correspond to the TARGET specified in the
// plugin's project file.
Q_EXPORT_PLUGIN2(FastMarching, FastMarchingPlugin)


QStringList FastMarchingPlugin::menulist() const
{
    return QStringList() << QObject::tr("ITK FastMarching")
            << QObject::tr("about this plugin");
}

QStringList FastMarchingPlugin::funclist() const
{
    return QStringList();
}


template <typename TPixelType>
class PluginSpecialized : public V3DITKFilterSingleImage< TPixelType, TPixelType >
{
  typedef V3DITKFilterSingleImage< TPixelType, TPixelType >   Superclass;
  typedef typename Superclass::Input3DImageType               ImageType;

  typedef itk::FastMarchingImageFilter< ImageType, ImageType > FilterType;

public:

  PluginSpecialized( V3DPluginCallback * callback ): Superclass(callback)
    {
    this->m_Filter = FilterType::New();
    this->RegisterInternalFilter( this->m_Filter, 1.0 );
    }

  virtual ~PluginSpecialized() {};


  void Execute(const QString &menu_name, QWidget *parent)
    {
    V3DITKGenericDialog dialog("FastMarching");

    dialog.AddDialogElement("StoppingTime",50.0, 0.0, 10000.0);

    const double stoppingTime = dialog.GetValue("StoppingTime");

    const double seedValue = 0.0;

    if( dialog.exec() != QDialog::Accepted )
      {
      return;
      }

    this->m_Filter->SetStoppingValue( stoppingTime );

    typedef typename FilterType::NodeContainer  NodeContainer;
    typedef typename FilterType::NodeType       NodeType;

    typename NodeContainer::Pointer seedPoints = NodeContainer::New();

    NodeType node;

    seedPoints->Initialize();

    v3dhandleList windowList = this->m_V3DPluginCallback->getImageWindowList();

    LandmarkList listOfLandmarks = this->m_V3DPluginCallback->getLandmark( windowList[0] ); // FIXME

    unsigned int numberOfSeedPoints = listOfLandmarks.size();

    if ( ! numberOfSeedPoints )
      {
      v3d_msg(QObject::tr("You should select one seed from your image."));
      return;
      }

    typedef itk::Image< TPixelType, 3 > OutputImageType; // FIXME
    typename OutputImageType::IndexType  seedPosition;

    for(unsigned int i = 0;  i < numberOfSeedPoints; i++ )
      {
      seedPosition[0] = listOfLandmarks[i].x - 1; // notice 0-based and 1-based difference
      seedPosition[1] = listOfLandmarks[i].y - 1;
      seedPosition[2] = listOfLandmarks[i].z - 1;

      node.SetValue( seedValue );
      node.SetIndex( seedPosition );

      seedPoints->InsertElement( i, node );
      }

    this->m_Filter->SetTrialPoints( seedPoints );

    this->Compute();
    }

  virtual void ComputeOneRegion()
    {

    this->m_Filter->SetInput( this->GetInput3DImage() );

    this->m_Filter->Update();

    this->SetOutputImage( this->m_Filter->GetOutput() );
    }


private:

    typename FilterType::Pointer   m_Filter;

};


#define EXECUTE_PLUGIN_FOR_ONE_IMAGE_TYPE( v3d_pixel_type, c_pixel_type ) \
  case v3d_pixel_type: \
    { \
    PluginSpecialized< c_pixel_type > runner( &callback ); \
    runner.Execute( menu_name, parent ); \
    break; \
    }


void FastMarchingPlugin::dofunc(const QString & func_name,
    const V3DPluginArgList & input, V3DPluginArgList & output, QWidget * parent)
{
  // empty by now
}


void FastMarchingPlugin::domenu(const QString & menu_name, V3DPluginCallback & callback, QWidget * parent)
{
  if (menu_name == QObject::tr("about this plugin"))
    {
    QMessageBox::information(parent, "Version info", "ITK FastMarching 1.0 (2010-Jun-21): this plugin is developed by Luis Ibanez.");
    return;
    }

  v3dhandle curwin = callback.currentImageWindow();
  if (!curwin)
    {
    v3d_msg(tr("You don't have any image open in the main window."));
    return;
    }

  Image4DSimple *p4DImage = callback.getImage(curwin);
  if (! p4DImage)
    {
    v3d_msg(tr("The input image is null."));
    return;
    }

  EXECUTE_PLUGIN_FOR_ALL_PIXEL_TYPES;
}

