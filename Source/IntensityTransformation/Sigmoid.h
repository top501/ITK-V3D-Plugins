/* Sigmoid.h
 * 2010-05-12: create this program by Luis Ibanez
 */


#ifndef __Sigmoid_H__
#define __Sigmoid_H__

//   Apply a Sigmoid transformation to the image intensity.
//

#include <QtGui>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <v3d_interface.h>

class SigmoidPlugin : public QObject, public V3DSingleImageInterface
{
    Q_OBJECT
    Q_INTERFACES(V3DSingleImageInterface)

public:
	SigmoidPlugin() {}
    QStringList menulist() const;
    void processImage(const QString &arg, Image4DSimple *p4DImage, QWidget *parent);

};

class SigmoidDialog : public QDialog
{
    Q_OBJECT
	
public:
    SigmoidDialog(Image4DSimple *p4DImage, QWidget *parent)
	{
		if (! p4DImage) return;
		
		printf("Passing data to data1d\n");
	
		ok     = new QPushButton("OK");
		cancel = new QPushButton("Cancel");
		
		gridLayout = new QGridLayout();
		
	  gridLayout->addWidget(cancel, 0,0); gridLayout->addWidget(ok, 0,1);
		setLayout(gridLayout);
		setWindowTitle(QString("Sigmoid"));
		
		connect(ok,     SIGNAL(clicked()), this, SLOT(accept()));
		connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));
	}
	
	~SigmoidDialog(){}
	
public slots:

		
public:
	QGridLayout *gridLayout;
	
	QPushButton* ok;
	QPushButton* cancel;
};



#endif


