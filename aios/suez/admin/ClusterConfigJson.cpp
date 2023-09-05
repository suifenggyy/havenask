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
#include "suez/admin/ClusterConfigJson.h"

namespace suez {

void ClusterConfigJson::Jsonize(autil::legacy::Jsonizable::JsonWrapper &json) {
    json.Jsonize("hippo_config", hippoConfig, hippoConfig);
    json.Jsonize("hosts", hosts, hosts);
    json.Jsonize("service_info_str", serviceInfoStr, serviceInfoStr);
}

} // namespace suez