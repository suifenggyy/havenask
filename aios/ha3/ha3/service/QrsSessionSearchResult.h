/*
 * Copyright 2014-present Alibaba Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include <memory>
#include <string>

#include "autil/CompressionUtil.h"
#include "autil/Log.h" // IWYU pragma: keep
#include "ha3/isearch.h"
#include "aios/network/gig/multi_call/common/ErrorCode.h"

namespace isearch {
namespace service {

struct QrsSessionSearchResult {
    QrsSessionSearchResult(const std::string resultStr_ = "",
                           autil::CompressType resultCompressType_ = autil::CompressType::NO_COMPRESS,
                           ResultFormatType formatType_ = RF_XML)
        : multicallEc(multi_call::MULTI_CALL_ERROR_RPC_FAILED)
        , resultStr(resultStr_)
        , resultCompressType(resultCompressType_)
        , formatType(formatType_)
    {
    }
    multi_call::MultiCallErrorCode multicallEc;
    std::string resultStr;
    std::string errorStr;
    std::string srcStr;
    autil::CompressType resultCompressType;
    ResultFormatType formatType;
};

typedef std::shared_ptr<QrsSessionSearchResult> QrsSessionSearchResultPtr;

} // namespace service
} // namespace isearch