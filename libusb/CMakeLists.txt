cmake_minimum_required(VERSION 3.9)

# This compiles libusb as a static library and provides it as a CMake target.
# Much cleaner and better than trying to use autotools or whatever nonsense they do.

set(src_files
	"libusb/libusb/core.c"
	"libusb/libusb/descriptor.c"
	"libusb/libusb/hotplug.c"
	"libusb/libusb/hotplug.h"
	"libusb/libusb/io.c"
	"libusb/libusb/libusb.h"
	"libusb/libusb/libusbi.h"
	"libusb/libusb/strerror.c"
	"libusb/libusb/sync.c"
	"libusb/libusb/version.h"
	"libusb/libusb/version_nano.h"
)

if (WIN32)
	list(APPEND src_files
		"libusb/libusb/os/poll_windows.c"
		"libusb/libusb/os/poll_windows.h"
		"libusb/libusb/os/threads_windows.c"
		"libusb/libusb/os/threads_windows.h"
		"libusb/libusb/os/windows_common.h"
		"libusb/libusb/os/windows_nt_common.c"
		"libusb/libusb/os/windows_nt_common.h"
		"libusb/libusb/os/windows_nt_shared_types.h"
		"libusb/libusb/os/windows_usbdk.c"
		"libusb/libusb/os/windows_usbdk.h"
		"libusb/libusb/os/windows_winusb.c"
		"libusb/libusb/os/windows_winusb.h"
	)
elseif (APPLE)
	list(APPEND src_files
		"libusb/libusb/os/poll_posix.c"
		"libusb/libusb/os/poll_posix.h"
		"libusb/libusb/os/darwin_usb.c"
		"libusb/libusb/os/darwin_usb.h"
		"libusb/libusb/os/threads_posix.c"
		"libusb/libusb/os/threads_posix.h"
	)
elseif (UNIX)
	list(APPEND src_files
		"libusb/libusb/os/poll_posix.c"
		"libusb/libusb/os/poll_posix.h"
		"libusb/libusb/os/linux_udev.c"
		"libusb/libusb/os/linux_usbfs.c"
		"libusb/libusb/os/linux_usbfs.h"
		"libusb/libusb/os/threads_posix.c"
		"libusb/libusb/os/threads_posix.h"
	)
endif ()

add_library(libusb STATIC
	${src_files}
)

target_include_directories(libusb PRIVATE "libusb/libusb")

# For config.h
if (WIN32)
	target_include_directories(libusb PRIVATE "libusb/msvc")
elseif (APPLE)
	target_include_directories(libusb PRIVATE "libusb/Xcode")
	target_link_libraries(libusb "-framework CoreFoundation" "-framework IOKit")
elseif (UNIX)

endif ()


