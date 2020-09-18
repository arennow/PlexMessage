//
//  SwiftExportDeclarations.h
//  PlexMessage
//
//  Created by Aaron Rennow on 1/24/18.
//  Copyright © 2018 Lithium Cube. All rights reserved.
//

#ifndef SwiftExportDeclarations_h
#define SwiftExportDeclarations_h

#ifndef __has_feature         // Optional of course.
#define __has_feature(x) 0  // Compatibility with non-clang compilers.
#endif
#ifndef __has_extension
#define __has_extension __has_feature // Compatibility with pre-3.0 compilers.
#endif
#ifndef __has_attribute
#define __has_attribute __has_feature
#endif

#if __has_feature(assume_nonnull)
#define CF_ASSUME_NONNULL_BEGIN _Pragma("clang assume_nonnull begin")
#define CF_ASSUME_NONNULL_END   _Pragma("clang assume_nonnull end")
#else
#define CF_ASSUME_NONNULL_BEGIN
#define CF_ASSUME_NONNULL_END
#endif


#if !__has_feature(nullability)
#ifndef _Nullable
#define _Nullable
#endif
#ifndef _Nonnull
#define _Nonnull
#endif
#ifndef _Null_unspecified
#define _Null_unspecified
#endif
#endif

#if __has_attribute(swift_name)
# define CF_SWIFT_NAME(_name) __attribute__((swift_name(#_name)))
#else
# define CF_SWIFT_NAME(_name)
#endif

#if __has_feature(attribute_availability_swift)
#define CF_SWIFT_UNAVAILABLE(_msg) __attribute__((availability(swift, unavailable, message=_msg)))
#else
#define CF_SWIFT_UNAVAILABLE(_msg)
#endif

//MARK: Enum stuff

#if __has_attribute(enum_extensibility)
#define __CF_ENUM_ATTRIBUTES __attribute__((enum_extensibility(open)))
#define __CF_OPTIONS_ATTRIBUTES __attribute__((flag_enum,enum_extensibility(open)))
#else
#define __CF_ENUM_ATTRIBUTES
#define __CF_OPTIONS_ATTRIBUTES
#endif

#define __CF_ENUM_GET_MACRO(_1, _2, NAME, ...) NAME
#if (__cplusplus && __cplusplus >= 201103L && (__has_extension(cxx_strong_enums) || __has_feature(objc_fixed_enum))) || (!__cplusplus && __has_feature(objc_fixed_enum))
#define __CF_NAMED_ENUM(_type, _name)     enum __CF_ENUM_ATTRIBUTES _name : _type _name; enum _name : _type
#define __CF_ANON_ENUM(_type)             enum __CF_ENUM_ATTRIBUTES : _type
#if (__cplusplus)
#define CF_OPTIONS(_type, _name) _type _name; enum __CF_OPTIONS_ATTRIBUTES : _type
#else
#define CF_OPTIONS(_type, _name) enum __CF_OPTIONS_ATTRIBUTES _name : _type _name; enum _name : _type
#endif
#else
#define __CF_NAMED_ENUM(_type, _name) _type _name; enum
#define __CF_ANON_ENUM(_type) enum
#define CF_OPTIONS(_type, _name) _type _name; enum
#endif

/* CF_ENUM supports the use of one or two arguments. The first argument is always the integer type used for the values of the enum. The second argument is an optional type name for the macro. When specifying a type name, you must precede the macro with 'typedef' like so:
 
 typedef CF_ENUM(CFIndex, CFComparisonResult) {
 ...
 };
 
 If you do not specify a type name, do not use 'typdef', like so:
 
 CF_ENUM(CFIndex) {
 ...
 };
 */
#define CF_ENUM(...) __CF_ENUM_GET_MACRO(__VA_ARGS__, __CF_NAMED_ENUM, __CF_ANON_ENUM, )(__VA_ARGS__)

#if __has_attribute(swift_wrapper)
#define _CF_TYPED_ENUM __attribute__((swift_wrapper(enum)))
#else
#define _CF_TYPED_ENUM
#endif

#endif /* SwiftExportDeclarations_h */
