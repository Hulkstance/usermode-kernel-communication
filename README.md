# Usermode Kernel Communication
Simple communication between usermode and kernel.

# How to run
1. Install and setup a virtual machine, for example: VMware
2. Enable `Virtualize Intel VT-x/EPT or AMD-V/RVI`

![Virtual Machine Settings](/screenshots/settings.png)

3. If your driver is not signed, you should enable test mode: `bcdedit /set testsigning on`
4. Use [OSR Loader](osronline.com) to load the kernel driver
5. Open the usermode application and that's it

# How to debug
You can use [DbgView](https://docs.microsoft.com/en-us/sysinternals/downloads/debugview) or [WinDbg](https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/debugger-download-tools). The advantage of WinDbg is that you know where it crashes.

If you decide to use WinDbg, follow the following steps: https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/setting-up-a-network-debugging-connection

![result](/screenshots/result.gif)

# DbgView

In case you want to use DbgView, you have to create the following registry key: `Computer\HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\Debug Print Filter`. Add a new DWORD value `DEFAULT` and set its Data field to `0xF`. You can then open DbgView, enable `Capture Kernel` and `Pass-Through options` under Capture and then restart DbgView. It's done.

![DbgView settings](/screenshots/dbgview-settings.png)
