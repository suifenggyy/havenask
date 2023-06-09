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

#include "ha3/config/SummaryProfileConfig.h"
#include "ha3/summary/SummaryProfileManager.h"
#include "navi/engine/Resource.h"
#include "suez_navi/resource/CavaPluginManagerR.h"
#include "suez_navi/resource/MetricsReporterR.h"
#include "suez_navi/resource/TableInfoR.h"

namespace suez_navi {

class SummaryProfileManagerR : public navi::Resource
{
public:
    SummaryProfileManagerR();
    ~SummaryProfileManagerR();
    SummaryProfileManagerR(const SummaryProfileManagerR &) = delete;
    SummaryProfileManagerR &operator=(const SummaryProfileManagerR &) = delete;
public:
    void def(navi::ResourceDefBuilder &builder) const override;
    bool config(navi::ResourceConfigContext &ctx) override;
    navi::ErrorCode init(navi::ResourceInitContext &ctx) override;
public:
    isearch::summary::SummaryProfileManagerPtr getManager() const;
public:
    static const std::string RESOURCE_ID;
private:
    TableInfoR *_tableInfoR;
    CavaPluginManagerR *_cavaPluginManagerR;
    MetricsReporterR *_metricReporterR;
    std::string _configPath;
    isearch::config::SummaryProfileConfig _summaryProfileConfig;

    isearch::summary::SummaryProfileManagerPtr _summaryProfileManager;
};

NAVI_TYPEDEF_PTR(SummaryProfileManagerR);

}
