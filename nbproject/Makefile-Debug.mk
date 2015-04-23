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
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Debug
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/BaseApplication.o \
	${OBJECTDIR}/src/main.o \
	${OBJECTDIR}/src/astar.o \
	${OBJECTDIR}/src/PathFind.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/usr/local/lib -L/usr/lib -lOgreMain -lOIS

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-Debug.mk dist/Debug/GNU-Linux-x86/ogrepathfinder

dist/Debug/GNU-Linux-x86/ogrepathfinder: ${OBJECTFILES}
	${MKDIR} -p dist/Debug/GNU-Linux-x86
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/ogrepathfinder ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/src/BaseApplication.o: nbproject/Makefile-${CND_CONF}.mk src/BaseApplication.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	$(COMPILE.cc) -g -I/usr/local/include/OGRE -I/usr/include/OIS -o ${OBJECTDIR}/src/BaseApplication.o src/BaseApplication.cpp

${OBJECTDIR}/src/main.o: nbproject/Makefile-${CND_CONF}.mk src/main.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	$(COMPILE.cc) -g -I/usr/local/include/OGRE -I/usr/include/OIS -o ${OBJECTDIR}/src/main.o src/main.cpp

${OBJECTDIR}/src/astar.o: nbproject/Makefile-${CND_CONF}.mk src/astar.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	$(COMPILE.cc) -g -I/usr/local/include/OGRE -I/usr/include/OIS -o ${OBJECTDIR}/src/astar.o src/astar.cpp

${OBJECTDIR}/src/PathFind.o: nbproject/Makefile-${CND_CONF}.mk src/PathFind.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	$(COMPILE.cc) -g -I/usr/local/include/OGRE -I/usr/include/OIS -o ${OBJECTDIR}/src/PathFind.o src/PathFind.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug
	${RM} dist/Debug/GNU-Linux-x86/ogrepathfinder

# Subprojects
.clean-subprojects:
