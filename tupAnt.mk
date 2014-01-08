##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=tupAnt
ConfigurationName      :=Debug
WorkspacePath          := "/home/dacuna/cppworkspace/proyectos"
ProjectPath            := "/home/dacuna/cppworkspace/proyectos/tupAnt"
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Diego Acuña
Date                   :=01/08/2014
CodeLitePath           :="/home/dacuna/.codelite"
LinkerName             :=g++
SharedObjectLinkerName :=g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.o.i
DebugSwitch            :=-gstab
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E 
ObjectsFileList        :="tupAnt.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  -lglog -lemon
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := ar rcus
CXX      := g++
CC       := gcc
CXXFLAGS :=  -std=c++11 -g -O0 -Wall $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall $(Preprocessors)


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/main$(ObjectSuffix) $(IntermediateDirectory)/ant$(ObjectSuffix) $(IntermediateDirectory)/tup$(ObjectSuffix) $(IntermediateDirectory)/game$(ObjectSuffix) $(IntermediateDirectory)/mersenne_random$(ObjectSuffix) $(IntermediateDirectory)/colony$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

$(IntermediateDirectory)/.d:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/main$(ObjectSuffix): main.cpp $(IntermediateDirectory)/main$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/dacuna/cppworkspace/proyectos/tupAnt/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main$(DependSuffix): main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main$(ObjectSuffix) -MF$(IntermediateDirectory)/main$(DependSuffix) -MM "main.cpp"

$(IntermediateDirectory)/main$(PreprocessSuffix): main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main$(PreprocessSuffix) "main.cpp"

$(IntermediateDirectory)/ant$(ObjectSuffix): ant.cpp $(IntermediateDirectory)/ant$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/dacuna/cppworkspace/proyectos/tupAnt/ant.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/ant$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ant$(DependSuffix): ant.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ant$(ObjectSuffix) -MF$(IntermediateDirectory)/ant$(DependSuffix) -MM "ant.cpp"

$(IntermediateDirectory)/ant$(PreprocessSuffix): ant.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ant$(PreprocessSuffix) "ant.cpp"

$(IntermediateDirectory)/tup$(ObjectSuffix): tup.cpp $(IntermediateDirectory)/tup$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/dacuna/cppworkspace/proyectos/tupAnt/tup.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/tup$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/tup$(DependSuffix): tup.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/tup$(ObjectSuffix) -MF$(IntermediateDirectory)/tup$(DependSuffix) -MM "tup.cpp"

$(IntermediateDirectory)/tup$(PreprocessSuffix): tup.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/tup$(PreprocessSuffix) "tup.cpp"

$(IntermediateDirectory)/game$(ObjectSuffix): game.cpp $(IntermediateDirectory)/game$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/dacuna/cppworkspace/proyectos/tupAnt/game.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/game$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/game$(DependSuffix): game.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/game$(ObjectSuffix) -MF$(IntermediateDirectory)/game$(DependSuffix) -MM "game.cpp"

$(IntermediateDirectory)/game$(PreprocessSuffix): game.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/game$(PreprocessSuffix) "game.cpp"

$(IntermediateDirectory)/mersenne_random$(ObjectSuffix): mersenne_random.cpp $(IntermediateDirectory)/mersenne_random$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/dacuna/cppworkspace/proyectos/tupAnt/mersenne_random.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/mersenne_random$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/mersenne_random$(DependSuffix): mersenne_random.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/mersenne_random$(ObjectSuffix) -MF$(IntermediateDirectory)/mersenne_random$(DependSuffix) -MM "mersenne_random.cpp"

$(IntermediateDirectory)/mersenne_random$(PreprocessSuffix): mersenne_random.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/mersenne_random$(PreprocessSuffix) "mersenne_random.cpp"

$(IntermediateDirectory)/colony$(ObjectSuffix): colony.cpp $(IntermediateDirectory)/colony$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/dacuna/cppworkspace/proyectos/tupAnt/colony.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/colony$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/colony$(DependSuffix): colony.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/colony$(ObjectSuffix) -MF$(IntermediateDirectory)/colony$(DependSuffix) -MM "colony.cpp"

$(IntermediateDirectory)/colony$(PreprocessSuffix): colony.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/colony$(PreprocessSuffix) "colony.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) $(IntermediateDirectory)/main$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/main$(DependSuffix)
	$(RM) $(IntermediateDirectory)/main$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/ant$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/ant$(DependSuffix)
	$(RM) $(IntermediateDirectory)/ant$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/tup$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/tup$(DependSuffix)
	$(RM) $(IntermediateDirectory)/tup$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/game$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/game$(DependSuffix)
	$(RM) $(IntermediateDirectory)/game$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/mersenne_random$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/mersenne_random$(DependSuffix)
	$(RM) $(IntermediateDirectory)/mersenne_random$(PreprocessSuffix)
	$(RM) $(IntermediateDirectory)/colony$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/colony$(DependSuffix)
	$(RM) $(IntermediateDirectory)/colony$(PreprocessSuffix)
	$(RM) $(OutputFile)
	$(RM) "../.build-debug/tupAnt"


