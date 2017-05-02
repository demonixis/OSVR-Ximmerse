// vs2015.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "xdevice.h"
#include <windows.h>



int get_int() {

	int value;
	while (scanf("%d", &value) != 1) {
		while (getchar() != '\n');
		printf("The format is not correct. Please re-enter it.！\n");
	}
	while (getchar() != '\n');
	return value;
}

char get_first() {
	char value;
	scanf("%c", &value);
	while (value == '\n' || value == 32)
	{
		scanf("%c", &value);
		while (getchar() != '\n');
	}
	while (getchar() != '\n');
	return value;
}


int menu() {
	int result;
	printf("*************************************************************\n");
	printf(" Please select：\n");
	printf(" 1.Get Device status\n");
	printf(" 2.Get event, head blob\n");
	printf(" 3.Get event, red controller\n");
	printf(" 4.Get event, green controller\n");
	printf("*************************************************************\n");
	result = get_int();
	while (result > 6 || result < 1) {
		printf("Please input 1-10！\n");
		result = get_int();
	}
	return result;
}


void func001() {
	char ch;

	XDeviceInit();
	int handle[3];
	handle[0] = XDeviceGetInputDeviceHandle("XHawk-0");
	handle[1] = XDeviceGetInputDeviceHandle("XCobra-0");
	handle[2] = XDeviceGetInputDeviceHandle("XCobra-1");

	for (int i = 0; i < 3; i++) {
		if (handle[i] < 0) {
			printf("get Input handle %d error, i\n");
		}
		XDeviceUpdateInputState(handle[i]);
		int status = XDeviceGetInt(handle[i], kField_ConnectionState, 0);
		switch (status) {
			case kConnectionState_Disconnected: printf("device %d is Disconnected\n", i); break;
			case kConnectionState_Scanning:     printf("device %d is Scanning\n", i); break;
			case kConnectionState_Connecting:   printf("device %d is Connecting\n", i); break;
			case kConnectionState_Connected:    printf("device %d is Connected\n", i); break;
			case kConnectionState_Error:        printf("device %d is Error:%d\n", i, XDeviceGetInt(handle[i], kField_ConnectionState, 0)); break;
			default:break;
		}
	}


	

	XDeviceExit();
}

void func002() {
	char ch;
	float position[3];
	XDeviceInit();
	int handle = XDeviceGetInputDeviceHandle("XHawk-0");
	if (handle < 0) {
		printf("get Input handle error\n");
	}

	do {
		XDeviceUpdateInputState(handle);
		int errorCode = XDeviceGetNodePosition(handle, 0, 2, position);
		if (errorCode >= 0) {
			printf("position  %.6f, %.6f, %.6f \n", position[0], position[1], position[2]);
		}
		ch = getchar();
	} while (ch != 'c' && ch != 'C');

	XDeviceExit();
}



void func003() {
	char ch;
	ControllerState *state=(ControllerState*)malloc(sizeof(ControllerState));
	int preTimestamp;

	printf("*************************************************************\n");
	printf(" Press the key：\n");
	printf(" 'c': exit \n");
	printf(" 'r': yaw recenter \n");
	printf(" 's': motor shake \n");
	printf(" 'p': motor stop shake\n");
	printf(" 'b': get battery level\n");
	printf("*************************************************************\n");


	XDeviceInit();
	int handle = XDeviceGetInputDeviceHandle("XCobra-0");
	int handle1 = XDeviceGetInputDeviceHandle("XHawk-0");
	if (handle < 0) {
		printf("get Input handle error\n");
	}

	do {
		XDeviceUpdateInputState(handle1);

		int errorCode = XDeviceGetInputState(handle, state);
	
		if (errorCode >= 0 && state->timestamp!= preTimestamp) {
			printf("position  %.6f, %.6f, %.6f \n", state->position[0], state->position[1], state->position[2]);
			printf("rotation  %.4f, %.4f, %.4f %.4f\n", state->rotation[0], state->rotation[2], state->rotation[3], state->rotation[4]);
			printf("buttons: ");
			printf("%s", (state->buttons & CONTROLLER_BUTTON_START)==0?"":"START ");
			printf("%s", (state->buttons & CONTROLLER_BUTTON_BACK) == 0 ? "" : "BACK ");
			printf("%s", (state->buttons & CONTROLLER_BUTTON_LEFT_SHOULDER) == 0 ? "" : "LEFT_SHOULDER ");
			printf("%s", (state->buttons & CONTROLLER_BUTTON_RIGHT_SHOULDER) == 0 ? "" : "RIGHT_SHOULDER ");
			printf("%s", (state->buttons & CONTROLLER_BUTTON_LEFT_THUMB) == 0 ? "" : "LEFT_THUMB ");
			printf("\n");
			printf("triggler  %.4f\n", state->axes[CONTROLLER_AXIS_LEFT_TRIGGER]);
			printf("touchPad  %.4f, %4f\n", state->axes[CONTROLLER_AXIS_LEFT_THUMB_X],state->axes[CONTROLLER_AXIS_LEFT_THUMB_Y]);
			preTimestamp = state->timestamp;
		}

		ch = getchar();
		if (ch == 'r' || ch == 'R') {
			XDeviceSendMessage(handle, kMessage_RecenterSensor, 0, 0);
		}else if (ch == 's' || ch == 'S') {
			int strength = 50; //(20~100) %
			int frequency = 0; //(default 0)
			int duration =  0; //ms 
			XDeviceSendMessage(handle, kMessage_TriggerVibration,
							  (int)((strength <= 0 ? 20 : strength) | ((frequency << 16) & 0xFFFF0000)),
							  (int)(duration * 1000));
		}else if (ch == 'p' || ch == 'P') {
			XDeviceSendMessage(handle, kMessage_TriggerVibration, -1, 0);
		}else if (ch == 'b' || ch == 'B') {
			printf("\n device  battery level %d\n\n", XDeviceGetInt(handle,kField_BatteryLevel,0));
		}


	} while (ch != 'c' && ch != 'C');

	XDeviceExit();
	free(state);
}


