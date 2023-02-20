//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#ifndef GAMEUI_UTIL_H
#define GAMEUI_UTIL_H
#ifdef _WIN32
#pragma once
#endif

char	*VarArgs( const char *format, ... );

void GameUI_MakeSafeName( const char *oldName, char *newName, int newNameBufSize );

#endif // GAMEUI_UTIL_H
