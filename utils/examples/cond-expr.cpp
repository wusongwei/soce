/*
* Licensed to the Apache Software Foundation (ASF) under one
* or more contributor license agreements. See the NOTICE file
* distributed with this work for additional information
* regarding copyright ownership. The ASF licenses this file
* to you under the Apache License, Version 2.0 (the
* "License"); you may not use this file except in compliance
* with the License. You may obtain a copy of the License at
*
*   http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing,
* software distributed under the License is distributed on an
* "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
* KIND, either express or implied. See the License for the
* specific language governing permissions and limitations
* under the License.
*/

#include <string>
#include "utils/cond-expr.h"
#include "log4rel/logger.h"

static bool compare(const std::string& lvalue,
                    const std::string& oper,
                    const std::string& rvalue)
{
    (void) lvalue;
    (void) rvalue;

    return (oper == "true") ? true : false;
}

void cond_expr()
{
    SOCE_DEBUG << _D("--------------- cond_expr ---------------");

    std::string str = "(a false 1) || ( ((b true 1)) && (c false 1 || d true 1)) && e true 1";

    soce::utils::CondExpr expr;
    expr.parse(str.c_str(), str.size());
    bool rc = expr.calc(std::bind(compare,
                                  std::placeholders::_1,
                                  std::placeholders::_2,
                                  std::placeholders::_3));
    expr.show();

    SOCE_DEBUG << _S("CondExpr", rc);
}
