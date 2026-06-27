#                                 Apache License
#                           Version 2.0, January 2004
#                        http://www.apache.org/licenses/
#
#   Copyright 2026 rainy-juzixiao
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.

# =============================================================================
# ReTTR — Docker build
# =============================================================================
#
#   Build arguments:
#     BUILD_TYPE    Release (default) | Debug
#     SHARED_LIBS   ON (default)      | OFF (static library)
#     BASE_IMAGE    ubuntu:24.04      (default)
#
#   Usage:
#     docker build -t rettr:release .
#
#     docker build \
#       --build-arg BUILD_TYPE=Debug \
#       --build-arg SHARED_LIBS=OFF \
#       -t rettr:debug-static .
#
#   Consume in a downstream CMake project:
#     FROM rettr:release AS deps
#     FROM ubuntu:24.04
#     COPY --from=deps /usr/local /usr/local
#     # then: find_package(ReTTR REQUIRED)
# =============================================================================

ARG BASE_IMAGE=ubuntu:24.04

FROM ${BASE_IMAGE} AS builder

ARG BUILD_TYPE=Release
ARG SHARED_LIBS=ON

RUN apt-get update && apt-get install -y --no-install-recommends \
    cmake g++ make git ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . .

RUN cmake -S . -B build \
    -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DRETTR_BUILD_WITH_DYNAMIC="${SHARED_LIBS}" \
    -DRETTR_BUILD_EXAMPLES=OFF \
    -DRETTR_BUILD_UNIT_TESTS=OFF \
    -DRETTR_USE_CXX26_REFLECTION_TS=OFF \
    && cmake --build build --parallel "$(nproc)" \
    && cmake --install build \
    && printf "ReTTR %s  Build: %s  Shared: %s\n" \
       "$(cat version.txt 2>/dev/null || echo unknown)" "${BUILD_TYPE}" "${SHARED_LIBS}"

FROM ${BASE_IMAGE} AS rettr

ARG BUILD_TYPE=Release
ARG SHARED_LIBS=ON

RUN apt-get update && apt-get install -y --no-install-recommends \
    libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

# Runtime library search path
ENV LD_LIBRARY_PATH=/usr/local/lib:${LD_LIBRARY_PATH}

COPY --from=builder /usr/local/include /usr/local/include
COPY --from=builder /usr/local/lib     /usr/local/lib
COPY --from=builder /usr/local/share   /usr/local/share

LABEL org.opencontainers.image.title="ReTTR"
LABEL org.opencontainers.image.description="Rewrite Technology Type Reflection — C++ runtime reflection library"
LABEL org.opencontainers.image.source="https://github.com/rainy-juzixiao/ReTTR"
LABEL org.opencontainers.image.licenses="Apache-2.0"
LABEL rettr.build-type="${BUILD_TYPE}"
LABEL rettr.shared-libs="${SHARED_LIBS}"
