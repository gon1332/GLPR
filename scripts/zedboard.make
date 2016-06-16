XIL_VERSION=2014.2

CC_PATH=/opt/Xilinx/SDK/$(XIL_VERSION)/gnu/arm/lin/bin
CXX=$(CC_PATH)/arm-xilinx-linux-gnueabi-g++
CXXFLAGS=-Wall -Wextra -std=c++11

INC_PATHS=-I/opt/Xilinx/SDK/$(XIL_VERSION)/data/embeddedsw/ThirdParty/opencv/include/opencv -I/opt/Xilinx/SDK/$(XIL_VERSION)/data/embeddedsw/ThirdParty/opencv/include
LIB_PATHS=-Wl,-rpath=/opt/Xilinx/SDK/$(XIL_VERSION)/data/embeddedsw/ThirdParty/opencv/lib -L/opt/Xilinx/SDK/$(XIL_VERSION)/data/embeddedsw/ThirdParty/opencv/lib

CLIBS=-lopencv_calib3d -lopencv_contrib -lopencv_core \
	-lopencv_features2d -lopencv_flann -lopencv_gpu \
	-lopencv_highgui -lopencv_imgproc -lopencv_legacy \
	-lopencv_ml -lopencv_objdetect \
	-lopencv_photo -lopencv_stitching \
	-lopencv_photo -lopencv_stitching \
	-lopencv_superres -lopencv_ts -lopencv_video \
	-lopencv_videostab -lrt -lpthread -lm -ldl

all: build-arm locate_lp text_isol morph_nor char_segm

build-arm:
	mkdir -p build-arm

locate_lp: 0_locate_lp/main.cpp
	$(CXX) $(CXXFLAGS) 0_locate_lp/main.cpp -o build-arm/loclp.out $(INC_PATHS) $(LIB_PATHS) $(CLIBS)

text_isol: 1_text_isolation/main.cpp
	$(CXX) $(CXXFLAGS) 1_text_isolation/main.cpp -o build-arm/txtiso.out $(INC_PATHS) $(LIB_PATHS) $(CLIBS)

morph_nor: 2_morphology_normalization/main.cpp
	$(CXX) $(CXXFLAGS) 2_morphology_normalization/main.cpp -o build-arm/morphology.out $(INC_PATHS) $(LIB_PATHS) $(CLIBS)

char_segm: 3_character_segmentation/main.cpp
	$(CXX) $(CXXFLAGS) 3_character_segmentation/main.cpp -o build-arm/charsegm.out $(INC_PATHS) $(LIB_PATHS) $(CLIBS)

clean:
	@rm -rf build-arm