void func004() {
	char ch;
	ControllerState *state = (ControllerState*)malloc(sizeof(ControllerState));
	int preTimestamp;
	XDeviceInit();
	int handle = XDeviceGetInputDeviceHandle("XCobra-1");
	int handle1 = XDeviceGetInputDeviceHandle("XHawk-0");
	if (handle < 0) {
		printf("get Input handle error\n");
	}

	do {
		XDeviceUpdateInputState(handle1);
		int errorCode = XDeviceGetInputState(handle, state);
		if (errorCode >= 0 && state->timestamp != preTimestamp) {
			printf("position  %.6f, %.6f, %.6f \n", state->position[0], state->position[1], state->position[2]);
			printf("rotation  %.4f, %.4f, %.4f %.4f\n", state->rotation[0], state->rotation[2], state->rotation[3], state->rotation[4]);
			printf("buttons: ");
			printf("%s", (state->buttons & CONTROLLER_BUTTON_START) == 0 ? "" : "START ");
			printf("%s", (state->buttons & CONTROLLER_BUTTON_BACK) == 0 ? "" : "BACK ");
			printf("%s", (state->buttons & CONTROLLER_BUTTON_LEFT_SHOULDER) == 0 ? "" : "LEFT_SHOULDER ");
			printf("%s", (state->buttons & CONTROLLER_BUTTON_RIGHT_SHOULDER) == 0 ? "" : "RIGHT_SHOULDER ");
			printf("%s", (state->buttons & CONTROLLER_BUTTON_LEFT_THUMB) == 0 ? "" : "LEFT_THUMB ");
			printf("\n");
			printf("triggler  %.4f\n", state->axes[CONTROLLER_AXIS_LEFT_TRIGGER]);
			printf("touchPad  %.4f, %4f\n", state->axes[CONTROLLER_AXIS_LEFT_THUMB_X], state->axes[CONTROLLER_AXIS_LEFT_THUMB_Y]);
			preTimestamp = state->timestamp;
		}
		preTimestamp = state->timestamp;
		ch = getchar();
		if (ch == 'r' || ch == 'R') {
			XDeviceSendMessage(handle, kMessage_RecenterSensor, 0, 0);
		}
		else if (ch == 's' || ch == 'S') {
			int strength = 50; //(20~100) %
			int frequency = 0; //(default 0)
			int duration = 0; //ms 
			XDeviceSendMessage(handle, kMessage_TriggerVibration,
				(int)((strength <= 0 ? 20 : strength) | ((frequency << 16) & 0xFFFF0000)),
				(int)(duration * 1000));
		}
		else if (ch == 'p' || ch == 'P') {
			XDeviceSendMessage(handle, kMessage_TriggerVibration, -1, 0);
		}
		else if (ch == 'b' || ch == 'B') {
			printf("\n device  battery level %d\n\n", XDeviceGetInt(handle, kField_BatteryLevel, 0)); // value range 0~100%
		}

	} while (ch != 'c' && ch != 'C');

	XDeviceExit();
	free(state);

}
int main()
{
	char ch;
	do
	{
		switch (menu())
		{
			case 1: func001(); break;
			case 2: func002(); break;
			case 3: func003(); break;
			case 4: func004(); break;
			default:break;
		}

		do{
			printf("Do you want to continue to operate？(y/n)");
			ch = get_first();
		} while (ch != 'y' && ch != 'Y' && ch != 'n' && ch != 'N');
	} while (ch == 'y' || ch == 'Y');

	return 0;
}

