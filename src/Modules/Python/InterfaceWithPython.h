/*
   For more information, please see: http://software.sci.utah.edu

   The MIT License

   Copyright (c) 2016 Scientific Computing and Imaging Institute,
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

#ifndef MODULES_PYTHON_INTERFACEWITHPYTHON_H
#define MODULES_PYTHON_INTERFACEWITHPYTHON_H

#include <Dataflow/Network/Module.h>
#include <Core/Thread/Mutex.h>
#include <Modules/Python/share.h>

namespace SCIRun
{
  namespace Core
  {
    namespace Algorithms
    {
      namespace Python
      {
        ALGORITHM_PARAMETER_DECL(PythonCode);
        ALGORITHM_PARAMETER_DECL(PythonString1Name);
        ALGORITHM_PARAMETER_DECL(PythonString2Name);
        ALGORITHM_PARAMETER_DECL(PythonString3Name);
        ALGORITHM_PARAMETER_DECL(PythonMatrix1Name);
        ALGORITHM_PARAMETER_DECL(PythonMatrix2Name);
        ALGORITHM_PARAMETER_DECL(PythonMatrix3Name);
        ALGORITHM_PARAMETER_DECL(PythonField1Name);
        ALGORITHM_PARAMETER_DECL(PythonField2Name);
        ALGORITHM_PARAMETER_DECL(PythonField3Name);
      }
    }
  }

  namespace Modules
  {
    namespace Python
    {
      class SCISHARE InterfaceWithPython : public SCIRun::Dataflow::Networks::Module,
        public Has3InputPorts<DynamicPortTag<MatrixPortTag>, DynamicPortTag<FieldPortTag>, DynamicPortTag<StringPortTag>>,
        public Has9OutputPorts<MatrixPortTag, MatrixPortTag, MatrixPortTag, FieldPortTag, FieldPortTag, FieldPortTag, StringPortTag, StringPortTag, StringPortTag>
      {
      public:
        InterfaceWithPython();
        virtual void execute() override;
        virtual void setStateDefaults() override;
        virtual bool hasDynamicPorts() const override { return true; }
        INPUT_PORT_DYNAMIC(0, InputMatrix, Matrix);
        INPUT_PORT_DYNAMIC(1, InputField, LegacyField);
        INPUT_PORT_DYNAMIC(2, InputString, String);
        OUTPUT_PORT(0, PythonMatrix1, Matrix);
        OUTPUT_PORT(1, PythonMatrix2, Matrix);
        OUTPUT_PORT(2, PythonMatrix3, Matrix);
        OUTPUT_PORT(3, PythonField1, LegacyField);
        OUTPUT_PORT(4, PythonField2, LegacyField);
        OUTPUT_PORT(5, PythonField3, LegacyField);
        OUTPUT_PORT(6, PythonString1, String);
        OUTPUT_PORT(7, PythonString2, String);
        OUTPUT_PORT(8, PythonString3, String);

        static const Dataflow::Networks::ModuleLookupInfo staticInfo_;
      private:
        static Core::Thread::Mutex lock_;
        std::string convertInputOutputSyntax(const std::string& code) const;
      };

    }
  }
}

#endif
