#include <fileformat_obj.h>

extern "C" __declspec(dllexport) vde::util::PluginBase* CreatePlugin(vde::util::PluginContext* context)
{
	return new FileFormat_OBJ(context->graphicsContext);
}
