//
//  EngineTime.cpp
//

#include <EngineTime.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

double dg::Time::Elapsed = 0;
double dg::Time::Delta = 0;

void dg::Time::Reset() {
  glfwSetTime(0);
}

void dg::Time::Update() {
  Delta = glfwGetTime();
  Elapsed += Delta;
  glfwSetTime(0);
}

