/******************************************************************************
* Copyright (c) 2014, Peter J. Gadomski (pete.gadomski@gmail.com)
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following
* conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in
*       the documentation and/or other materials provided
*       with the distribution.
*     * Neither the name of Hobu, Inc. or Flaxen Geo Consulting nor the
*       names of its contributors may be used to endorse or promote
*       products derived from this software without specific prior
*       written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
* AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
* OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
* OF SUCH DAMAGE.
****************************************************************************/

#include <pdal/drivers/sbet/Writer.hpp>

#include <pdal/drivers/sbet/Common.hpp>
#include <pdal/PointBuffer.hpp>

namespace pdal
{
namespace drivers
{
namespace sbet
{

void SbetWriter::processOptions(const Options& options)
{
    m_filename = options.getOption("filename").getValue<std::string>();
}


void SbetWriter::ready(PointContext ctx)
{
    m_stream.reset(new OLeStream(m_filename));

    // Find dimensions in the current schema that map to the SBET dimensions.
    std::vector<Dimension> neededDims = getDefaultDimensions();
    m_dims.clear();
    for (auto di = neededDims.begin(); di != neededDims.end(); ++di)
    {
        Dimension& dim = *di;
        m_dims.push_back(ctx.schema()->getDimension(dim.getName(), getName()));
    }
}


void SbetWriter::write(const PointBuffer& buf)
{
    m_callback->setTotal(buf.size());
    m_callback->invoke(0);
    for (PointId idx = 0; idx < buf.size(); ++idx)
    {
        for (auto di = m_dims.begin(); di != m_dims.end(); ++di)
        {
            // If a dimension doesn't exist, write 0.
            DimensionPtr d = *di;
            *m_stream << (d ? buf.getFieldAs<double>(d, idx) : 0.0);
        }
        if (idx % 100 == 0)
            m_callback->invoke(idx + 1);
    }
    m_callback->invoke(buf.size());
}

} // namespace sbet
} // namespace drivers
} // namespace pdal

