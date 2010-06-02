/* InvertIntensity.h
 * 2010-05-12: create this program by Luis Ibanez
 */


#ifndef __InvertIntensity_H__
#define __InvertIntensity_H__

//   Invert the image intensity.
//

#include <QtGui>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <v3d_interface.h>

class InvertIntensityPlugin : public QObject, public V3DSingleImageInterface
{
    Q_OBJECT
    Q_INTERFACES(V3DSingleImageInterface)

public:
	InvertIntensityPlugin() {}
    QStringList menulist() const;
    void processImage(const QString &arg, Image4DSimple *p4DImage, QWidget *parent);

};

class InvertIntensityDialog : public QDialog
{
    Q_OBJECT
	
public:
    InvertIntensityDialog(Image4DSimple *p4DImage, QWidget *parent)
	{
		if (! p4DImage) return;
		
		printf("Passing data to data1d\n");
	
		ok     = new QPushButton("OK");
		cancel = new QPushButton("Cancel");
		
		gridLayout = new QGridLayout();
		
	  gridLayout->addWidget(cancel, 0,0); gridLayout->addWidget(ok, 0,1);
		setLayout(gridLayout);
		setWindowTitle(QString("Invert Intensity"));
		
		connect(ok,     SIGNAL(clicked()), this, SLOT(accept()));
		connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));
	}
	
	~InvertIntensityDialog(){}
	
public slots:

		
public:
	QGridLayout *gridLayout;
	
	QPushButton* ok;
	QPushButton* cancel;
};



#endif


