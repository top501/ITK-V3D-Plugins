#ifndef __UnaryFunctor_H__
#define __UnaryFunctor_H__


#include <QtGui>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <v3d_interface.h>

class UnaryFunctorPlugin : public QObject, public V3DPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(V3DPluginInterface)

public:
	UnaryFunctorPlugin() {}
  QStringList menulist() const;
	QStringList funclist() const;

	void domenu(const QString & menu_name, V3DPluginCallback & callback, QWidget * parent);

	virtual void dofunc(const QString & func_name,
			const V3DPluginArgList & input, V3DPluginArgList & output, QWidget * parent);

};

#endif
