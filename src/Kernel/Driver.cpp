#include <ntddk.h>
#include <wdf.h>

#define IOCTL_TEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1337, METHOD_BUFFERED, FILE_ANY_ACCESS)

UNICODE_STRING DeviceName, SymbolicLinkName;
PDEVICE_OBJECT DeviceObject;

NTSTATUS FUNC_IRP_FILTER(PDEVICE_OBJECT DriverObject, PIRP Irp)
{
	UNREFERENCED_PARAMETER(DriverObject);
	UNREFERENCED_PARAMETER(Irp);

	return STATUS_SUCCESS;
}

NTSTATUS FUNC_IRP_MJ_DEVICE_CONTROL(PDEVICE_OBJECT DriverObject, PIRP Irp)
{
	UNREFERENCED_PARAMETER(DriverObject);

	PIO_STACK_LOCATION pIoStackLocation = IoGetCurrentIrpStackLocation(Irp);
	
	void* buffer = Irp->AssociatedIrp.SystemBuffer;
	char* msg = "Hello from kernel";
	
	DbgPrint("IRP_MJ_DEVICE_CONTROL called.\n");

	switch (pIoStackLocation->Parameters.DeviceIoControl.IoControlCode)
	{
	case IOCTL_TEST:
		DbgPrint("Message received: %s\n", buffer);

		RtlZeroMemory(buffer, pIoStackLocation->Parameters.DeviceIoControl.InputBufferLength);
		RtlCopyMemory(buffer, msg, strlen(msg));

		Irp->IoStatus.Information = strlen(msg);
		Irp->IoStatus.Status = STATUS_SUCCESS;

		break;
	default:
		Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
		break;
	}

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	DbgPrint("Request completed.\n");

	return Irp->IoStatus.Status;
}

void DriverUnload(PDRIVER_OBJECT DriverObject)
{
	IoDeleteSymbolicLink(&SymbolicLinkName);
	IoDeleteDevice(DriverObject->DeviceObject);

	DbgPrint("Driver unloaded.\n");
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);

	NTSTATUS status;

	DbgPrint("Entry point called.\n");

	RtlInitUnicodeString(&DeviceName, L"\\Device\\ExampleKernelDriver");
	RtlInitUnicodeString(&SymbolicLinkName, L"\\DosDevices\\ExampleKernelDriver");

	status = IoCreateDevice(DriverObject,
		0,
		&DeviceName,
		FILE_DEVICE_UNKNOWN,
		FILE_DEVICE_SECURE_OPEN,
		FALSE,
		&DeviceObject);

	if (!NT_SUCCESS(status))
	{
		DbgPrint("IoCreateDevice failed.\n");
		return status;
	}

	status = IoCreateSymbolicLink(&SymbolicLinkName, &DeviceName);

	if (!NT_SUCCESS(status))
	{
		DbgPrint("IoCreateSymbolicLink failed.\n");
		return status;
	}

	// We need to implement IRP_MJ_CREATE otherwise CreateFileW in usermode fails with error code 1
	for (auto i = 0u; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
		DriverObject->MajorFunction[i] = FUNC_IRP_FILTER;

	DriverObject->DriverUnload = DriverUnload;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = FUNC_IRP_MJ_DEVICE_CONTROL;

	DbgPrint("Initialized driver.\n");

	return STATUS_SUCCESS;
}
