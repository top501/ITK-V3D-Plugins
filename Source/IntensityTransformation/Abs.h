#ifndef __AbsImageFilter_H__
#define __AbsImageFilter_H__

#include "V3DITKPluginDefaultHeader.h"

class AbsPlugin : public QObject, public V3DPluginInterface
{
  Q_OBJECT
  Q_INTERFACES(V3DPluginInterface)
  V3DITKPLUGIN_DEFAULT_CLASS_DECLARATION_BODY(Abs);
};

#endif
