#pragma once
#include <stdint.h>

#include "dosfs/dosfs.h"
#include "print.h"
#include "kbd.h"
#include "v86defs.h"
#include "helper.h"

void HexEditor(uint8_t *path, VOLINFO *vi);
void TextViewTest(uint8_t *path, VOLINFO *vi);
void ProgramLoadTest(uint8_t *path, VOLINFO *vi);
