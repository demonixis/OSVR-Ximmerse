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
#define TRACKER_PER_CONTROLLER 1
#define ANALOG_PER_CONTROLLER 2
#define BUTTONS_PER_CONTROLLER 8

namespace
{
	class XimmerseDevice
	{
	public:
		XimmerseDevice(OSVR_PluginRegContext ctx)
		{
			m_controllerState = (ControllerState*)malloc(sizeof(ControllerState));

			m_handles[0] = XDeviceGetInputDeviceHandle("XHawk-0");
			m_handles[1] = XDeviceGetInputDeviceHandle("XCobra-0");
			m_handles[2] = XDeviceGetInputDeviceHandle("XCobra-1");

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
			OSVR_Vec3 position;
			OSVR_Quaternion orientation;
			OSVR_AnalogState analog[ANALOG_PER_CONTROLLER * MAX_CONTROLLERS];
			OSVR_ButtonState buttons[BUTTONS_PER_CONTROLLER * MAX_CONTROLLERS];

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
						osvrDeviceTrackerSendPosition(m_dev, m_tracker, &position, (TRACKER_PER_CONTROLLER * i) + 1);

						SetOsvrQuaternion(&orientation, m_controllerState->rotation);
						osvrDeviceTrackerSendOrientation(m_dev, m_tracker, &orientation, (TRACKER_PER_CONTROLLER * i) + 1);
					}
					// X-Cobras-0 & 1
					else
					{
						analog[ANALOG_PER_CONTROLLER * i + 0] = m_controllerState->axes[CONTROLLER_AXIS_LEFT_THUMB_X];
						analog[ANALOG_PER_CONTROLLER * i + 1] = m_controllerState->axes[CONTROLLER_AXIS_LEFT_THUMB_Y];

						buttons[BUTTONS_PER_CONTROLLER * i + 0] = m_controllerState->buttons && CONTROLLER_BUTTON_LEFT_TRIGGER == 0;
						buttons[BUTTONS_PER_CONTROLLER * i + 1] = m_controllerState->buttons && CONTROLLER_BUTTON_LEFT_SHOULDER == 0;
						buttons[BUTTONS_PER_CONTROLLER * i + 2] = m_controllerState->buttons && CONTROLLER_BUTTON_GUIDE == 0;
						buttons[BUTTONS_PER_CONTROLLER * i + 3] = m_controllerState->buttons && CONTROLLER_BUTTON_BACK == 0;
						buttons[BUTTONS_PER_CONTROLLER * i + 4] = m_controllerState->buttons && CONTROLLER_BUTTON_DPAD_UP == 0;
						buttons[BUTTONS_PER_CONTROLLER * i + 5] = m_controllerState->buttons && CONTROLLER_BUTTON_DPAD_DOWN == 0;
						buttons[BUTTONS_PER_CONTROLLER * i + 6] = m_controllerState->buttons && CONTROLLER_BUTTON_DPAD_LEFT == 0;
						buttons[BUTTONS_PER_CONTROLLER * i + 7] = m_controllerState->buttons && CONTROLLER_BUTTON_DPAD_RIGHT == 0;

						SetOsvrVector3(&position, m_controllerState->position);
						osvrDeviceTrackerSendPosition(m_dev, m_tracker, &position, (TRACKER_PER_CONTROLLER * i) + 1);

						SetOsvrQuaternion(&orientation, m_controllerState->rotation);
						osvrDeviceTrackerSendOrientation(m_dev, m_tracker, &orientation, (TRACKER_PER_CONTROLLER * i) + 1);
					}
				}
			}

			osvrDeviceAnalogSetValues(m_dev, m_analog, analog, ANALOG_PER_CONTROLLER * MAX_CONTROLLERS);
			osvrDeviceButtonSetValues(m_dev, m_button, buttons, BUTTONS_PER_CONTROLLER * MAX_CONTROLLERS);

			return OSVR_RETURN_SUCCESS;
		}

	private:
		void SetOsvrVector3(OSVR_Vec3 *position, float *data)
		{
			osvrVec3SetX(position, data[0]);
			osvrVec3SetY(position, data[1]);
			osvrVec3SetZ(position, data[2]);
		}

		void SetOsvrQuaternion(OSVR_Quaternion *orientation, float *data)
		{
			osvrQuatSetX(orientation, data[0]);
			osvrQuatSetX(orientation, data[1]);
			osvrQuatSetX(orientation, data[2]);
			osvrQuatSetX(orientation, data[3]);
		}

	private:
		osvr::pluginkit::DeviceToken m_dev;
		OSVR_TrackerDeviceInterface m_tracker;
		OSVR_AnalogDeviceInterface m_analog;
		OSVR_ButtonDeviceInterface m_button;
		int m_handles[3];
		ControllerState *m_controllerState;
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