#include <osvr/PluginKit/PluginKit.h>
#include <osvr/PluginKit/TrackerInterfaceC.h>
#include "net_demonixis_ximmerse_json.h"
#include <xdevice.h>

namespace
{
	class XimmerseDevice
	{
	public:
		XimmerseDevice(OSVR_PluginRegContext ctx)
		{
			OSVR_DeviceInitOptions opts = osvrDeviceCreateInitOptions(ctx);
			osvrDeviceTrackerConfigure(opts, &m_tracker);
			m_deviceToken.initAsync(ctx, "Ximmerse Outside/In Controllers", opts);
			m_deviceToken.sendJsonDescriptor(net_demonixis_ximmerse_json);
			m_deviceToken.registerUpdateCallback(this);
		}

		~XimmerseDevice()
		{
		}

		OSVR_ReturnCode update()
		{
			return OSVR_RETURN_SUCCESS;
		}

	private:
		osvr::pluginkit::DeviceToken m_deviceToken;
		OSVR_TrackerDeviceInterface m_tracker;
	};

	class XimmerseManager
	{
	public:
		XimmerseManager() : m_initialized(false) { }

		~XimmerseManager()
		{

		}

		OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx)
		{
			if (!m_initialized)
			{
				int init = -1;
				if (init == 0)
				{
					m_initialized = true;
					osvr::pluginkit::registerObjectForDeletion(ctx, new XimmerseDevice(ctx));
				}
				else
					return OSVR_RETURN_FAILURE;
			}

			return OSVR_RETURN_SUCCESS;
		}

	private:
		bool m_initialized;
	};
}

OSVR_PLUGIN(net_demonixis_ximmerse)
{
	osvr::pluginkit::PluginContext context(ctx);
	context.registerHardwareDetectCallback(new XimmerseManager());
	return OSVR_RETURN_SUCCESS;
}