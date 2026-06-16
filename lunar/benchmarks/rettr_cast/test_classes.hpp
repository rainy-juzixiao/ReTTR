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

#ifndef RTETR_TESTCLASSES_HPP
#define RETTR_TESTCLASSES_HPP

#include <rettr/rettr.hpp>

#define CLASS(CLASS_NAME)                                                                                                             \
    struct CLASS_NAME {                                                                                                               \
        virtual ~CLASS_NAME() {                                                                                                       \
        }                                                                                                                             \
        RETTR_ENABLE() virtual int getType() {                                                                                        \
            return dummyIntValue;                                                                                                     \
        }                                                                                                                             \
        int dummyIntValue;                                                                                                            \
    };

#define CLASS_INHERIT(CLASS1, CLASS2)                                                                                                 \
    struct CLASS1 : CLASS2 {                                                                                                          \
        virtual int getType() {                                                                                                       \
            return static_cast<int>(dummyDoubleValue);                                                                                \
        }                                                                                                                             \
        RETTR_ENABLE(CLASS2) double dummyDoubleValue;                                                                                 \
    };

#define CLASS_VIRTUAL_INHERIT(CLASS1, CLASS2)                                                                                         \
    struct CLASS1 : virtual CLASS2 {                                                                                                  \
        virtual int getType() {                                                                                                       \
            return static_cast<int>(dummyDoubleValue);                                                                                \
        }                                                                                                                             \
        RETTR_ENABLE(CLASS2) double dummyDoubleValue;                                                                                 \
    };

#define CLASS_MULTI_INHERIT_2(CLASS1, CLASS2, CLASS3)                                                                                 \
    struct CLASS1 : CLASS2, CLASS3 {                                                                                                  \
        virtual int getType() {                                                                                                       \
            return static_cast<int>(dummyBoolValue);                                                                                  \
        }                                                                                                                             \
        RETTR_ENABLE(CLASS2, CLASS3) bool dummyBoolValue;                                                                             \
    };

#define CLASS_MULTI_INHERIT_3(CLASS1, CLASS2, CLASS3, CLASS4)                                                                         \
    struct CLASS1 : CLASS2, CLASS3, CLASS4 {                                                                                          \
        virtual int getType() {                                                                                                       \
            return static_cast<int>(dummyBoolValue);                                                                                  \
        }                                                                                                                             \
        RETTR_ENABLE(CLASS2, CLASS3, CLASS4) bool dummyBoolValue;                                                                     \
    };

#define CLASS_MULTI_INHERIT_5(CLASS1, CLASS2, CLASS3, CLASS4, CLASS5, CLASS6)                                                         \
    struct CLASS1 : CLASS2, CLASS3, CLASS4, CLASS5, CLASS6 {                                                                          \
        virtual int getType() {                                                                                                       \
            return static_cast<int>(dummyBoolValue);                                                                                  \
        }                                                                                                                             \
        RETTR_ENABLE(CLASS2, CLASS3, CLASS4, CLASS5, CLASS6) bool dummyBoolValue;                                                     \
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

CLASS(ClassDiamondTop)

CLASS_VIRTUAL_INHERIT(ClassDiamondLeft1, ClassDiamondTop)
CLASS_INHERIT(ClassDiamondLeft2, ClassDiamondLeft1)
CLASS_INHERIT(ClassDiamondLeft3, ClassDiamondLeft2)
CLASS_INHERIT(ClassDiamondLeft4, ClassDiamondLeft3)
CLASS_INHERIT(ClassDiamondLeft5, ClassDiamondLeft4)
CLASS_INHERIT(ClassDiamondLeft6, ClassDiamondLeft5)

CLASS_VIRTUAL_INHERIT(ClassDiamondMiddle1, ClassDiamondTop)
CLASS_INHERIT(ClassDiamondMiddle2, ClassDiamondMiddle1)
CLASS_INHERIT(ClassDiamondMiddle3, ClassDiamondMiddle2)
CLASS_INHERIT(ClassDiamondMiddle4, ClassDiamondMiddle3)
CLASS_INHERIT(ClassDiamondMiddle5, ClassDiamondMiddle4)
CLASS_INHERIT(ClassDiamondMiddle6, ClassDiamondMiddle5)

CLASS_VIRTUAL_INHERIT(ClassDiamondRight1, ClassDiamondTop)
CLASS_INHERIT(ClassDiamondRight2, ClassDiamondRight1)
CLASS_INHERIT(ClassDiamondRight3, ClassDiamondRight2)
CLASS_INHERIT(ClassDiamondRight4, ClassDiamondRight3)
CLASS_INHERIT(ClassDiamondRight5, ClassDiamondRight4)
CLASS_INHERIT(ClassDiamondRight6, ClassDiamondRight5)

CLASS_MULTI_INHERIT_3(ClassDiamondFinal, ClassDiamondLeft5, ClassDiamondMiddle5, ClassDiamondRight5)

#endif
