#pragma once

#ifdef INLINER_EXPORTS
#define EX __declspec(dllexport)
#else
#define EX __declspec(dllimport)
#endif

