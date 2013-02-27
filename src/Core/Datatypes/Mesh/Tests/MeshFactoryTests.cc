/*
   For more information, please see: http://software.sci.utah.edu

   The MIT License

   Copyright (c) 2012 Scientific Computing and Imaging Institute,
   University of Utah.

   License for the specific language governing rights and limitations under
   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.
*/

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Core/Datatypes/Mesh/MeshFactory.h>
#include <Core/Datatypes/Mesh/FieldInformation.h>

using namespace SCIRun::Core::Datatypes;
using namespace SCIRun::Core::Geometry;
using ::testing::_;
using ::testing::NiceMock;
using ::testing::DefaultValue;
using ::testing::Return;

TEST(MeshFactoryTests, CanCreateLatticeVolumeMesh)
{
  int basisOrder = 1;
  FieldInformation lfi("LatVolMesh", basisOrder, "double");
  int sizex,sizey,sizez;
  sizex = sizey = sizez = 4;
  Point minb(0,0,0);
  Point maxb(4,4,4);
  MeshHandle mesh = MeshFactory::Instance().CreateMesh(lfi, MeshConstructionParameters(sizex, sizey, sizez, minb, maxb));
  ASSERT_TRUE(mesh);
}

TEST(MeshFactoryTests, CanCreateTriSurfMesh)
{
  int basisOrder = 1; // Not sure if this is being used correctly below.
  FieldInformation lfi("TriSurfMesh", basisOrder, "double");
  MeshHandle mesh = MeshFactory::Instance().CreateMesh(lfi.get_mesh_type_id());
  ASSERT_TRUE(mesh);
}