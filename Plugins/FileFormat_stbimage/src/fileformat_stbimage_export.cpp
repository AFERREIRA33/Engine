#include <fileformat_stbimage.h>

extern "C" __declspec(dllexport) vde::util::PluginBase* CreatePlugin(vde::util::PluginContext* context)
{
	return new FileFormat_stbimage(context->graphicsContext);
}
