/* BilateralFilter.cxx
 * 2010-06-02: create this program by Lei Qu
 */

#include <QtGui>

#include <math.h>
#include <stdlib.h>

#include "BilateralFilter.h"

// ITK Header Files
#include "itkImportImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkBilateralImageFilter.h"
#include "itkImageFileWriter.h"

// Q_EXPORT_PLUGIN2 ( PluginName, ClassName )
// The value of PluginName should correspond to the TARGET specified in the
// plugin's project file.
Q_EXPORT_PLUGIN2(BilateralFilter, ITKBilateralFilterPlugin)

QStringList ITKBilateralFilterPlugin::menulist() const
{
	return QStringList() << QObject::tr("ITK Bilateral Filter ...");
}

template<typename TPixelType>
class ITKBilateralFilterSpecializaed
{
public:
	void Execute(const QString &arg, Image4DSimple *p4DImage, QWidget *parent)
	{
		const unsigned int Dimension = 3;

		//------------------------------------------------------------------
		//import image from V3D
		typedef TPixelType PixelType;
		typedef itk::Image< PixelType,  Dimension > ImageType_input;
		typedef itk::ImportImageFilter<PixelType, Dimension> ImportFilterType;

		typename ImportFilterType::Pointer importFilter = ImportFilterType::New();

		//set ROI region
		typename ImportFilterType::RegionType region;
		typename ImportFilterType::IndexType start;
		start.Fill(0);
		typename ImportFilterType::SizeType size;
		size[0] = p4DImage->getXDim();
		size[1] = p4DImage->getYDim();
		size[2] = p4DImage->getZDim();
		region.SetIndex(start);
		region.SetSize(size);
		importFilter->SetRegion(region);

		//set image Origin
		typename ImageType_input::PointType origin;
		origin.Fill(0.0);
		importFilter->SetOrigin(origin);
		//set spacing
		typename ImportFilterType::SpacingType spacing;
		spacing.Fill(1.0);
		importFilter->SetSpacing(spacing);

		//set import image pointer
		PixelType * data1d = reinterpret_cast<PixelType *> (p4DImage->getRawData());
		unsigned long int numberOfPixels = p4DImage->getTotalBytes();
		const bool importImageFilterWillOwnTheBuffer = false;
		importFilter->SetImportPointer(data1d, numberOfPixels,importImageFilterWillOwnTheBuffer);

		//------------------------------------------------------------------
		//setup filter: cast datatype to float for anisotropic process
		typedef itk::Image< float, Dimension >   	ImageType_mid;
		typedef itk::RescaleIntensityImageFilter<ImageType_input, ImageType_mid > RescaleFilterType_input;

		typename RescaleFilterType_input::Pointer rescaler_8u_32f = RescaleFilterType_input::New();
		rescaler_8u_32f->SetOutputMinimum(   0 );
		rescaler_8u_32f->SetOutputMaximum( 255 );

		//------------------------------------------------------------------
		//setup filter: Gradient Anisotropic Diffusion
		typedef itk::BilateralImageFilter<ImageType_mid,ImageType_mid> AniFilterType;
		typename AniFilterType::Pointer filter = AniFilterType::New();

		//set paras
		double rangeSigma = 5.0;
		double domainSigmas[ Dimension ];
		for(unsigned int i=0;i<Dimension;i++)
			domainSigmas[i]=6.0;
		filter->SetDomainSigma( domainSigmas );
		filter->SetRangeSigma( rangeSigma );

		//------------------------------------------------------------------
		//setup filter: cast datatype back to PixelType for output
		typedef itk::RescaleIntensityImageFilter<ImageType_mid,ImageType_input> RescaleFilterType_output;

		typename RescaleFilterType_output::Pointer rescaler_32f_8u = RescaleFilterType_output::New();
		rescaler_32f_8u->SetOutputMinimum(   0 );
		rescaler_32f_8u->SetOutputMaximum( 255 );

		//------------------------------------------------------------------
		//setup filter: write processed image to disk
		typedef itk::ImageFileWriter< ImageType_input >  WriterType;
		typename WriterType::Pointer writer = WriterType::New();
		writer->SetFileName("output.tif");

		//------------------------------------------------------------------
		//build pipeline
		rescaler_8u_32f->SetInput(importFilter->GetOutput());
		filter->SetInput(rescaler_8u_32f->GetOutput());
		rescaler_32f_8u->SetInput(filter->GetOutput());
		writer->SetInput(rescaler_32f_8u->GetOutput());

		//------------------------------------------------------------------
		//update the pixel value
		if (arg == QObject::tr("ITK Bilateral Filter ..."))
		{
			ITKBilateralFilterDialog d(p4DImage, parent);

			if (d.exec() != QDialog::Accepted)
			{
				return;
			}
			else
			{
				try
				{
					writer->Update();
				}
				catch(itk::ExceptionObject &excp)
				{
					std::cerr<<excp<<std::endl;
					return;
				}
			}

		}
		else
		{
			return;
		}

		//------------------------------------------------------------------
		//copy data back to V3D
		typedef itk::ImageRegionConstIterator<ImageType_input> IteratorType;
		IteratorType it(rescaler_32f_8u->GetOutput(), rescaler_32f_8u->GetOutput()->GetRequestedRegion());
		it.GoToBegin();
		while(!it.IsAtEnd())
		{
			*data1d=it.Get();
			++it;
			++data1d;
		}
	}

};

#define EXECUTE( v3d_pixel_type, c_pixel_type ) \
  case v3d_pixel_type: \
    { \
	  ITKBilateralFilterSpecializaed< c_pixel_type > runner; \
    runner.Execute( arg, p4DImage, parent ); \
    break; \
    } 

#define EXECUTE_ALL_PIXEL_TYPES \
    if (! p4DImage) return; \
    ImagePixelType pixelType = p4DImage->getDatatype(); \
    switch( pixelType )  \
      {  \
      EXECUTE( V3D_UINT8, unsigned char );  \
      EXECUTE( V3D_UINT16, unsigned short int );  \
      EXECUTE( V3D_FLOAT32, float );  \
      case V3D_UNKNOWN:  \
        {  \
        }  \
      }  

void ITKBilateralFilterPlugin::processImage(const QString &arg,
		Image4DSimple *p4DImage, QWidget *parent)
{
	EXECUTE_ALL_PIXEL_TYPES;
}