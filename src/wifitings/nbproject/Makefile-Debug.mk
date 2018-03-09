#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/617a183b/cmd.o \
	${OBJECTDIR}/_ext/617a183b/int_tools.o \
	${OBJECTDIR}/_ext/617a183b/llargekeymap.o \
	${OBJECTDIR}/_ext/617a183b/mac_tools.o \
	${OBJECTDIR}/_ext/617a183b/stringprt.o \
	${OBJECTDIR}/_ext/617a183b/time_tools.o \
	${OBJECTDIR}/dllmain.o \
	${OBJECTDIR}/ieee80211.o \
	${OBJECTDIR}/liw.o \
	${OBJECTDIR}/realwifitings.o \
	${OBJECTDIR}/wifipcap.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-fvisibility=hidden
CXXFLAGS=-fvisibility=hidden

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L../lib -Wl,-rpath,'./'

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libwifitings.${CND_DLIB_EXT}

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libwifitings.${CND_DLIB_EXT}: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libwifitings.${CND_DLIB_EXT} ${OBJECTFILES} ${LDLIBSOPTIONS} -lboost_system -lboost_thread -lpcap -lpthread -lnl -lleelog -shared -fPIC

${OBJECTDIR}/_ext/617a183b/cmd.o: ../../tools/cmd.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/617a183b
	${RM} "$@.d"
	$(COMPILE.cc) -g -DWIFITINGS_EXPORTS -I../include -I../../include -I.. -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/617a183b/cmd.o ../../tools/cmd.cpp

${OBJECTDIR}/_ext/617a183b/int_tools.o: ../../tools/int_tools.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/617a183b
	${RM} "$@.d"
	$(COMPILE.cc) -g -DWIFITINGS_EXPORTS -I../include -I../../include -I.. -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/617a183b/int_tools.o ../../tools/int_tools.cpp

${OBJECTDIR}/_ext/617a183b/llargekeymap.o: ../../tools/llargekeymap.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/617a183b
	${RM} "$@.d"
	$(COMPILE.cc) -g -DWIFITINGS_EXPORTS -I../include -I../../include -I.. -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/617a183b/llargekeymap.o ../../tools/llargekeymap.cpp

${OBJECTDIR}/_ext/617a183b/mac_tools.o: ../../tools/mac_tools.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/617a183b
	${RM} "$@.d"
	$(COMPILE.cc) -g -DWIFITINGS_EXPORTS -I../include -I../../include -I.. -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/617a183b/mac_tools.o ../../tools/mac_tools.cpp

${OBJECTDIR}/_ext/617a183b/stringprt.o: ../../tools/stringprt.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/617a183b
	${RM} "$@.d"
	$(COMPILE.cc) -g -DWIFITINGS_EXPORTS -I../include -I../../include -I.. -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/617a183b/stringprt.o ../../tools/stringprt.cpp

${OBJECTDIR}/_ext/617a183b/time_tools.o: ../../tools/time_tools.cpp
	${MKDIR} -p ${OBJECTDIR}/_ext/617a183b
	${RM} "$@.d"
	$(COMPILE.cc) -g -DWIFITINGS_EXPORTS -I../include -I../../include -I.. -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/617a183b/time_tools.o ../../tools/time_tools.cpp

${OBJECTDIR}/dllmain.o: dllmain.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -DWIFITINGS_EXPORTS -I../include -I../../include -I.. -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/dllmain.o dllmain.cpp

${OBJECTDIR}/ieee80211.o: ieee80211.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -DWIFITINGS_EXPORTS -I../include -I../../include -I.. -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ieee80211.o ieee80211.cpp

${OBJECTDIR}/liw.o: liw.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -DWIFITINGS_EXPORTS -I../include -I../../include -I.. -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/liw.o liw.cpp

${OBJECTDIR}/realwifitings.o: realwifitings.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -DWIFITINGS_EXPORTS -I../include -I../../include -I.. -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/realwifitings.o realwifitings.cpp

${OBJECTDIR}/wifipcap.o: wifipcap.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -DWIFITINGS_EXPORTS -I../include -I../../include -I.. -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/wifipcap.o wifipcap.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
