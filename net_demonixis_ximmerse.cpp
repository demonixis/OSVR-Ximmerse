// Internal Includes
#include <osvr/PluginKit/PluginKit.h>
#include <osvr/PluginKit/TrackerInterfaceC.h>
#include <osvr/PluginKit/AnalogInterfaceC.h>
#include <osvr/PluginKit/ButtonInterfaceC.h>

// Generated JSON header file
#include "net_demonixis_ximmerse_json.h"

// Library/third-party Includes
#include "xdevice.h"

// Standard Includes

#define MAX_CONTROLLERS 2
#define ANALOG_PER_CONTROLLER 2
#define BUTTONS_PER_CONTROLLER 8

namespace
{
	class XimmerseDevice
	{
	public:
		XimmerseDevice(OSVR_PluginRegContext ctx)
		{
			// Ximmerse Setup
			m_controllerState = (ControllerState*)malloc(sizeof(ControllerState));
			m_handles[0] = XDeviceGetInputDeviceHandle("XHawk-0");
			m_handles[1] = XDeviceGetInputDeviceHandle("XCobra-0");
			m_handles[2] = XDeviceGetInputDeviceHandle("XCobra-1");

			// OSVR Setup
			OSVR_DeviceInitOptions opts = osvrDeviceCreateInitOptions(ctx);
			osvrDeviceTrackerConfigure(opts, &m_tracker);
			osvrDeviceAnalogConfigure(opts, &m_analog, ANALOG_PER_CONTROLLER * MAX_CONTROLLERS);
			osvrDeviceButtonConfigure(opts, &m_button, BUTTONS_PER_CONTROLLER * MAX_CONTROLLERS);
			m_dev.initAsync(ctx, "Ximmerse Outside/In Controllers", opts);
			m_dev.sendJsonDescriptor(net_demonixis_ximmerse_json);
			m_dev.registerUpdateCallback(this);
		}

		~XimmerseDevice()
		{
			free(m_controllerState);
		}

		OSVR_ReturnCode update()
		{
			for (int i = 0; i < 3; i++)
			{
				osvrVec3Zero(&position);
				osvrQuatSetIdentity(&orientation);

				XDeviceUpdateInputState(m_handles[i]);

				if (XDeviceGetInputState(m_handles[i], m_controllerState) >= 0)
				{
					// X-Hawk
					if (i == 0)
					{
						SetOsvrVector3(&position, m_controllerState->position);
						osvrDeviceTrackerSendPosition(m_dev, m_tracker, &position, 0);

						SetOsvrQuaternion(&orientation, m_controllerState->rotation);
						osvrDeviceTrackerSendOrientation(m_dev, m_tracker, &orientation, 0);
					}
					// X-Cobras-0 & 1
					else
					{
						int idx = i - 1;

						osvrDeviceAnalogSetValue(m_dev, m_analog, m_controllerState->axes[CONTROLLER_AXIS_LEFT_THUMB_X], ANALOG_PER_CONTROLLER * idx + 0);
						osvrDeviceAnalogSetValue(m_dev, m_analog, m_controllerState->axes[CONTROLLER_AXIS_LEFT_THUMB_Y], ANALOG_PER_CONTROLLER * idx + 1);

						osvrDeviceButtonSetValue(m_dev, m_button, m_controllerState->buttons && CONTROLLER_BUTTON_LEFT_TRIGGER == 0, BUTTONS_PER_CONTROLLER * idx + 0);
						osvrDeviceButtonSetValue(m_dev, m_button, m_controllerState->buttons && CONTROLLER_BUTTON_LEFT_SHOULDER == 0, BUTTONS_PER_CONTROLLER * idx + 1);
						osvrDeviceButtonSetValue(m_dev, m_button, m_controllerState->buttons && CONTROLLER_BUTTON_GUIDE == 0, BUTTONS_PER_CONTROLLER * idx + 2);
						osvrDeviceButtonSetValue(m_dev, m_button, m_controllerState->buttons && CONTROLLER_BUTTON_BACK == 0, BUTTONS_PER_CONTROLLER * idx + 3);
						osvrDeviceButtonSetValue(m_dev, m_button, m_controllerState->buttons && CONTROLLER_BUTTON_DPAD_UP == 0, BUTTONS_PER_CONTROLLER * idx + 4);
						osvrDeviceButtonSetValue(m_dev, m_button, m_controllerState->buttons && CONTROLLER_BUTTON_DPAD_DOWN == 0, BUTTONS_PER_CONTROLLER * idx + 5);
						osvrDeviceButtonSetValue(m_dev, m_button, m_controllerState->buttons && CONTROLLER_BUTTON_DPAD_LEFT == 0, BUTTONS_PER_CONTROLLER * idx + 6);
						osvrDeviceButtonSetValue(m_dev, m_button, m_controllerState->buttons && CONTROLLER_BUTTON_DPAD_RIGHT == 0, BUTTONS_PER_CONTROLLER * idx + 7);
		
						SetOsvrVector3(&position, m_controllerState->position);
						osvrDeviceTrackerSendPosition(m_dev, m_tracker, &position, i);

						SetOsvrQuaternion(&orientation, m_controllerState->rotation);
						osvrDeviceTrackerSendOrientation(m_dev, m_tracker, &orientation, i);
					}
				}
			}

			return OSVR_RETURN_SUCCESS;
		}

	private:
		void SetOsvrVector3(OSVR_Vec3 *position, float *data)
		{
			osvrVec3SetX(position, data[0]);
			osvrVec3SetY(position, data[1]);
			osvrVec3SetZ(position, data[2]);
		}

		void SetOsvrQuaternion(OSVR_Quaternion *quaternion, float *data)
		{
			osvrQuatSetX(quaternion, data[0]);
			osvrQuatSetX(quaternion, data[1]);
			osvrQuatSetX(quaternion, data[2]);
			osvrQuatSetX(quaternion, data[3]);
		}

	private:
		osvr::pluginkit::DeviceToken m_dev;
		OSVR_TrackerDeviceInterface m_tracker;
		OSVR_AnalogDeviceInterface m_analog;
		OSVR_ButtonDeviceInterface m_button;
		OSVR_Vec3 position;
		OSVR_Quaternion orientation;
		ControllerState *m_controllerState;
		int m_handles[3];
	};

	class XimmerseManager
	{
	public:
		XimmerseManager() : m_initialized(false) { }

		~XimmerseManager()
		{
			XDeviceExit();
		}

		OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx)
		{
			if (!m_initialized)
			{
				int init = XDeviceInit();
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
