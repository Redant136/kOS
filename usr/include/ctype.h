#pragma once

#ifndef K_CTYPE_H
#define K_CTYPE_H

#ifdef __cplusplus
extern "C"
{
#endif

  static int isdigit(char c){
    return c<='9'&&c>='0';
  }
  // test
  static int tolower(int c){
    if(c<='Z'&&c>='A')
      return c+('A'-'a');
    return c;
  }
  static int toupper(int c){
    if (c <= 'Z' && c >= 'A')
      return c + ('a' - 'A');
    return c;
  }

#ifdef __cplusplus
}
#endif

#endif