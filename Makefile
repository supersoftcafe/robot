
TARGET_SYSTEM	:= RASPBERRY_PI
# TARGET_SYSTEM	:= TEST



ifeq ($(TARGET_SYSTEM),RASPBERRY_PI)
PREFIX		:= arm-linux-gnueabihf-
OPTIMIZE	:= -mcpu=arm1176jzf-s
LIBS		:= -lbcm2835 -lbcm_host -lvchiq_arm -lvcos -lopenmaxil -lEGL -lGLESv2
OBJS		:= obj/providers/GPIOPortManager.o

ROOT		:= ../root
PIFS		:= ../pifs
export PATH	:= /home/mbrown/cross/rpi-gcc-4.7.2/bin:$(PATH)
IPATH		:= -I$(ROOT)/include -I$(PIFS)/usr/include -I$(PIFS)/opt/vc/include -I$(PIFS)/opt/vc/include/IL -I$(PIFS)/opt/vc/include/interface/vcos/pthreads -I$(PIFS)/opt/vc/include/interface/vmcs_host/linux
LPATH 		:= -L$(ROOT)/lib -L$(PIFS)/usr/lib -L$(PIFS)/opt/vc/lib
else
PREFIX		:=
OPTIMIZE	:=
LIBS		:= -lomxil-bellagio
OBJS		:= obj/providers/FakePortManager.o
IPATH		:=
LPATH		:=
endif

LIBS		+= -lpthread
OPTIMIZE 	+= -O2
# -flto -O3
# -funroll-loops -ffast-math

CPP		:= $(PREFIX)g++
STRIP		:= $(PREFIX)strip
CFLAGS		:= $(OPTIMIZE) -DTARGET_$(TARGET_SYSTEM) $(IPATH) -c -std=c++0x -Wall
LFLAGS		:= $(OPTIMIZE) $(LPATH) $(LIBS)

# -DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX -D_LINUX -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -U_FORTIFY_SOURCE -DHAVE_LIBOPENMAX=2 -DOMX -DOMX_SKIP64BIT -DUSE_EXTERNAL_OMX -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST -DUSE_VCHIQ_ARM

OBJS +=	obj/interfaces/IPort.o\
	obj/interfaces/IPortManager.o\
	obj/interfaces/IPortTranslator.o\
	obj/interfaces/IMainLoop.o\
	obj/interfaces/IFileWatcher.o\
	obj/interfaces/ITimer.o\
	obj/interfaces/ITime.o\
	obj/interfaces/IEventQueue.o\
	obj/interfaces/IGenericService.o\
	obj/interfaces/ICameraManager.o\
	obj/interfaces/ICamera.o\
	obj/providers/SelectMainLoop.o\
	obj/providers/PollEventQueue.o\
	obj/providers/OpenMAXCameraManager.o\
	obj/tools/Pipe.o\
	obj/tools/System.o\
	obj/tools/InputReader.o\
	obj/tools/Exception.o\
	obj/RobotControl.o\
	obj/RobotV2.o\
	obj/test2.o

all: robot

clean:
	rm -rf obj/ robot

robot: $(OBJS)
	$(CPP) -o robot $(OBJS) $(LFLAGS)
	$(STRIP) robot


obj/%.o: src/%.cpp
	@mkdir -p `dirname $@`
	$(CPP) $(CFLAGS) $< -o $@

