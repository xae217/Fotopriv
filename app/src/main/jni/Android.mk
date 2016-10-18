LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

#opencv
OPENCVROOT:= /home/xae18/dev/opencv/platforms/build_android_arm/install/
OPENCV_CAMERA_MODULES:=on
OPENCV_INSTALL_MODULES:=on
OPENCV_LIB_TYPE:=SHARED
include ${OPENCVROOT}/sdk/native/jni/OpenCV.mk

LOCAL_SRC_FILES := com_xae18_fotopriv_NativeClass.cpp
LOCAL_LDLIBS += -llog
LOCAL_MODULE := MyLib

include $(BUILD_SHARED_LIBRARY)