#ifndef __FastMarching_H__
#define __FastMarching_H__

#include "V3DITKPluginDefaultHeader.h"

class FastMarchingPlugin : public QObject, public V3DPluginInterface
{
  Q_OBJECT
  Q_INTERFACES(V3DPluginInterface)
  V3DITKPLUGIN_DEFAULT_CLASS_DECLARATION_BODY(FastMarching);
};

#endif
