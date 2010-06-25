#include <QtGui>

#include <math.h>
#include <stdlib.h>

#include "IntensityWindowing.h"
#include "V3DITKFilterSingleImage.h"

// ITK Header Files
#include "itkIntensityWindowingImageFilter.h"


// Q_EXPORT_PLUGIN2 ( PluginName, ClassName )
// The value of PluginName should correspond to the TARGET specified in the
// plugin's project file.
Q_EXPORT_PLUGIN2(IntensityWindowing, IntensityWindowingPlugin)


QStringList IntensityWindowingPlugin::menulist() const
{
    return QStringList() << QObject::tr("ITK IntensityWindowing")
            << QObject::tr("about this plugin");
}

QStringList IntensityWindowingPlugin::funclist() const
{
    return QStringList();
}


template <typename TPixelType>
class PluginSpecialized : public V3DITKFilterSingleImage< TPixelType, TPixelType >
{
  typedef V3DITKFilterSingleImage< TPixelType, TPixelType >   Superclass;
  typedef typename Superclass::Input3DImageType               ImageType;

  typedef itk::IntensityWindowingImageFilter< ImageType, ImageType > FilterType;

public:

  PluginSpecialized( V3DPluginCallback * callback ): Superclass(callback)
    {
    this->m_Filter = FilterType::New();
    }

  virtual ~PluginSpecialized() {};

  
  void Execute(const QString &menu_name, QWidget *parent)
    {
    this->Compute(); 
    }

  virtual void ComputeOneRegion()
    {

    this->m_Filter->SetInput( this->GetInput3DImage() );

    if( !this->ShouldGenerateNewWindow() )
      {
      this->m_Filter->InPlaceOn();
      }
    
    this->m_Filter->Update();

    this->SetOutputImage( this->m_Filter->GetOutput() );
    }
  
  virtual void SetupParameters()
    {
    // These values should actually be provided by the Qt Dialog...
    this->m_Filter->Factor(1);
    this->m_Filter->Offset(1);
    this->m_Filter->OutputMaximum(1.0);
    this->m_Filter->OutputMinimum(-1.0);
    this->m_Filter->WindowMaximum(50.0);
    this->m_Filter->WindowMinimum(-50.0);
    }

private:

    typename FilterType::Pointer   m_Filter;

};


#define EXECUTE_PLUGING_FOR_ONE_IMAGE_TYPE( v3d_pixel_type, c_pixel_type ) \
  case v3d_pixel_type: \
    { \
    PluginSpecialized< c_pixel_type > runner( &callback ); \
    runner.Execute( menu_name, parent ); \
    break; \
    } 

 
void IntensityWindowingPlugin::dofunc(const QString & func_name,
    const V3DPluginArgList & input, V3DPluginArgList & output, QWidget * parent)
{
  // empty by now
}


void IntensityWindowingPlugin::domenu(const QString & menu_name, V3DPluginCallback & callback, QWidget * parent)
{
  if (menu_name == QObject::tr("about this plugin"))
    {
    QMessageBox::information(parent, "Version info", "ITK IntensityWindowing 1.0 (2010-Jun-21): this plugin is developed by Luis Ibanez.");
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
