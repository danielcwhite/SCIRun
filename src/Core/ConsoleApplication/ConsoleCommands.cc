/*
For more information, please see: http://software.sci.utah.edu

The MIT License

Copyright (c) 2015 Scientific Computing and Imaging Institute,
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

#include <Core/ConsoleApplication/ConsoleCommands.h>
#include <Core/Algorithms/Base/AlgorithmVariableNames.h>
#include <Dataflow/Engine/Controller/NetworkEditorController.h>
#include <Core/Application/Application.h>
#include <Dataflow/Serialization/Network/XMLSerializer.h>
#include <Dataflow/Serialization/Network/NetworkDescriptionSerialization.h>
#include <Dataflow/Network/Module.h>
#include <Core/Logging/ConsoleLogger.h>
#include <Core/Python/PythonInterpreter.h>
#include <boost/algorithm/string.hpp>

using namespace SCIRun::Core;
using namespace Commands;
using namespace Console;
using namespace SCIRun::Dataflow::Networks;
using namespace Algorithms;

LoadFileCommandConsole::LoadFileCommandConsole()
{
  addParameter(Name("FileNum"), 0);
}

//TODO: find a better place for this function
namespace
{
  void quietModulesIfNotVerbose()
  {
    if (!Application::Instance().parameters()->verboseMode())
      Module::defaultLogger_.reset(new SCIRun::Core::Logging::NullLogger);
  }
}

/// @todo: real logger
#define LOG_CONSOLE(x) std::cout << "[SCIRun] " << x << std::endl;

bool LoadFileCommandConsole::execute()
{
  quietModulesIfNotVerbose();

  auto inputFiles = Application::Instance().parameters()->inputFiles();
  std::string filename;
  if (!inputFiles.empty())
    filename = inputFiles[0];
  else
  {
    filename = get(Variables::Filename).toFilename().string();
  }

  LOG_CONSOLE("Attempting load of " << filename);
  if (!boost::filesystem::exists(filename))
  {
    LOG_CONSOLE("File does not exist: " << filename);
    return false;
  }
  try
  {
    auto openedFile = XMLSerializer::load_xml<NetworkFile>(filename);

    if (openedFile)
    {
      Application::Instance().controller()->clear();
      Application::Instance().controller()->loadNetwork(openedFile);
      LOG_CONSOLE("File load done: " << filename);
      return true;
    }
    LOG_CONSOLE("File load failed: " << filename);
  }
  catch (...)
  {
    LOG_CONSOLE("File load failed: " << filename);
  }
  return false;
}

bool SaveFileCommandConsole::execute()
{
  return !saveImpl(get(Variables::Filename).toFilename().string()).empty();
}

bool ExecuteCurrentNetworkCommandConsole::execute()
{
  LOG_CONSOLE("Executing network...");
  Application::Instance().controller()->connectNetworkExecutionFinished([](int code){ LOG_CONSOLE("Execution finished with code " << code); });
  Application::Instance().controller()->stopExecutionContextLoopWhenExecutionFinishes();
  auto t = Application::Instance().controller()->executeAll(nullptr);
  LOG_CONSOLE("Execution started.");
  t->join();
  LOG_CONSOLE("Execute thread stopped. Entering interactive mode.");

  InteractiveModeCommandConsole interactive;
  return interactive.execute();
}

QuitAfterExecuteCommandConsole::QuitAfterExecuteCommandConsole()
{
  addParameter(Name("RunningPython"), false);
}

bool QuitAfterExecuteCommandConsole::execute()
{
  LOG_CONSOLE("Quit after execute is set.");
  Application::Instance().controller()->connectNetworkExecutionFinished([](int code)
  {
    LOG_CONSOLE("Goodbye! Exit code: " << code);
    exit(code);
  });
  return true;
}

QuitCommandConsole::QuitCommandConsole()
{
  addParameter(Name("RunningPython"), false);
}

bool QuitCommandConsole::execute()
{
  LOG_CONSOLE("Goodbye!");
  exit(0);
  return true;
}

bool PrintHelpCommand::execute()
{
  std::cout << Application::Instance().commandHelpString() << std::endl;
  return true;
}

bool PrintVersionCommand::execute()
{
  std::cout << Application::Instance().version() << std::endl;
  return true;
}

bool PrintModulesCommand::execute()
{
  std::cout << "MODULE LIST as of " << Application::Instance().version() << "\n" << Application::Instance().moduleList() << std::endl;
  return true;
}

bool InteractiveModeCommandConsole::execute()
{
#ifdef BUILD_WITH_PYTHON
  quietModulesIfNotVerbose();
  PythonInterpreter::Instance().run_string("import SCIRunPythonAPI; from SCIRunPythonAPI import *");
  std::string line;

  while (true)
  {
    std::cout << "scirun5> " << std::flush;
    std::getline(std::cin, line);
    if (line == "quit") // TODO: need fix for ^D entry || (!x.empty() && x[0] == '\004'))
      break;
    if (std::cin.eof())
      break;
    if (!PythonInterpreter::Instance().run_string(line))
      break;
  }
  std::cout << "\n[SCIRun] Goodbye!" << std::endl;
  exit(0);
#endif
  return true;
}

bool RunPythonScriptCommandConsole::execute()
{
  quietModulesIfNotVerbose();

  auto script = Application::Instance().parameters()->pythonScriptFile();
  if (script)
  {
#ifdef BUILD_WITH_PYTHON
    LOG_CONSOLE("RUNNING PYTHON SCRIPT: " << *script);

    Application::Instance().controller()->clear();
    PythonInterpreter::Instance().run_string("import SCIRunPythonAPI; from SCIRunPythonAPI import *");
    PythonInterpreter::Instance().run_file(script->string());

    //TODO: not sure what else to do here. Probably wait on a condition variable, or just loop forever
    if (!Application::Instance().parameters()->interactiveMode())
    {
      while (true)
      {
        LOG_CONSOLE("Running Python script.");
        boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
      }
    }
    LOG_CONSOLE("Done running Python script.");
    return true;
#else
    LOG_CONSOLE("Python disabled, cannot run script " << *script);
    return false;
#endif
  }
  return false;
}
