#include "diginavisplugin.h"

#include "diginavis.h"

DiginavisPlugin::DiginavisPlugin()
{
}

QObject *DiginavisPlugin::createControl()
{
    return new Diginavis();
}
