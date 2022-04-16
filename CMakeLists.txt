cmake_minimum_required(VERSION 3.16)

set(CMAKE_OSX_DEPLOYMENT_TARGET 10.14)

project(Robocut
	VERSION 1.1.0
	DESCRIPTION "A program to control Graphtec vinyl cutters."
	LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Set the CMAKE_PREFIX_PATH environment variable to (e.g) ~/Qt/5.8/clang_64 so it can find Qt.
# CoreTools is for win/macdeployqt
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Svg CoreTools)

qt_standard_project_setup()
set(CMAKE_AUTORCC TRUE)

add_subdirectory(libusb)

set(src_files
	"Bezier.cpp"
	"Bezier.h"
	"CommandLineOptions.cpp"
	"CommandLineOptions.h"
	"Common.cpp"
	"Common.h"
	"CutDialog.cpp"
	"CutDialog.h"
	"CutDialog.ui"
	"CuttingDialog.cpp"
	"CuttingDialog.h"
	"CuttingDialog.ui"
	"CuttingThread.cpp"
	"CuttingThread.h"
	"FileData.cpp"
	"FileData.h"
	"MeasureItem.cpp"
	"MeasureItem.h"
	"MainWindow.cpp"
	"MainWindow.h"
	"MainWindow.ui"
	"PathPaintDevice.cpp"
	"PathPaintDevice.h"
	"PathPaintEngine.cpp"
	"PathPaintEngine.h"
	"PathScene.cpp"
	"PathScene.h"
	"PathSorter.cpp"
	"PathSorter.h"
	"Plotter.cpp"
	"Plotter.h"
	"Resources.qrc"
	"Result.h"
	"SvgPreviewModel.cpp"
	"SvgPreviewModel.h"
	"SvgRenderer.cpp"
	"SvgRenderer.h"
	"ZoomableGraphicsView.cpp"
	"ZoomableGraphicsView.h"
	"main.cpp"
)

if (WIN32)
	# Windows icon.
	list(APPEND src_files "WindowsResources.rc")

elseif (APPLE)
	# OSX Icon
	# This is the property added to Info.plist
	set(MACOSX_BUNDLE_ICON_FILE "${PROJECT_NAME}.icns")

	# Add the file to the source.
	set(osx_icon_file "${CMAKE_SOURCE_DIR}/icon/${PROJECT_NAME}.icns")
	set_source_files_properties("${osx_icon_file}" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

	list(APPEND src_files "${osx_icon_file}")
endif ()

add_executable(${PROJECT_NAME} MACOSX_BUNDLE WIN32
	${src_files}
)

target_link_libraries(${PROJECT_NAME}
	Qt6::Widgets
	Qt6::Svg
	libusb
)

target_include_directories(${PROJECT_NAME} PRIVATE ".")
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE "nanoflann/include")

target_compile_definitions(${PROJECT_NAME} PRIVATE "-DPROJECT_VERSION=\"${PROJECT_VERSION}\"")

set_target_properties(${PROJECT_NAME} PROPERTIES
	WIN32_EXECUTABLE TRUE
	MACOSX_BUNDLE TRUE
	# A custom Info.plist is used to enable high dpi on OSX.
	MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/MacOSXBundleInfo.plist.in"
)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	# More sensible warnings/errors.
	target_compile_options(${PROJECT_NAME} PRIVATE
		-Weverything
		-Werror
		-pedantic
		-Wno-c++98-compat
		-Wno-c++98-compat-pedantic

		# These are too noisy.
		-Wno-padded
		-Wno-switch-enum
		-Wno-exit-time-destructors
		-Wno-global-constructors
		-Wno-shadow-field-in-constructor

		# Moc causes these.
		-Wno-redundant-parens
	)
endif ()

# Packaging
if (WIN32)
	add_custom_target(pack
		# Make a temporary directory for the zip.
		COMMAND "${CMAKE_COMMAND}" "-E" "make_directory" "${CMAKE_BINARY_DIR}/Zip/${PROJECT_NAME}"

		# Copy the EXE there.
		COMMAND "${CMAKE_COMMAND}" "-E" "copy" "$<TARGET_FILE:${PROJECT_NAME}>"
		        "${CMAKE_BINARY_DIR}/Zip/${PROJECT_NAME}/${PROJECT_NAME}.exe"

		# Deploy Qt to that directory. All the options are to save space.
		COMMAND Qt6::windeployqt "--release" "--compiler-runtime"
		        "--no-opengl-sw" "--no-translations" "--no-system-d3d-compiler"
				"${CMAKE_BINARY_DIR}/Zip/${PROJECT_NAME}/${PROJECT_NAME}.exe"

		DEPENDS ${PROJECT_NAME}
		COMMENT "Deploying Qt"
		VERBATIM
	)

	# Zip the result.
	add_custom_target(artefact
		COMMAND 7z a "${CMAKE_BINARY_DIR}/${PROJECT_NAME}-${PROJECT_VERSION}.zip"
		    "${CMAKE_BINARY_DIR}/Zip/${PROJECT_NAME}"
		DEPENDS pack
		COMMENT "Creating Zip"
		VERBATIM
	)

elseif (APPLE)
	# Note there is a slight bug (https://bugreports.qt.io/browse/QTBUG-60324) in macdeployqt
	# which means we really want the working directory to be the place where the dmg is generated.
	add_custom_target(artefact
		COMMAND Qt6::macdeployqt "${PROJECT_NAME}.app" "-dmg"
		COMMAND "${CMAKE_COMMAND}" "-E" "rename" "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.dmg"
		        "${CMAKE_BINARY_DIR}/${PROJECT_NAME}-${PROJECT_VERSION}.dmg"
		DEPENDS ${PROJECT_NAME}
		WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
		COMMENT "Creating DMG"
		VERBATIM
	)
else ()
	# This is just here for consistency with platforms that don't produce zip or dmg.
	add_custom_Target(artefact
		DEPENDS ${PROJECT_NAME}
		COMMENT "Not generating build artefact"
		VERBATIM
	)

	install(TARGETS ${PROJECT_NAME}
		RUNTIME DESTINATION bin)

endif ()
