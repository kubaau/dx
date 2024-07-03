#pragma once

#include <ext/DDSTextureLoader.h>

#include "DXTypedefs.hpp"
#include "IOTypedefs.hpp"

SRVPtr loadDDSTexture(Device&, Context&, FilePath);
SRVPtr loadBMPTexture(Device&, Context&, FilePath);
SRVPtr create2DTextureArray(Device&, Context&, const FilePaths&);
SRVPtr createRandomTexture(Device&);
