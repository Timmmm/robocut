cmake_minimum_required(VERSION 3.9)

project(Robocut
	VERSION 1.0.11
	DESCRIPTION "A program to control Graphtec vinyl cutters."
	LANGUAGES C CXX)

# Set the CMAKE_PREFIX_PATH environment variable to (e.g) ~/Qt/5.8/clang_64 so it can find Qt.
find_package(Qt5 COMPONENTS Core Widgets Svg REQUIRED)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_subdirectory(libusb)

set(src_files
	"main.cpp"
	"MainWindow.cpp"
	"Plotter.cpp"
	"Common.cpp"
	"PathPaintEngine.cpp"
	"CutDialog.cpp"
	"CuttingDialog.cpp"
	"CuttingThread.cpp"
	"PathPaintPage.cpp"
	"PathSorter.cpp"
	"ProgramOptions.cpp"
	"MainWindow.h"
	"Plotter.h"
	"NoCopy.h"
	"Common.h"
	"PathPaintEngine.h"
	"CutDialog.h"
	"CuttingDialog.h"
	"CuttingThread.h"
	"PathPaintPage.h"
	"PathSorter.h"
	"ProgramOptions.h"
	"MainWindow.ui"
	"CutDialog.ui"
	"CuttingDialog.ui"
	"Resources.qrc"
)

if (WIN32)
	# Windows icon.
	list(APPEND src_files "WindowsResources.rc")

elseif (APPLE)
	# OSX Icon
	# This is the property added to Info.plist
	set(MACOSX_BUNDLE_ICON_FILE "Robocut.icns")

	# Add the file to the source.
	set(osx_icon_file "${CMAKE_SOURCE_DIR}/icon/Robocut.icns")
	set_source_files_properties("${osx_icon_file}" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

	list(APPEND src_files "${osx_icon_file}")
endif ()

# Automatically generate moc, UI & Resource Files
set(CMAKE_AUTOMOC TRUE)
set(CMAKE_AUTOUIC TRUE)
set(CMAKE_AUTORCC TRUE)

add_executable(Robocut MACOSX_BUNDLE WIN32
	${src_files}
)

target_link_libraries(Robocut
	Qt5::Widgets
	Qt5::Svg
	libusb
)

target_compile_definitions(Robocut PRIVATE "-DROBOCUT_VERSION=\"${PROJECT_VERSION}\"")

# Packaging
if (WIN32)
	add_custom_target(pack
		# Make a temporary directory for the zip.
		COMMAND "${CMAKE_COMMAND}" "-E" "make_directory" "${CMAKE_BINARY_DIR}/Pack"

		# Copy the EXE there.
		COMMAND "${CMAKE_COMMAND}" "-E" "copy" "$<TARGET_FILE:Robocut>"
		    "${CMAKE_BINARY_DIR}/Pack/Robocut.exe"

		# Deploy Qt to that directory. All the options are to save space.
		COMMAND "${_qt5Core_install_prefix}/bin/windeployqt.exe" "--release" "--compiler-runtime"
		    "--no-angle" "--no-opengl-sw" "--no-translations" "--no-system-d3d-compiler"
			"${CMAKE_BINARY_DIR}/Pack/Robocut.exe"

		DEPENDS Robocut
		COMMENT "Deploying Qt"
		VERBATIM
	)

    # Zip the result. This is a weird way of doing it but the cmake tar command isn't very flexible
    # and this is the only way to avoid getting a directory called 'Pack' in the zip.
    #
    # You can't glob at this point because the files aren't actually there yet.
    set(zip_files
        "${CMAKE_BINARY_DIR}/Pack/platforms/qwindows.dll"
        "${CMAKE_BINARY_DIR}/Pack/libgcc_s_dw2-1.dll"
        "${CMAKE_BINARY_DIR}/Pack/libstdc++-6.dll"
        "${CMAKE_BINARY_DIR}/Pack/libwinpthread-1.dll"
        "${CMAKE_BINARY_DIR}/Pack/Qt5Core.dll"
        "${CMAKE_BINARY_DIR}/Pack/Qt5Gui.dll"
        "${CMAKE_BINARY_DIR}/Pack/Qt5Widgets.dll"
        "${CMAKE_BINARY_DIR}/Pack/Qt5SVG.dll"
        "${CMAKE_BINARY_DIR}/Pack/Robocut.exe"
    )

    add_custom_target(artefact
        COMMAND "${CMAKE_COMMAND}" "-E" "tar" "cfv"
            "${CMAKE_BINARY_DIR}/Robocut-${PROJECT_VERSION}.zip" "--format=zip" "--" ${zip_files}
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/Pack/"
        DEPENDS pack
        COMMENT "Creating Zip"
        VERBATIM
    )

elseif (APPLE)
	# Note there is a slight bug (https://bugreports.qt.io/browse/QTBUG-60324) in macdeployqt
	# which means we really want the working directory to be the place where the dmg is generated.
	add_custom_target(artefact
		COMMAND "${_qt5Core_install_prefix}/bin/macdeployqt" "Robocut.app" "-dmg"
		COMMAND "${CMAKE_COMMAND}" "-E" "rename" "${CMAKE_BINARY_DIR}/Robocut.dmg"
		    "${CMAKE_BINARY_DIR}/Robocut-${PROJECT_VERSION}.dmg"
		DEPENDS Robocut
		WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
		COMMENT "Creating DMG"
		VERBATIM
	)
else ()
	# This is just here for consistency with platforms that don't produce zip or dmg.
	add_custom_Target(artefact
		DEPENDS Robocut
		COMMENT "Not generating build artefact"
		VERBATIM)
endif ()
