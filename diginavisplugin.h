#ifndef DIGINAVISPLUGIN_H
#define DIGINAVISPLUGIN_H

#include "App/PluginInterface.h"

class DiginavisPlugin: public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.uavos.gcs.PluginInterface/1.0")
    Q_INTERFACES(PluginInterface)
public:
    DiginavisPlugin();
    QObject *createControl();
};

#endif // DIGINAVISPLUGIN_H
