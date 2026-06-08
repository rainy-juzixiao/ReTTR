/*
 * Copyright 2026 rainy-juzixiao
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef RETTR_TEST_CLASSES_HPP
#define RETTR_TEST_CLASSES_HPP

#include <rettr/rettr.hpp>

#define CLASS(CLASS_NAME)                                                                                                             \
    struct CLASS_NAME {                                                                                                               \
        virtual ~CLASS_NAME() {                                                                                                       \
        }                                                                                                                             \
        ENABLE_RETTR_CAST() virtual int getType() {                                                                                   \
            return dummyIntValue;                                                                                                     \
        }                                                                                                                             \
        int dummyIntValue = 0;                                                                                                        \
    };

#define CLASS_INHERIT(CLASS1, CLASS2)                                                                                                 \
    struct CLASS1 : CLASS2 {                                                                                                          \
        virtual int getType() {                                                                                                       \
            return static_cast<int>(dummyDoubleValue);                                                                                \
        }                                                                                                                             \
        ENABLE_RETTR_CAST(CLASS2) double dummyDoubleValue = 1;                                                                        \
    };

#define CLASS_MULTI_INHERIT_2(CLASS1, CLASS2, CLASS3)                                                                                 \
    struct CLASS1 : CLASS2, CLASS3 {                                                                                                  \
        virtual int getType() {                                                                                                       \
            return static_cast<int>(dummyBoolValue);                                                                                  \
        }                                                                                                                             \
        ENABLE_RETTR_CAST(CLASS2, CLASS3) bool dummyBoolValue = false;                                                                \
    };

#define CLASS_MULTI_INHERIT_5(CLASS1, CLASS2, CLASS3, CLASS4, CLASS5, CLASS6)                                                         \
    struct CLASS1 : CLASS2, CLASS3, CLASS4, CLASS5, CLASS6 {                                                                          \
        virtual int getType() {                                                                                                       \
            return static_cast<int>(dummyBoolValue);                                                                                  \
        }                                                                                                                             \
        ENABLE_RETTR_CAST(CLASS2, CLASS3, CLASS4, CLASS5, CLASS6) bool dummyBoolValue = true;                                         \
    };

CLASS(ClassSingleBase)
CLASS_INHERIT(ClassSingle1A, ClassSingleBase)
CLASS_INHERIT(ClassSingle2A, ClassSingle1A)
CLASS_INHERIT(ClassSingle3A, ClassSingle2A)
CLASS_INHERIT(ClassSingle4A, ClassSingle3A)
CLASS_INHERIT(ClassSingle5A, ClassSingle4A)
CLASS_INHERIT(ClassSingle6A, ClassSingle5A)

CLASS_INHERIT(ClassSingle1B, ClassSingleBase)
CLASS_INHERIT(ClassSingle2B, ClassSingle1B)
CLASS_INHERIT(ClassSingle3B, ClassSingle2B)
CLASS_INHERIT(ClassSingle4B, ClassSingle3B)
CLASS_INHERIT(ClassSingle5B, ClassSingle4B)
CLASS_INHERIT(ClassSingle6B, ClassSingle5B)

CLASS_INHERIT(ClassSingle1C, ClassSingleBase)
CLASS_INHERIT(ClassSingle2C, ClassSingle1C)
CLASS_INHERIT(ClassSingle3C, ClassSingle2C)
CLASS_INHERIT(ClassSingle4C, ClassSingle3C)
CLASS_INHERIT(ClassSingle5C, ClassSingle4C)
CLASS_INHERIT(ClassSingle6C, ClassSingle5C)

CLASS_INHERIT(ClassSingle1D, ClassSingleBase)
CLASS_INHERIT(ClassSingle2D, ClassSingle1D)
CLASS_INHERIT(ClassSingle3D, ClassSingle2D)
CLASS_INHERIT(ClassSingle4D, ClassSingle3D)
CLASS_INHERIT(ClassSingle5D, ClassSingle4D)
CLASS_INHERIT(ClassSingle6D, ClassSingle5D)

CLASS_INHERIT(ClassSingle1E, ClassSingleBase)
CLASS_INHERIT(ClassSingle2E, ClassSingle1E)
CLASS_INHERIT(ClassSingle3E, ClassSingle2E)
CLASS_INHERIT(ClassSingle4E, ClassSingle3E)
CLASS_INHERIT(ClassSingle5E, ClassSingle4E)
CLASS_INHERIT(ClassSingle6E, ClassSingle5E)

CLASS(ClassMultipleBaseA)
CLASS_INHERIT(ClassMultiple1A, ClassMultipleBaseA)
CLASS_INHERIT(ClassMultiple2A, ClassMultiple1A)
CLASS_INHERIT(ClassMultiple3A, ClassMultiple2A)
CLASS_INHERIT(ClassMultiple4A, ClassMultiple3A)
CLASS_INHERIT(ClassMultiple5A, ClassMultiple4A)
CLASS_INHERIT(ClassMultiple6A, ClassMultiple5A)

CLASS(ClassMultipleBaseB)
CLASS_INHERIT(ClassMultiple1B, ClassMultipleBaseB)
CLASS_INHERIT(ClassMultiple2B, ClassMultiple1B)
CLASS_INHERIT(ClassMultiple3B, ClassMultiple2B)
CLASS_INHERIT(ClassMultiple4B, ClassMultiple3B)
CLASS_INHERIT(ClassMultiple5B, ClassMultiple4B)
CLASS_INHERIT(ClassMultiple6B, ClassMultiple5B)

CLASS(ClassMultipleBaseC)
CLASS_INHERIT(ClassMultiple1C, ClassMultipleBaseC)
CLASS_INHERIT(ClassMultiple2C, ClassMultiple1C)
CLASS_INHERIT(ClassMultiple3C, ClassMultiple2C)
CLASS_INHERIT(ClassMultiple4C, ClassMultiple3C)
CLASS_INHERIT(ClassMultiple5C, ClassMultiple4C)
CLASS_INHERIT(ClassMultiple6C, ClassMultiple5C)

CLASS(ClassMultipleBaseD)
CLASS_INHERIT(ClassMultiple1D, ClassMultipleBaseD)
CLASS_INHERIT(ClassMultiple2D, ClassMultiple1D)
CLASS_INHERIT(ClassMultiple3D, ClassMultiple2D)
CLASS_INHERIT(ClassMultiple4D, ClassMultiple3D)
CLASS_INHERIT(ClassMultiple5D, ClassMultiple4D)
CLASS_INHERIT(ClassMultiple6D, ClassMultiple5D)

CLASS(ClassMultipleBaseE)
CLASS_INHERIT(ClassMultiple1E, ClassMultipleBaseE)
CLASS_INHERIT(ClassMultiple2E, ClassMultiple1E)
CLASS_INHERIT(ClassMultiple3E, ClassMultiple2E)
CLASS_INHERIT(ClassMultiple4E, ClassMultiple3E)
CLASS_INHERIT(ClassMultiple5E, ClassMultiple4E)
CLASS_INHERIT(ClassMultiple6E, ClassMultiple5E)

CLASS_MULTI_INHERIT_5(FinalClass, ClassMultiple6A, ClassMultiple6B, ClassMultiple6C, ClassMultiple6D, ClassMultiple6E)

struct DiamondTop {
    double foo = 12;
    ENABLE_RETTR_CAST()
};

struct DiamondLeft : virtual DiamondTop {
    bool _left_var = true;
    ENABLE_RETTR_CAST(DiamondTop)
};

struct DiamondRight : virtual DiamondTop {
    std::string _text = "Hello World";
    ENABLE_RETTR_CAST(DiamondTop)
};


struct DiamondBottom : DiamondLeft, DiamondRight {
    int _finalVar = 42;
    ENABLE_RETTR_CAST(DiamondLeft, DiamondRight)
};

CLASS(ClassMulti1A)

CLASS(ClassMulti1B)
CLASS(ClassMulti2B)
CLASS(ClassMulti3B)
CLASS(ClassMulti4B)
CLASS(ClassMulti5B)
CLASS(ClassMulti6B)

CLASS_MULTI_INHERIT_2(ClassMulti2A, ClassMulti1A, ClassMulti1B)
CLASS_MULTI_INHERIT_2(ClassMulti3A, ClassMulti2A, ClassMulti2B)
CLASS_MULTI_INHERIT_2(ClassMulti4A, ClassMulti3A, ClassMulti3B)
CLASS_MULTI_INHERIT_2(ClassMulti5A, ClassMulti4A, ClassMulti4B)
CLASS_MULTI_INHERIT_2(ClassMulti6A, ClassMulti5A, ClassMulti5B)
CLASS_MULTI_INHERIT_2(ClassMulti7A, ClassMulti6A, ClassMulti6B)

#endif
