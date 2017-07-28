//
//  util.h
//  ImageFader
//
//  Created by Drew Gottlieb on 7/27/17.
//  Copyright © 2017 Drew Gottlieb. All rights reserved.
//

#pragma once

#include <GLUT/glut.h>

char *read_tga(const char *filename, int *width, int *height);
void *file_contents(const char *filename, GLint *length);
