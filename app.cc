#include "app.hh"

#include "implot.h"
#include "imgui.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

void renderUi(void);

enum paramNames
  {
    paramAcceleration,
    paramSensitivity,
    paramSpeedCap,
    paramSensitivityCap,
    paramOffset,
    paramMidpoint,
    paramExponent,
    paramsLen,
  };

const char* params[] = {
  [paramAcceleration] = "Acceleration",
  [paramSensitivity] = "Sensitivity",
  [paramSpeedCap] = "SpeedCap",
  [paramSensitivityCap] = "SensitivityCap",
  [paramOffset] = "Offset",
  [paramMidpoint] = "Midpoint",
  [paramExponent] = "Exponent",
  nullptr
};

const char* intParams[] = {
  "ScrollsPerTick",
  nullptr
};

static float paramValues[sizeof(params)] = {0};
static int intParamValues[sizeof(intParams)] = {0};

const char* accelModes[] = {
  "linear",
  "classic",
  "motivity",
};

const static ImGuiWindowFlags window_flags =
  ImGuiWindowFlags_NoDocking
  | ImGuiWindowFlags_NoTitleBar
  | ImGuiWindowFlags_NoCollapse
  | ImGuiWindowFlags_NoResize
  | ImGuiWindowFlags_NoMove
  | ImGuiWindowFlags_NoBringToFrontOnFocus
  | ImGuiWindowFlags_NoNavFocus;

inline float
min(float a, float b)
{
  return a<b?a:b;
}

void
getModuleParameter(const char* name, char* buffer)
{
  FILE* fd;
  char path[0x200];

  sprintf(path, "/sys/module/leetmouse/parameters/%s", name);
  fd = fopen(path, "r");
  assert(fd != NULL);

  fread(buffer, sizeof(char), sizeof(buffer), fd);
  fclose(fd);
}

float
getModuleParameterFloat(const char* name)
{
  float result;
  char buffer[0x10];

  getModuleParameter(name, buffer);
  sscanf(buffer, "%f", &result);

  return result;
}

int
getModuleParameterInt(const char* name)
{
  int result;
  char buffer[0x10];

  getModuleParameter(name, buffer);
  sscanf(buffer, "%d", &result);

  return result;
}

void
setModuleParameter(const char* name, const char* val)
{
  FILE* fd;
  char path[0x200];
  size_t len;

  sprintf(path, "/sys/module/leetmouse/parameters/%s", name);
  fd = fopen(path, "w");
  assert(fd != NULL);

  len = strlen(val);
  fwrite(val, sizeof(char), len, fd);
  
  fclose(fd);
}

void
setModuleParameterFloat(const char* name, float val)
{
  char buffer[0x10];
  
  sprintf(buffer, "%.2ff", val);
  setModuleParameter(name, buffer);
}

void
setModuleParameterInt(const char* name, int val)
{
  char buffer[0x10];
  
  sprintf(buffer, "%d", val);
  setModuleParameter(name, buffer);
}

const char*
findAccelMode(void)
{
  int currentValue;
  currentValue = getModuleParameterInt("AccelerationMode");
  return accelModes[currentValue+1];
}

void
init(void)
{
  for (int i = 0; params[i] != nullptr; ++i)
    paramValues[i] = getModuleParameterFloat(params[i]);
  for (int i = 0; intParams[i] != nullptr; ++i)
    intParamValues[i] = getModuleParameterInt(intParams[i]);
}

void
update(void)
{
  renderUi();
}

void
renderUi(void)
{
  {
    const ImGuiViewport* viewport;
    static int currentAccelMode;
        
    viewport = ImGui::GetMainViewport();
        
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
        
    ImGui::Begin("leetmouse gui", nullptr, window_flags);
        
    ImGui::Text("leetmouse gui");

    ImGui::BeginGroup();
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
    {
      ImGui::Combo("mode", &currentAccelMode, accelModes, 3);

      for (int i = 0; params[i] != nullptr; ++i)
        {
          ImGui::SliderFloat(params[i], &paramValues[i], -10.0f, 10.0f, "%.2f");
        }
    
      for (int i = 0; intParams[i] != nullptr; ++i)
        {
          ImGui::SliderInt(intParams[i], &intParamValues[i], -10, 10);
        }
      
      if (ImGui::Button("update"))
        {
          for (int i = 0; params[i] != nullptr; ++i)
            setModuleParameterFloat(params[i], paramValues[i]);
          for (int i = 0; intParams[i] != nullptr; ++i)
            setModuleParameterInt(intParams[i], intParamValues[i]);
          setModuleParameterInt("AccelerationMode", currentAccelMode + 1);
          setModuleParameterInt("update", 1);
        }
    }
    ImGui::PopItemWidth();
    ImGui::EndGroup();

    ImGui::SameLine();

    if (ImPlot::BeginPlot("My Plot"))
      {
        float x[30] = {0};
        float y[30] = {0};
        for (int i = 0; i < 30; ++i)
          {
            x[i] = i;
            y[i] = i;
            if (paramValues[paramSpeedCap] != 0)
              y[i] = min(y[i], paramValues[paramSpeedCap]);
            y[i] -= paramValues[paramOffset];
            
            y[i] *= paramValues[paramAcceleration];
            y[i] += 1;
          }
        ImPlot::PlotLine("My Line Plot", x, y, 30);
        ImPlot::EndPlot();
      }

    ImGui::End();
  }

}
