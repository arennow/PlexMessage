//
//  PreprocessorDeclarations.h
//  Now Plexing
//
//  Created by Aaron Rennow on 1/24/18.
//  Copyright © 2018 Lithium Cube. All rights reserved.
//

#ifndef PreprocessorDeclarations_h
#define PreprocessorDeclarations_h

#if defined(__GNUC__) && !defined(__clang__)
#define __gcc__
#endif

#ifdef __clang__
#define nestedFunc(returnType, name, parameterList) returnType(^name)(parameterList) = ^returnType(parameterList)
#elif defined(__gcc__)
#define nestedFunc(returnType, name, parameterList) returnType name(parameterList)
#else
#error Nested functions/closures not supported by compiler
#endif

#define length_of_fixed_length_array(ary) sizeof(ary)/sizeof(ary[0])

#endif /* PreprocessorDeclarations_h */
