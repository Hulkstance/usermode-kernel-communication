#include <Windows.h>
#include <stdio.h>

#define IOCTL_TEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1337, METHOD_BUFFERED, FILE_ANY_ACCESS)

bool LoadDriver()
{
	const char* msg = "Hello from usermode";
	char buffer[50] = { 0 };

	HANDLE driver = CreateFileW(
		L"\\\\.\\ExampleKernelDriver",
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		nullptr,
		OPEN_EXISTING,
		0,
		INVALID_HANDLE_VALUE);

	if (driver == INVALID_HANDLE_VALUE)
	{
		DWORD error = GetLastError();
		printf("CreateFileW failed. %d\n", error);
		return false;
	}

	if (!DeviceIoControl(
		driver, 
		IOCTL_TEST, 
		const_cast<char*>(msg),
		static_cast<DWORD>(strlen(msg)),
		buffer, 
		sizeof(buffer),
		nullptr, 
		nullptr))
	{
		printf("DeviceIoControl failed.\n");
		return false;
	}

	printf("Buffer: %s\n", buffer);

	CloseHandle(driver);

	return true;
}

int main()
{
	LoadDriver();

	getchar();

	return 0;
}