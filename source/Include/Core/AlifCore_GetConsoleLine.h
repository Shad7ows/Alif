﻿#pragma once

extern AlifSizeT optInd;
extern const wchar_t* optArg;

void alif_resetConsoleLine();

int alif_getConsoleLine(AlifSizeT, wchar_t* const*);
