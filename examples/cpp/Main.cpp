// Copyright (c) 2020 North Star Imaging, Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
// following conditions are met:
//   * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
//     disclaimer.
//   * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
//     following disclaimer in the documentation and/or other materials provided with the distribution.
//   * Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote
//     products derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
// USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#define _CRT_SECURE_NO_WARNINGS

#include <vector>
#include <cstdio>
#include <string>
#include <sstream>
#include <cstdlib>
#include <memory>
#include <iostream>
#include <iomanip>
#include <fstream>

#include "efX_SDK.h"

//using namespace NSI::efX;

int main(int argc, char* argv[])
{
	using VolumePtr = std::shared_ptr<NSI::efX::Volume>;

	if (argc != 3)
	{
		std::cout << "This program needs 2 argument which is the input file path and the output file path." << std::endl;
		return 1;
	}

	std::string inputFile = argv[1];
	std::string outputFile = argv[2];

	size_t pos = outputFile.find_last_of(".");
	std::string outputName = outputFile.substr(0, pos);
	std::string outputExt = outputFile.substr(pos);

	//std::cout << outputFile << std::endl;
	//std::cout << outputName << std::endl;
	//std::cout << outputExt << std::endl;

	std::stringstream ss;
	ss << outputName << ".json";
	
	std::ofstream info (ss.str(), std::ios_base::out);
	if (!info.is_open())
	{
		std::cout << "Could not open Info file for writing." << std::endl;
		return 1;
	}

	VolumePtr vol = VolumePtr(NSI::efX::Volume::Create());
	vol->open(inputFile.c_str());

	size_t width = static_cast<size_t>(vol->slice_width());
	size_t height = static_cast<size_t>(vol->slice_height());
	size_t depth = static_cast<size_t>(vol->num_slices());
	info << "{" << std::endl;
	info << "\"Comment\":" << "\" This file was produced by the efX_SDK program. \"," << std::endl;
	info << "\"Voxels\":[" << width  << "," << height << "," << depth  << "],"<<  std::endl;
	info << "\"Location\": {" << std::endl;
	NSI::efX::Vec3_d vmin = vol->vmin();
	info << "  \"Min\": [" << vmin.x  << "," << vmin.z << "," << vmin.y << "]," << std::endl;
	NSI::efX::Vec3_d vmax = vol->vmax();
	info << "  \"Max\": [" << vmax.x  << "," << vmax.z << "," << vmax.y << "]" << std::endl;
	info << "  }," << std::endl;
	NSI::efX::Vec3_d voxSize = vol->voxel_size();
	info << "\"Voxel Size\": ["<< voxSize.x << ", " << voxSize.z << ", " << voxSize.y << "]," << std::endl;
	info << "\"DataRange\": [" << vol->data_min() << ", " << vol->data_max() << "]," << std::endl;
	info << "\"File\": {" << std::endl;
	info << "  \"Name\":\"" << outputName << ".raw\"," << std::endl;
	info << "  \"NbSlices\": " << depth << std::endl;
	info << "  }" << std::endl;
	info << "}" << std::endl;
	info.close();


	std::vector<float> slice;
	slice.resize(width * height);
	size_t totalBytes = depth * slice.size();

	ss.str("");
	ss << outputName << ".raw";
	FILE* f = fopen(ss.str().c_str(), "wb");
	if (f == nullptr)
	{
		std::cout << "Could not open output file for writing." << std::endl;
		return 1;
	}

	for(size_t i = 0; i < depth;i++)
	{
		vol->read_slice(slice.data(), i);
		fwrite(slice.data(), 4, slice.size(), f);
	}
	fclose(f);
	vol->close();

	return 0;
}
